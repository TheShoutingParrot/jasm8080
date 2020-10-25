#include "jasm8080.h"

char *instructionsStr[totalInstructionPlaceholders] = { 
	/* stack, i/o and machine control */
	"NOP", "HLT", "POP",
	"PUSH", "OUT", "IN",
	"DI", "EI", "SPHL",

	/* branch and program flow */
	"JMP", 
	"RET", 
	"CALL", 
	"JNZ", "JZ", 
	"JNC", "JC", 
	"JPO", "JPE",
	"JP", "JM", 
	"RNZ", "RZ", 
	"RNC", "RC",
	"RPO", "RPE",
	"RP", "RM",
	"CNZ", "CZ",
	"CNC", "CC",
	"CPO", "CPE",
	"CP", "CM",
	"PCHL", "RST",

	/* data transfer */
	"LXI", "STAX", "MVI", 
	"LDAX", "SHLD", "LHLD",
	"STA", "LDA", "MOV",
	"XTHL", "XCHG", 

	/* arithmetic */
	"INX", "INR", "DCR",
	"DCX", "ADD", "ADC",
	"SUB", "SBB", "ADI",
	"ACI", "SUI", "SBI",

	/* logical */
	"RLC", "RAL", "RRC",
	"RAR", "DAA", "CMA",
	"STC", "CMC", "ANA",
	"XRA", "ORA", "CMP",
	"ANI", "XRI", "ORI",
	"CPI",
};

char *pseudoInstructionsStr[totalPseudoInstructionPlaceholders] = 
	{ "ORG", "DB", "DW", "DS", "EQU" };

int main(int argv, char *argc[]) {
	FILE *sourceFile,
	     *objectFile;

	int i;
	char *sourceFilename, *objectFilename;

	sourceFilename = calloc(100, sizeof(char));
	objectFilename = calloc(100, sizeof(char));

	*sourceFilename = '\0';
	*objectFilename = '\0';

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
	if(*objectFilename == '\0')
		die(__FILE__, __LINE__, "no object filename given");

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

	word = 0;

	if(number != NULL)
		*number = true;

	if(tolower(*(str + strlen(str) - 1)) == 'h') {
		uint16_t temp;

		for(i = 0; i < strlen(str) - 1; i++) {
			if(!isxdigit(*(str+i))) {
				if(number != NULL)
					*number = false;
				return 0;
			}
			
			if(isdigit(*str+i))
				temp = *(str + i) - '0';
			else
				temp = 10 + (tolower(*(str + i)) - 'a');

			word = word * 0x10 + temp;
		}
	}

	else {
		for(i = 0; i < strlen(str); i++) {
			if(!isdigit(*(str+i))) {
				if(number != NULL)
					*number = false;
				return 0;
			}

			word = word * 10 + (*(str + i) - '0');
		}
	}

	return word;
}
