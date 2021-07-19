/*
BSD 3-Clause License

Copyright (c) 2021, Alex Free
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

#define VER "1.4"
#define C_YEAR "2021"

#if defined(__B_ENDIAN__) || (macintosh) || (__BYTE_ORDER__ == BIG_ENDIAN) // big endian detection
#define B_ENDIAN 1
#else
#define B_ENDIAN 0
#endif


int rom_patch_offset, sub_address, add_address, behavior_bank_type_min, behavior_bank_type_max, code, type, read_codes, noop1, noop2, codes_written, min_support, max_support, lines_read;

// For getting ROM file info
unsigned char rom_byte_1, rom_byte_2, rom_byte_3, rom_byte_4;
unsigned int change1, change2, bechange;

typedef enum { FALSE, TRUE } bool;

bool cic_fail, eight_bit, sixteen_bit, end_input, kb_input, file_input, valid_input, parse_died, option_select, cicrom, rominfo, txtwrom, kbwrom, got_file_name;

FILE * rom;
FILE * txt;

char yn_response[2], option[2], gs_txt[256], filename[256];

void usage()
{
printf("Usage:\nsm64gsw <rom.z64> <file.txt>	Write codes written in a .txt file into a ROM file\nsm64gsw <rom.z64>	Type codes to write into a ROM file\nsm64gsw -m <rom.z64>	Modify the CIC checksum in a ROM file\nsm64gsw -i <rom.z64>	Get info on a ROM file\nsm64gsw -a	Use sm64gsw in argless mode/menu mode\n");
end_input = TRUE;
}

unsigned int big_endian_safe_uint(unsigned int in)
{
return bechange = ((in>>24)&0xff) | ((in<<8)&0xff0000) | ((in>>8)&0xff00) | ((in<<24)&0xff000000);
}

void load_file_name(int open_mode)
{
	got_file_name = FALSE;

	while(!got_file_name)
	{
		if(open_mode == 1)
		{
			printf("Enter the ROM file name:");
			fgets(filename, 256, stdin);
			filename[strcspn(filename, "\n")] = 0;
			rom = fopen(filename, "r+b");
			if(rom == NULL)
			{
				printf("Loading the ROM file: %s failed!\n", filename);
			}
			else
			{
				got_file_name = TRUE;
			}
		}	
		else if(open_mode == 2)
		{
			printf("Enter the TXT file name:");
			fgets(filename, 256, stdin);
			filename[strcspn(filename, "\n")] = 0;
			txt = fopen(filename, "r");
			if(txt == NULL)
			{
				printf("Loading the TXT file: %s failed!\n", filename);
			}
			else
			{
				got_file_name = TRUE;
			}
			
		}
	}
	got_file_name = FALSE;
}

void argless_mode()
{
	printf("Select mode:\n1 - Write codes in a .txt file to a ROM file\n2 - Write codes using the keyboard to a ROM file\n3 - Get info on a ROM file\n4 - Manually patch checksum in a ROM file\nOption Number:");
	option_select = FALSE;

	while(!option_select)
	{
	fgets(option, 2, stdin);
	scanf("%*[^\n]");
	scanf("%*c");
		if(strcmp(option,"1") == 0) 
		{
		option_select = TRUE;
		txtwrom = TRUE;
		}
		else if(strcmp(option,"2") == 0) 
		{
		option_select = TRUE;
		kbwrom = TRUE;
		}
		else if(strcmp(option,"3") == 0) 
		{
		option_select = TRUE;
		rominfo = TRUE;
		}
		else if(strcmp(option,"4") == 0)			
		{
		option_select = TRUE;
		cicrom = TRUE;
		}
		else			
		{
		printf("Invalid input, try again:");
		}
	}
	option_select = FALSE;
}

void check_rom()
{
// Read byte by byte so endian-ness is irrelevant
	fseek(rom, 0, SEEK_SET);
// 59th byte starts region ID
	fseek(rom, 59, SEEK_SET);
	fread(&rom_byte_1, 1, 1, rom);
	fread(&rom_byte_2, 1, 1, rom);
	fread(&rom_byte_3, 1, 1, rom);
	fread(&rom_byte_4, 1, 1, rom);
	
	if(rom_byte_1 == 0x4E && rom_byte_2 == 0x53 && rom_byte_3 == 0x4D && rom_byte_4 == 0x4A) //NSMJ in hex, both OG japan and shindou ROMs contain this
	{
	fseek(rom, 0, SEEK_SET);
// 35th byte starts ER M in OG japan and ERMA in shindou, how convenient. This is because shindou says the game name is SUPERMARIO64, all other versions say SUPER MARIO 64
	fseek(rom, 35, SEEK_SET);
	fread(&rom_byte_1, 1, 1, rom);
	fread(&rom_byte_2, 1, 1, rom);
	fread(&rom_byte_3, 1, 1, rom);
	fread(&rom_byte_4, 1, 1, rom);
		if(rom_byte_1 == 0x45 && rom_byte_2 == 0x52 && rom_byte_3 == 0x20 && rom_byte_4 == 0x4D) //ER M in hex
		{
		printf("Validated ROM as .z64 Japan\n");
		}
		else if(rom_byte_1 == 0x45 && rom_byte_2 == 0x52 && rom_byte_3 == 0x4D && rom_byte_4 == 0x41) //ERMA in hex
		{
		printf("Validated ROM as .z64 Shindou Edition\n");
		}
		else
		{
// Should be impossible
		printf("Error: Can not figure out if this is the original Japan or the Shindou Edition ROM!\n");
		end_input = TRUE;
		}
	}
	else if(rom_byte_1 == 0x4E && rom_byte_2 == 0x53 && rom_byte_3 == 0x4D && rom_byte_4 == 0x45) //NSME in hex
	{
	printf("Validated ROM as .z64 USA\n");
	}
	else if(rom_byte_1 == 0x4E && rom_byte_2 == 0x53 && rom_byte_3 == 0x4D && rom_byte_4 == 0x50) //NSMP in hex
	{
	printf("Validated ROM as .z64 PAL\n");
	}
	else
	{
	fseek(rom, 0, SEEK_SET);
	fread(&rom_byte_1, 1, 1, rom);
	fread(&rom_byte_2, 1, 1, rom);
	fread(&rom_byte_3, 1, 1, rom);
	fread(&rom_byte_4, 1, 1, rom);
		if(rom_byte_1 == 0x37 && rom_byte_2 == 0x80 && rom_byte_3 == 0x40 && rom_byte_4 == 0x12) // First 4 bytes of a little endian sm64 rom which is NOT supported
		{
		printf("Error: This is a little-endian, .n64 format SM64 ROM file. SM64GSW requires a big-endian, .z64 format SM64 ROM file.\n");
		end_input = TRUE;
		}
		else
		{
		printf("Error: This does not appear to be a valid SM64 ROM!\n");
		end_input = TRUE;
		}
	}
}

void noop()
{
	noop1 = 0x066C;
	noop2 = 0x0678;
	fseek(rom, noop1, SEEK_SET);
	fwrite("0", 1, 1, rom);
	fseek(rom, noop2, SEEK_SET);
	fwrite("0", 1, 1, rom);
	printf("CRC check has been modified.\n");
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
		
		parse_died = TRUE;
	}
	
	if(!parse_died)
	printf("%X%X %X%X\n", type, code, change1, change2);

	if(type == 0x81 || type == 0xA1 || type == 0xF1 || type == 0x2A)
	{
	sixteen_bit = TRUE;
	printf("81 code type\n");
	rom_patch_offset = code-sub_address;
	cic_fail = TRUE;
	}	
	else if(type == 0x80 || type == 0xA0 || type == 0xF0 || type == 0x50)
	{
	eight_bit = TRUE;
		
		if(code >= behavior_bank_type_min && code <= behavior_bank_type_max) 
		{
		printf("Behavior bank code type\n");
		rom_patch_offset = code + add_address;	
		} 
		else
		{
		printf("80 code type\n");
		rom_patch_offset = code-sub_address;
		cic_fail = TRUE;
		}
	}
	else if(type == 0x88 || type == 0xD0 || type == 0xD2 || type == 0x89 || type == 0xD1 || type == 0xD3 || type == 0xEE || type == 0xDD || type == 0xCC || type == 0xDE || type == 0xFF || type == 0x3C || type == 0x50)
	{
	printf("The %X code type is not supported\n", type);
	parse_died = TRUE;
	}
	else
	{
	printf("Unknown code type: %X\n", type);
	parse_died = TRUE;
	end_input = TRUE;
	}

	if(!parse_died)
	{
	printf("Offset: %X\n", rom_patch_offset);
	fseek(rom, rom_patch_offset, SEEK_SET);
	
		if(sixteen_bit)
		{
		printf("Writing %2X...\n", change1);
			#if defined(B_ENDIAN)
				big_endian_safe_uint(change1);
				fwrite(&bechange, 1, 1, rom);
			#else
				fwrite(&change1, 1, 1, rom);
			#endif
		}
		
		if(eight_bit || sixteen_bit)
		{
		printf("Writing %2X...\n", change2);
			#if defined(B_ENDIAN)
				big_endian_safe_uint(change2);
				fwrite(&bechange, 1, 1, rom);
			#else
				fwrite(&change2, 1, 1, rom);
			#endif
		sixteen_bit = FALSE;
		eight_bit = FALSE;
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
						valid_input = TRUE;
					}
					else if(strcmp(yn_response,"n") == 0) 	
					{
						end_input = TRUE;
						valid_input = TRUE;				
					}
					else
					{
						printf("Invalid input, try again:");
					}
			}		
			valid_input = FALSE;
		}
	}
}

#if defined(macintosh) // mac os 9 only uses argless mode
int main ()
#else
int main(int argc, char *argv[])
#endif
{
min_support = 0x246000;
max_support = 0x333000;
behavior_bank_type_min = 0x0EB180;
behavior_bank_type_max = 0x0F083C;
add_address = 0x12EC80;
sub_address = 0x245000;
codes_written = 0;
lines_read = 0;

got_file_name = FALSE;
file_input = FALSE;
kb_input = FALSE;
end_input = FALSE;
valid_input = FALSE;
parse_died = FALSE;
cic_fail = FALSE;
sixteen_bit = FALSE;
eight_bit = FALSE;
cicrom = FALSE;
rominfo = FALSE;
txtwrom = FALSE;
kbwrom = FALSE;

printf("SuperMario64GameSharkWriter version %s\nCopyright (c) %s, Alex Free\n", VER, C_YEAR);

#if !defined(macintosh)
	if(argc == 3) 
	{
		if(strcmp(argv[1],"-m")==0) 
		{
		rom = fopen(argv[2], "r+b");
			if(rom == NULL)
			{
			printf("Loading the ROM file: %s failed!\n", argv[1]);
			return(1);
			}
		check_rom();
		noop();
		end_input = TRUE;
		}
		else if(strcmp(argv[1],"-i")==0)
		{
		rom = fopen(argv[2], "r+b");
			if(rom == NULL)
			{
			printf("Loading the ROM file: %s failed!\n", argv[1]);
			return(1);
			}
		check_rom();
		end_input = TRUE;
		}
		else
		{	
		rom = fopen(argv[1], "r+b");
		txt = fopen(argv[2], "r");
			if(rom == NULL)
			{
			printf("Loading the ROM file: %s failed!\n", argv[1]);
			return(1);
			}
			else
			{
			check_rom();
			}
			
			if(txt == NULL)
			{
			printf("Error: Loading the txt file: %s failed!\n", argv[2]);
			return(1);
			}
			else
			{
			printf("Writing codes in the txt file: %s to the ROM file: %s\n", argv[2], argv[1]);
			file_input = TRUE;
			}
		}
	}
	else if(argc == 2)
	{
		if(strcmp(argv[1],"-a")==0)
		{
			argless_mode();
		}
		else
		{
			rom = fopen(argv[1], "r+b");
			if(rom == NULL)
			{
				printf("Error: Loading the ROM file: %s failed!\n", argv[1]);
				return(1);
			}
			else
			{
				check_rom();
			}
		kb_input = TRUE;
		}
	}
	else
	{
		usage();
	}
#else
	argless_mode();
#endif

	if(kbwrom)
	{
		load_file_name(1);
		if(rom == NULL)
		{
			printf("Loading the ROM file failed!\n");
			return(1);
		}
		check_rom();
		kb_input = TRUE;
	}
	
	if(txtwrom)
	{
		load_file_name(1);
		load_file_name(2);

		if(rom == NULL)
		{
			printf("Loading the ROM file failed!\n");
			return(1);
		}
		
		if(txt == NULL)
		{
			printf("Loading the TXT file failed!\n");
			return(1);
		}
		
		check_rom();
		file_input = TRUE;
	}
	
	if(rominfo)
	{
		load_file_name(1);
		if(rom == NULL)
		{
			printf("Loading the ROM file failed!\n");
			return(1);
		}
		check_rom();
		end_input = TRUE;
	}

	if(cicrom)
	{
		load_file_name(1);		
		if(rom == NULL)
		{
			printf("Loading the ROM file failed!\n");
			return(1);
		}
		check_rom();
		noop();
		end_input = TRUE;
	}
		

	while(!end_input)
	{
	sixteen_bit = FALSE;
	eight_bit = FALSE;

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
				read_codes = fscanf(txt, "%2X%6X %2X%2X[^\n]", &type, &code, &change1, &change2);	
				if(read_codes != EOF && !parse_died)
				{
					lines_read++;
					parse();
				}
				else
				{
					printf("File read complete\n");
					end_input = TRUE;
					break;
				}
			}			
		}	
	}

	if(cic_fail)
	noop();
	
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
