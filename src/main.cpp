/*
 * main.cpp
 *
 *  Created on: 2017?6?11?
 *      Author: kohil
 */

#include "IntelHexAdapter.hpp"

int main(int argc,char *argv[]){
	UNUSE(argc);
	UNUSE(argv);
	//H://workspace_py/test/Debug/
	Intel_HEX_Adapter hex_adapter ("test.hex");
    auto x  = hex_adapter.toBinary();
    auto keys = x.keys();
    for (auto iter = keys.constBegin(); iter != keys.constEnd(); iter ++ ) {
    	cout << Int2Hex(*iter) << " "<< Int2Hex(x[*iter]) << endl;
	}

}


