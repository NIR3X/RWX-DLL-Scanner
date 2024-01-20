# RWX DLL Scanner - Find signed RWX dlls

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

## Finding DLL Files

To generate the dlls.txt file, you can use the [DLL Finder](https://github.com/NIR3X/DLL-Finder) utility, which scans all available drives and extracts the list of DLL files.

```bash
dll_finder.exe
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

## License
[![GNU AGPLv3 Image](https://www.gnu.org/graphics/agplv3-155x51.png)](https://www.gnu.org/licenses/agpl-3.0.html)  

This program is Free Software: You can use, study share and improve it at your
will. Specifically you can redistribute and/or modify it under the terms of the
[GNU Affero General Public License](https://www.gnu.org/licenses/agpl-3.0.html) as
published by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
