# FootswitchMidi / FootswitchOSC

## Overview

FootswitchMidi is a [Jack][] Midi client for those cheap foot switches sold by
[DealExtreme][] and [PCsensor][], i.e. it translates switch actions to Midi
commands that can be used to control (Jack) Midi applications. FootswitchOSC
translates switch actions to [OSC][] messages. The foot switches can be
programmed with the [Footswitch command line tool][footswitch].

## Known limitations

Right now the program is still in a very early stage, but basic functionality
seems to be working.

Some of the known limitations are:

* Not robust with respect to plugging and unplugging
* Only supports models with three switches
* Footswitch needs to be programmed / prepared correctly: Program it with
  `./footswitch -1 -k '<01>' -2 -k '<02>' -3 -k '<03>'`
* Midi messages hardcoded for now
* Midi messages not very accurate in time
* Only one OSC destination. Does not support multiple listeners (i.e.
  applications) via OSC.
* Does not shutdown cleanly (i.e. deregister with libusb and Jack). This does
  not seem to be a problem in practice.
* With multiple pressed switches, a switch release might give the wrong switch
  (this is a hardware limitation)

## Building

The program requires [Jack][], [libusb-1.0][] and [liblo][], as well as
[cmake][] for building. On Ubuntu you can install them with:

```
$ sudo apt-get install libjack-dev libusb-1.0-0-dev liblo-dev cmake
```

To download, build, and install FootswitchMidi and FootswitchOSC to the home
directory:

```
$ git clone https://github.com/meznom/footswitchmidi.git
$ cd footswitchmidi
$ mkdir build
$ cd build
$ cmake -DCMAKE_INSTALL_PREFIX=~ ..
$ make install
```

Run FootswitchMidi or FootswitchOSC:

```
# Requires that Jack is running
$ footswitchmidi
# Send OSC messages of the form "/footswitch [i:button] [i:on/off]" to
# localhost, port 57120 (this is the default behaviour)
$ footswitchosc --host 127.0.0.1 --port 57120
# Get a useful help message
$ footswitchmidi --help
```

To quit FootswichMidi or FootswitchOSC, simply press `Ctrl-C` (send `SIGTERM`).

## License

The code is distributed under the two-clause BSD license. Have a look at the
`LICENSE` file for details.

---
Burkhard Ritter (<burkhard@seite9.de>), March 2015


[Jack]: http://jackaudio.org/
[OSC]: http://opensoundcontrol.org/
[DealExtreme]: http://www.dx.com/p/usb-triple-action-foot-switch-keyboard-control-foot-pedal-56508#.U_0L1db7sjg
[PCsensor]: http://www.pcsensor.com/index.php?_a=category&cat_id=41
[footswitch]: https://github.com/rgerganov/footswitch
[libusb-1.0]: http://www.libusb.org/
[liblo]: http://liblo.sourceforge.net/
[cmake]: http://www.cmake.org/
