#include "jaz80.h"

void addToSymbolList(uint16_t location, uint8_t type, union symbol symbol, struct symbolList **listHead) {
        if(allocatedSymbolList < (sizeof(**listHead) * (++sizeOfSymbolList))) {
                allocatedSymbolList += (sizeof(**listHead) * 100);
                *listHead = realloc(*listHead, allocatedSymbolList);
        }

        (*listHead + sizeOfSymbolList - 1)->location    = location;
        (*listHead + sizeOfSymbolList - 1)->type        = type;
        (*listHead + sizeOfSymbolList - 1)->symbol      = symbol;
}

void deleteSymbolList(struct symbolList *listHead) {
        free(listHead);

        sizeOfSymbolList = 0;
}

#ifdef DEBUG
void printSymbolList(struct symbolList *listHead) {
	size_t i;

        puts("printing symbol list....");

        for(i = 0; i < sizeOfSymbolList; i++) {
                printf("symbol list %p: %X ", (listHead+i), (listHead+i)->location);
                switch((listHead+i)->type) {
                        case typeInstruction:
                               printf("instruction %d\t%s\n", (listHead+i)->symbol.instruction,
					       instructionsStr[(listHead+i)->symbol.instruction]);
                               break;

                        case typeRegister:
                               printf("register %d\n", (listHead+i)->symbol.registerSymbol);
                               break;

                        case typeImmediateByte:
                               printf("byte %X\n", (listHead+i)->symbol.immediateByte);
                               break;

                        case typeImmediateWord:
                               printf("word %X\n", (listHead+i)->symbol.immediateWord);
                               break;

                        case typeAddress:
                               printf("address %p\n", (listHead+i)->symbol.address);
                               break;

                        case typeAddressLabel:
                               printf("address label \"%s\"\n", (listHead+i)->symbol.label);
                               break;

                        case typeWordLabel:
                               printf("word label \"%s\"\n", (listHead+i)->symbol.label);
                               break;

                        case typeDefinedByte:
                               printf("db %X %c\n", (listHead+i)->symbol.immediateByte, (listHead+i)->symbol.immediateByte);
                               break;

			case typeDefinedWord:
                               printf("dw %X\n", (listHead+i)->symbol.immediateWord);
                               break;

			case typeDefinedSpace:
                               printf("ds %X\n", (listHead+i)->symbol.immediateWord);
			       break;

			case typeCondition:
			       printf("condition %d\t%s\n", (listHead+i)->symbol.condition,
					       conditionsStr[(listHead+i)->symbol.condition]);
			       break;

                        default:
                               puts("unknown");
                               break;

                }
        }
}

void printLabelList(struct labelList *listHead) {
        size_t i;

	puts("printing label list...");

        for(i = 0; i < sizeOfLabelList; i++) {
                printf("labels list %zu %p: %X %p %s\n", i, (listHead+i), (listHead+i)->location, (listHead+i)->label, (listHead+i)->label);
        }
}

#endif /* #ifdef DEBUG */

void addToLabelList(uint16_t location, char *label, struct labelList **listHead) {
        char *str;

        if(allocatedLabelList < (sizeof(**listHead) * (++sizeOfLabelList))) {
                allocatedLabelList += (sizeof(**listHead) * 10);
                *listHead = realloc(*listHead, allocatedLabelList);
        }

        str = malloc(256);
        strcpy(str, label);

        (*listHead + sizeOfLabelList - 1)->location     = location;
        (*listHead + sizeOfLabelList - 1)->label        = str;
}

void deleteLabelList(struct labelList *listHead) {
        size_t i;

        for(i = 0; i < sizeOfLabelList; i++) {
                free((listHead+i)->label);
        }

        free(listHead);

        sizeOfLabelList = 0;
}

int getLabel(struct labelList *listHead, char *str) {
        size_t i;

        for(i = 0; i < sizeOfLabelList; i++) {
                if(!strcmp(str, (listHead+i)->label))
                        return (int)(listHead+i)->location;
        }

        return -1;
}

void addToConstantList(char *label, struct labelList **listHead) {
	addToLabelList(0, label, listHead);
}

void addConstantToListEntry(uint16_t constant, struct labelList *listEntry) {
        listEntry->location     = constant;
}
