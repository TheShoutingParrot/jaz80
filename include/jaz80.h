#ifndef	_JAZ80_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>

/* constants */

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/* enums */

enum symbolTypes {
        typeInstruction = 0,
        typeRegister = 1,
	typeFirstRegister = 2,
        typeImmediateByte = 3,
        typeImmediateWord = 4,
        typeAddress = 5,
	typeCondition = 6,
	typeRegisterOrImmediateByte = 7,
	typeRegisterOrImmediateWord = 8,
	typeRegisterOrImmediateVal = 9,
	typeRegisterOrAddress = 10,
        typeAddressLabel = 11,
        typeWordLabel = 12,
        typeOrgAddress = 13,
	typePort = 14,
        typeUnknown = 15,
	typeEqu,
	typeEquValue,
        typeDefinedByte,
	typeDefinedWord,
	typeDefinedSpace,
};

enum registers {
        rB, rC,
	rD, rE,
        rH, rL,
	rHL_mem,
        rA, rF,
	rAF,
        rSP,
	rBC, rDE, rHL,
	rBC_mem, rDE_mem,
	rSP_mem,
	rAF_shadow,
	rI, rR,
	rC_mem,
        rTotal,
};

enum instructions {
	/* stack, i/o and machine flow */
        NoOperation, Halt, Pop,
	Push, Out, In,
	DisableInterrupts, EnableInterrupts,

	/* branch and program flow */
        Jump, Return, Call,
	DecrementJumpIfNotZero, JumpRegister,
	RestartSubroutine,

	/* data transfer*/
	Load, Exchange, ExchangeShadows,

	/* arithmetic */
	Increment, 
	Decrement,
	Add, AddWithCarry,
	Sub, SubWithBorrow, 

	/* logical */
	RotateALeft, RotateALeftCarry, RotateARight,
	RotateARightCarry, DecimalAdjust, ComplementA,
	SetCarry, ComplementCarry, 
	AndA, XorA, OrA, 
	Compare,

	/* Extended Instructions */
	/* stack, i/o and machine flow */
	InterruptMode, LoadI, LoadIR,
	Ini, IniR,
	OutI, OutIR,
	InD, InDR,
	OutD, OutDR,
	ReturnN, ReturnI,
	Rrd, Rld,

	/* arithmetic  */
	CompareHL, CompareHLR,
	CompareD, CompareDR,

	/* data transfer*/
	LoadD, LoadDR,

	/* logical */
	Negate,

	RotateLeftC, RotateRightC,
	RotateLeft, RotateRight,
	ShiftLeftA, ShiftRightA, ShiftRightL,

	TestBit, ResetBit, SetBit,	

        totalInstructionPlaceholders,
};

enum pseudoInstructions {
        Org,
        DefineByte,
	DefineWord,
	DefineSpace,
	Equals,
        totalPseudoInstructionPlaceholders,
};

enum conditions {
	ifNotZero, ifZero,
	ifNotCarry, ifCarry,
	ifOdd, ifEven,
	ifPlus, ifMinus,
	totalConditions,
};

/* data structs */

union symbol {
        uint8_t         instruction,
			registerSymbol,
			immediateByte,
			condition;
        uint16_t        immediateWord,
			address;
        char            *label;
};

struct symbolList {
        uint16_t location;

        uint8_t type;
        union symbol symbol;
};

struct labelList {
        uint16_t location;

        char *label;
};

/* variables */

extern char *instructionsStr[totalInstructionPlaceholders];
extern char *pseudoInstructionsStr[totalPseudoInstructionPlaceholders];
extern char *conditionsStr[totalConditions];

extern size_t sizeOfSymbolList,
       allocatedSymbolList,
       sizeOfLabelList,
       allocatedLabelList;

/* functions */

void addToSymbolList(uint16_t location, uint8_t type, union symbol symbol, struct symbolList **listHead);
void deleteSymbolList(struct symbolList *listHead);
void printSymbolList(struct symbolList *listHead);

void addToLabelList(uint16_t location, char *label, struct labelList **listHead);
void deleteLabelList(struct labelList *listHead);
void printLabelList(struct labelList *listHead);
int getLabel(struct labelList *listHead, char *str);

void addToConstantList(char *label, struct labelList **listHead);
void addConstantToListEntry(uint16_t constant, struct labelList *listEntry);

uint16_t convertStrToWord(char *str, bool *number);

uint8_t recognizePseudoInstruction(char *str);
uint8_t recognizeInstruction(char *str);
uint8_t recognizeRegister(char *str);

void convertTokenToSymbolListEntry(
                uint16_t *location, 
                uint8_t *expectedType, 
                uint8_t *nextExpectedType,
                bool *hasLabels,
                char *token, 
		char *prevToken,
                struct symbolList **listHead,
                struct labelList **labelListHead);

void firstPass(FILE *sourceFile, struct symbolList **listHead, struct labelList **labelListHead);

uint8_t symbolsToOpcode(struct symbolList *listHead, uint8_t *opcode, size_t *index);

void secondPass(FILE *objectFile, struct symbolList *listHead);

void die(const char *filename, const size_t line, const char *fmt, ...);
void info(const char *filename, const size_t line, const char *fmt, ...);
bool prompt(const char *prompText, ...);

#define	_JAZ80_H
#endif /* #ifndef _JASZ80_H */
