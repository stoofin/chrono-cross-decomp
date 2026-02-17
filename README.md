# Chrono Cross Decompilation
[![Build Status]][actions] [![Progress]][progress site]

[Build Status]: https://github.com/jdperos/chrono-cross-decomp/actions/workflows/build.yaml/badge.svg
[actions]: https://github.com/jdperos/chrono-cross-decomp/actions/workflows/build.yml
[Progress]: https://decomp.dev/jdperos/chrono-cross-decomp.svg?mode=shield&measure=code&category=all&label=Progress
[progress site]: https://decomp.dev/jdperos/chrono-cross-decomp

This project is an in-progress matching decompilation of Chrono Cross for PlayStation 1. The project currently targets the Japanese release (SLPS 023.64) extracted from the Chrono Cross: Radical Dreamers Edition remaster.

<img src="https://assets.nintendo.com/image/upload/ar_16:9,b_auto:border,c_lpad/b_white/f_auto/q_auto/dpr_1.5/c_scale,w_1700/store/software/switch/70010000032650/718759a53a0b804d4fb16080d210a44ea18a8bca3277bc3472a3b988f1a382ef" alt="Chrono Cross" />

## Why Radical Dreamers Edition?

The original PlayStation 1 release of Chrono Cross uses complex custom disk sectoring and data packing that makes it extremely difficult to extract cleanly. Fortunately, the 2022 Radical Dreamers Edition remaster provides us with a much cleaner extraction path.

The remaster works by running an emulator that loads the original PS1 game from a file called `cdrom.dat` - which is essentially a zip file containing an unmodified copy of the original Japanese PS1 release (SLPS 023.64). The emulator then injects modern features like new fonts and artwork at runtime.

By extracting `cdrom.dat` from Radical Dreamers Edition, we get direct access to the original PS1 binary without having to deal with the nightmare of custom disk sectoring.


## Thanks

This project stands on the shoulders of giants and owes a huge debt to:

