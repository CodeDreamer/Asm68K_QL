# Asm68K_QL

Cross-assembler for the **Sinclair QL**.

## History
The 68000 assembler code comes from the [Easy68k project](http://www.easy68k.com/) version that was re-packaged as a [command line tool](https://github.com/rayarachelian/EASy68K-asm).  
The syntax has been altered to be compatible with Sinclair QL assemblers like Metacomco and GST and the relocatable nature of standard QL code.

## Platform
The code runs on Windows. A Visual Studio project is provided to build the code.  
The original makefile for Linux is unused, but it should make it easy to recompile for Linux (if you'd like to do that, also check some of the early commits and re-enable conditionally some of the old code as needed).

## Command line syntax
`Asm68K.exe code.asm`

This will create a QL 68000 binary output `code.bin` containing a QL executable. The file has a header understood by emulators that contains the size of the executable data space.  
It will also create a listing file `code.L68` where you can check for errors and warnings, and the `code.S68` output in the S-record format originally used by Easy68K and Motorola tools.  
For additional command line options, see main() in assemble.cpp.

## Changes
Changes compared to the original Easy68K-derived code:
- Different syntax for PC-relative code, as used by many Sinclair QL assemblers. For example, use `label` instead of `label(PC)`.  
  The assembler's expression evaluator has been extensively modified to track what expressions are relative vs. absolute, and issue errors when an incorrect type of expression is used.  
  **Warning: this change is not complete**: This may not affect you, but I think the case of constant labels used as operands but without the .W or .L suffix is not handled. Supporting it would involve moving constant expression computation from the second to the first pass of the assembler. To avoid issues, always use #, .W or .L with constant values.
- Output binary file in QL format in addition or instead of the S-record output.
- Added NOP directive to align addresses (same syntax as the Metacomco assembler for the QL).
- Added SIZE directive to specify data space for the QL executable.
- Code is now relocatable by default (use `org` to switch to absolute addressing).

## Poor-man linking
The assembler should be able to generate object files for later linking with Easy68K, but I have not tested that.  
Since the assembler is very fast, I just create a main asm file where I include all other files using the `include` directive.
