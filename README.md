# GoodBoy

Because he's a Good Boy

![He's a Good Boy](https://raw.githubusercontent.com/0ctobyte/goodboy/master/goodboy.png)

GoodBoy is a (yet another) gameboy emulator. The goal of this project is to learn more about emulation and to (hopefully) learn how to write clean & modern C++ code.

## Status

Currently the CPU core passes all of blargg's CPU instruction tests. It can play Tetris. Memory bank controller support is WIP.
There is a limited debugger mode; more details below.

## Build

[cmake](https://cmake.org) and [sfml](https://www.sfml-dev.org) are required to build this project. To build, run the following commands:

```
mkdir build
cmake ..
make
```

To run a ROM:

```
./goodboy <rom file>
```

## Debugger

GoodBoy has a debugger mode and tracing mode. To enable CPU instruction tracing you can use the `-t` option:

```
./goodboy -t <rom file>
```

To run the debugger mode you can use the `-d` option:

```
./goodboy -d <rom file>
```

For help on more options, please use the `-h` option.

The GoodBoy debugger is an ncurses terminal app and supports the following features:

* Halting and resuming execution
* Single stepping through instructions
* Dumping register state
* Modifying and viewing single registers (8-bit and 16-bit registers are both supported)
* Modifying and viewing a single memory location
* Scrolling support to view complete trace in the terminal

For command usage, type `h` in the debugger.
