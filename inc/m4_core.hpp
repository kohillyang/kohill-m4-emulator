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
			rstr = rstr
					+ QString("r%1:").arg(i,0,10)
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



#define LSL_I_CON(x) (BITS(x, 11, 15 ) == 0x00)
#define LSL_R_CON(x) (BITS(x, 6, 15)  ==0x102)
#define LSR_I_CON(x) (BITS(instruction,11,15)== 0x01)
#define LSR_R_CON(x) (BITS(instruction,6,15)== 0x103)

uint32_t lsl(CORE_REGS &core_regs, uint16_t instruction);


class M4_Core: Object {
public:
	M4_Core(QString filename) : intel_hex_adapter(filename),core_regs() {
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
		cout << "0x788 "<< core_regs.toString() << endl;
		//expect r0:0x40000000 r1:0x5 r2:0x0 cpsr:0x20000173
		core_regs[0] = 2;
		core_regs[1] = 5;
		lsl(core_regs, 0x7c8);//lsl r0,r1,#31
		cout << "0x7c8 " << core_regs.toString() << endl;
		//expect r0:0x80000000 r1:0x5 r2:0x0 cpsr:0x80000173
		core_regs[0] = 2;
		lsl(core_regs, 0x4088);
		core_regs[1] = 5;
		cout << "0x4088 " << core_regs.toString() << endl;
		//expect r0:0x40   r1:0x5
	}
};

}
;

