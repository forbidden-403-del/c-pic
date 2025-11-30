# C Position-Independent Code (PIC)

Cross‑platform C demo that compiles a small program into **position‑independent code** and shows how to:

- resolve Windows APIs manually using the PEB + PE exports,
- use raw Linux syscalls without libc,
- extract a compact “shellcode‑like” code blob from a normal executable.

> ⚠️ This project is intended solely for educational and research purposes in low-level systems and compiler design. Do not use it to break laws, violate school or workplace policies, or service terms.

[GitHub Project](https://github.com/mrzaxaryan/c-pic)

---

## Table of Contents

- [Overview](#overview)
- [Quick Start (Step‑by‑step)](#quick-start-step-by-step)
  - [1. Set up the toolchain](#1-set-up-the-toolchain)
  - [2. Build all targets](#2-build-all-targets)
  - [3. Explore the artifacts](#3-explore-the-artifacts)
- [Supported Targets](#supported-targets)
- [How It Works](#how-it-works)
  - [Windows](#windows)
  - [Linux](#linux)
- [Project Layout](#project-layout)
- [Research & Ethics](#research--ethics)
- [Reference](#reference)

## Overview

Position-independent code is essential for scenarios where code must execute correctly regardless of its memory location. This is especially relevant for shellcode, dynamic libraries, and certain security applications. The sample program in this repository is simple—it prints "Hello world!" and exits cleanly—but the techniques it demonstrates are broadly applicable to more complex payloads and use cases.

At a high level, this project demonstrates how to:

- How to write position-independent code that has no dependencies on external libraries, headers, runtime environments, compiler routines, or other runtime overhead. For Windows and Linux platforms.
- Fixes common pitfalls when writing position-independent code in C, especially regarding data access and string literals.
- Provide simple scripts to build and extract position-independent code blobs for various architectures and platforms.

Writing *truly* position‑independent code in plain C comes with a few constraints:

- you can't rely on global or static variables that need relocations;
- you have to avoid hidden references to external libraries and CRT startup code;
- you must be careful with string literals and constant data, which compilers like to scatter into separate sections.

This project fixes that by: 
- providing a custom linker script to keep code and read-only data together.
- ensuring that all data is properly aligned for position-independent access.

However, putting read-only data in the same section will resolve most of the issues related to position-independent code, but in Windows i386 architecture, MSVC, GCC, and Clang compilers still emit absolute addresses for string literals and callback functions.

This project fixes that by having no external library dependencies and no headers included so it could be possible to compile as an ELF Linux i386 architecture where string literals are accessed using relative addressing. 
The core logic is in:
- Always compile for ELF Linux platform even for Windows targets is specified. It is possible as project does not rely on any Windows-specific headers or libraries; only LLVM/Clang needed to produce position-independent code that written in C, a more high-level language than assembler.
- **primitives.h** defines primitives for Linux and Windows platforms and fixes the issues like **WCHAR is an unsigned short type in Windows but an int type in Linux.**

Moreover, it ensures that the output maintains its position-independent characteristics, allowing for more flexibility in deployment and execution.

---

## Quick Start (Step‑by‑step)

### 1. Set up the toolchain

1. Use a **Windows host** machine.
2. Install an **LLVM / Clang** toolchain (for example, the official Windows installer).
3. Make sure the following tools are available on your `PATH`:
   - `clang` and `lld` (able to target the desired triples),
   - `llvm-objcopy`,
   - `llvm-objdump`,
   - `llvm-strings`.
4. Clone or download this repository, then open a **Developer Command Prompt** or PowerShell in the repo root.

### 2. Build all targets

From the repository root, run:

```bat
build.bat
```

The script will:

- clean the `bin` directory,
- build several **Windows‑style** payloads into `bin\windows\`,
- build several **Linux** payloads into `bin\linux\`,
- for each target, produce:
  - an `.elf` file (full linked binary),
  - a `.bin` file (raw `.text` segment – the PIC blob),
  - a `_disasm.txt` file (disassembly of the code segment),
  - a `_sections.txt` file (section layout),
  - a `_strings.txt` file (dump of printable strings).

At the end of each toolchain run, the script prints the size of the extracted `.bin` blob.

### 3. Explore the artifacts

After a successful build you should see something like:

```text
bin/
  windows/
    windows_i386.elf / .bin / ...
    windows_amd64.elf / .bin / ...
    windows_arm32.elf / .bin / ...
    windows_arm64.elf / .bin / ...
  linux/
    linux_i386.elf / .bin / ...
    linux_amd64.elf / .bin / ...
    linux_arm64.elf / .bin / ...
```

A few ideas for exploration:

- Open the `_disasm.txt` files in your editor and follow the control flow from `main`.
- Compare the Windows and Linux variants for the same architecture.
- Compare 32‑bit vs 64‑bit codegen, especially how they access data and call functions.
- Embed one of the `.bin` blobs into a tiny loader and execute it at a non‑default address to verify that it behaves like PIC.

> 💡 Keep all experiments inside disposable VMs or containers.
> That makes it easier to tear down and rebuild environments and reduces the risk of accidentally violating any policies.

---

## Supported Targets

- **Windows‑style payloads** (code expects Windows user‑mode structures such as the PEB)
  - 32‑bit x86 (`windows_i386`, triple `i386-unknown-linux-gnu`)
  - 64‑bit x86‑64 (`windows_amd64`, triple `x86_64-unknown-linux-gnu`)
  - 32‑bit ARMv7 (`windows_arm32`, triple `thumbv7a-none-linux-gnueabi`)
  - 64‑bit ARM64 (`windows_arm64`, triple `aarch64-unknown-linux-gnu`)
- **Linux payloads**
  - 32‑bit x86 (`linux_i386`, triple `i386-unknown-linux-gnu`)
  - 64‑bit x86‑64 (`linux_amd64`, triple `x86_64-unknown-linux-gnu`)
  - 64‑bit ARM64 (`linux_arm64`, triple `aarch64-unknown-linux-gnu`)

For the “Windows” variants, the OS‑level ABI is controlled entirely by the `PLATFORM_WINDOWS*` macros in the source, not by the target triple.
The triple is just a convenient way to ask Clang for the right CPU and instruction set and to get an ELF container we can inspect.

---

## How It Works

### Windows

On Windows classic PEB walking techniques are used to locate `Kernel32.dll` and resolve the address of `WriteConsoleA` at runtime, avoiding any static imports.

- `peb.h` and `peb.h` define the necessary PEB and module structures for each architecture and accessing the PEB.
- `pe.h` and `pe.c` implement PE header parsing and export directory walking to find function addresses by name.
- `console.windows.c` implements `WriteConsole` by calling the resolved `WriteConsoleA` function pointer.
- `environment.windows.c` implements `ExitProcess` by resolving and calling the `ExitProcess` API similarly.

### Linux

On Linux syscalls are used directly to perform console output and process exit, avoiding any libc dependencies.

- `console.linux.c` implements `WriteConsole` using inline assembly to invoke the `write` syscall on `stdout`
  (`rax = 1` on x86‑64, appropriate registers on other architectures).
- `environment.linux.c` implements `ExitProcess` with the correct `exit` syscall for each architecture.
- `main.c` is shared between platforms: it computes the string length, calls `WriteConsole`, then `ExitProcess`, and contains error checking for syscall results.

Because the Linux path does not rely on any external libraries or global data, the resulting code is naturally position‑independent once the
`.text`+`.rodata` segment has been relocated as a whole.

---

## Project Layout

```text
src/
  main.c               # Shared entrypoint that prints "Hello world!"
  primitives.h         # Minimal type & integer / pointer definitions
  string.{c,h}         # Small helpers: length + case-insensitive comparisons
  environment.h        # Common macros + platform-specific exit glue
  environment.linux.c  # Linux ExitProcess implementation (raw syscalls)
  console.h            # Platform-neutral WriteConsole declaration
  console.windows.c    # Windows implementation, uses manual API resolution
  console.linux.c      # Linux implementation, uses raw syscalls
  peb.{c,h}            # PEB structures + module enumeration (Windows only)
  pe.{c,h}             # PE header / export directory parsing helpers
linker.script          # lld linker script: pack .text / .rodata into one segment
build.bat              # Top-level build script for all targets
toolchain.bat          # Per-target build & extraction pipeline
```

---

## Research & Ethics

This repository is intended for:

- learning how binaries, loaders, and calling conventions work;
- experimenting with compiler output and custom link layouts;
- building intuition for what “position‑independent code” actually means.

It is **not** intended for:

- bypassing security controls,
- running untrusted third‑party payloads,
- or any use that would violate laws, school rules, workplace policies, or service terms.

Always:

- work in isolated test environments (VMs, lab machines, or containers);
- only experiment on systems you own or have explicit permission to use;
- follow local laws and responsible security‑research practices.

---

## Reference

