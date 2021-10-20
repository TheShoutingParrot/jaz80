#include "jaz80.h"

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

		case Load:
			if((listHead+*(index)+1)->type == typeRegister &&
					((listHead+*(index)+2)->type == typeImmediateWord)) {
				length = 3;

				switch((listHead+*(index)+1)->symbol.registerSymbol) {
					case rBC:
						*(opcode+0) = 0x01;

						break;

					case rDE:
						*(opcode+0) = 0x11;

						break;

					case rHL:
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
			}

			else if((listHead+*(index)+1)->type == typeRegister &&
					(listHead+*(index)+2)->type == typeImmediateByte) {
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

					case rHL_mem:
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
			}

			else if((listHead+*(index)+1)->type == typeRegister &&
					(listHead+*(index)+2)->type == typeRegister) {

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

					case rHL_mem:
						*(opcode+0) = 0x70 + (listHead+*(index)+2)->symbol.registerSymbol;
						break;
	
					case rA:
						if((listHead+*(index)+2)->symbol.registerSymbol == rBC_mem)
							*(opcode+0) = 0x0A;
						else if((listHead+*(index)+2)->symbol.registerSymbol == rDE_mem)
							*(opcode+0) = 0x1A;
						else if((listHead+*(index)+2)->symbol.registerSymbol == rI) {
							length = 2;

							*(opcode+0) = 0xED;
							*(opcode+0) = 0x57;
						}
						else if((listHead+*(index)+2)->symbol.registerSymbol == rR) {
							length = 2;

							*(opcode+0) = 0xED;
							*(opcode+0) = 0x5F;
						}
						else
							*(opcode+0) = 0x78 + (listHead+*(index)+2)->symbol.registerSymbol;
						break;


					case rBC_mem:
						if((listHead+*(index)+2)->symbol.registerSymbol == rA)
							*(opcode+0) = 0x02;

						else 
							die(__FILE__, __LINE__, "Impossible combination of registers with (bc)!");

						break;

					case rDE_mem:
						if((listHead+*(index)+2)->symbol.registerSymbol == rA)
							*(opcode+0) = 0x12;

						else 
							die(__FILE__, __LINE__, "Impossible combination of registers with (de)!");
								
						break;
						
					case rSP:
						switch((listHead+*(index)+2)->symbol.registerSymbol) {
							case rHL:
								*(opcode+0) = 0xF9;

								break;

							default:
								die(__FILE__, __LINE__, "unknown register %d %d", (listHead+*(index)+1)->symbol.registerSymbol);
						}

						break;

					case rI:
						length = 2;

						if((listHead+*(index)+2)->symbol.registerSymbol == rA) {
							*(opcode+0) = 0xED;
							*(opcode+1) = 0x47;
						}

						else 
							die(__FILE__, __LINE__, "Impossible combination of registers with I!");

						break;

					case rR:
						length = 2;

						if((listHead+*(index)+2)->symbol.registerSymbol == rA) {
							*(opcode+0) = 0xED;
							*(opcode+1) = 0x4F;
						}

						else 
							die(__FILE__, __LINE__, "Impossible combination of registers with R!");

						break;

					default:
						die(__FILE__, __LINE__, "unknown register %d", (listHead+*(index)+1)->symbol.registerSymbol, 
								(listHead+*(index)+2)->symbol.registerSymbol);
				}

				*index += 3;

			}


			else if((listHead+*(index)+1)->type == typeRegister &&
					((listHead+*(index)+2)->type == typeAddress)) {
				length = 3;
				switch((listHead+*(index)+1)->symbol.registerSymbol) {
					case rHL:
						*(opcode+0) = 0x2A;

						*(opcode+1) = ((listHead+(*index)+2)->symbol.address & 0xFF);
						*(opcode+2) = ((listHead+(*index)+2)->symbol.address & 0xFF00) >> 8;

						break;

					case rA:
						*(opcode+0) = 0x3A;

						*(opcode+1) = ((listHead+(*index)+2)->symbol.address & 0xFF);
						*(opcode+2) = ((listHead+(*index)+2)->symbol.address & 0xFF00) >> 8;

						break;

					case rBC:
						length = 4;

						*(opcode+0) = 0xED;
						*(opcode+1) = 0x4B;

						*(opcode+2) = ((listHead+(*index)+2)->symbol.address & 0xFF);
						*(opcode+3) = ((listHead+(*index)+2)->symbol.address & 0xFF00) >> 8;

						break;

					case rDE:
						length = 4;

						*(opcode+0) = 0xED;
						*(opcode+1) = 0x5B;

						*(opcode+2) = ((listHead+(*index)+2)->symbol.address & 0xFF);
						*(opcode+3) = ((listHead+(*index)+2)->symbol.address & 0xFF00) >> 8;

						break;
					
					case rSP:
						length = 4;

						*(opcode+0) = 0xED;
						*(opcode+1) = 0x7B;

						*(opcode+2) = ((listHead+(*index)+2)->symbol.address & 0xFF);
						*(opcode+3) = ((listHead+(*index)+2)->symbol.address & 0xFF00) >> 8;

						break;

					default:
						die(__FILE__, __LINE__, "unrecognized register for instruction \"ld r, (**)\"");
				}
					
				*index += 3;
			}

			else if((listHead+*(index)+1)->type == typeAddress &&
					(listHead+*(index)+2)->type == typeRegister) {
				length = 3;

				switch((listHead+*(index)+2)->symbol.registerSymbol) {
					case rHL:
						*(opcode+0) = 0x22;

						*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
						*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

						break;

					case rA:
						*(opcode+0) = 0x32;

						*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
						*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

						break;

					case rBC:
						length = 4;

						*(opcode+0) = 0xED;
						*(opcode+1) = 0x43;

						*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF);
						*(opcode+3) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

						break;

					case rDE:
						length = 4;

						*(opcode+0) = 0xED;
						*(opcode+1) = 0x53;

						*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF);
						*(opcode+3) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

						break;
						
					case rSP:
						length = 4;

						*(opcode+0) = 0xED;
						*(opcode+1) = 0x73;

						*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF);
						*(opcode+3) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

						break;

					default:
						die(__FILE__, __LINE__, "unrecognized register for instruction \"ld (**), r\"");
				}

				*index += 3;
			} 

			else {
				die(__FILE__, __LINE__, "LD instruction not working %d %d %d", (listHead+*(index)+1)->symbol.immediateWord, (listHead+*(index)+2)->type, (listHead+*(index))->location);
			}

			break;

		case Exchange:
			length = 1;

			if((listHead+*(index)+1)->symbol.registerSymbol == rSP_mem
					&& (listHead+*(index)+2)->symbol.registerSymbol == rHL)
				*(opcode+0) = 0xE3;
			else if((listHead+*(index)+1)->symbol.registerSymbol == rHL
					&& (listHead+*(index)+2)->symbol.registerSymbol == rDE)
				*(opcode+0) = 0xEB;
			else if((listHead+*(index)+1)->symbol.registerSymbol == rAF
					&& (listHead+*(index)+2)->symbol.registerSymbol == rAF_shadow)
				*(opcode+0) = 0x08;
			else
				die(__FILE__, __LINE__, "unrecognized register combination for the exchange instruction"); 

			*index += 3;

			break;

		case ExchangeShadows:
			length = 1;

			*(opcode+0) = 0xD9;

			*index += 1;

			break;


		case Halt:
			length = 1;

			*(opcode+0) = 0x76;

			*index += 1;

			break;
			
		case Jump:
			length = 3;

			if((listHead+*(index)+1)->type == typeCondition) {

				switch((listHead+*(index)+1)->symbol.condition) {
					case ifNotZero:
						*(opcode+0) = 0xC2;

						break;

					case ifZero:
						*(opcode+0) = 0xCA;

						break;

					case ifNotCarry:
						*(opcode+0) = 0xD2;

						break;

					case ifCarry:
						*(opcode+0) = 0xDA;

						break;

					case ifOdd:
						*(opcode+0) = 0xE2;

						break;

					case ifEven:
						*(opcode+0) = 0xEA;

						break;

					case ifPlus:
						*(opcode+0) = 0xF2;

						break;

					case ifMinus:
						*(opcode+0) = 0xFA;

						break;

					default:
						die(__FILE__, __LINE__, "unknown condition");
				}

				*(opcode+1) = ((listHead+(*index)+2)->symbol.address & 0xFF);
				*(opcode+2) = ((listHead+(*index)+2)->symbol.address & 0xFF00) >> 8;

				*index += 3;
			}

			else if((listHead+*(index)+1)->type == typeRegister 
					&& (listHead+*(index)+1)->symbol.registerSymbol == rHL_mem) {
				length = 1;

				*(opcode+0) = 0xE9;

				(*index) += 2;
			}

			else {
				*(opcode+0) = 0xC3;

				*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
				*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

				*index += 2;
			}

			break;

		case JumpRegister:
			length = 2;

			if((listHead+*(index)+1)->type == typeCondition) {

				switch((listHead+*(index)+1)->symbol.condition) {
					case ifNotZero:
						*(opcode+0) = 0x20;

						break;

					case ifZero:
						*(opcode+0) = 0x28;

						break;

					case ifNotCarry:
						*(opcode+0) = 0x30;

						break;

					case ifCarry:
						*(opcode+0) = 0x38;

						break;

					default:
						die(__FILE__, __LINE__, "unknown condition");
				}

				*(opcode+1) = (listHead+(*index)+2)->symbol.immediateByte;

				*index += 3;
			}

			else {
				*(opcode+0) = 0x18;

				*(opcode+1) = (listHead+(*index)+1)->symbol.immediateByte;

				*index += 2;
			}

			break;

		case Return:
			length = 1;

			if((listHead+*(index)+1)->type == typeCondition) {

				switch((listHead+*(index)+1)->symbol.condition) {
					case ifNotZero:
						*(opcode+0) = 0xC0;

						break;

					case ifZero:
						*(opcode+0) = 0xC8;

						break;

					case ifNotCarry:
						*(opcode+0) = 0xD0;

						break;

					case ifCarry:
						*(opcode+0) = 0xD8;

						break;

					case ifOdd:
						*(opcode+0) = 0xE0;

						break;

					case ifEven:
						*(opcode+0) = 0xE8;

						break;

					case ifPlus:
						*(opcode+0) = 0xF0;

						break;

					case ifMinus:
						*(opcode+0) = 0xF8;

						break;

					default:
						die(__FILE__, __LINE__, "unknown condition");
				}

				*index += 2;
			}

			else {
				*(opcode+0) = 0xC9;

				(*index)++;
			}

			break;

		case Call:
			length = 3;

			if((listHead+*(index)+1)->type == typeCondition) {

				switch((listHead+*(index)+1)->symbol.condition) {
					case ifNotZero:
						*(opcode+0) = 0xC4;

						break;

					case ifZero:
						*(opcode+0) = 0xCC;

						break;

					case ifNotCarry:
						*(opcode+0) = 0xD4;

						break;

					case ifCarry:
						*(opcode+0) = 0xDC;

						break;

					case ifOdd:
						*(opcode+0) = 0xE4;

						break;

					case ifEven:
						*(opcode+0) = 0xEC;

						break;

					case ifPlus:
						*(opcode+0) = 0xF4;

						break;

					case ifMinus:
						*(opcode+0) = 0xFC;

						break;

					default:
						die(__FILE__, __LINE__, "unknown condition");
				}

				*(opcode+1) = ((listHead+(*index)+2)->symbol.address & 0xFF);
				*(opcode+2) = ((listHead+(*index)+2)->symbol.address & 0xFF00) >> 8;

				*index += 3;
			}

			else {

				*(opcode+0) = 0xCD;

				*(opcode+1) = ((listHead+(*index)+1)->symbol.address & 0xFF);
				*(opcode+2) = ((listHead+(*index)+1)->symbol.address & 0xFF00) >> 8;

				*index += 2;
			}

			break;

		case DecrementJumpIfNotZero:
			length = 2;

			*(opcode+0) = 0x10;
			*(opcode+1) = (listHead+(*index+1))->symbol.immediateByte;

			*index += 2;

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
				case rAF:
					*(opcode+0) = 0xF1;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register pair");
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
				case rAF:
					*(opcode+0) = 0xF5;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register pair");
			}

			*index += 2;

			break;

		case Out:
			if((listHead+*(index)+2)->symbol.registerSymbol == rC_mem) {
				length = 2;

				*(opcode+0) = 0xED;

				switch((listHead+*(index)+1)->symbol.registerSymbol) {
					case rB:
						*(opcode+1) = 0x41;
						break;
					case rC:
						*(opcode+1) = 0x49;
						break;
					case rD:
						*(opcode+1) = 0x51;
						break;
					case rE:
						*(opcode+1) = 0x59;
						break;
					case rH:
						*(opcode+1) = 0x61;
						break;
					case rL:
						*(opcode+1) = 0x69;
						break;
					case rA:
						*(opcode+1) = 0x79;
						break;
				}

				*index += 3;
			}

			else if((listHead+*(index)+2)->symbol.registerSymbol == rA) { 
				length = 2;

				*(opcode+0) = 0xD3;

				*(opcode+1) = ((listHead+*(index)+1)->symbol.immediateByte);
	
				*index += 3;
			}

			

			else {
				die(__FILE__, __LINE__, "unrecognized register");
			}

			break;

		case In:

			if((listHead+*(index)+2)->symbol.registerSymbol == rC_mem) {
				length = 2;

				*(opcode+0) = 0xED;

				switch((listHead+*(index)+1)->symbol.registerSymbol) {
					case rB:
						*(opcode+1) = 0x40;
						break;
					case rC:
						*(opcode+1) = 0x48;
						break;
					case rD:
						*(opcode+1) = 0x50;
						break;
					case rE:
						*(opcode+1) = 0x58;
						break;
					case rH:
						*(opcode+1) = 0x60;
						break;
					case rL:
						*(opcode+1) = 0x68;
						break;
					case rA:
						*(opcode+1) = 0x78;
						break;
				}


				*index += 3;
			}

			else if((listHead+*(index)+1)->symbol.registerSymbol == rA) { 
				length = 2;

				*(opcode+0) = 0xDB;

				*(opcode+1) = ((listHead+*(index)+2)->symbol.immediateByte);

				*index += 3;
			}

			else {
				die(__FILE__, __LINE__, "unrecognized register");
			}

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
				case rHL_mem:
					*opcode = 0x34;
					break;
				case rA:
					*opcode = 0x3C;
					break;
				case rBC:
					*opcode = 0x03;
					break;
				case rDE:
					*opcode = 0x13;
					break;
				case rHL:
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
				case rHL_mem:
					*opcode = 0x35;
					break;
				case rA:
					*opcode = 0x3D;
					break;
				case rBC:
					*opcode = 0x0B;
					break;
				case rDE:
					*opcode = 0x1B;
					break;
				case rHL:
					*opcode = 0x2B;
					break;
				case rSP:
					*opcode = 0x3B;
					break;
				default:
					die(__FILE__, __LINE__, "unrecognized register");
			}		

			*index += 2;

			break;

		case Add:
			if((listHead+*index+2)->type == typeImmediateByte) {
				length = 2;

				*(opcode+0) = 0xC6;
				*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			}

			else if((listHead+*index+1)->symbol.registerSymbol == rA) {
				if((listHead+*index+2)->symbol.registerSymbol > rA) {	
					die(__FILE__, __LINE__, "unrecognized register");
				}

				else {
					length = 1;
	
					*(opcode+0) = 0x80 + ((listHead+*index+2)->symbol.registerSymbol);

				}
			}

			else if((listHead+*index+1)->symbol.registerSymbol == rHL) {
				length = 1;

				switch((listHead+*index+2)->symbol.registerSymbol) {
					case rBC:
						*(opcode+0) = 0x09;

						break;

					case rDE:
						*(opcode+0) = 0x19;

						break;

					case rHL:
						*(opcode+0) = 0x29;

						break;

					case rSP:
						*(opcode+0) = 0x39;

						break;

					default:
						die(__FILE__, __LINE__, "unrecognized register");
				}
			}

			else
				die(__FILE__, __LINE__, "ADD instruction assembling not working");

			*index += 3;

			break;

		case AddWithCarry:
			if((listHead+*index+2)->type == typeImmediateByte) {
				length = 2;

				*(opcode+0) = 0xCE;
				*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			}

			else if((listHead+*index+1)->symbol.registerSymbol == rA) {
				if((listHead+*index+2)->symbol.registerSymbol > rA) {	
					die(__FILE__, __LINE__, "unrecognized register");
				}

				else {
					length = 1;
	
					*(opcode+0) = 0x88 + ((listHead+*index+2)->symbol.registerSymbol);
				}
			}

			else if((listHead+*index+1)->symbol.registerSymbol == rHL) {
				length = 2;
	
				switch((listHead+*index+2)->symbol.registerSymbol) {
					case rBC:
						*(opcode+0) = 0xED;
						*(opcode+1) = 0x4A;
						break;
					case rDE:
						*(opcode+0) = 0xED;
						*(opcode+1) = 0x5A;
						break;
					case rHL:
						*(opcode+0) = 0xED;
						*(opcode+1) = 0x6A;
						break;
					case rSP:
						*(opcode+0) = 0xED;
						*(opcode+1) = 0x7A;
						break;
					default:
						die(__FILE__, __LINE__, "unrecognized register");
				}
			}

			*index += 3;

			break;

		case Sub:
			if((listHead+*index+1)->type == typeRegister) {
				length = 1;

				*(opcode+0) = 0x90 + ((listHead+*index+1)->symbol.registerSymbol);
			}

			else {
				length = 2;

				*(opcode+0) = 0xD6;
				*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;
			}

			*index += 2;

			break;

		case SubWithBorrow:
			length = 1;

			if((listHead+*index+2)->type == typeImmediateByte) {
				length = 2;

				*(opcode+0) = 0xDE;
				*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

			}

			else if((listHead+*index+1)->symbol.registerSymbol == rA) {
				if((listHead+*index+2)->symbol.registerSymbol > rA) {	
					die(__FILE__, __LINE__, "unrecognized register");
				}

				else {
					length = 1;
	
					*(opcode+0) = 0x98 + ((listHead+*index+2)->symbol.registerSymbol);
				}
			}

			else if((listHead+*index+1)->symbol.registerSymbol == rHL) {
				length = 2;
	
				switch((listHead+*index+2)->symbol.registerSymbol) {
					case rBC:
						*(opcode+0) = 0xED;
						*(opcode+1) = 0x42;
						break;
					case rDE:
						*(opcode+0) = 0xED;
						*(opcode+1) = 0x52;
						break;
					case rHL:
						*(opcode+0) = 0xED;
						*(opcode+1) = 0x62;
						break;
					case rSP:
						*(opcode+0) = 0xED;
						*(opcode+1) = 0x72;
						break;
					default:
						die(__FILE__, __LINE__, "unrecognized register");
				}
			}

			*index += 3;

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
			if((listHead+*(index)+1)->type == typeImmediateByte)  {
				length = 2;

				*(opcode+0) = 0xE6;
				*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

				*index += 2;
			}

			else {
				length = 1;

				*(opcode+0) = 0xA0 + (listHead+*(index)+1)->symbol.registerSymbol;

				*index += 2;
			}

			break;

		case XorA:
			if((listHead+*(index)+1)->type == typeImmediateByte)  {
				length = 2;

				*(opcode+0) = 0xEE;
				*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

				*index += 2;
			}

			else {
				length = 1;

				*opcode = 0xA8 + (listHead+*(index)+1)->symbol.registerSymbol;

				*index += 2;
			}

			break;

		case OrA:
			if((listHead+*(index)+1)->type == typeImmediateByte)  {
				length = 2;

				*(opcode+0) = 0xF6;
				*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

				*index += 2;
			}

			else {
				length = 1;

				*(opcode+0) = 0xB0 + (listHead+*(index)+1)->symbol.registerSymbol;

				*index += 2;
			}

			break;

		case Compare:
			if((listHead+*(index)+1)->type == typeImmediateByte)  {
				length = 2;

				*(opcode+0) = 0xFE;
				*(opcode+1) = (listHead+*index+1)->symbol.immediateByte;

				*index += 2;
			}
			else {
				length = 1;

				*opcode = 0xB8 + (listHead+*(index)+1)->symbol.registerSymbol;

				*index += 2;
			}

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

		case Negate:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0x44;

			*index += 1;

			break;

		case InterruptMode:
			length = 3;

			*(opcode+0) = 0xED;

			if((listHead+*index+1)->symbol.immediateByte == 0)
				*(opcode+0) = 0x46;
			else if((listHead+*index+1)->symbol.immediateByte == 1)
				*(opcode+0) = 0x56;
			else if((listHead+*index+1)->symbol.immediateByte == 2)
				*(opcode+0) = 0x5E;

			*index += 1;

			break;

		case LoadI:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xA0;

			*index += 1;

			break;

		case LoadIR:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xB0;

			*index += 1;

			break;

		case CompareHL:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xA1;

			*index += 1;

			break;

		case CompareHLR:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xB1;

			*index += 1;

			break;

		case Ini:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xA2;

			*index += 1;

			break;

		case IniR:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xB2;

			*index += 1;

			break;

		case OutI:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xA3;

			*index += 1;

			break;

		case OutIR:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xB3;

			*index += 1;

			break;

		case LoadD:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xA8;

			*index += 1;

			break;

		case LoadDR:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xB8;

			*index += 1;

			break;

		case CompareD:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xA9;

			*index += 1;

			break;

		case CompareDR:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xB9;

			*index += 1;

			break;

		case InD:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xAA;

			*index += 1;

			break;

		case InDR:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xBA;

			*index += 1;

			break;

		case OutD:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xAB;

			*index += 1;

			break;

		case OutDR:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0xBB;

			*index += 1;

			break;

		case ReturnN:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0x45;

			*index += 1;

			break;

		case ReturnI:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0x4D;

			*index += 1;

			break;

		case Rrd:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0x66;

			*index += 1;

			break;
			
		case Rld:
			length = 2;

			*(opcode+0) = 0xED;
			*(opcode+1) = 0x6F;

			*index += 1;

			break;

		case RotateLeftC:
			length = 2;

			*(opcode+0) = 0xCB;

			if((listHead+*index+1)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) = 0x06;
			else if((listHead+*index+1)->symbol.registerSymbol == rA)
				*(opcode+1) = 0x07;
			else {
				*(opcode+1) = 0x00 + (listHead+*index+1)->symbol.registerSymbol;
			}

			*index += 2;

			break;

		case RotateRightC:
			length = 2;

			*(opcode+0) = 0xCB;

			if((listHead+*index+1)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) = 0x0E;
			else if((listHead+*index+1)->symbol.registerSymbol == rA)
				*(opcode+1) = 0x0F;
			else
				*(opcode+1) = 0x08 + (listHead+*index+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case RotateLeft:
			length = 2;

			*(opcode+0) = 0xCB;

			if((listHead+*index+1)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) = 0x16;
			else if((listHead+*index+1)->symbol.registerSymbol == rA)
				*(opcode+1) = 0x17;
			else
				*(opcode+1) = 0x10 + (listHead+*index+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case RotateRight:
			length = 2;

			*(opcode+0) = 0xCB;

			if((listHead+*index+1)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) = 0x1E;
			else if((listHead+*index+1)->symbol.registerSymbol == rA)
				*(opcode+1) = 0x1F;
			else
				*(opcode+1) = 0x18 + (listHead+*index+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case ShiftLeftA:
			length = 2;

			*(opcode+0) = 0xCB;

			if((listHead+*index+1)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) = 0x26;
			else if((listHead+*index+1)->symbol.registerSymbol == rA)
				*(opcode+1) = 0x27;
			else
				*(opcode+1) = 0x20 + (listHead+*index+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case ShiftRightA:
			length = 2;

			*(opcode+0) = 0xCB;

			if((listHead+*index+1)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) = 0x2E;
			else if((listHead+*index+1)->symbol.registerSymbol == rA)
				*(opcode+1) = 0x2F;
			else
				*(opcode+1) = 0x28 + (listHead+*index+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case ShiftRightL:
			length = 2;

			*(opcode+0) = 0xCB;

			if((listHead+*index+1)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) = 0x3E;
			else if((listHead+*index+1)->symbol.registerSymbol == rA)
				*(opcode+1) = 0x3F;
			else
				*(opcode+1) = 0x38 + (listHead+*index+1)->symbol.registerSymbol;

			*index += 2;

			break;

		case TestBit:
			length = 2;

			*(opcode+0) = 0xCB;
			*(opcode+1) = 0x40;

			if((listHead+*index+2)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4) + 
					(((listHead+*index+1)->symbol.immediateByte % 2) ? 0xE : 0x6);
			else if((listHead+*index+2)->symbol.registerSymbol == rA)
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4) + 
					(((listHead+*index+1)->symbol.immediateByte % 2) ? 0xF : 0x7);
			else {
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4);
				*(opcode+1) += (((listHead+*index+1)->symbol.immediateByte % 2) ? 8 : 0);
				*(opcode+1) += (listHead+*index+2)->symbol.registerSymbol;
			}

			*index += 3;

			break;

		case ResetBit:
			length = 2;

			*(opcode+0) = 0xCB;
			*(opcode+1) = 0x80;

			if((listHead+*index+2)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4) + 
					(((listHead+*index+1)->symbol.immediateByte % 2) ? 0xE : 0x6);
			else if((listHead+*index+2)->symbol.registerSymbol == rA)
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4) + 
					(((listHead+*index+1)->symbol.immediateByte % 2) ? 0xF : 0x7);
			else {
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4);
				*(opcode+1) += (((listHead+*index+1)->symbol.immediateByte % 2) ? 8 : 0);
				*(opcode+1) += (listHead+*index+2)->symbol.registerSymbol;
			}


			*index += 3;

			break;

		case SetBit:
			length = 2;

			*(opcode+0) = 0xCB;
			*(opcode+1) = 0xC0;

			if((listHead+*index+2)->symbol.registerSymbol == rHL_mem)
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4) + 
					(((listHead+*index+1)->symbol.immediateByte % 2) ? 0xE : 0x6);
			else if((listHead+*index+2)->symbol.registerSymbol == rA)
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4) + 
					(((listHead+*index+1)->symbol.immediateByte % 2) ? 0xF : 0x7);
			else {
				*(opcode+1) += ((listHead+*index+1)->symbol.immediateByte / 2 << 4);
				*(opcode+1) += (((listHead+*index+1)->symbol.immediateByte % 2) ? 8 : 0);
				*(opcode+1) += (listHead+*index+2)->symbol.registerSymbol;
			}


			*index += 3;

			break;

		default:
			die(__FILE__, __LINE__, "unknown instruction: %d : %s at location %X", (listHead+(*index))->symbol.instruction, instructionsStr[(listHead+(*index))->symbol.instruction], (listHead+(*index))->location);
	}

	return length;
}

void secondPass(FILE *objectFile, struct symbolList *listHead) {
	uint8_t *opcode, j;
	size_t i;

	info(__FILE__, __LINE__, "starting secondpass...");

	opcode = (uint8_t *)malloc(3 * sizeof(*opcode));

	for(i = 0; i < sizeOfSymbolList;) {
		uint8_t length = symbolsToOpcode(listHead, opcode, &i);

		for(j = 0; j < length; j++) {
			fwrite((opcode+j), sizeof(*opcode), 1, objectFile);
		}
	}

	info(__FILE__, __LINE__, "secondpass was successful...");
}
