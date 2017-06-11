/*
 * m4_core.cpp
 *
 *  Created on: 2017?6?11?
 *      Author: kohil
 */
#include "public_header.hpp"
#include "IntelHexAdapter.hpp"

class M4_Core : Object{
public:
	M4_Core(QString filename) :intel_hex_adapter(filename){

	}
	const QString toString(){
		return intel_hex_adapter.toString();
	}
private:
	Intel_HEX_Adapter intel_hex_adapter;
};



