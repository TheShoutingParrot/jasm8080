#include "jasm8080.h"

uint8_t recognizeInstruction(char *str) {
	uint8_t i;

	for(i = 0; i < totalInstructionPlaceholders; i++) {
		if(!strcasecmp(str, instructionsStr[i])) {
			return i;
		}
	}

	return totalInstructionPlaceholders;
}

uint8_t	recognizePseudoInstruction(char *str) {
	uint8_t i;

	for(i = 0; i < totalPseudoInstructionPlaceholders; i++) {
		if(!strcasecmp(str, pseudoInstructionsStr[i])) {
			return i;
		}
	}

	return totalPseudoInstructionPlaceholders;

}

uint8_t recognizeRegister(char *str) {
	if(!(strlen(str) <= 3))
		return rTotal;

	switch(toupper(*str)) {
		case 'B':
			return rB;
		case 'C':
			return rC;
		case 'D':
			return rD;
		case 'E':
			return rE;
		case 'H':
			return rH;
		case 'L':
			return rL;
		case 'A':
			return rA;
		case 'M':
			return rM;
		case 'S':
			if(toupper(*(str+1)) == 'P')
				return rSP;
			return rTotal;
		case 'P':
			if(toupper(*(str+1)) == 'S' && toupper(*(str+2)) == 'W')
				return rPSW;
			return rTotal;
		default:
			return rTotal;
	}
}

