#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define ALPHALEN 26

char * createKeys(char * key_fp)
{
	int position_a = 'a';

	FILE * f = fopen(key_fp, "r");
	if (f == NULL)
	{
		fprintf(stderr,"There was a problem opening the key file");
		exit(EXIT_FAILURE);
	}

	char * key_table = malloc(ALPHALEN);
	char * key_table_rev = malloc(ALPHALEN);
	int key_index;

	//populate key table with values
	for(key_index = 0; key_index < ALPHALEN; key_index++)
	{
		key_table[key_index] = -1; //neg means no key available for char
		key_table_rev[key_index] = -1;
	}

	int c;
	int keyFound = 0; //flag for when we find our key per sentence
	key_index = 0;

	//scroll through each character, populating our key_table
	while((c = tolower(fgetc(f))) != EOF) //only consider lower case in our key_table
	{
		if(key_index > ALPHALEN) //if we filled our table, exit the loop
		{
			break;
		}
		if(keyFound == 1) // look for end of sentence after key is found
		{
			if(c == '.' || c == '?' || c == '!')
				keyFound = 0;
		}

		//assigning key_table values
		else if (keyFound == 0 &&
			isalpha(c) &&
			key_table[key_index] == -1 &&
			key_table_rev[c - position_a] == -1) // conditions for finding a key
		{
			key_table[key_index] = c - position_a;
			key_table_rev[c - position_a] = key_index;
			key_index++;
			keyFound = 1;
		}
	}
	//print out key table
	FILE * fp = fopen("key_used.txt", "w");
	for(key_index = 0; key_index < ALPHALEN; key_index++)
	{
		if(key_table[key_index] == -1)
		{
			fprintf(stderr, "ERROR: Not enough keys available\n");
			exit(EXIT_FAILURE);
		}

		fprintf(fp, "%c%s%c\n",
			key_index + position_a,
			" = ",
			key_table[key_index] + position_a);
	}
	fclose(fp);
	return key_table;
}

void encrypt(char * input_fp, int encode_check, char * key_table)
{
	int position_a = 'a';
	int key_index;

	FILE * f = fopen(input_fp, "r");
	if (f == NULL)
	{
		fprintf(stderr, "There was a problem opening the input file\n");
		exit(EXIT_FAILURE);
	}

	//create decrypt table
	char * key_table_rev = malloc(ALPHALEN);
	for(key_index = 0; key_index < ALPHALEN; key_index++)
	{
		key_table_rev[(int) key_table[key_index]] = key_index;
	}

	int c;
	int encrypted_char;
	while((c = fgetc(f)) != EOF)
	{
		if(isalpha(c))
		{
			key_index = tolower(c) - position_a;
			if(encode_check == 1)
				encrypted_char = position_a + key_table[key_index];
			else
				encrypted_char = position_a + key_table_rev[key_index];

			isupper(c) ? printf("%c", toupper(encrypted_char)) : printf("%c", encrypted_char);
		}
		else
		{
			printf("%c", c);
		}
	}
	printf("\n");
}

int main(int argc, char *args[])
{
	if (argc < 2 || argc > 4)
	{
		fprintf(stderr, "INPUT ERROR: incorrect number of args\n");
		return EXIT_FAILURE;
	}

	char * key_fp = args[1];
	char * input_fp = args[2];
	int encode_check = 0;
	if (argc == 4)
	{
		encode_check = 1;
	}

	encrypt(input_fp, encode_check, createKeys(key_fp));

	return EXIT_SUCCESS;
}
