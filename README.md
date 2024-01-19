# RWX-DLL-Scanner - Find signed RWX dlls

## Overview

RWX DLL Scanner is a utility written in C++ that scans a list of DLL files to identify signed sections with read, write, and execute permissions (RWX). The utility checks for the presence of a security directory in the optional header to detect digitally signed sections.

## Features

- Scans DLL files for RWX sections.
- Detects digitally signed sections using the security directory.
- Provides information about the identified sections.
- Distinguishes between 32-bit and 64-bit DLLs.

## Usage

1. Create a text file named `dlls.txt` containing the list of DLL filenames to be scanned.
2. Run the executable, and it will process each DLL file in the list.

```bash
rwx_dll_scanner.exe
```

## Building

Make sure you have a C++ compiler and the necessary dependencies installed. Use the following commands to build the executable:

```bash
g++ -std=c++2a -Wall -O2 -march=x86-64 -fno-rtti rwx_dll_scanner.cpp -o rwx_dll_scanner.exe -s -static
```

## Example

Consider the following example output:

```bash
section0: ".text"
32-bit dll: "example.dll"
```

In this example, the utility found a RWX section named ".text" in the "example.dll" file.

## Notes:

* The utility skips files that cannot be opened or have invalid DOS or NT headers.
* Minimum section size for consideration is set to 0x64000.