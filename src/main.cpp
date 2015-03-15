/*
 * Copyright (c) 2014, 2015 Burkhard Ritter
 * This code is distributed under the two-clause BSD License.
 */

#include <iostream>
#include <string>
#include "footswitch.hpp"
#include "messagequeue.hpp"
#include "jackmidi.hpp"
#include "osc.hpp"

struct FootSwitchMessage
{
    unsigned char button;
    bool on;
};

struct Options
{
    bool debug = false;
    bool help = false;
    bool osc = false;
    bool midi = true;
    std::string osc_host = "127.0.0.1";
    std::string osc_port = "57120";
};

Options parseCommandLineArguments(int argc, char** argv)
{
    Options o;

    std::string name = argv[0];
    if (name=="footswitchmidi")
    {
        o.midi = true;
        o.osc = false;
    }
    else if (name=="footswitchosc")
    {
        o.midi = false;
        o.osc = true;
    }

    for (int i=1; i<argc; i++)
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
        else if (a=="--osc" || a=="-o")
        {
            o.osc = true;
        }
        else if (a=="--no-osc")
        {
            o.osc = false;
        }
        else if (a=="--midi" || a=="-m")
        {
            o.midi = true;
        }
        else if (a=="--no-midi")
        {
            o.midi = false;
        }
        else if (a=="--osc-host" && (i+1)<argc && std::string(argv[i+1])[0]!='-')
        {
            o.osc_host = std::string(argv[i+1]);
        }
        else if (a=="--osc-port" && (i+1)<argc && std::string(argv[i+1])[0]!='-')
        {
            o.osc_port = std::string(argv[i+1]);
        }
    }
    return o;
}

void printUsageMessage ()
{
    std::cout
    << "FootswitchMidi / FootswitchOSC: Make the cheap PCsensor / DealExtreme " << std::endl
    << "foot switches available via Jack Midi or OSC." << std::endl
    << std::endl
    << "Usage: footswitchmidi [options]" << std::endl
    << "   or: footswitchosc [options]" << std::endl
    << std::endl
    << "-h, --help     \tShow this help message" << std::endl
    << "-d, --debug    \tPrint debug messages" << std::endl
    << "-m, --midi     \tEnable Jack Midi (default when invoked as footswitchmidi)" << std::endl
    << "    --no-midi  \tDisable Jack Midi" << std::endl
    << "-o, --osc      \tEnable OSC (default when invoked as footswitchosc)" << std::endl
    << "    --no-osc   \tDisable OSC" << std::endl
    << "    --osc-host \tHost to send OSC messages to (default: 127.0.0.1)" << std::endl
    << "    --osc-port \tPort to send OSC messages to (default: 57120)" << std::endl
    << std::endl
    << "For footswitchmidi to work, you need to have the footswitch programmed with:" << std::endl
    << "   $ ./footswitch -1 -k '<01>' -2 -k '<02>' -3 -k '<03>' " << std::endl
    << "The footswitch program is available at https://github.com/rgerganov/footswitch." << std::endl;
}

int main (int argc, char** argv)
{
    /*
     * Command line options
     */
    Options o;
    o = parseCommandLineArguments(argc, argv);
    if (o.help)
    {
        printUsageMessage();
        std::exit(EXIT_SUCCESS);
    }

    /*
     * Initialize footswitch
     */
    FootSwitch f;
    f.debug_output = o.debug;
    f.handle_release = true;
    if (!f.init())
    {
        std::cerr << "Could not initialize footswitch" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "Initialized footswitch" << std::endl;

    /*
     * Initialize message queue and Jack Midi
     */
    MessageQueue<FootSwitchMessage, 100> queue;
    if (!queue.is_lock_free())
    {
        std::cerr << "Our message queue is not lock free!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    JackMidi<decltype(queue)> jack(queue);
    if (o.midi)
    {
        if (!jack.init())
        {
            std::cerr << "Could not initialize Jack Midi" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        std::cout << "Initialized Jack Midi" << std::endl;
    }

    /*
     * Initialize OSC
     */
     OSC osc{o.osc_host,o.osc_port};
     if (o.osc)
     {
         std::cout << "Initialized OSC" << std::endl;
     }

    /*
     * Run main event loop --- do something on every footswitch button event
     */
    f.run([&queue,&osc,&o](unsigned char i, bool on) {
        if (o.midi)
        {
            const bool r = queue.push(FootSwitchMessage{i, on});
            if (!r)
                std::cerr << "Could not write message to queue --- queue is full" << std::endl;
        }
        if (o.osc)
        {
            osc.notify(FootSwitchMessage{i, on});
        }
    });

    /*
     * Clean up
     */
    f.deinit();
    if (o.midi)
        jack.deinit();
}
