/*
 * Copyright (c) 2014 Burkhard Ritter
 * This code is distributed under the two-clause BSD License.
 */
/*
 * For footswitchmidi to work, we need to have the footswitch programmed with:
 * ./footswitch -1 -k '<01>' -2 -k '<02>' -3 -k '<03>'
 */

#include <iostream>
#include "footswitch.hpp"
#include "messagequeue.hpp"
#include "jackmidi.hpp"

struct FootSwitchMessage
{
    unsigned char button;
    bool on;
};

int main (int argc, char** argv)
{
    MessageQueue<FootSwitchMessage, 100> queue;
    if (!queue.is_lock_free())
    {
        std::cerr << "Our message queue is not lock free!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    JackMidi<decltype(queue)> jack(queue);

    jack.init();

    FootSwitch f;
    f.debug_output = false;
    f.handle_release = true;
    if (!f.init())
    {
        std::cerr << "Could not initialize footswitch" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    f.run([&queue](unsigned char i, bool on) {
        const bool r = queue.push(FootSwitchMessage{i, on});
        if (!r)
            std::cerr << "Could not write message to queue --- queue is full" << std::endl;
    });
    
    f.deinit();

    jack.deinit();
}
