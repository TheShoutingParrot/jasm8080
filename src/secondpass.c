#include "jasm8080.h"

uint8_t symbolsToOpcode(struct symbolList *listHead, uint8_t *opcode, size_t *index) {
	uint8_t length;

	if((listHead+(*index))->type == typeDefinedByte) {
		*(opcode+0) = (listHead+(*index))->symbol.immediateByte;

		(*index)++;

		return 1;
	}

	else if((listHead+(*index))->type == typeDefinedWord) {
		*(opcode+0) = (listHead+(*index))->symbol.immediateWord & 0xFF;
		*(opcode+1) = ((listHead+(*index))->symbol.immediateWord & 0xFF00) >> 8;

		(*index)++;

		return 2;
	}

	else if((listHead+(*index))->type == typeDefinedSpace) {
		length = (listHead+((*index)))->symbol.immediateWord;

		(*index)++;

		return length; 
	}

	switch((listHead+(*index))->symbol.instruction) {
		case NoOperation:
			length = 1;

			*(opcode+0) = 0;

			(*index)++;

			break;

		case LoadImmediate:
			length = 3;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*(opcode+0) = 0x01;

					break;

				case rD:
					*(opcode+0) = 0x11;

					break;

				case rH:
					*(opcode+0) = 0x21;

					break;

				case rSP:
					*(opcode+0) = 0x31;

					break;

				default:
					die(__FILE__, __LINE__, "unknown register");
			}

			*(opcode+1) = ((listHead+(*index)+2)->symbol.immediateWord & 0xFF);
			*(opcode+2) = ((listHead+(*index)+2)->symbol.immediateWord & 0xFF00) >> 8;

			*index += 3;

			break;

		case StoreX:
			length = 1;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*(opcode+0) = 0x02;

					break;

				case rD:
					*(opcode+0) = 0x12;

					break;

				default:
					die(__FILE__, __LINE__, "unknown register pair");
			}

			*index += 2;

			break;

		case MoveImmediate:
			length = 2;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*(opcode+0) = 0x06;

					break;
				case rC:
					*(opcode+0) = 0x0E;

					break;
				
				case rD:
					*(opcode+0) = 0x16;

					break;
				case rE:
					*(opcode+0) = 0x1E;

					break;

				case rH:
					*(opcode+0) = 0x26;

					break;

				case rL:
					*(opcode+0) = 0x2E;

					break;

				case rM:
					*(opcode+0) = 0x36;

					break;

				case rA:
					*(opcode+0) = 0x3E;

					break;

				default:
					die(__FILE__, __LINE__, "unknown register");
			}

			*(opcode+1) = (listHead+(*index)+2)->symbol.immediateByte;

			*index += 3;

			break;


		case LoadX:
			length = 1;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*(opcode+0) = 0x0A;

					break;

				case rD:
					*(opcode+0) = 0x1A;

					break;

				default:
					die(__FILE__, __LINE__, "unknown register pair");
			}

			*index += 2;


			break;

		case StoreHL:
			length = 3;

			*(opcode+0) = 0x22;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case LoadHL:
			length = 3;

			*(opcode+0) = 0x2A;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case StoreA:
			length = 3;

			*(opcode+0) = 0x32;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case LoadA:
			length = 3;

			*(opcode+0) = 0x3A;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case Move:
			length = 1;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*(opcode+0) = 0x40 + (listHead+*(index)+2)->symbol.registerSymbol;
					break;
				case rC:
					*(opcode+0) = 0x48 + (listHead+*(index)+2)->symbol.registerSymbol;
					break;
				
				case rD:
					*(opcode+0) = 0x50 + (listHead+*(index)+2)->symbol.registerSymbol;
					break;

				case rE:
					*(opcode+0) = 0x58 + (listHead+*(index)+2)->symbol.registerSymbol;
					break;

				case rH:
					*(opcode+0) = 0x60 + (listHead+*(index)+2)->symbol.registerSymbol;
					break;

				case rL:
					*(opcode+0) = 0x68 + (listHead+*(index)+2)->symbol.registerSymbol;
					break;

				case rM:
					*(opcode+0) = 0x70 + (listHead+*(index)+2)->symbol.registerSymbol;
					break;
	
				case rA:
					*(opcode+0) = 0x78 + (listHead+*(index)+2)->symbol.registerSymbol;
					break;

				default:
					die(__FILE__, __LINE__, "unknown register");
			}

			*index += 3;

			break;

		case Halt:
			length = 1;

			*(opcode+0) = 0x76;

			*index += 1;

			break;
			
		case Jump:
			length = 3;

			*(opcode+0) = 0xC3;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case JumpIfNotZero:
			length = 3;

			*(opcode+0) = 0xC2;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case JumpIfZero:
			length = 3;

			*(opcode+0) = 0xCA;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case JumpIfNotCarry:
			length = 3;

			*(opcode+0) = 0xD2;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case JumpIfCarry:
			length = 3;

			*(opcode+0) = 0xDA;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case JumpIfOdd:
			length = 3;

			*(opcode+0) = 0xE2;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case JumpIfEven:
			length = 3;

			*(opcode+0) = 0xEA;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case JumpIfPlus:
			length = 3;

			*(opcode+0) = 0xF2;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case JumpIfMinus:
			length = 3;

			*(opcode+0) = 0xFA;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case Return:
			length = 1;

			*(opcode+0) = 0xC9;

			(*index)++;

			break;

		case ReturnIfNotZero:
			length = 1;

			*(opcode+0) = 0xC0;

			(*index)++;

			break;

		case ReturnIfZero:
			length = 1;

			*(opcode+0) = 0xC8;

			(*index)++;

			break;
			
		case ReturnIfNotCarry:
			length = 1;

			*(opcode+0) = 0xD0;

			(*index)++;

			break;

		case ReturnIfCarry:
			length = 1;

			*(opcode+0) = 0xD8;

			(*index)++;

			break;

		case ReturnIfOdd:
			length = 1;

			*(opcode+0) = 0xE0;

			(*index)++;

			break;

		case ReturnIfEven:
			length = 1;

			*(opcode+0) = 0xE8;

			(*index)++;

			break;

		case ReturnIfPlus:
			length = 1;

			*(opcode+0) = 0xF0;

			(*index)++;

			break;

		case ReturnIfMinus:
			length = 1;

			*(opcode+0) = 0xF8;

			(*index)++;

			break;

		case Call:
			length = 3;

			*(opcode+0) = 0xCD;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case CallIfNotZero:
			length = 3;

			*(opcode+0) = 0xC4;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case CallIfZero:
			length = 3;

			*(opcode+0) = 0xCC;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case CallIfNotCarry:
			length = 3;

			*(opcode+0) = 0xD4;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case CallIfCarry:
			length = 3;

			*(opcode+0) = 0xDC;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case CallIfOdd:
			length = 3;

			*(opcode+0) = 0xE4;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case CallIfEven:
			length = 3;

			*(opcode+0) = 0xEC;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case CallIfPlus:
			length = 3;

			*(opcode+0) = 0xF4;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;

		case CallIfMinus:
			length = 3;

			*(opcode+0) = 0xFC;

			*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
			*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

			*index += 2;

			break;


		case ExchangeHLandStack:
			length = 1;

			*(opcode+0) = 0xE3;

			*index += 1;

			break;

		case ExchangeHLandDE:
			length = 1;

			*(opcode+0) = 0xEB;

			*index += 1;

			break;

		case Pop:
			length = 1;
			
			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*(opcode+0) = 0xC1;
					break;
				case rD:
					*(opcode+0) = 0xD1;
					break;
				case rH:
					*(opcode+0) = 0xE1;
					break;
				case rPSW:
					*(opcode+0) = 0xF1;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register");
			}

			*index += 2;

			break;

		case Push:
			length = 1;
			
			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*(opcode+0) = 0xC5;
					break;
				case rD:
					*(opcode+0) = 0xD5;
					break;
				case rH:
					*(opcode+0) = 0xE5;
					break;
				case rPSW:
					*(opcode+0) = 0xF5;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register");
			}

			*index += 2;

			break;

		case Out:
			length = 2;

			*(opcode+0) = 0xD3;

			*(opcode+1) = ((listHead+*(index)+1)->symbol.immediateByte);

			*index += 2;

			break;

		case In:
			length = 2;

			*(opcode+0) = 0xDB;

			*(opcode+1) = ((listHead+*(index)+1)->symbol.immediateByte);

			*index += 2;

			break;

		case DisableInterrupts:
			length = 1;

			*(opcode+0) = 0xF3;

			*index += 1;

			break;

		case EnableInterrupts:
			length = 1;

			*(opcode+0) = 0xFB;

			*index += 1;

			break;

		case LoadSpHl:
			length = 1;

			*(opcode+0) = 0xF9;

			*index += 1;

			break;

		case IncrementRegisterPair:
			length = 1;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*opcode = 0x03;
					break;
				case rD:
					*opcode = 0x13;
					break;
				case rH:
					*opcode = 0x23;
					break;
				case rSP:
					*opcode = 0x33;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register");
			}

			*index += 2;

			break;

		case Increment:
			length = 1;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*opcode = 0x04;
					break;
				case rC:
					*opcode = 0x0C;
					break;
				case rD:
					*opcode = 0x14;
					break;
				case rE:
					*opcode = 0x1C;
					break;
				case rH:
					*opcode = 0x24;
					break;
				case rL:
					*opcode = 0x2C;
					break;
				case rM:
					*opcode = 0x34;
					break;
				case rA:
					*opcode = 0x3C;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register");
			}		

			*index += 2;
		
			break;

		case Decrement:
			length = 1;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*opcode = 0x05;
					break;
				case rC:
					*opcode = 0x0D;
					break;
				case rD:
					*opcode = 0x15;
					break;
				case rE:
					*opcode = 0x1D;
					break;
				case rH:
					*opcode = 0x25;
					break;
				case rL:
					*opcode = 0x2D;
					break;
				case rM:
					*opcode = 0x35;
					break;
				case rA:
					*opcode = 0x3D;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register");
			}		

			*index += 2;

			break;
		
		case DecrementRegisterPair:
			length = 1;

			switch((listHead+*(index)+1)->symbol.registerSymbol) {
				case rB:
					*opcode = 0x0B;
					break;
				case rD:
					*opcode = 0x1B;
					break;
				case rH:
					*opcode = 0x2B;
					break;
				case rSP:
					*opcode = 0x3B;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register %d", (listHead+*(index)+1)->symbol.registerSymbol);
			}

			*index += 2;

			break;

		case Add:
			length = 1;

			*(opcode+0) = 0x80 + ((listHead+*index+1)->symbol.registerSymbol);

			*index += 2;

			break;

		case AddWithCarry:
			length = 1;

			*(opcode+0) = 0x88 + ((listHead+*index+1)->symbol.registerSymbol);

			*index += 2;

			break;

		case Sub:
			length = 1;

			*(opcode+0) = 0x90 + ((listHead+*index+1)->symbol.registerSymbol);

			*index += 2;

			break;

		case SubWithBorrow:
			length = 1;

			*(opcode+0) = 0x98 + ((listHead+*index+1)->symbol.registerSymbol);

			*index += 2;

			break;

		case AddImmediate:
			length = 2;

			*(opcode+0) = 0xC6;
			*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			*index += 2;

			break;

		case AddImmediateWithCarry:
			length = 2;

			*(opcode+0) = 0xCE;
			*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			*index += 2;

			break;

		case SubImmediate:
			length = 2;

			*(opcode+0) = 0xD6;
			*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			*index += 2;

			break;

		case SubImmediateWithBorrow:
			length = 2;

			*(opcode+0) = 0xDE;
			*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			*index += 2;

			break;

		case RotateALeft:
			length = 1;

			*(opcode) = 0x07;

			*index += 1;
			
			break;

		case RotateALeftCarry:
			length = 1;

			*(opcode) = 0x17;

			*index += 1;

			break;

		case RotateARight:
			length = 1;

			*(opcode) = 0x0F;

			*index += 1;
			
			break;

		case RotateARightCarry:
			length = 1;

			*(opcode) = 0x1F;

			*index += 1;

			break;		

		case DecimalAdjust:
			length = 1;

			*(opcode) = 0x27;

			*index += 1;

			break;

		case ComplementA:
			length = 1;

			*(opcode) = 0x2F;

			*index += 1;

			break;

		case SetCarry:
			length = 1;

			*(opcode) = 0x37;

			*index += 1;

			break;

		case ComplementCarry:
			length = 1;

			*(opcode) = 0x3F;

			*index += 1;

			break;

		case AndA:
			length = 1;

			*(opcode+0) = 0xA0 + (listHead+*(index)+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case XorA:
			length = 1;

			*opcode = 0xA8 + (listHead+*(index)+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case OrA:
			length = 1;

			*(opcode+0) = 0xB0 + (listHead+*(index)+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case Compare:
			length = 1;

			*opcode = 0xB8 + (listHead+*(index)+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case AndImmediate:
			length = 2;

			*(opcode+0) = 0xE6;
			*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			*index += 2;

			break;

		case XorImmediate:
			length = 2;

			*(opcode+0) = 0xEE;
			*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			*index += 2;

			break;

		case OrImmediate:
			length = 2;

			*(opcode+0) = 0xF6;
			*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			*index += 2;

			break;

		case CompareImmediate:
			length = 2;

			*(opcode+0) = 0xFE;
			*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			*index += 2;

			break;

		case PcFromHl:
			length = 1;

			*opcode = 0xE9;

			*index += 1;

			break;

		case RestartSubroutine:
			length = 1;

			switch((listHead+*index+1)->symbol.immediateByte) {
				case 0:
					*opcode = 0xC7;
					break;
				case 1:
					*opcode = 0xCF;
					break;
				case 2:
					*opcode = 0xD7;
					break;
				case 3:
					*opcode = 0xDF;
					break;
				case 4:
					*opcode = 0xD7;
					break;
				case 5:
					*opcode = 0xDF;
					break;
				case 6:
					*opcode = 0xD7;
					break;
				case 7:
					*opcode = 0xDF;
					break;
				default:
					die(__FILE__, __LINE__, "expected restart subroutine byte");
			}

			*index += 2;

			break;

		default:
			die(__FILE__, __LINE__, "unknown instruction: %d", (listHead+(*index))->symbol.instruction);
	}

	return length;
}

void secondPass(FILE *objectFile, struct symbolList *listHead) {
	uint8_t *opcode;
	uint8_t length, j;
	size_t i;

	info(__FILE__, __LINE__, "starting secondpass...");

	opcode = (uint8_t *)malloc(3 * sizeof(*opcode));

	for(i = 0; i < sizeOfSymbolList;) {
		length = symbolsToOpcode(listHead, opcode, &i);

		for(j = 0; j < length; j++) {
			fwrite((opcode+j), sizeof(*opcode), 1, objectFile);
		}
	}

	info(__FILE__, __LINE__, "secondpass was successful...");
}
