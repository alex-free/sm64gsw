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

#define VER "1.2"

int rom_patch_offset, sub_address, add_address, behavior_bank_type_min, behavior_bank_type_max, code, change1, change2, type, read_codes, min_support, max_support, noop1, noop2;

typedef enum { FALSE, TRUE } bool;

bool cic_fail, eight_bit, sixteen_bit, end_input, kb_input, file_input, valid_input, parse_died;

FILE * rom;
FILE * txt;

char yn_response[2];
char gs_txt[255];

void usage()
{
printf("Usage:\nsm64gsw -m <rom.z64>	Modify the CIC checksum in ROM\nsm64gsw <rom.z64> <file.txt>	Write codes in .txt File into ROM\nsm64gsw <rom.z64>	Type codes to write into ROM\n");
end_input = TRUE;
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
	printf("Code is not in the supported range, can not write to ROM\n");
	parse_died = TRUE;
	end_input = TRUE;
	}

	if(!parse_died)
	printf("%X\n%X\n%X%X\n", type, code, change1, change2);

	if(type == 0x81 || type == 0xA1 || type == 0xF1 || type == 0x2A)
	{
	sixteen_bit = TRUE;
	printf("81 code type\n");
	rom_patch_offset = code-sub_address;
	cic_fail = TRUE;
	}	
	else if(type == 0x80 || type == 0xA0 || type == 0xF0)
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
	printf("This code type is not supported\n");
	parse_died = TRUE;
	}
	else
	{
	printf("Unknown code type\n");
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
		fwrite(&change1, 1, 1, rom);
		printf("Writing %2X...\n", change2);
		fwrite(&change2, 1, 1, rom);
		}
		else if(eight_bit)
		{
		printf("Writing %2X...\n", change2);
		fwrite(&change2, 1, 1, rom);
		}		
		if(kb_input && !parse_died)
		{
		printf("Enter another code?\n");
			while(!valid_input)
			{
				printf("(y/n): ");
				scanf("%s[\n]", &yn_response);
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
					printf("Invalid input, try again\n");
					}
			}		
			valid_input = FALSE;			
		}
	}
}

int main(int argc, char *argv[])
{
min_support = 0x246000;
max_support = 0x333000;
behavior_bank_type_min = 0x0EB180;
behavior_bank_type_max = 0x0F083C;
add_address = 0x12EC80;
sub_address = 0x245000;
file_input = FALSE;
kb_input = FALSE;
end_input = FALSE;
valid_input = FALSE;
parse_died = FALSE;
cic_fail = FALSE;
	
printf("SuperMario64GameSharkWriter version %s\nCopyright (c) 2021, Alex Free\n", VER);

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
		noop();
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
			
			if(txt == NULL)
			{
			printf("Loading the txt file: %s failed!\n", argv[2]);
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
		rom = fopen(argv[1], "r+b");
		if(rom == NULL)
		{
		printf("Loading the ROM file: %s failed!\n", argv[1]);
		return(1);
		}
		else
		{
		printf("Loaded ROM file: %s for keyboard entry of codes\n", argv[1]);
		}
		kb_input = TRUE;
	}
	else
	{
		usage();
	}
	
	while(!end_input)
	{
	sixteen_bit = FALSE;
	eight_bit = FALSE;

	if(kb_input)
	{
	printf("Code:");
	scanf("%2X%6X %2X%2X[^\n]", &type, &code, &change1, &change2);
	parse();
	}
	else if(file_input)
	{
		while(1)
		{
		read_codes = fscanf(txt, "%2X%6X %2X%2X[^\n]", &type, &code, &change1, &change2);	
			if(read_codes != EOF && !parse_died)
			{
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
	
	if(parse_died)
	printf("Writing to ROM could not continue, aborted\n");	

return(0);
}
