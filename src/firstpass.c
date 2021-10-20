#include "jaz80.h"

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
	if(!(strnlen(str, 5) <= 4))
		return rTotal;

	if(strnlen(str, 2) == 1) {
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

			case 'I':
				return rI;
			case 'R':
				return rR;
		}

		return rTotal;
	}

	else if(!strcasecmp(str, "BC"))
		return rBC;
	else if(!strcasecmp(str, "DE"))
		return rDE;
	else if(!strcasecmp(str, "HL"))
		return rHL;

	else if(!strcasecmp(str, "SP"))
		return rSP;

	else if(!strcasecmp(str, "(C)"))
		return rC_mem;
	else if(!strcasecmp(str, "(BC)"))
		return rBC_mem;
	else if(!strcasecmp(str, "(DE)"))
		return rDE_mem;
	else if(!strcasecmp(str, "(SP)"))
		return rSP_mem;
	else if(!strcasecmp(str, "(HL)"))
		return rHL_mem;
	
	else if(!strcasecmp(str, "AF"))
		return rAF;
	else if(!strcasecmp(str, "AF'"))
		return rAF;

	return rTotal;
}

uint8_t recognizeCondition(char *str) {
	uint8_t i;


	if(!(strnlen(str, 3) <= 2))
		return totalConditions;

	for(i = 0; i < totalConditions; i++) {

		if(!strcasecmp(str, conditionsStr[i])) {
			return i;
		}
	}

	return totalConditions;
}

