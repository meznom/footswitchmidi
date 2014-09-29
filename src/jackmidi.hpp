/*
 * Copyright (c) 2014 Burkhard Ritter
 * This code is distributed under the two-clause BSD License.
 */
#ifndef JACKMIDI_HPP
#define JACKMIDI_HPP

#include <jack/jack.h>
#include <jack/midiport.h>

template<class MessageQueue>
class JackMidi
{
private:
    // Can or should I use smart pointers instead?
    jack_client_t* client;
    jack_port_t* output_port;
    bool active;
    MessageQueue& queue;

public:
    JackMidi (MessageQueue& queue_)
    : queue(queue_)
    {}

    bool init ()
    {
        client = jack_client_open("footswitchmidi", JackNullOption, nullptr);
        if (client == nullptr)
            return false;

        jack_set_process_callback(client, process_callback, this);
        jack_set_sample_rate_callback(client, sample_rate_callback, this);
        jack_on_shutdown(client, shutdown_callback, this);
        
        output_port = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
        if (output_port == nullptr)
            return false;

        if (jack_activate(client) != 0)
            return false;
        
        return true;
    }

    bool deinit ()
    {
        if (jack_deactivate(client) != 0)
            return false;

        if (jack_client_close(client) != 0)
            return false;

        return true;
    }

private:
    int process (jack_nframes_t nframes)
    {
        void* port_buffer = jack_port_get_buffer(output_port, nframes);
        jack_midi_clear_buffer(port_buffer);

        typename MessageQueue::Message m;
        while (queue.pop(m))
        {
            const unsigned char channel = 0;
            const unsigned char controller_number = 20 + m.button;
            unsigned char controller_value;
            if (m.on)
                controller_value = 127;
            else
                controller_value = 0;

            jack_midi_data_t* buffer = jack_midi_event_reserve(port_buffer, 0, 3);
            buffer[0] = 0xB0 + channel;
            buffer[1] = controller_number;
            buffer[2] = controller_value;
        }
        return 0;
    }

    int sample_rate (jack_nframes_t nframes)
    {
        return 0;
    }

    void shutdown ()
    {
    }

    static int process_callback (jack_nframes_t nframes, void* arg)
    {
        JackMidi* self = static_cast<JackMidi*>(arg);
        return self->process(nframes);
    }

    static int sample_rate_callback (jack_nframes_t nframes, void* arg)
    {
        JackMidi* self = static_cast<JackMidi*>(arg);
        return self->sample_rate(nframes);
    }

    static void shutdown_callback (void* arg)
    {
        JackMidi* self = static_cast<JackMidi*>(arg);
        return self->shutdown();
    }
};

#endif /* JACKMIDI_HPP */
