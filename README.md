# GoodBoy

Because he's a Good Boy

![He's a Good Boy](https://raw.githubusercontent.com/0ctobyte/goodboy/master/goodboy.png)

GoodBoy is a (yet another) gameboy emulator. The goal of this project is to learn more about emulation and to (hopefully) learn how to write clean & modern C++ code.

## Status

Currently the CPU core passes all of blargg's CPU instruction tests. It can play Tetris. Memory bank controller support is WIP.

## Build

![cmake](https://cmake.org) and ![sfml](https://www.sfml-dev.org) are required to build this project. To build, run the following commands:

```
mkdir build
cmake ..
make
```
