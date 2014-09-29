/*
 * Copyright (c) 2014 Burkhard Ritter
 * This code is distributed under the two-clause BSD License.
 */
#ifndef MESSAGEQUEUE_HPP
#define MESSAGEQUEUE_HPP

#include <atomic>

// http://www.codeproject.com/Articles/43510/Lock-Free-Single-Producer-Single-Consumer-Circular
// https://bitbucket.org/KjellKod/lock-free-wait-free-circularfifo
//
// A lockfree circular fifo, single producer, single consumer
template<class Message_, size_t Size>
class MessageQueue
{
public:
    typedef Message_ Message;

private:
    const static size_t N = Size + 1;
    Message queue[N];
    std::atomic<size_t> i_read;
    std::atomic<size_t> i_write;

public:

    MessageQueue ()
    : i_read(0), i_write(0)
    {}

    bool pop (Message& m)
    {
        const auto i_r = i_read.load();
        const auto i_w = i_write.load();
        if (i_r==i_w)
            return false;
        
        m = queue[i_r];
        const auto i_r_next = (i_r+1)%N;
        i_read.store(i_r_next);
        return true;
    }
    
    bool push (Message m)
    {
        const auto i_w = i_write.load();
        const auto i_w_next = (i_w+1)%N;
        const auto i_r = i_read.load();
        if (i_w_next==i_r)
            return false;
        
        queue[i_w] = m;
        i_write.store(i_w_next);
        return true;
    }

    bool is_lock_free () const
    {
        return i_read.is_lock_free() && i_write.is_lock_free();
    }
};

#endif /* MESSAGEQUEUE_HPP */