/* converts a token (for example "NOP") to a entry to the symbol list 
 * (in the case of "NOP" the list entry would be: TypeInstruction, NoOperation) */
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

			/* if it's something else than an instruction we assume that it's a label for a variable */
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
					case Halt:
					case RotateALeft:
					case RotateALeftCarry:
					case DecimalAdjust:
					case SetCarry:
					case RotateARight:
					case RotateARightCarry:
					case ComplementA:
					case ComplementCarry:
					case ExchangeShadows:
						*expectedType = typeUnknown;

						*nextExpectedType = typeUnknown;

						(*location)++;

						break;

					case Negate:
					case LoadI:
					case LoadIR:
					case CompareHL:
					case CompareHLR:
					case Ini:
					case IniR:
					case OutI:
					case OutIR:
					case LoadD:
					case LoadDR:
					case CompareD:
					case CompareDR:
					case InD:
					case InDR:
					case OutD:
					case OutDR:
					case ReturnN:
					case ReturnI:
					case Rrd:
					case Rld:
						*expectedType = typeUnknown;

						*nextExpectedType = typeUnknown;

						(*location) += 2;

						break;

					case InterruptMode:
						*expectedType = typeImmediateByte;

						*nextExpectedType = typeUnknown;

						(*location)++; /* Even though this is a ED instruction it still just adds a one to the instruction,
								* to save a bit of (my) time in needless programming, the immediate byte does the
								* second location incrementation for us. */

						break;

					case Pop:
					case Push:
					case Increment:
					case Decrement:
						*expectedType = typeRegister;

						*nextExpectedType = typeUnknown;

						break;

					case Sub:
						*expectedType = typeRegisterOrImmediateByte;

						*nextExpectedType = typeUnknown;

						break;



					case RestartSubroutine: 
						*expectedType = typeImmediateByte;

						/* We do NOT increment the location here because it will be incremented
						 * anyway when handling the next token (the immediate byte) */

						break;

					case DecrementJumpIfNotZero:
						*expectedType = typeImmediateByte;

						(*location)++;

						break;

					case Jump:
					case Call:
						*expectedType = typeCondition;

						*nextExpectedType = typeAddress;

						break;

					case Return:
						*expectedType = typeCondition;

						*nextExpectedType = typeUnknown;

						break;

					case AndA:
					case XorA:
					case OrA:
					case Compare:
						*expectedType = typeRegisterOrImmediateByte;

						*nextExpectedType = typeUnknown;

						break;

					case Exchange:
						*expectedType = typeRegister;

						*nextExpectedType = typeRegister;

						break;

					case Add:
					case AddWithCarry:
					case SubWithBorrow:
						*expectedType = typeRegister;

						*nextExpectedType = typeRegisterOrImmediateByte;

						break;

					case Out:
						*expectedType = typePort;

						*nextExpectedType = typeRegister;

						break;

					case In:
						*expectedType = typeRegister;

						*nextExpectedType = typePort;

						break;

					case Load:
						*expectedType = typeRegisterOrAddress;

						*nextExpectedType = typeRegisterOrImmediateVal;

						break;

					case JumpRegister:
						*expectedType = typeCondition;

						*nextExpectedType = typeImmediateByte;

						break;

					case RotateLeftC:
					case RotateRightC:
					case RotateLeft:
					case RotateRight:
					case ShiftLeftA:
					case ShiftRightA:
					case ShiftRightL:
						*expectedType = typeRegister;

						*nextExpectedType = typeUnknown;

						(*location)++;

						break;

					case TestBit:
					case ResetBit:
					case SetBit:
						*expectedType = typeImmediateByte;

						*nextExpectedType = typeRegister;

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

				if((((*listHead) + (sizeOfSymbolList - 3))->type == typeInstruction &&
						((*listHead) + (sizeOfSymbolList - 3))->symbol.instruction == Load &&
						((*listHead) + (sizeOfSymbolList - 2))->type == typeAddress &&
						(tempByte == rBC || tempByte == rDE || tempByte == rSP)) ||
						((((*listHead) + (sizeOfSymbolList - 3))->type == typeInstruction &&
						((*listHead) + (sizeOfSymbolList - 3))->symbol.instruction == Load &&
						((*listHead) + (sizeOfSymbolList - 2))->type == typeRegister &&
						(tempByte == rI || tempByte == rR))) ||
						(((*listHead) + (sizeOfSymbolList - 2))->type == typeInstruction &&
						(((*listHead) + (sizeOfSymbolList - 2))->symbol.instruction == SubWithBorrow 
						 || ((*listHead) + (sizeOfSymbolList - 2))->symbol.instruction == AddWithCarry) &&
						tempByte == rHL) ||
						((((*listHead) + (sizeOfSymbolList - 2))->type == typeInstruction) &&
						(((*listHead) + (sizeOfSymbolList - 2))->symbol.instruction == Load) && 
						(tempByte == rI || tempByte == rR))) {
					*location += 2;
				}

				else if((((*listHead) + (sizeOfSymbolList - 3))->type == typeInstruction &&
						(((*listHead) + (sizeOfSymbolList - 3))->symbol.instruction == SubWithBorrow
						 || ((*listHead) + (sizeOfSymbolList - 3))->symbol.instruction == AddWithCarry) &&
						((*listHead) + (sizeOfSymbolList - 2))->symbol.registerSymbol == rHL) ||
						(((*listHead) + (sizeOfSymbolList - 3))->type == typeInstruction &&
						(((*listHead) + (sizeOfSymbolList - 3))->symbol.instruction == Load) &&
						(((*listHead) + (sizeOfSymbolList - 2))->symbol.registerSymbol == rI ||
						 ((*listHead) + (sizeOfSymbolList - 2))->symbol.registerSymbol == rR)));

				else if(((*nextExpectedType != typeRegister) 
						&& (*nextExpectedType != typeRegisterOrImmediateVal) 
						&& (*nextExpectedType != typeRegisterOrImmediateByte)
						&& (*nextExpectedType != typeRegisterOrImmediateWord))) {
					(*location)++;
				}

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
			
					addToSymbolList((*location)+=2, typeWordLabel, symbol, listHead);
				
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
			
					addToSymbolList((*location), typeWordLabel, symbol, listHead);
					*location += 2;
				
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
			if((*token == '(') && (*(token + strlen(token) - 1) == ')')) {
				if(recognizeRegister(token) == rHL_mem) {
					symbol.registerSymbol = rHL_mem;

					/* We do this because it's actually a register that is in question, not an address */
					(*location)--; 

					addToSymbolList(*location, typeRegister, symbol, listHead);

					(*location)++; 
					
					*expectedType = typeUnknown;

					return;
				}

				else {
					if(((((*listHead) + (sizeOfSymbolList - 1))->type) == typeRegister) &&
							((((*listHead) + (sizeOfSymbolList - 1))->symbol.registerSymbol== rBC))) {
						(*location)++;
					}

					tempStr = (char *)malloc(256);
					strcpy(tempStr, token);
					strncpy(token, (tempStr+1), strlen(tempStr) - 2); /* dangerous? */
					*(token + strlen(tempStr) - 2) = '\0';

					free(tempStr);
					tempStr = NULL;
				}
			}


			tempWord = convertStrToWord(token, &tempBool);


			if(!tempBool) {

				if((temp = getLabel(*labelListHead, token)) == -1) {
					tempStr = (char *)malloc(256);
					strcpy(tempStr, token);

					symbol.label = tempStr;
			
					addToSymbolList(*location, typeAddressLabel, symbol, listHead);
					*location += 2;
			

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
			if((*token == '\'' && *(token+strnlen(token, 256)-1) == '\'') || (*token == '\"' && *(token+strnlen(token, 256)-1) == '\"')) {
				for(tempWord = 1; tempWord < strnlen(token, 256) - 1; tempWord++) {
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

		case typeCondition:

			tempByte = recognizeCondition(token);


			/* this means that it's an address meaning that this jump/call/jump register (jr) instruction 
			 * is without conditions */
			if(tempByte == totalConditions) { 
				/* if the next expected type is an address it means that 
				 * this is a conditionless jump or call so we just need to 
				 * convert the address to a symbol list entry */
				if(*nextExpectedType == typeAddress || *nextExpectedType == typeImmediateByte) {
					(*location)++;

					*expectedType = *nextExpectedType;
					*nextExpectedType = typeUnknown;


					/* this could be dangerous */
					convertTokenToSymbolListEntry(location, 
						expectedType, nextExpectedType, 
						hasLabels,
						token,
						prevToken,
						listHead, labelListHead);



					return;
				}

				/* this means that it was probably a return without conditions
				 * or something like that so we can just happily increment the
				 * location */
				else {

					(*location)++;

					*expectedType = typeUnknown;

					return;
				}
			}

			symbol.condition = tempByte;

			addToSymbolList(*location, typeCondition, symbol, listHead);


			(*location)++;

			*expectedType = typeUnknown;

			break;

		case typeRegisterOrImmediateByte:
			/* This is how we test that is it a register or a byte */
			tempByte = recognizeRegister(token); /* TODO: Make this better */


			/* In this case it's a byte */
			if(tempByte == rTotal) {
				(*location)++;

				*expectedType = typeImmediateByte;

				/* This could be dangerous */
				convertTokenToSymbolListEntry(location, 
					expectedType, nextExpectedType, 
					hasLabels,
					token,
					prevToken,
					listHead, labelListHead);
			}

			/* In this case it's a register */
			else {
				*expectedType = typeRegister;

				/* This could be dangerous */
				convertTokenToSymbolListEntry(location, 
					expectedType, nextExpectedType, 
					hasLabels,
					token,
					prevToken,
					listHead, labelListHead);
			}

			break;


		case typeRegisterOrImmediateVal:
			tempByte = recognizeRegister(token);

			/* In this case it's an address */
			if(tempByte == rTotal && *token == '(' && *(token + strlen(token) - 1) == ')') {
				(*location)++;

				*expectedType = typeAddress;

				if(*nextExpectedType != typeUnknown)
					*nextExpectedType = typeRegister;

				/* This could be dangerous */
				convertTokenToSymbolListEntry(location, 
					expectedType, nextExpectedType, 
					hasLabels,
					token,
					prevToken,
					listHead, labelListHead);
			}

			else if(tempByte == rTotal && 
					((*listHead + sizeOfSymbolList - 1))->type == typeRegister &&
					((*listHead + sizeOfSymbolList - 1))->symbol.registerSymbol >= rBC) { /* In this case we expect a word */
				(*location)++;

				*expectedType = typeImmediateWord;

				/* This could be dangerous */
				convertTokenToSymbolListEntry(location, 
					expectedType, nextExpectedType, 
					hasLabels,
					token,
					prevToken,
					listHead, labelListHead);

			}

			else if(tempByte == rTotal) { /* In this case we expect a byte */
				(*location)++;

				*expectedType = typeImmediateByte;

				/* This could be dangerous */
				convertTokenToSymbolListEntry(location, 
					expectedType, nextExpectedType, 
					hasLabels,
					token,
					prevToken,
					listHead, labelListHead);
			}


			else { /* we expect a register */
				*expectedType = typeRegister;

				/* This could be dangerous */
				convertTokenToSymbolListEntry(location, 
					expectedType, nextExpectedType, 
					hasLabels,
					token,
					prevToken,
					listHead, labelListHead);
			}

			break;

		case typeRegisterOrAddress:
			/* This is how we test that is it a register or a byte */
			tempByte = recognizeRegister(token); /* TODO: Make this better */

			/* In this case it's an address */
			if(tempByte == rTotal && *token == '(' && *(token + strlen(token) - 1) == ')') {
				*expectedType = typeAddress;

				if(*nextExpectedType != typeUnknown)
					*nextExpectedType = typeRegister;

				/* This could be dangerous */
				convertTokenToSymbolListEntry(location, 
					expectedType, nextExpectedType, 
					hasLabels,
					token,
					prevToken,
					listHead, labelListHead);
			}

			/* In this case it's a register */
			else {
				*expectedType = typeRegister;

				/* This could be dangerous */
				convertTokenToSymbolListEntry(location, 
					expectedType, nextExpectedType, 
					hasLabels,
					token,
					prevToken,
					listHead, labelListHead);
			}

			break;

		case typePort:
			/* In principle the port number should be surrounded by parentheses 
			 * but in this assembler it will work with them or without */
			if((*token == '(') && (*(token + strlen(token) - 1) == ')')) {
				if(recognizeRegister(token) == rC_mem) {
					symbol.registerSymbol = rC_mem;

					/* We do this because it's actually a register that is in question, not an address */
					addToSymbolList((*location)++, typeRegister, symbol, listHead);
					
					*expectedType = typeUnknown;

					return;
				}

				tempStr = (char *)malloc(256);
				strcpy(tempStr, token);
				strncpy(token, (tempStr+1), strlen(tempStr) - 2); /* dangerous? */
				*(token + strlen(tempStr) - 2) = '\0';

				free(tempStr);
				tempStr = NULL;
			}

			tempByte = (uint8_t)convertStrToWord(token, &tempBool);

			if(!tempBool) {
				if((temp = getLabel(*labelListHead, token)) == -1) {
					tempStr = (char *)malloc(256);
					strcpy(tempStr, token);

					symbol.label = tempStr;
			
					addToSymbolList((*location)++, typeWordLabel, symbol, listHead);
				
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

		line = (char *)malloc(1024);

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
			if(*(token + strnlen(token, 100) - 1) == ':') {
				char *labelStr;
				labelStr = (char *)malloc(strnlen(token, 100));

				strcpy(labelStr, token);
				*(labelStr + strnlen(labelStr, 100) - 1) = '\0';

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
			
			tempStr = (line + (token - line + strnlen(token, 100) + 1));
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

		if(expectedNextType1 < typeUnknown) {
			/* This situation (exception to the above if) is encountered when there's a 
			 * "lonely"/conditionless return. */
			if(expectedNextType1 == typeCondition && expectedNextType2 == typeUnknown)
				location++; /* this is probably dangerous */
			else {
				die(__FILE__, __LINE__, "uncompleted statement");
			}
		}
	}

END_OF_LOOP1:

	if(hasLabels) {
		int temp;

		for(i = 0; i < sizeOfSymbolList; i++) {
			if((*listHead + i)->type == typeAddressLabel || (*listHead + i)->type == typeWordLabel) {
				if((temp = getLabel(*labelListHead, (*listHead + i)->symbol.label)) != -1) {
					if((*listHead + i)->type == typeAddressLabel) {
						(*listHead + i)->type = typeAddress;
						(*listHead + i)->symbol.address = (uint16_t)temp;
					}

					else if((*listHead + i)->type == typeWordLabel) {
						(*listHead + i)->type = typeImmediateWord;
						(*listHead + i)->symbol.address = (uint16_t)temp;
					}
				}
				else
					die(__FILE__, __LINE__, "label \"%s\" not found",
							(*listHead + i)->symbol.label);
			}
		}
	}

	info(__FILE__, __LINE__, "firstpass was successful...");
}
