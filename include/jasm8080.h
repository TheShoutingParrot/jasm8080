#ifndef	_JASM8080_H

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
        typeInstruction,
        typeRegister,
	typeFirstRegister,
        typeImmediateByte,
        typeImmediateWord,
        typeAddress,
        typeLabel,
        typeOrgAddress,
        typeUnknown,
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
        rM,
        rA, rF, rPSW,
        rSP,
        rTotal,
};

enum instructions {
	/* stack, i/o and machine flow */
        NoOperation, Halt, Pop,
	Push, Out, In,
	DisableInterrupts, EnableInterrupts, LoadSpHl,

	/* branch and program flow */
        Jump, Return, Call,
	JumpIfNotZero, JumpIfZero,
	JumpIfNotCarry, JumpIfCarry,
	JumpIfOdd, JumpIfEven,
	JumpIfPlus, JumpIfMinus,
	ReturnIfNotZero, ReturnIfZero,
	ReturnIfNotCarry, ReturnIfCarry,
	ReturnIfOdd, ReturnIfEven,
	ReturnIfPlus, ReturnIfMinus,
	CallIfNotZero, CallIfZero,
	CallIfNotCarry, CallIfCarry,
	CallIfOdd, CallIfEven,
	CallIfPlus, CallIfMinus,
	PcFromHl, RestartSubroutine,

	/* data transfer*/
       	LoadImmediate, StoreX, MoveImmediate,
	LoadX, StoreHL, LoadHL,
	StoreA, LoadA, Move,
	ExchangeHLandStack, ExchangeHLandDE,

	/* arithmetic */
	IncrementRegisterPair, Increment, Decrement,
	DecrementRegisterPair, Add, AddWithCarry,
	Sub, SubWithBorrow, AddImmediate, AddImmediateWithCarry,
	SubImmediate, SubImmediateWithBorrow,

	/* logical */
	RotateALeft, RotateALeftCarry, RotateARight,
	RotateARightCarry, DecimalAdjust, ComplementA,
	SetCarry, ComplementCarry, AndA,
	XorA, OrA, Compare,
	AndImmediate, XorImmediate, OrImmediate,
	CompareImmediate,

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

/* data structs */

union symbol {
        uint8_t         instruction;
        uint8_t         registerSymbol;
        uint8_t         immediateByte;
        uint16_t        immediateWord;
        uint16_t        address;
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

size_t sizeOfSymbolList,
       allocatedSymbolList;

size_t sizeOfLabelList,
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

#define	_JASM8080_H
#endif /* #ifndef _JASM8080_H */
