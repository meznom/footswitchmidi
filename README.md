# FootswitchMidi

## Overview

FootswitchMidi is a [Jack][] Midi client for those cheap foot switches sold by
[DealExtreme][] and [PCsensor][], i.e. it translates switch action to Midi
commands that can be used to control (Jack) Midi applications. The foot
switches can be programmed with the [Footswitch command line tool][footswitch].

## Known limitations

Right now the program is still in a very early and experimental stage, but
basic functionality seems to be working.

Some of the known limitations are:

* Not robust with respect to plugging and unplugging
* Only supports models with three switches
* Footswitch needs to be programmed / prepared correctly
* Midi messages hardcoded for now
* Midi messages not very accurate in time
* With multiple pressed switches, a switch release might give the wrong switch
  (this is a hardware limitation)

## Building

The program requires [Jack][] and [libusb-1.0][], as well as [cmake][] for
building. On Ubuntu you can install them with:

```
$ sudo apt-get install libjack-dev libusb-1.0-0-dev cmake
```

To download, build and run FootswitchMidi:

```
$ git clone https://github.com/meznom/footswitchmidi.git
$ cd footswitchmidi
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./src/footswitchmidi
```

## License

The code is distributed under the two-clause BSD license. Have a look at the
`LICENSE` file for details.

---
Burkhard Ritter (<burkhard@seite9.de>), September 2014


[Jack]: http://jackaudio.org/
[DealExtreme]: http://www.dx.com/p/usb-triple-action-foot-switch-keyboard-control-foot-pedal-56508#.U_0L1db7sjg 
[PCsensor]: http://www.pcsensor.com/index.php?_a=category&cat_id=41
[footswitch]: https://github.com/rgerganov/footswitch
[libusb-1.0]: http://www.libusb.org/ 
[cmake]: http://www.cmake.org/
