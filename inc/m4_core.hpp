/*
 * m4_core.cpp
 *
 *  Created on: 2017?6?11?
 *      Author: kohil
 */
#include "public_header.hpp"
#include "IntelHexAdapter.hpp"
#include "m4_core.h"
namespace M4_core_thumb16 {
class CORE_REGS : Object{
private:
	uint32_t r[16];
	uint32_t cpsr;
public:
	QString toString(){
		QString rstr = "";
		for (int i = 0; i < 16; i++) {
			rstr = rstr +QString("r%1:").arg(i,0,10)
					+ Int2Hex(r[i]) + " "
					;
		}
	rstr += QString("cpsr:") + Int2Hex(cpsr);
		return rstr;
	}
	CORE_REGS(){
		memset(r, 0, sizeof(r));
		cpsr = 0x173;
	}
	uint32_t& operator [](uint32_t index) {
		AbortAssert(index < 16, QString("index out of bound.%0").arg(index));
		return r[index];
	}
	uint32_t& operator [](int index) {
		AbortAssert(index < 16, QString("index out of bound.%0").arg(index));
		return r[index];
	}
	void updateCpsrByResult(uint32_t r) {
		cpsr |= CPSR_BIT_N & r; //N
		r ? (cpsr &= ~CPSR_BIT_Z):(cpsr |= CPSR_BIT_Z) ; //Z
//		Copy N bit to CPSR from r.
		cpsr &= ~CPSR_BIT_N;
		cpsr |= r & CPSR_BIT_N; //N

	}
	void updateCpsrCarryBit(bool C){
		C ? (cpsr |= CPSR_BIT_C) : (cpsr &= ~CPSR_BIT_C);
	}
};
static uint32_t mask_table(uint32_t i){
	return (1<<i) -1;
}
//static uint32_t BITS(uint32_t d,uint32_t startbit,uint32_t stopbit){
//	AbortAssert(startbit <=31,"");
//	AbortAssert(stopbit <=31,"");
//	return (d & (mask_table(stopbit-startbit+1) << startbit )) >> startbit;
//}
static uint32_t BITS(uint16_t d,uint32_t startbit,uint32_t stopbit){
	AbortAssert(startbit <=16,"");
	AbortAssert(stopbit <=16,"");
	return (d & (mask_table(stopbit-startbit+1) << startbit )) >> startbit;
}
#define LSL_I_CON(x) (BITS(x, 11, 15 ) == 0x00)
#define LSL_R_CON(x) (BITS(x, 6, 15)  ==0x102)


/*
 * * lsr(immediate)
 * 	lsl r0,r1,#2 -> 0x0088 -> 0b0 00010 001 000
 * 	                              #2  r1 r0
 * 	It can optionally update the condition flags based on the result.
 * 	0 0 0 0 0 imm5 Rm Rd
 * 	* lsr(register)
 * 	Logical Shift Right (register) shifts a register value right by a variable number of bits,
 * 	shifting in zeros, and writes the result to the destination register.
 * 	 The variable number of bits is read from the bottom byte of a register.
 * 	 It can optionally update the condition flags based on the result.
 *
 */
uint32_t lsl(CORE_REGS &core_regs, uint16_t instruction) {
	if(LSL_I_CON(instruction)){
		uint16_t imm5 = (instruction & (0b11111 << 6)) >> 6;
		uint32_t rm = (instruction & (0b111 << 3)) >> 3;
		uint32_t rd = (instruction & (0b111 << 0)) >> 0;
		uint32_t fromData = core_regs[rm];
		uint32_t result = fromData << imm5;
		core_regs[rd] = result;
		core_regs.updateCpsrByResult(result);
		((1 << (32 - imm5)) & fromData) ? core_regs.updateCpsrCarryBit(true) :
				core_regs.updateCpsrCarryBit(false);
		return result;
	}//lsr(immediate)
	else if(LSL_R_CON(instruction)){
		uint32_t rm = (instruction & (0b111 << 3)) >> 3;
		uint32_t rd = (instruction & (0b111 << 0)) >> 0;
		uint32_t n = core_regs[rm] & 0xff;
		uint32_t fromData = core_regs[rm];
		uint32_t result = fromData << n;
		core_regs[rd] = result;
		core_regs.updateCpsrByResult(result);
		((1 << (32 - n)) & fromData) ? core_regs.updateCpsrCarryBit(true) :
					core_regs.updateCpsrCarryBit(false);

		return result;
	}
	else{
		AbortAssert(1,Int2Hex(instruction));
		return 0;
	}
}


class M4_Core: Object {
public:
	M4_Core(QString filename) :
			intel_hex_adapter(filename),core_regs() {
		unitTest();
	}
private:
	Intel_HEX_Adapter intel_hex_adapter;
	CORE_REGS core_regs;
	void unitTest() {
		//for lsr lsl r0,r1,#2
		core_regs[0] = 2;
		core_regs[1] = 5;
		lsl(core_regs, 0x788);//lsl r0,r1,#30
		cout << core_regs.toString() << endl;
		//expect r0:0x40000000 r1:0x5 r2:0x0 cpsr:0x20000173
		core_regs[0] = 2;
		core_regs[1] = 5;
		lsl(core_regs, 0x7c8);//lsl r0,r1,#31
		cout << core_regs.toString() << endl;
		//expect r0:0x80000000 r1:0x5 r2:0x0 cpsr:0x80000173
	}
};

}
;

