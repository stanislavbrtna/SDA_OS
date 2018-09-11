/*
Copyright (c) 2018 Stanislav Brtna

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "sda_util.h"

void init_kblayout_standard(psvcKbdLayout *l) {
	l->keyStr[0] = (uint8_t*)"1";
	l->key[0]    = (uint8_t*)"1";
	l->keyStr[1] = (uint8_t*)"ě";
	l->key[1]    = (uint8_t*)"ě";
	l->keyStr[2] = (uint8_t*)"š";
	l->key[2]    = (uint8_t*)"š";
	l->keyStr[3] = (uint8_t*)"č";
	l->key[3]    = (uint8_t*)"č";
	l->keyStr[4] = (uint8_t*)"ř";
	l->key[4]    = (uint8_t*)"ř";
	l->keyStr[5] = (uint8_t*)"ž";
	l->key[5]    = (uint8_t*)"ž";
	l->keyStr[6] = (uint8_t*)"ý";
	l->key[6]    = (uint8_t*)"ý";
	l->keyStr[7] = (uint8_t*)"á";
	l->key[7]    = (uint8_t*)"á";
	l->keyStr[8] = (uint8_t*)"í";
	l->key[8]    = (uint8_t*)"í";
	l->keyStr[9] = (uint8_t*)"é";
	l->key[9]    = (uint8_t*)"é";

	l->keyStr[10] = (uint8_t*)"q";
	l->key[10]    = (uint8_t*)"q";
	l->keyStr[11] = (uint8_t*)"w";
	l->key[11]    = (uint8_t*)"w";
	l->keyStr[12] = (uint8_t*)"e";
	l->key[12]    = (uint8_t*)"e";
	l->keyStr[13] = (uint8_t*)"r";
	l->key[13]    = (uint8_t*)"r";
	l->keyStr[14] = (uint8_t*)"t";
	l->key[14]    = (uint8_t*)"t";
	l->keyStr[15] = (uint8_t*)"z";
	l->key[15]    = (uint8_t*)"z";
	l->keyStr[16] = (uint8_t*)"u";
	l->key[16]    = (uint8_t*)"u";
	l->keyStr[17] = (uint8_t*)"i";
	l->key[17]    = (uint8_t*)"i";
	l->keyStr[18] = (uint8_t*)"o";
	l->key[18]    = (uint8_t*)"o";
	l->keyStr[19] = (uint8_t*)"p";
	l->key[19]    = (uint8_t*)"p";

	l->keyStr[20] = (uint8_t*)"a";
	l->key[20]    = (uint8_t*)"a";
	l->keyStr[21] = (uint8_t*)"s";
	l->key[21]    = (uint8_t*)"s";
	l->keyStr[22] = (uint8_t*)"d";
	l->key[22]    = (uint8_t*)"d";
	l->keyStr[23] = (uint8_t*)"f";
	l->key[23]    = (uint8_t*)"f";
	l->keyStr[24] = (uint8_t*)"g";
	l->key[24]    = (uint8_t*)"g";
	l->keyStr[25] = (uint8_t*)"h";
	l->key[25]    = (uint8_t*)"h";
	l->keyStr[26] = (uint8_t*)"j";
	l->key[26]    = (uint8_t*)"j";
	l->keyStr[27] = (uint8_t*)"k";
	l->key[27]    = (uint8_t*)"k";
	l->keyStr[28] = (uint8_t*)"l";
	l->key[28]    = (uint8_t*)"l";

	l->keyStr[29] = (uint8_t*)"y";
	l->key[29]    = (uint8_t*)"y";
	l->keyStr[30] = (uint8_t*)"x";
	l->key[30]    = (uint8_t*)"x";
	l->keyStr[31] = (uint8_t*)"c";
	l->key[31]    = (uint8_t*)"c";
	l->keyStr[32] = (uint8_t*)"v";
	l->key[32]    = (uint8_t*)"v";
	l->keyStr[33] = (uint8_t*)"b";
	l->key[33]    = (uint8_t*)"b";
	l->keyStr[34] = (uint8_t*)"n";
	l->key[34]    = (uint8_t*)"n";
	l->keyStr[35] = (uint8_t*)"m";
	l->key[35]    = (uint8_t*)"m";

	l->keyStr[36] = (uint8_t*)"Prog";
	l->keyStr[37] = (uint8_t*)"Esc";
	l->key[37]    = (uint8_t*)"ignored";
	l->keyStr[38] = (uint8_t*)"Fx";

	l->key[38]    = (uint8_t*)"\x01\x02\x00";
	// value returned with shift key (switch to another layout)
	l->key[39]    = (uint8_t*)"\x01\x01\x00";
}

void init_kblayout_uppcase(psvcKbdLayout *l) {
	l->keyStr[0] = (uint8_t*)"1";
	l->key[0]    = (uint8_t*)"1";
	l->keyStr[1] = (uint8_t*)"2";
	l->key[1]    = (uint8_t*)"2";
	l->keyStr[2] = (uint8_t*)"3";
	l->key[2]    = (uint8_t*)"3";
	l->keyStr[3] = (uint8_t*)"4";
	l->key[3]    = (uint8_t*)"4";
	l->keyStr[4] = (uint8_t*)"5";
	l->key[4]    = (uint8_t*)"5";
	l->keyStr[5] = (uint8_t*)"6";
	l->key[5]    = (uint8_t*)"6";
	l->keyStr[6] = (uint8_t*)"7";
	l->key[6]    = (uint8_t*)"7";
	l->keyStr[7] = (uint8_t*)"8";
	l->key[7]    = (uint8_t*)"8";
	l->keyStr[8] = (uint8_t*)"9";
	l->key[8]    = (uint8_t*)"9";
	l->keyStr[9] = (uint8_t*)"0";
	l->key[9]    = (uint8_t*)"0";

	l->keyStr[10] = (uint8_t*)"Q";
	l->key[10]    = (uint8_t*)"Q";
	l->keyStr[11] = (uint8_t*)"W";
	l->key[11]    = (uint8_t*)"W";
	l->keyStr[12] = (uint8_t*)"E";
	l->key[12]    = (uint8_t*)"E";
	l->keyStr[13] = (uint8_t*)"R";
	l->key[13]    = (uint8_t*)"R";
	l->keyStr[14] = (uint8_t*)"T";
	l->key[14]    = (uint8_t*)"T";
	l->keyStr[15] = (uint8_t*)"Z";
	l->key[15]    = (uint8_t*)"Z";
	l->keyStr[16] = (uint8_t*)"U";
	l->key[16]    = (uint8_t*)"U";
	l->keyStr[17] = (uint8_t*)"I";
	l->key[17]    = (uint8_t*)"I";
	l->keyStr[18] = (uint8_t*)"O";
	l->key[18]    = (uint8_t*)"O";
	l->keyStr[19] = (uint8_t*)"P";
	l->key[19]    = (uint8_t*)"P";

	l->keyStr[20] = (uint8_t*)"A";
	l->key[20]    = (uint8_t*)"A";
	l->keyStr[21] = (uint8_t*)"S";
	l->key[21]    = (uint8_t*)"S";
	l->keyStr[22] = (uint8_t*)"D";
	l->key[22]    = (uint8_t*)"D";
	l->keyStr[23] = (uint8_t*)"F";
	l->key[23]    = (uint8_t*)"F";
	l->keyStr[24] = (uint8_t*)"G";
	l->key[24]    = (uint8_t*)"G";
	l->keyStr[25] = (uint8_t*)"H";
	l->key[25]    = (uint8_t*)"H";
	l->keyStr[26] = (uint8_t*)"J";
	l->key[26]    = (uint8_t*)"J";
	l->keyStr[27] = (uint8_t*)"K";
	l->key[27]    = (uint8_t*)"K";
	l->keyStr[28] = (uint8_t*)"L";
	l->key[28]    = (uint8_t*)"L";

	l->keyStr[29] = (uint8_t*)"Y";
	l->key[29]    = (uint8_t*)"Y";
	l->keyStr[30] = (uint8_t*)"X";
	l->key[30]    = (uint8_t*)"X";
	l->keyStr[31] = (uint8_t*)"C";
	l->key[31]    = (uint8_t*)"C";
	l->keyStr[32] = (uint8_t*)"V";
	l->key[32]    = (uint8_t*)"V";
	l->keyStr[33] = (uint8_t*)"B";
	l->key[33]    = (uint8_t*)"B";
	l->keyStr[34] = (uint8_t*)"N";
	l->key[34]    = (uint8_t*)"N";
	l->keyStr[35] = (uint8_t*)"M";
	l->key[35]    = (uint8_t*)"M";

	l->keyStr[36] = (uint8_t*)"Prog";
	l->keyStr[37] = (uint8_t*)"Esc";
	l->key[37]    = (uint8_t*)"ignored";
	l->keyStr[38] = (uint8_t*)"Fx";
	l->key[38]    = (uint8_t*)"\x01\x02\x00";
	// value returned with shift key (switch to another layout)
	l->key[39]    = (uint8_t*)"\x01\x00";
}


void init_kblayout_special(psvcKbdLayout *l) {
	l->keyStr[0] = (uint8_t*)"+";
	l->key[0]    = (uint8_t*)"+";
	l->keyStr[1] = (uint8_t*)"@";
	l->key[1]    = (uint8_t*)"@";
	l->keyStr[2] = (uint8_t*)"#";
	l->key[2]    = (uint8_t*)"#";
	l->keyStr[3] = (uint8_t*)"$";
	l->key[3]    = (uint8_t*)"$";
	l->keyStr[4] = (uint8_t*)"%";
	l->key[4]    = (uint8_t*)"%";
	l->keyStr[5] = (uint8_t*)"^";
	l->key[5]    = (uint8_t*)"^";
	l->keyStr[6] = (uint8_t*)"&";
	l->key[6]    = (uint8_t*)"&";
	l->keyStr[7] = (uint8_t*)"*";
	l->key[7]    = (uint8_t*)"*";
	l->keyStr[8] = (uint8_t*)";";
	l->key[8]    = (uint8_t*)";";
	l->keyStr[9] = (uint8_t*)"-";
	l->key[9]    = (uint8_t*)"-";

	l->keyStr[10] = (uint8_t*)"=";
	l->key[10]    = (uint8_t*)"=";
	l->keyStr[11] = (uint8_t*)"ě";
	l->key[11]    = (uint8_t*)"ě";
	l->keyStr[12] = (uint8_t*)"é";
	l->key[12]    = (uint8_t*)"é";
	l->keyStr[13] = (uint8_t*)"ř";
	l->key[13]    = (uint8_t*)"ř";
	l->keyStr[14] = (uint8_t*)"ť";
	l->key[14]    = (uint8_t*)"ť";
	l->keyStr[15] = (uint8_t*)"ž";
	l->key[15]    = (uint8_t*)"ž";
	l->keyStr[16] = (uint8_t*)"ú";
	l->key[16]    = (uint8_t*)"ú";
	l->keyStr[17] = (uint8_t*)"í";
	l->key[17]    = (uint8_t*)"í";
	l->keyStr[18] = (uint8_t*)"ó";
	l->key[18]    = (uint8_t*)"ó";
	l->keyStr[19] = (uint8_t*)"_";
	l->key[19]    = (uint8_t*)"_";

	l->keyStr[20] = (uint8_t*)"á";
	l->key[20]    = (uint8_t*)"á";
	l->keyStr[21] = (uint8_t*)"š";
	l->key[21]    = (uint8_t*)"š";
	l->keyStr[22] = (uint8_t*)"ď";
	l->key[22]    = (uint8_t*)"ď";
	l->keyStr[23] = (uint8_t*)"\"";
	l->key[23]    = (uint8_t*)"\"";
	l->keyStr[24] = (uint8_t*)"!";
	l->key[24]    = (uint8_t*)"!";
	l->keyStr[25] = (uint8_t*)"?";
	l->key[25]    = (uint8_t*)"?";
	l->keyStr[26] = (uint8_t*)"ů";
	l->key[26]    = (uint8_t*)"ů";
	l->keyStr[27] = (uint8_t*)".";
	l->key[27]    = (uint8_t*)".";
	l->keyStr[28] = (uint8_t*)":";
	l->key[28]    = (uint8_t*)":";

	l->keyStr[29] = (uint8_t*)"ý";
	l->key[29]    = (uint8_t*)"ý";
	l->keyStr[30] = (uint8_t*)",";
	l->key[30]    = (uint8_t*)",";
	l->keyStr[31] = (uint8_t*)"č";
	l->key[31]    = (uint8_t*)"č";
	l->keyStr[32] = (uint8_t*)"/";
	l->key[32]    = (uint8_t*)"/";
	l->keyStr[33] = (uint8_t*)"\\";
	l->key[33]    = (uint8_t*)"\\";
	l->keyStr[34] = (uint8_t*)"ň";
	l->key[34]    = (uint8_t*)"ň";
	l->keyStr[35] = (uint8_t*)"'";
	l->key[35]    = (uint8_t*)"'";

	l->keyStr[36] = (uint8_t*)"Prog";
	l->keyStr[37] = (uint8_t*)"Esc";
	l->key[37]    = (uint8_t*)"ignored";
	l->keyStr[38] = (uint8_t*)"Ab";
	l->key[38]    = (uint8_t*)"\x01\x00";
	l->key[39]    = (uint8_t*)"\x01\x03\x00";
}


void init_kblayout_special_shift(psvcKbdLayout *l) {
	l->keyStr[0] = (uint8_t*)"(";
	l->key[0]    = (uint8_t*)"(";
	l->keyStr[1] = (uint8_t*)")";
	l->key[1]    = (uint8_t*)")";
	l->keyStr[2] = (uint8_t*)"<";
	l->key[2]    = (uint8_t*)"<";
	l->keyStr[3] = (uint8_t*)">";
	l->key[3]    = (uint8_t*)">";
	l->keyStr[4] = (uint8_t*)"{";
	l->key[4]    = (uint8_t*)"{";
	l->keyStr[5] = (uint8_t*)"}";
	l->key[5]    = (uint8_t*)"}";
	l->keyStr[6] = (uint8_t*)"~";
	l->key[6]    = (uint8_t*)"~";
	l->keyStr[7] = (uint8_t*)"|";
	l->key[7]    = (uint8_t*)"|";
	l->keyStr[8] = (uint8_t*)";";
	l->key[8]    = (uint8_t*)";";
	l->keyStr[9] = (uint8_t*)"-";
	l->key[9]    = (uint8_t*)"-";

	l->keyStr[10] = (uint8_t*)"=";
	l->key[10]    = (uint8_t*)"=";
	l->keyStr[11] = (uint8_t*)"Ě";
	l->key[11]    = (uint8_t*)"Ě";
	l->keyStr[12] = (uint8_t*)"É";
	l->key[12]    = (uint8_t*)"É";
	l->keyStr[13] = (uint8_t*)"Ř";
	l->key[13]    = (uint8_t*)"Ř";
	l->keyStr[14] = (uint8_t*)"Ť";
	l->key[14]    = (uint8_t*)"Ť";
	l->keyStr[15] = (uint8_t*)"Ž";
	l->key[15]    = (uint8_t*)"Ž";
	l->keyStr[16] = (uint8_t*)"Ú";
	l->key[16]    = (uint8_t*)"Ú";
	l->keyStr[17] = (uint8_t*)"Í";
	l->key[17]    = (uint8_t*)"Í";
	l->keyStr[18] = (uint8_t*)"Ó";
	l->key[18]    = (uint8_t*)"Ó";
	l->keyStr[19] = (uint8_t*)"_";
	l->key[19]    = (uint8_t*)"_";

	l->keyStr[20] = (uint8_t*)"Á";
	l->key[20]    = (uint8_t*)"Á";
	l->keyStr[21] = (uint8_t*)"Š";
	l->key[21]    = (uint8_t*)"Š";
	l->keyStr[22] = (uint8_t*)"Ď";
	l->key[22]    = (uint8_t*)"Ď";
	l->keyStr[23] = (uint8_t*)"\"";
	l->key[23]    = (uint8_t*)"\"";
	l->keyStr[24] = (uint8_t*)"!";
	l->key[24]    = (uint8_t*)"!";
	l->keyStr[25] = (uint8_t*)"?";
	l->key[25]    = (uint8_t*)"?";
	l->keyStr[26] = (uint8_t*)"Ů";
	l->key[26]    = (uint8_t*)"Ů";
	l->keyStr[27] = (uint8_t*)".";
	l->key[27]    = (uint8_t*)".";
	l->keyStr[28] = (uint8_t*)":";
	l->key[28]    = (uint8_t*)":";

	l->keyStr[29] = (uint8_t*)"Ý";
	l->key[29]    = (uint8_t*)"Ý";
	l->keyStr[30] = (uint8_t*)",";
	l->key[30]    = (uint8_t*)",";
	l->keyStr[31] = (uint8_t*)"Č";
	l->key[31]    = (uint8_t*)"Č";
	l->keyStr[32] = (uint8_t*)"/";
	l->key[32]    = (uint8_t*)"/";
	l->keyStr[33] = (uint8_t*)"\\";
	l->key[33]    = (uint8_t*)"\\";
	l->keyStr[34] = (uint8_t*)"Ň";
	l->key[34]    = (uint8_t*)"Ň";
	l->keyStr[35] = (uint8_t*)"'";
	l->key[35]    = (uint8_t*)"'";

	l->keyStr[36] = (uint8_t*)"Prog";
	l->keyStr[37] = (uint8_t*)"Esc";
	l->key[37]    = (uint8_t*)"ignored";
	l->keyStr[38] = (uint8_t*)"Ab";
	l->key[38]    = (uint8_t*)"\x01\x00";
	l->key[39]    = (uint8_t*)"\x01\x02\x00";
}
