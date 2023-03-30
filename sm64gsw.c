/*
BSD 3-Clause License

Copyright (c) 2023, Alex Free
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define VER "2.0"

unsigned int rom_patch_offset;
unsigned int code;
unsigned int type;
unsigned int read_codes;
unsigned int codes_written = 0;
unsigned int min_support = 0x246000;
unsigned int max_support = 0x333000;
unsigned int behavior_bank_type_min = 0x0EB180;
unsigned int behavior_bank_type_max = 0x0F083C;
unsigned int add_address = 0x12EC80;
unsigned int sub_address = 0x245000;
unsigned int lines_read = 0;
unsigned int rom_id;
unsigned int change1, change2;

unsigned short noop1 = 0x066C;
unsigned short noop2 = 0x0678;

bool file_input = 0;
bool kb_input = 0;
bool end_input = 0;
bool valid_input = 0;
bool parse_died = 0;
bool cic_fail = 0;
bool sixteen_bit = 0;
bool eight_bit = 0;
bool cicrom = 0;

FILE * rom;
FILE * txt;

char yn_response[2], option[2], gs_txt[256], filename[256];

void usage()
{
	printf("Usage:\nsm64gsw <rom.z64> <file.txt>	Write codes written in a .txt file into a ROM file\nsm64gsw <rom.z64>	Type codes to write into a ROM file\nsm64gsw -m <rom.z64>	Modify the CIC checksum in a ROM file\nsm64gsw -i <rom.z64>	Get info on a ROM file\nsm64gsw -a	Use sm64gsw in argless mode/menu mode\n");
	end_input = 1;
}

void parse()
{
	if(code < min_support || code > max_support)
	{
		if(file_input)
		{
			printf("WARNING: The code on line number %d is not in the supported range, sm64gsw can not write it to ROM. This code may not work.\n", lines_read);
		}
		else
		{
			printf("WARNING: This code is not in the supported range, sm64gsw can not write it to ROM. This code may not work.\n");
		}
		
		parse_died = 1;
	}
	
	if(!parse_died)
		printf("%X%X %X%X\n", type, code, change1, change2);

	if(type == 0x81 || type == 0xA1 || type == 0xF1 || type == 0x2A)
	{
		sixteen_bit = 1;
		printf("81 code type\n");
		rom_patch_offset = code-sub_address;
		cic_fail = 1;
	}	
	else if(type == 0x80 || type == 0xA0 || type == 0xF0 || type == 0x50)
	{
		eight_bit = 1;
		
		if(code >= behavior_bank_type_min && code <= behavior_bank_type_max) 
		{
		printf("Behavior bank code type\n");
		rom_patch_offset = code + add_address;	
		} 
		else
		{
		printf("80 code type\n");
		rom_patch_offset = code-sub_address;
		cic_fail = 1;
		}
	}
	else if(type == 0x88 || type == 0xD0 || type == 0xD2 || type == 0x89 || type == 0xD1 || type == 0xD3 || type == 0xEE || type == 0xDD || type == 0xCC || type == 0xDE || type == 0xFF || type == 0x3C || type == 0x50)
	{
		printf("The %X code type is not supported\n", type);
		parse_died = 1;
	}
	else
	{
		printf("Unknown code type: %X\n", type);
		parse_died = 1;
		end_input = 1;
	}

	if(!parse_died)
	{
		printf("Offset: %X\n", rom_patch_offset);
		fseek(rom, rom_patch_offset, SEEK_SET);
	
		if(sixteen_bit)
		{
			printf("Writing %02X...\n", change1);
			fwrite(&change1, 1, 1, rom);
		}
		
		if(eight_bit || sixteen_bit)
		{
			printf("Writing %02X...\n", change2);
			fwrite(&change2, 1, 1, rom);
			sixteen_bit = 0;
			eight_bit = 0;
			codes_written++;
		}

		if(kb_input && !end_input)
		{
			printf("Enter another code?\n");
			while(!valid_input)
			{
				printf("(y/n): ");
				fgets(yn_response, 2, stdin);
				scanf("%*[^\n]");
				scanf("%*c");
					if(strcmp(yn_response,"y") == 0) 
					{
						valid_input = 1;
					}
					else if(strcmp(yn_response,"n") == 0) 	
					{
						end_input = 1;
						valid_input = 1;				
					}
					else
					{
						printf("Invalid input, try again:");
					}
			}		
			valid_input = 0;
		}
	}
}


int main(int argc, char *argv[])
{
	printf("SuperMario64GameSharkWriter v%s\n(c)2023 3-BSD Alex Free\n", VER);

	if(argc == 2 || argc == 3)
	{
		rom = fopen(argv[1], "rb+");
		
		if(rom == NULL)
		{
			printf("Loading the ROM file: %s failed!\n", argv[1]);
			return(1);
		}
		
		fseek(rom, 0x3B, SEEK_SET);
		fread(&rom_id, 1, 4, rom);
		//printf("Got ROM ID: %08X\n", rom_id);

		if(rom_id == 0x4A4D534E)
		{
			printf("Validated ROM as .z64 Japan\n");
		} else if(rom_id == 0x454D534E) {
			printf("Validated ROMf as .z64 USA\n");
		} else if(rom_id == 0x504D534E) {
			printf("Validated ROM as .z64 PAL\n");
		} else {
			printf("Error, this is not a valid SM64 .z64 ROM file\n");
			fclose(rom);
			return(1);
		}
	} else {
		printf("Usage:\nsm64gsw <rom file> <txt file>	Write codes written in a .txt file into a ROM file\nsm64gsw <rom file>	Type codes to write into a ROM file\n");
		return(1);
	}

	if(argc == 3) 
	{
		txt = fopen(argv[2], "r");

		if(txt == NULL)
		{
			printf("Error: Loading the txt file: %s failed!\n", argv[2]);
			return(1);
		} else {
			printf("Writing codes in the txt file: %s to the ROM file: %s\n", argv[2], argv[1]);
			file_input = 1;
		}
	} else if(argc == 2) {
		kb_input = 1;
	}

	while(!end_input)
	{
		sixteen_bit = 0;
		eight_bit = 0;

		if(kb_input)
		{
			printf("Code:");
			scanf("%2X%6X %2X%2X", &type, &code, &change1, &change2);
			scanf("%*[^\n]");
			scanf("%*c");
			parse();
		}
		else if(file_input)
		{
			while(1)
			{
				read_codes = fscanf(txt, "%2X%6X %2X%2X", &type, &code, &change1, &change2);	
				if(read_codes != EOF && read_codes != 0 && !parse_died)
				{
					lines_read++;
					parse();
				}
				else
				{
					printf("File read complete\n");
					end_input = 1;
					break;
				}
			}			
		}	
	}

	if(cic_fail)
	{
		fseek(rom, noop1, SEEK_SET);
		fwrite("0", 1, 1, rom);
		fseek(rom, noop2, SEEK_SET);
		fwrite("0", 1, 1, rom);
		printf("CRC check has been modified.\n");
	}

	if(rom != NULL) 
		fclose(rom);
	
	if(txt != NULL)
		fclose(txt);
	
	if(codes_written != 0)
	{
		if(codes_written == 1)
		{
			printf("Wrote %d code\n", codes_written);
		}
		else if(codes_written > 1)
		{
			printf("Wrote %d codes\n", codes_written);
		}
	}

	return(0);
}