- [splat](https://github.com/ethteck/splat) - Binary splitting and disassembly
- [spimdisasm](https://github.com/Decompollaborate/spimdisasm) - MIPS disassembly
- [Xenogears decomp](https://github.com/ladysilverberg/xenogears-decomp) - Project structure and infrastructure inspiration
- [MASPSX](https://github.com/mkst/maspsx) - PSY-Q compatible assembler
- [objdiff](https://github.com/encounter/objdiff) - Function diffing tool
- [decomp.dev](https://decomp.dev/) - Progress tracking and visualization
- [decomp.me](https://decomp.me/) - Collaborative decompilation platform
- [The Chrono Compendium](https://chronocompendium.com/) community for reverse engineering research
- [q-gears reverse](https://github.com/Akari1982/q-gears_reverse) - Reverse engineering of similar games

## Building

### Prerequisites

These instructions assume you're using a Debian-based Linux distribution (Ubuntu, Debian, etc.). If you're on Windows, you can use [WSL2](https://docs.microsoft.com/en-us/windows/wsl/install).

### Install System Packages

```bash
sudo apt update
sudo apt install git make 7zip \
    binutils-mips-linux-gnu gcc-mips-linux-gnu \
    python3 python3-pip python3-venv
```

If on an ARM-based architecture or VM, you'll need these packages as well
```bash
sudo apt install qemu-user-static binfmt-support
```

### Clone the Repository

```bash
git clone --recursive https://github.com/jdperos/chrono-cross-decomp.git
```

### Install Python Dependencies

We use a virtual environment to manage Python packages:

```bash
cd chrono-cross-decomp
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

**Important:** Each time you open a new terminal, you'll need to reactivate the virtual environment:

```bash
source .venv/bin/activate
```

### Obtaining the ROM

You need to own a legal copy of Chrono Cross: Radical Dreamers Edition. The game is available on:
- Steam
- PlayStation 4/5
- Nintendo Switch
- Xbox

#### Extracting from Radical Dreamers Edition

**For Steam (PC):**

1. Locate your game installation directory:
    - **Windows**: `C:\Program Files (x86)\Steam\steamapps\common\CHRONO CROSS  THE RADICAL DREAMERS EDITION\`
    - **Linux**: idk lol

2. Find the `cdrom.dat` file (in the `data` folder)

3. Copy it to the project's `assets/disc/` directory:

```bash
# From the project root
mkdir -p assets/disc
cp "/path/to/cdrom.dat" assets/disc/cdrom.dat
```

4. Verify the checksum:

```bash
sha1sum assets/disc/cdrom.dat
# Should match: 354e6593b639c245ba41db1d0bd49664feb4c65e
```

**For Console Versions:**

Extracting from console versions requires additional steps beyond the scope of this README.

### Extract Game Data

Once you have the ROM in place:

```bash
make extract
```

This will extract cdrom.dat to expose all of the program data

### Build and Verify

Build the project and verify it matches the original:

```bash
make clean-check
```

If successful, you should see:
```
build/out/extracted/prog/slps_023.64: OK
```

### Additional Make Commands

- `make generate` - Split the binary into assembly only
- `make build` - Build the executable without checking
- `make check` - Compare built executable against original
- `make clean` - Clean build artifacts
- `make reset` - Wipe repository (bigger clean)
- `make generate-context` - Generate context file for m2c and decomp.me
- `make objdiff-config` - Generate objdiff configuration

## Contributing

Happily welcoming contributions! Here are some ways you can help:

### Decompiling Functions

The primary goal is to match functions - converting assembly back into C code that compiles to identical machine code.

1. **Choose a function** to work on from the `src/` directory
2. **Use [decomp.me](https://decomp.me)** to collaboratively match the function
    - Platform: PlayStation 1
    - Compiler: `gcc 2.8.1-psx + maspsx`
    - Flags: `-Wall -Wa,--aspsx-version=2.79,--expand-div -g3 -O2 -G0`
    - Source ASM copied from `asm/` directory after `make` generate commands
    - Context can be generated using `make generate-context` into `ctx/project.ctx.c`
3. Alternatively Use objdiff locally for matching
    - Call `make objdiff-config` to generate asm, objdiff.json, and context file
    - Open objdiff and point the project to the root directory
        - Preferred workflow on Windows/WSL is to run native Windows objdiff and point it to the directory in WSL
    - Explore the objects in the project viewer to find your function target
    - Run this command, replacing the paths and function name
    ```
    python ./tools/cc_decompile.py -c \ 
        ./src/slps_023.64/path/to/source.c /
        ./asm/slps_023.64/nonmatchings/path/to/source/func_01234567.s 
    ```
    - Replace the `INCLUDE_ASM()` macro in your corresponding file with the generated code as a starting point
4. **Submit a pull request** with your matched function

### Reverse Engineering

Help document and understand the game's systems:

- Join the Chrono Cross repository [Ghidra server on decomp.dev](https://ghidra.decomp.dev/) for static analysis
- Name functions, variables, and data structures in `config/symbol_addrs.*.txt` and source/header files
- Identify program layout and splits in `config/*.yaml` files
- Document game systems and algorithms
- Use emulator debuggers (PCSX Redux, no$psx) for dynamic analysis

### Tooling and Automation

Improve the build system and developer experience:

- Enhance Python scripts
- Improve objdiff/decomp.me integration
- Create Ghidra scripts for importing symbols
- Document workflows

## Project Structure

```
chrono-cross-decomp/
├── asm/                            # Splat-generated assembly (the "target") (gitignored)
├── assets                          # User-provided assets
│   ├── disc/                       # ROM and extracted game data (gitignored)
│   │   ├── extracted/              # Game files extracted from cdrom.dat
│   │   │   └── prog/               # Game program data
│   │   │       └── slps_023.64     # Main game executable
│   │   └── cdrom.dat               # Disk image copied from Radical Dreamers edition
│   └── psyq/                       # Psyq binaries
│       └── 450/                    # Version 4.5
│           ├── lib/                # User-provided lib files
│           └── obj/                # Extracted obj files
├── build/                          # Build files (gitignored)
│   ├── asm/                        # Built from assembly for comparison
│   ├── out/                        # Map and elf files
│   └── src/                        # Built from decompiled source code
├── config/                         # Splat configuration and symbol addresses
├── ctx/                            # Generated context files (gitignored)
├── include/                        # C header files
│   ├── psyq/                       # PSY-Q SDK headers
│   └── system/                     # Game system headers
├── linker/                         # Generated linker scripts (gitignored)
├── src/                            # Decompiled C source code
│   └── slps_023.64/                # Main executable source
├── tools/                          # Build tools and scripts
```

## Communication

- **Issues**: Use GitHub issues for bug reports and feature requests
- **Pull Requests**: Submit PRs for matched functions and improvements

## Learning Resources

New to decompilation? Check out these resources:

- [decomp.me FAQ](https://decomp.me/faq) - Learn the basics of matching decompilation
- [MIPS Assembly Reference](https://www.mips.com/) - Understanding PS1 assembly
- [C Programming Tutorial](https://www.learn-c.org/) - Brush up on C
- [Compiler Explorer](https://godbolt.org/) - See how C compiles to assembly

## Progress

Progress is tracked and visualized on [decomp.dev](https://decomp.dev/yourusername/chrono-cross-decomp).


## Legal

This project is for educational and preservation purposes only. You must own a legal copy of Chrono Cross to extract the ROM. We do not distribute any copyrighted game assets.

Chrono Cross © 1999-2022 Square Enix Co., Ltd. All Rights Reserved.
