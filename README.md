# CHIP-8 Emulator

A CHIP-8 emulator written in C with SDL2 for graphics and input.

CHIP-8 is an interpreted programming language from the late 1970s, originally designed for the COSMAC VIP microcomputer. This emulator implements all 35 original instructions and can run standard CHIP-8 ROMs.

## Building

Requires CMake and SDL2.

### Install dependencies

**Ubuntu/Debian:**
```bash
sudo apt install cmake libsdl2-dev
```

**Arch/Manjaro:**
```bash
sudo pacman -S cmake sdl2
```

### Compile

```bash
mkdir cmake-build-debug && cd cmake-build-debug
cmake ..
make
```

## Usage

```bash
./chip8_emulator <path_to_rom>
```

Example:
```bash
./chip8_emulator roms/ibm_logo.ch8
```

## Controls

The original CHIP-8 hex keypad is mapped to the left side of a QWERTY keyboard:

```
Keyboard        CHIP-8 Keypad
+-+-+-+-+       +-+-+-+-+
|1|2|3|4|       |1|2|3|C|
+-+-+-+-+       +-+-+-+-+
|Q|W|E|R|       |4|5|6|D|
+-+-+-+-+       +-+-+-+-+
|A|S|D|F|       |7|8|9|E|
+-+-+-+-+       +-+-+-+-+
|Z|X|C|V|       |A|0|B|F|
+-+-+-+-+       +-+-+-+-+
```

Press **ESC** to quit.

## Features

- All 35 CHIP-8 instructions implemented
- 64x32 monochrome display scaled to 1280x640
- Keyboard input mapped to standard QWERTY layout
- Delay and sound timers running at 60Hz

## Project Structure

- `chip8.h` — CHIP-8 system struct and function declarations
- `chip8.c` — Emulator core: initialization, ROM loading, instruction decoding and execution
- `main.c` — SDL2 window, rendering, input handling, and main loop

## Resources

- [Tobias V. Langhoff's guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) — High-level emulator guide
- [Timendus test suite](https://github.com/Timendus/chip8-test-suite) — Test ROMs for verifying instruction correctness
- [CHIP-8 Archive](https://johnearnest.github.io/chip8Archive/) — Public domain games

## License

MIT
