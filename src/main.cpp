/*
 * Copyright (c) 2014 Burkhard Ritter
 * This code is distributed under the two-clause BSD License.
 */
/*
 * For footswitchmidi to work, we need to have the footswitch programmed with:
 * ./footswitch -1 -k '<01>' -2 -k '<02>' -3 -k '<03>'
 */

#include <iostream>
#include <string>
#include "footswitch.hpp"
#include "messagequeue.hpp"
#include "jackmidi.hpp"

struct FootSwitchMessage
{
    unsigned char button;
    bool on;
};

struct Options
{
    Options()
    : debug{false}, help{false}
    {}

    bool debug;
    bool help;
};

Options parseCommandLineArguments(int argc, char** argv)
{
    Options o;
    for (int i=0; i<argc; i++)
    {
        std::string a = argv[i];
        if (a=="--help" || a=="-h")
        {
            o.help = true;
        }
        else if (a=="--debug" || a=="-d")
        {
            o.debug = true;
        }
    }
    return o;
}

void printUsageMessage ()
{
    std::cout
    << "FootswitchMidi: Make the cheap PCsensor / DealExtreme foot switches " << std::endl
    << "available via Jack Midi." << std::endl
    << std::endl
    << "Usage: footswitchmidi [options]" << std::endl
    << std::endl
    << "-d, --debug \tPrint debug messages" << std::endl
    << "-h, --help  \tShow this help message" << std::endl
    << std::endl
    << "For footswitchmidi to work, you need to have the footswitch programmed with:" << std::endl
    << "   $ ./footswitch -1 -k '<01>' -2 -k '<02>' -3 -k '<03>' " << std::endl
    << "The footswitch program is available at https://github.com/rgerganov/footswitch." << std::endl;
}

int main (int argc, char** argv)
{
    Options o;
    o = parseCommandLineArguments(argc, argv);
    if (o.help)
    {
        printUsageMessage();
        std::exit(EXIT_SUCCESS);
    }

    MessageQueue<FootSwitchMessage, 100> queue;
    if (!queue.is_lock_free())
    {
        std::cerr << "Our message queue is not lock free!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    JackMidi<decltype(queue)> jack(queue);

    jack.init();

    FootSwitch f;
    f.debug_output = o.debug;
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
