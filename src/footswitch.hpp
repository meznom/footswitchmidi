/*
 * Copyright (c) 2014 Burkhard Ritter
 * This code is distributed under the two-clause BSD License.
 */
#ifndef FOOTSWITCH_HPP
#define FOOTSWITCH_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <libusb.h>

class FootSwitch
{
public:
    const uint16_t VID = 0x0C45;
    const uint16_t PID = 0x7403;

    bool handle_release = true;
    bool debug_output = false;

private:
    libusb_context* con = nullptr;
    libusb_device_handle* dev = nullptr;

public:
    FootSwitch ()
    {}

    void error (const std::string& msg)
    {
        std::cerr << msg << std::endl;
    }

    void debug (const std::string& msg, const unsigned char* response=nullptr)
    {
        if (debug_output)
        {
            std::cerr << msg << std::endl;
            if (response!=nullptr)
            {
                for (int i=0; i<8; i++)
                {
                    auto c = response[i];
                    std::cerr << static_cast<int>(c) << " ";
                }
                std::cerr << std::endl;
            }
        }
    }

    bool init ()
    {
        int r;

        r = libusb_init(&con);
        if (r!=0)
        {
            error("Cannot initialize libusb");
            return false;
        }
        libusb_set_debug(con, LIBUSB_LOG_LEVEL_WARNING);
        dev = libusb_open_device_with_vid_pid(con, VID, PID);
        if (dev==nullptr)
        {
            error("Cannot find footswitch");
            return false;
        }
        libusb_set_auto_detach_kernel_driver(dev, 1);
        libusb_claim_interface(dev, 0);
        if (r!=0)
        {
            error("Cannot claim interface");
            return false;
        }
        return true;
    }

    bool deinit ()
    {
        int r;
        r = libusb_release_interface(dev,0);
        if (r!=0)
        {
            error("Cannot release interface");
            return false;
        }
        libusb_close(dev);
        libusb_exit(con);
        return true;
    }

    template<class CallbackType>
    bool run (CallbackType callback)
    {
        unsigned char response[8];
        int transferred;
        int r;
        unsigned char pressed[3] = {0,0,0};
        unsigned char n_pressed = 0;
        
        while (true)
        {
            // asynchronous libusb interface might be better...
            r = libusb_interrupt_transfer(dev, 0x81, response, 8, &transferred, 100);
            if (r==0 && transferred==8)
            {
                if (response[0]==1 && response[1]==0 && response[2]==0 && 
                    (response[3]==1 || response[3]==2 || response[3]==3))
                {
                    auto button = response[3];
                    std::stringstream ss;
                    ss << "Switch " << static_cast<int>(button) << " pressed";
                    debug(ss.str(), response);

                    if (n_pressed<3) 
                    {
                        n_pressed++;
                        pressed[n_pressed-1] = button;
                        callback(button, true);
                    }
                }
                else if (response[0]==1 && response[1]==0 && response[2]==0 && response[3]==0)
                {
                    if (n_pressed>0)
                    {
                        n_pressed--;
                        if (handle_release)
                        {
                            auto button = pressed[n_pressed];
                            
                            std::stringstream ss;
                            ss << "Switch " << static_cast<int>(button) << " released";
                            debug(ss.str(), response);
                            
                            callback(button,false);
                        }
                    }
                    else
                    {
                        debug("Unknown switch released", response);
                    }
                }
            }
            else if (r!=LIBUSB_ERROR_TIMEOUT)
            {
                error("Could not read anything");
                return false;
            }
        }
        return true;
    }
};

#endif /* FOOTSWITCH_HPP */
