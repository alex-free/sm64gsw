SM64GSW by Alex Free

==Info==
An open source (3-BSD) console program that can write GameShark/Xploder64 codes directly into a
Super Mario 64 USA ROM.

SM64GSW performs Messiaen64's manual hex editing method written at https://sites.google.com/site/messiaen64/patching-gameshark-codes in software automatically so you don't 
have to hex edit your SM64 ROM manually.

==Requirements==
- The Windows version is a 32-bit binary compatible with Windows 95 and above.
- Windows 95 requires MSVCR.dll (Windows 95 OSR2.5 and above already include it). One way you can get this is by updating Internet Explorer to version 4.0.
- A Super Mario 64 USA ROM in .z64 or .N64 (N64 SDK) format. The .n64 and .v64 formats are not supported.

==Usage==

1) Start SM64GSW in one of 2 different ways.
 
Command line method:
- Open cmd.exe or command.com.
- Execute the 'sm64gsw' command with your ROM as the argument. For example, the complete command could be 
"sm64gsw C:\sm64.z64".

Windows Explorer method:
- Drag your ROM file on top of the sm64gsw.exe file.

2) Select your code input preference at the input selection menu.

SM64GSW can write codes contained in a txt file of your choosing, or you can input them with the keyboard.
In keyboard mode after each code is entered, it will be written to the ROM and you will be asked if you want to enter another code or to exit.
In file mode, you will be prompted to enter the file path (that must not contain spaces!) for a txt file containing codes. The txt file 
can contain a code or multiple codes, one code per line. The file 'gs.txt' in this release is an example of this.
You can manually type in the file path, drag and drop the rom into the SM64GSW.exe window, or copy - paste the file path if in cmd.exe.

If SM64GSW tells you 'Attention: At least one code invalidates the internal ROM checksum, run checksum64 
on your ROM before playing' you MUST run the 'chksum64' program on the ROM file after writing your codes. You
 can do this by dropping your newly modified ROM on top of the chksum64 executable (Chksum64 is written by Andreas Sterbenz).

3) Test the ROM.

==What Codes Work?==

There is an incomplete list of some codes that work in the file named 'known_working_codes.txt' in this release.

Supported RAM range is 80246000-80333000. The 3rd-8th digits of a code must be a hexadecimal value in the 
range of 246000-333000 to work.

Not all N64 GameShark/Xploder64 code types are supported. This is due to certain code types not translating 
into just hex writes for various reasons, such as codes that:
- Require physical hardware (like a GS button).
- Perform live checks and or dynamic modification.
- Simply not being appliable (Expansion pack releated, non-Super Mario 64 security chip related).

Code types are defined by the first 2 hexadecimal digits of a code. For example, in the level select code 
'A032D58C 0001' 'A0' is the code type. For multi-line codes, if any lines contain an incompatible code type 
the entire code can not be written into ROM. See the table below for specifics.

Supported codes include 80, A0, F0, 81, A1, and 2A. 80, A0, and F0 must be in the X0XXXXXX-00XX format.

==License==
- This is open source software, released under the 3-Clause BSD license (read license.txt). It **should** compile
and work on other little-endian Operating Systems.
 
==Changelog==
==Version 1.0.1==
- Fixes a bug which prevented codes from being entered by the keyboard, after an invalid code was entered.