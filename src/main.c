#include "jaz80.h"

char *instructionsStr[totalInstructionPlaceholders] = { 
	/* stack, i/o and machine control */
	"NOP", "HALT", "POP",
	"PUSH", "OUT", "IN",
	"DI", "EI",

	/* branch and program flow */
	"JP", "RET", "CALL", 
	"DJNZ", "JR",
	"RST",

	/* data transfer */
	"LD", "EX", "EXX",

	/* arithmetic */
	"INC", 
	"DEC", 
	"ADD", "ADC",
	"SUB", "SBC", 

	/* logical */
	"RLCA", "RLA", "RRCA",
	"RRA", "DAA", "CPL",
	"SCF", "CCF",
	"AND", "XOR", "OR",
	"CP",

	/* Extended Instructions */

	/* stack, i/o and machine control */
	"IM", "LDI", "LDIR", 
	"INI", "INIR",
	"OUTI", "OTIR",
	"IND", "INDR",
	"OUTD", "OTDR",
	"RETN", "RETI",
	"RRD", "RLD",

	/* data transfer */
	"LDD", "LDDR",

	/* arithmetic */
	"CPI", "CPIR",
	"CPD", "CPDR",

	/* logical */
	"NEG",

	/* Bit instructions (CB) */
	"RLC", "RRC",
	"RL", "RR",
	"SLA", "SRA", "SRL",

	"BIT", "RES", "SET",
};

char *pseudoInstructionsStr[totalPseudoInstructionPlaceholders] = 
	{ "ORG", "DB", "DW", "DS", "EQU" };

char *conditionsStr[totalConditions] = {
	"NZ", "Z",
	"NC", "C",
	"PO", "PE",
	"P", "M"
};

size_t sizeOfSymbolList,
       allocatedSymbolList,
       sizeOfLabelList,
       allocatedLabelList;

int main(int argv, char *argc[]) {
	FILE *sourceFile,
	     *objectFile;

	int i;
	char *sourceFilename, *objectFilename;

	sourceFilename = calloc(100, sizeof(char));
	objectFilename = calloc(100, sizeof(char));

	*sourceFilename = '\0';
	

	/* parse throught arguments */
	for(i = 1; i < argv; i++) {
		if(*argc[i] == '-') {
			if(!strcmp(argc[i], "-o")) {
				strcpy(objectFilename, argc[++i]);
			}
		}

		else {
			strcpy(sourceFilename, argc[i]);
		}
	}

	if(*sourceFilename == '\0')
		die(__FILE__, __LINE__, "no source filename given");
	if(*objectFilename == '\0') {
		for(i = 0; i < strcspn(sourceFilename, "."); i++) {
			*(objectFilename+i) = *(sourceFilename+i);
		}

		*(objectFilename+i) = '\0';

		strcat(objectFilename, ".com");

		if(access(objectFilename, F_OK) != -1) {
			if(!prompt("Do you wish to overwrite the file \"%s\"?", objectFilename))
				return EXIT_SUCCESS;
		}

		info(__FILE__, __LINE__, "assembling source to file \"%s\"", objectFilename);
	}

	sourceFile = fopen(sourceFilename, "r");
	objectFile = fopen(objectFilename, "wb");

	struct symbolList *symbolList;
	struct labelList *labelList;

	symbolList = NULL;
	labelList = NULL;

	sizeOfSymbolList = 0;
	allocatedSymbolList = 0;

	sizeOfLabelList = 0;
	allocatedLabelList = 0;

	/* parse throught the assembly source code and convert to a list of symbols */
	firstPass(sourceFile, &symbolList, &labelList);

	/* delete the list of labels */
	deleteLabelList(labelList);

	/* convert list of symbols to object code */
	secondPass(objectFile, symbolList);

	/* close files and exit */
	fclose(sourceFile);
	fclose(objectFile);

	info(__FILE__, __LINE__, ANSI_COLOR_GREEN "\"%s\" has been successfully assembled to object code file \"%s\"" ANSI_COLOR_RESET, sourceFilename, objectFilename);

	return EXIT_SUCCESS;
}

uint16_t convertStrToWord(char *str, bool *number) {
	uint16_t word;
	uint8_t i;
	bool neg;

	neg = false;
	word = 0;

	if(number != NULL)
		*number = true;

	if(tolower(*(str + strnlen(str, 100) - 1)) == 'h') {
		uint16_t temp;

		i = 0;
		if(*str == '-') {
			i = 1;
			neg = true;
		}

		for(; i < strnlen(str, 100) - 1; i++) {
			if(!isxdigit(*(str+i))) {
				if(number != NULL)
					*number = false;
				return 0;
			}
			
			if(isdigit(*(str+i)))
				temp = *(str + i) - '0';
			else {
				temp = 10 + (tolower(*(str + i)) - 'a');
			}

			word = word * 0x10 + temp;
		}

		if(neg)
			word = -word;
	}

	else {
		i = 0;
		if(*str == '-') {
			i = 1;
			neg = true;
		}
		
		for(; i < strnlen(str, 100); i++) {
			if(!isdigit(*(str+i))) {
				if(number != NULL)
					*number = false;
				return 0;
			}

			word = word * 10 + (*(str + i) - '0');
		}

		if(neg)
			word = -word;
	}

	return word;
}
