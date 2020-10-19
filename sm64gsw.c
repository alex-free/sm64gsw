/*
BSD 3-Clause License

Copyright (c) 2020, Alex Free
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

int arg_length, rom_patch_offset, sub_address, add_address, behavior_bank_type_min, behavior_bank_type_max, code, change1, change2, type, read_codes, min_support, max_support;

typedef enum { FALSE, TRUE } bool;

bool cic_fail, eight_bit, sixteen_bit, end_input, kb_input, file_input, valid_input, parse_died;

FILE * rom;
FILE * txt;

char yn_response[2];
char gs_txt[255];

void parse()
{
	if(code < min_support || code > max_support)
	{
	printf("Code is not in the supported range, can not write to ROM\n");
	parse_died = TRUE;
	end_input = TRUE;
	}

	if(rom == NULL)
	{
	printf("Loading the ROM file failed\n");
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

		if(change1 != 0x00)
		{
		printf("Unsupported or invalid code type.\n");
		parse_died = TRUE;
		}
		
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
	fseek(rom, 0, SEEK_SET);
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
			fflush(stdin);
				if(fgets(yn_response, sizeof(yn_response), stdin) != NULL)
				{
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
					printf("Invalid input, enter y or n\n");
					}
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
	
printf("GameShark SM64 ROM Patcher version 1.0.1\nCopyright (c) 2020, Alex Free\n");

	if(argc == 2) 
	{
	printf("ROM file selected: %s\n", argv[1]);
	rom = fopen(argv[1], "rwb+");
	}
    else
    {
	printf("Usage:\nsm64gsw <romfile>\n");
	end_input = TRUE;
    }
	
	while(!end_input)
	{
	sixteen_bit = FALSE;
	eight_bit = FALSE;
	end_input = FALSE;

		while(!kb_input && !file_input) 
		{
		printf("\n**Code Input Selection Menu**\nf - Read code(s) in a txt file\nk - Manually type code(s)\nq - Quit\n(f/k/q): ");
		fflush(stdin);
			if(fgets(yn_response, sizeof(yn_response), stdin) != NULL)
			{
				if(strcmp(yn_response,"f") == 0) 
				{
				file_input = TRUE;
				}
				else if(strcmp(yn_response,"k") == 0) 	
				{
				kb_input = TRUE;		
				} 
				else if(strcmp(yn_response,"q") == 0) 	
				{
				return(0);		
				} 
				else
				{
				printf("Invalid input, enter f or k\n");
				}
			}
		}
	if(kb_input)
	{
	fflush(stdin);
	printf("Code:");
	scanf("%2X%6X %2X%2X", &type, &code, &change1, &change2);
	parse();
	}
	else if(file_input)
	{
	fflush(stdin);
	printf("Enter the filepath of the TXT file containing codes: ");
		scanf("%255s", gs_txt);
		txt = fopen(gs_txt, "r");
		if(txt == NULL)
		{
		printf("Could not open the file: %s, please note that there can not be any spaces in the filepath\n", gs_txt);
		parse_died = TRUE;
		end_input = TRUE;
		}
		else
		{
			while(1)
			{
			read_codes = fscanf(txt, "%2X%6X %2X%2X", &type, &code, &change1, &change2);	
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
}
	if(rom != NULL) 
	fclose(rom);
	
	if(txt != NULL)
	fclose(txt);
	
	if(cic_fail)
	printf("At least one code invalidates the internal ROM checksum, run chksum64 on your ROM before playing\n");
	
	if(parse_died)
	printf("Writing to ROM could not continue, aborted\n");	
	
printf("Press any key to continue\n");
fflush(stdin);
getchar();
return(0);
}
