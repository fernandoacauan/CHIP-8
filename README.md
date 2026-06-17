# CHIP-8 Interpreter
> **Author: Fernando A.**

An **CHIP-8 interpreter** written in C++, rendered in real-time via SDL2. Every subsystem (memory, display, CPU and allocator) is implemented from scratch, with an emphasis on low-level correctness and performance.

<img width="669" height="371" alt="chip8-test-rom" src="https://github.com/user-attachments/assets/de7850c9-397b-427f-9707-6a9c70c85304" />


## Key Features

*   **Full Instruction Set**: All 35 standard CHIP-8 opcodes are implemented, including arithmetic, bitwise operations, flow control, sprite drawing, and I/O.
*   **Table-Driven Dispatch**: Opcodes are decoded and routed through a 16-entry function pointer table (`s_mainTable`), with secondary dispatch tables for the `0x0`, `0x8`, `0xE` and `0xF` opcode families.
*   **Branchless Memory R/W**: `CMemory::Read()` and `Write()` perform bounds checking using borrow-bit arithmetic, with no branches, no conditionals, just bitwise masking.
*   **XOR Sprite Rendering with Collision**: `CDisplay::Draw()` faithfully implements the CHIP-8 XOR pixel model with wrap-around and VF collision detection.
*   **Arena-Backed Allocation**: All subsystem objects are placed into arena-managed memory, eliminating fragmentation and enabling instant bulk deallocation.
*   **Delay and Sound Timers**: `DT` and `ST` are decremented once per frame through `CInterpreter::TickTimers()`, running at 60 Hz.

## Technical Architecture

The emulator is organized into four independent subsystems, all instantiated from a shared `CArena` at startup:

### CInterpreter - CPU
Holds the full CHIP-8 register file: 16 general-purpose `V[0..F]` registers, the 16-bit `I` and `PC` registers, an 8-bit stack pointer `SP`, delay timer `DT`, sound timer `ST`, and a 16-level call stack. Each call to `Execute()` fetches one big-endian 16-bit opcode from RAM, advances `PC` by 2, and dispatches to the correct handler via `s_mainTable`.

### CInstructions - Opcode Handlers
All instructions are implemented as static methods of `CInstructions`. Arithmetic operations like `SUB` and `SUBN` use a branchless borrow-bit trick to set VF without any conditional branching. BCD encoding (`Fx33`) uses integer multiplication instead of modulo division.

### CMemory - 4KB RAM
Holds a flat 4096-byte RAM array. Font sprites for digits `0-F` are pre-loaded at address `0x000`, and ROMs are loaded starting at `0x200`. Both `Read()` and `Write()` use the same branchless bounds check: the borrow from `address - RAM_SIZE` is extracted into a mask that zeroes the result on out-of-bounds access, with no branches or undefined behaviour.

### CDisplay - 64×32 Framebuffer
Stores the pixel state as a flat `64 * 32` byte array. Sprites are drawn row by row, with pixel coordinates wrapping around the screen edges using modulo. Each active sprite bit XORs the target pixel; if a pixel transitions from set to clear, the collision flag is raised.

## Memory Layout

| Region | Address | Content |
|---|---|---|
| Font sprites | `0x000 – 0x04F` | Built-in 4×5 hex digit glyphs (0-F) |
| Reserved | `0x050 – 0x1FF` | Unused |
| ROM / Program | `0x200 – 0xFFF` | Loaded from file at startup |

## Keyboard Mapping

The standard CHIP-8 hex keypad (`0x0–0xF`) is mapped to:

```
CHIP-8   →   Keyboard
1 2 3 C       1 2 3 4
4 5 6 D  →    Q W E R
7 8 9 E       A S D F
A 0 B F       Z X C V
```

## Usage

```bash
CHIP-8.exe <rom_file>
```

### Example
```bash
CHIP-8.exe roms/pong.ch8
```

## Building

The project depends on **SDL2**. Link against `SDL2.lib` / `libSDL2` and include the `inc/` directory.

## Implementation Notes

*   **Cycles per frame**: 5 opcodes are executed per frame before timers are ticked and the display is refreshed.
*   **Target framerate**: ~60 FPS (frame delay of `100 / 60` ms).