void convertTokenToSymbolListEntry(
		uint16_t *location, 
		uint8_t *expectedType, 
		uint8_t *nextExpectedType,
		bool *hasLabels,
		char *token,
		char *prevToken,
		struct symbolList **listHead,
		struct labelList **labelListHead) {

	union symbol symbol;

	int		temp;
	uint8_t 	tempByte;
	uint16_t	tempWord;
	bool		tempBool;
	char		*tempStr;

	switch(*expectedType) {
		case typeUnknown:
			switch(recognizePseudoInstruction(token)) {
				case Org:
					*expectedType = typeOrgAddress;

					*nextExpectedType = typeUnknown;

					return;

				case DefineByte:
					*expectedType = typeDefinedByte;

					*nextExpectedType = typeUnknown;

					return;

				case DefineWord:
					*expectedType = typeDefinedWord;

					*nextExpectedType = typeUnknown;

					return;

				case DefineSpace:
					*expectedType = typeDefinedSpace;

					*nextExpectedType = typeUnknown;

					return;

				default:
					break;
			}

		case typeInstruction:
			tempByte = recognizeInstruction(token);

			if(tempByte == totalInstructionPlaceholders) {
				*expectedType = typeEqu;
			}

			else {
				symbol.instruction = tempByte;
			
				addToSymbolList(*(location), typeInstruction, symbol, listHead);

				switch(tempByte) {
					case NoOperation:
					case DisableInterrupts:
					case EnableInterrupts:
					case LoadSpHl:
					case Return:
					case ExchangeHLandStack:
					case ExchangeHLandDE:
					case Halt:
					case RotateALeft:
					case RotateALeftCarry:
					case DecimalAdjust:
					case SetCarry:
					case RotateARight:
					case RotateARightCarry:
					case ComplementA:
					case ComplementCarry:
					case ReturnIfNotZero:
					case ReturnIfZero:
					case ReturnIfNotCarry:
					case ReturnIfCarry:
					case ReturnIfOdd:
					case ReturnIfEven:
					case ReturnIfPlus:
					case ReturnIfMinus:
					case PcFromHl:
						*expectedType = typeUnknown;

						*nextExpectedType = typeUnknown;

						(*location)++;

						break;

					case LoadImmediate:
						*expectedType = typeRegister;

						*nextExpectedType = typeImmediateWord;

						break;

					case Pop:
					case Push:
					case StoreX:
					case LoadX:
					case IncrementRegisterPair:
					case Increment:
					case Decrement:
					case DecrementRegisterPair:
					case Add:
					case AddWithCarry:
					case Sub:
					case SubWithBorrow:
					case AndA:
					case XorA:
					case OrA:
					case Compare:
						*expectedType = typeRegister;

						*nextExpectedType = typeUnknown;

						break;

					case MoveImmediate:
						*expectedType = typeRegister;

						*nextExpectedType = typeImmediateByte;

						break;

					case Move:
						*expectedType = typeRegister;

						*nextExpectedType = typeRegister;

						break;

					case StoreHL:
					case LoadHL:
					case StoreA:
					case LoadA:
					case Jump:
					case Call:
					case JumpIfNotZero:
					case JumpIfZero:
					case JumpIfNotCarry:
					case JumpIfCarry:
					case JumpIfOdd:
					case JumpIfEven:
					case JumpIfPlus:
					case JumpIfMinus:
					case CallIfNotZero:
					case CallIfZero:
					case CallIfNotCarry:
					case CallIfCarry:
					case CallIfOdd:
					case CallIfEven:
					case CallIfPlus:
					case CallIfMinus:
						*expectedType = typeAddress;

						*nextExpectedType = typeUnknown;
				
						(*location)++;

						break;

					case Out:
					case In:
					case AddImmediate:
					case AddImmediateWithCarry:
					case SubImmediate:
					case SubImmediateWithBorrow:
					case AndImmediate:
					case XorImmediate:
					case OrImmediate:
					case CompareImmediate:	
					case RestartSubroutine: 
						*expectedType = typeImmediateByte;

						(*location)++;

						break;

					default:

						break;
				}
			}

			break;

		case typeRegister:
			tempByte = recognizeRegister(token);

			if(tempByte == rTotal) {
				die(__FILE__, __LINE__, "expected register not \"%s\"", token);
			}

			else {
				symbol.registerSymbol = tempByte;

				addToSymbolList(*location, typeRegister, symbol, listHead);

				if(*nextExpectedType != typeRegister)
					(*location)++;

				*expectedType = typeUnknown;
			}

			break;

		case typeImmediateByte:
			tempByte = (uint8_t)convertStrToWord(token, &tempBool);

			if(!tempBool) {
				if((temp = getLabel(*labelListHead, token)) == -1) {
					tempStr = (char *)malloc(256);
					strcpy(tempStr, token);

					symbol.label = tempStr;
			
					addToSymbolList((*location)+=2, typeLabel, symbol, listHead);
				
					*expectedType = typeUnknown;

					*hasLabels = true;

					return;
				}

				else
					tempByte = (uint8_t)temp;
			}

			symbol.immediateByte = tempByte;

			addToSymbolList((*location)++, typeImmediateByte, symbol, listHead);

			*expectedType = typeUnknown;

			break;

		case typeImmediateWord:
			tempWord = convertStrToWord(token, &tempBool);

			if(!tempBool) {
				if((temp = getLabel(*labelListHead, token)) == -1) {
					tempStr = (char *)malloc(256);
					strcpy(tempStr, token);

					symbol.label = tempStr;
			
					addToSymbolList((*location)+=2, typeLabel, symbol, listHead);
				
					*expectedType = typeUnknown;

					*hasLabels = true;

					return;
				}

				else
					tempWord = (uint16_t)temp;
			}

			symbol.immediateWord = tempWord;

			addToSymbolList((*location), typeImmediateWord, symbol, listHead);

			*location += 2;

			*expectedType = typeUnknown;

			break;

		case typeAddress:
			tempWord = convertStrToWord(token, &tempBool);

			if(!tempBool) {
				if((temp = getLabel(*labelListHead, token)) == -1) {
					tempStr = (char *)malloc(256);
					strcpy(tempStr, token);

					symbol.label = tempStr;
			
					addToSymbolList((*location)+=2, typeLabel, symbol, listHead);
				
					*expectedType = typeUnknown;

					*hasLabels = true;

					return;
				}

				else
					tempWord = (uint16_t)temp;
			}

			symbol.address = tempWord;

			addToSymbolList((*location), typeAddress, symbol, listHead);

			*location += 2;

			*expectedType = typeUnknown;

			break;

		case typeOrgAddress:
			*location = convertStrToWord(token, NULL);

			*expectedType = typeUnknown;

			*nextExpectedType = typeUnknown;

			break;

		case typeDefinedByte:
			if((*token == '\'' && *(token+strlen(token)-1) == '\'') || (*token == '\"' && *(token+strlen(token)-1) == '\"')) {
				for(tempWord = 1; tempWord < strlen(token) - 1; tempWord++) {
					symbol.immediateByte = *(token+tempWord);

					addToSymbolList((*location)++, typeDefinedByte, symbol, listHead);
				}
			}

			else {
				symbol.immediateByte = convertStrToWord(token, NULL);

				addToSymbolList((*location)++, typeDefinedByte, symbol, listHead);

				*expectedType = typeDefinedByte;

				*nextExpectedType = typeUnknown;
			}

			break;

		case typeDefinedWord:
			symbol.immediateWord = convertStrToWord(token, NULL);

			addToSymbolList((*location)++, typeDefinedWord, symbol, listHead);

			*expectedType = typeDefinedWord;

			*nextExpectedType = typeUnknown;

			break;

		case typeDefinedSpace:
			symbol.immediateWord = convertStrToWord(token, NULL);

			addToSymbolList(*location, typeDefinedSpace, symbol, listHead);
			*location += symbol.immediateWord;

			*expectedType = typeUnknown;

			break;


		case typeEqu:
			if(!strcasecmp(token, "equ")) {
				addToLabelList(0, prevToken, labelListHead);

				*expectedType = typeEquValue;

				*nextExpectedType = typeUnknown;
			}
			else {
				if(!strcasecmp(token, "db")) {
					addToLabelList(*location, prevToken, labelListHead);

					*expectedType = typeDefinedByte;
				}

				else if(!strcasecmp(token, "dw")) {
					addToLabelList(*location, prevToken, labelListHead);

					*expectedType = typeDefinedWord;
				}

				else if(!strcasecmp(token, "ds")) {
					addToLabelList(*location, prevToken, labelListHead);

					*expectedType = typeDefinedSpace;
				}

				else
					die(__FILE__, __LINE__, "expected instruction or pseudoinstruction not \"%s\"", token);
			}

			break;

		case typeEquValue:
			addConstantToListEntry(convertStrToWord(token, NULL), *labelListHead + sizeOfLabelList - 1);
			break;

		default:
			exit(1);
	}
}
void firstPass(FILE *sourceFile, struct symbolList **listHead, struct labelList **labelListHead) {
	char ch,
	     *line, *token, 
	     *tempStr, *tempStr2,
	     *prevToken;
	char separators[] = " ,\t";
	size_t i;
	uint8_t expectedNextType1, expectedNextType2;
	uint16_t location;
	bool hasLabels;

	hasLabels = false;

	location = 0;

	info(__FILE__, __LINE__, "starting firstpass...");

	for(;;) {
		expectedNextType1 = typeUnknown;
		expectedNextType2 = typeUnknown;

		line = (char *)malloc(512);

		i = 0;
		while((ch = fgetc(sourceFile)) != '\n') {
			if(ch == ';') {
				while(fgetc(sourceFile) != '\n')
					if(ch == EOF)
						goto END_OF_LOOP1;
				break;
			}

			*(line+i++) = ch;

			if(ch == EOF)
				goto END_OF_LOOP1;
		}

		*(line+i) = '\0';

		tempStr2 = NULL;

		token = strtok(line, separators);

		while(token != NULL) {
			if(*(token + strlen(token) - 1) == ':') {
				char *labelStr;
				labelStr = (char *)malloc(strlen(token));

				strcpy(labelStr, token);
				*(labelStr + strlen(labelStr) - 1) = '\0';

				addToLabelList(location, labelStr, labelListHead);

				free(labelStr);
			}

			else {
				convertTokenToSymbolListEntry(&location, 
						&expectedNextType1, &expectedNextType2, 
						&hasLabels,
						token, 
						prevToken,
						listHead, labelListHead);
			}

			if(expectedNextType1 == typeUnknown && expectedNextType2 != typeUnknown) {
				expectedNextType1 = expectedNextType2;
				expectedNextType2 = typeUnknown;
			}

			prevToken = token;
			
			tempStr = (line + (token - line + strlen(token) + 1));
			tempStr += strspn(tempStr, separators);

			if(*(tempStr) == '\0')
				break;
			else if(*tempStr == '\"') {
				token = tempStr;
				tempStr += strcspn(tempStr+1, "\"") + 2;
				*tempStr = '\0';

				tempStr2 = tempStr + 1;
			}
			else if(tempStr2 != NULL) {
				token = strtok(tempStr2, separators);

				tempStr2 = NULL;
			}
			else {
				token = strtok(NULL, separators);
			}
		}

		free(line);

		if(expectedNextType1 < typeUnknown)
			die(__FILE__, __LINE__, "uncompleted statement");
	}

END_OF_LOOP1:

	if(hasLabels) {
		int temp;

		for(i = 0; i < sizeOfSymbolList; i++) {
			if((*listHead + i)->type == typeLabel) {
				if((temp = getLabel(*labelListHead, (*listHead + i)->symbol.label)) != -1) {
					(*listHead + i)->type = typeAddress;
					(*listHead + i)->symbol.address = (uint16_t)temp;
				}

				else
					die(__FILE__, __LINE__, "label not found");
			}
		}
	}

	info(__FILE__, __LINE__, "firstpass was successful...");
}
