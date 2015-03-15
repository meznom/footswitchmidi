/*
 * Copyright (c) 2015 Burkhard Ritter
 * This code is distributed under the two-clause BSD License.
 */
#ifndef OSC_HPP
#define OSC_HPP

#include <list>
#include <string>

#include <lo/lo.h>
#include <lo/lo_cpp.h>

class OSC
{
private:
    std::string host;
    std::string port;
    lo::Address listener;

public:
    OSC (std::string host_ = "127.0.0.1", std::string port_ = "57120")
    : host{host_}, port{port_}, listener{host_,port_}
    {
    }

    template<class Message>
    void notify (Message m)
    {
        listener.send("/footswitch", "ii", static_cast<int>(m.button), static_cast<int>(m.on));
    }
};

#endif /* OSC_HPP */
