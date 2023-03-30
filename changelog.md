# Changelog

## Version 1.4 - 7/18/2021

*   Classic Mac OS support.
*   SM64GSW now tells you what line an unsupported code is on when reading a .txt file of codes.
*   Added verification and official support of SM64 USA, Japan, PAL, and Shindou edition ROMs in .z64 (big-endian) format.
*   Implemented an arg-less mode. Used by default on classic macos, manually triggerable by `sm64gsw -a`.
*   Added a codes written counter, you can now see how many codes you wrote with SM64GSW automatically.
*   Rewrote input system.
*   Now works on any other not mentioned big endian platform that defines `__BYTE_ORDER__ == BIG_ENDIAN` or `BIG_ENDIAN`.

## Version 1.3 - 2/19/2021

*   Finally, Big Endian PowerPC Mac OS X is supported! Other Big Endian platforms are not yet however.

## Version 1.2 - 1/22/2021

*   Fixed a bug on opening a ROM file for keyboard entry.

## Version 1.1 - 1/13/2021

*   Proper Linux/Unix compatibility.
*   Fixed bug preventing the ROM from being written to on Windows 95.
*   Added a makefile supporting Linux and Windows targets.
*   CIC check skip method has been implemented.
*   New args system.
*   Supports all 8 bit write code types in the supported range.
*   Rewritten known\_working\_codes.txt file.

## Version 1.0.1 - 10/8/2020

*   Fixes a bug which prevented codes from being entered by the keyboard, after an invalid code was entered.
