# C Position-Independent Code (PIC)

Cross‑platform C demo that compiles a small program into **position‑independent code** and shows how to:

- resolve Windows APIs manually using the PEB + PE exports,
- use raw Linux syscalls without libc,
- extract a compact “shellcode‑like” code blob from a normal executable.

> ⚠️ This project is for **low‑level systems / compiler education and research only**.  
> Do **not** use it to break laws, violate school rules, workplace policies, or service terms.

[GitHub Project](https://github.com/mrzaxaryan/c-pic)

---

## Table of Contents

- [Overview](#overview)
- [Quick Start (Step‑by‑step)](#quick-start-step-by-step)
  - [1. Set up the toolchain](#1-set-up-the-toolchain)
  - [2. Build all targets](#2-build-all-targets)
  - [3. Explore the artifacts](#3-explore-the-artifacts)
- [Features](#features)
- [Supported Targets](#supported-targets)
- [How It Works](#how-it-works)
  - [Windows Path](#windows-path)
  - [Linux Path](#linux-path)
- [Project Layout](#project-layout)
- [Strings & Data in PIC](#strings--data-in-pic)
- [Research & Ethics](#research--ethics)
- [Reference](#reference)

## Overview

The sample program itself just prints `"Hello world!"` and exits cleanly,
but the techniques are general and can be extended to more complex payloads.

At a high level, this project demonstrates how to:

- obtain the Process Environment Block (PEB) manually on Windows;
- walk the loader data structures to find the base address of `Kernel32.dll`;
- parse PE export tables to resolve functions such as `WriteConsoleA`;
- perform direct Linux syscalls (`write`, `exit`) without libc;
- cross‑compile the same C source for multiple OS / architecture targets;
- extract a compact, contiguous shellcode‑like blob from the resulting binary.

Writing *truly* position‑independent code in plain C comes with a few constraints:

- you can't rely on global or static variables that need relocations;
- you have to avoid hidden references to external libraries and CRT startup code;
- you must be careful with string literals and constant data, which compilers like to scatter into separate sections.

This repository takes a pragmatic approach:

- all logic lives in a tiny set of `.c` / `.h` files with no CRT, no `<windows.h>`, and no libc;
- a custom linker script keeps `.text` and `.rodata` together in one loadable segment;
- a small build pipeline extracts just that segment into raw bytes, plus some helper metadata.

Think of it as a minimal, readable “PIC lab” that you can disassemble, modify, and extend for your own experiments.

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

## Features

- **No CRT / libc** – only minimal integer / pointer / string primitives.
- **No Windows headers** – required NT / PE structures are re‑declared locally.
- **Manual API resolution** – functions obtained via PEB + PE export parsing.
- **Position‑independent output** – safe to copy the code blob to an arbitrary address (as long as you move the whole segment).
- **Cross‑OS, cross‑arch** – Windows & Linux; x86, x86‑64, and ARM64.
- **Single `.text` segment** – linker script keeps code & read‑only data together.
- **Shellcode‑oriented artifacts** – build emits a raw byte blob plus basic metadata.

---

## Supported Targets

The build scripts currently target:

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

You can tweak or extend `build.bat` / `toolchain.bat` to add or remove targets as needed.

---

## How It Works

### Windows Path

On Windows builds (`PLATFORM_WINDOWS`):

1. **Locate the PEB.**  
   `GetCurrentPEB` in `peb.c` uses the appropriate register for each architecture
   (`FS` / `GS` / `r9` / `x18`) to obtain a pointer to the current process’s PEB.

2. **Find Kernel32.**  
   `GetModuleHandleFromPEB` walks the doubly‑linked list in `PEB_LDR_DATA->InMemoryOrderModuleList`
   and looks for a module whose `BaseDllName` matches `"kernel32.dll"` (case‑insensitive).

3. **Resolve exports.**  
   `GetExportAddress` in `pe.c` parses the PE headers starting from the module base:
   - validates the DOS header (`MZ`) and NT header (`PE\0\0`),
   - locates the export directory in the data directory,
   - walks the `AddressOfNames` / `AddressOfNameOrdinals` / `AddressOfFunctions` arrays,
   - and returns the VA of a function whose name matches the requested string.

4. **Call `WriteConsoleA`.**  
   `console.windows.c` defines a `WriteConsole` wrapper that:
   - pulls a PEB pointer with `GetCurrentPEB`,
   - resolves `Kernel32.dll`,
   - uses `GetExportAddress` to find `WriteConsoleA`,
   - and finally calls it with the process’s standard output handle.

5. **Exit cleanly.**  
   On “Windows” builds, `ExitProcess(code)` is implemented as a macro that simply `return`s from `main`.
   In a real process this function would normally be called through the real `ExitProcess` API;
   here we care about the code blob itself, not the surrounding host process.

### Linux Path

On Linux builds (`PLATFORM_LINUX`):

- `console.linux.c` implements `WriteConsole` using inline assembly to invoke the `write` syscall on `stdout`
  (`rax = 1` on x86‑64, appropriate registers on other architectures).
- `environment.linux.c` implements `ExitProcess` with the correct `exit` syscall for each architecture.
- `main.c` is shared between platforms: it just computes the string length, calls `WriteConsole`, then `ExitProcess`.

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

## Strings & Data in PIC

Position‑independent **code** is usually straightforward; the tricky part is **data**:

- Compilers like to place string literals and constants in `.rdata` / `.rodata`.
- If you later extract only a single section (for shellcode‑like use),
  any pointers into other sections become invalid.
- On x86‑64, RIP‑relative addressing hides much of this:
  as long as code and data live in the same contiguous region, relative references keep working.
- On 32‑bit x86 Windows, compilers often emit **absolute addresses**, so a blob that gets relocated freely will break unless you:
  - keep strings on the stack,
  - or write a tiny loader that fixes up absolute pointers before jumping to the payload.

This repository uses a pragmatic mix:

- a custom `linker.script` keeps `.text` and `.rodata` together in one loadable segment;
- the C code avoids surprising global data and keeps the example intentionally tiny;
- comments in the source highlight architecture‑specific pitfalls where you’d need extra work
  for more complex payloads (especially on 32‑bit x86).

If you extend the project with additional global data or lots of string literals,
keep these constraints in mind so the extracted blob stays truly position‑independent.

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

