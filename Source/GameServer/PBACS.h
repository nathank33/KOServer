bool   biguard = 0;
unsigned int radd[29];
unsigned int thadd[29];
unsigned char  opcode[29];
// tmm reyiz

void iGuardLoad() {// böyte iþlemesi lazým ama servera res lazým
	radd[0] = 0x004A90D3;
	opcode[0] = 0x2B;
	thadd[0] = 0x015E1600;
	radd[1] = 0x004A9C16;
	opcode[1] = 0x04;
	thadd[1] = 0x015E1600;
	radd[2] = 0x004DFE54;
	opcode[2] = 0x06;
	thadd[2] = 0x015E1600;
	radd[3] = 0x006E3B58;
	opcode[3] = 0x48;
	thadd[3] = 0x015E1600;
	radd[4] = 0x00868E75;
	opcode[4] = 0x31;
	thadd[4] = 0x015E1600;
	radd[5] = 0x0080E0C8;
	opcode[5] = 0x30;
	thadd[5] = 0x015E1600;
	radd[6] = 0x004E19F7;
	opcode[6] = 0x68;
	thadd[6] = 0x015E1600;
	radd[7] = 0x005F529A;
	opcode[7] = 0x68;
	thadd[7] = 0x015E1600;
	radd[8] = 0x005F532B;
	opcode[8] = 0x68;
	thadd[8] = 0x015E1600;
	radd[9] = 0x004E0E71;
	opcode[9] = 0x30;
	thadd[9] = 0x015E1600;
	radd[10] = 0x006007C2;
	opcode[10] = 0x30;
	thadd[10] = 0x015E1600;
	radd[11] = 0x0080D347;
	opcode[11] = 0x30;
	thadd[11] = 0x015E1600;
	radd[12] = 0x0080D494;
	opcode[12] = 0x30;
	thadd[12] = 0x015E1600;
	radd[13] = 0x0062B054;
	opcode[13] = 0x68;
	thadd[13] = 0x015E1600;
	radd[14] = 0x0062B17A;
	opcode[14] = 0x68;
	thadd[14] = 0x015E1600;
	radd[15] = 0x0062B0B4;
	opcode[15] = 0x68;
	thadd[15] = 0x015E1600;
	radd[16] = 0x0062DE22;
	opcode[16] = 0x68;
	thadd[16] = 0x015E1600;
	radd[17] = 0x005F5113;
	opcode[17] = 0x68;
	thadd[17] = 0x015E1600;
	radd[18] = 0x005F5185;
	opcode[18] = 0x68;
	thadd[18] = 0x015E1600;
	radd[19] = 0x005F51D3;
	opcode[19] = 0x68;
	thadd[19] = 0x015E1600;
	radd[20] = 0x005F5DF7;
	opcode[20] = 0x68;
	thadd[20] = 0x015E1600;
	radd[21] = 0x0062B00A;
	opcode[21] = 0x68;
	thadd[21] = 0x015E1600;
	radd[22] = 0x0062C2C2;
	opcode[22] = 0x68;
	thadd[22] = 0x015E1600;
	radd[23] = 8891056;
	opcode[23] = 0x31;
	thadd[23] = 0x015E1600;//6f68f1   87b42e
	radd[24] = 0x6f68f1;
	opcode[24] = 0x31;
	thadd[24] = 0x015E1600;
	radd[25] = 0x87b42e;
	opcode[25] = 0x31;
	thadd[25] = 0x015E1600;// yeppa
	/*7301853 49
	8441621 48
	8446070 48*/
	radd[26] = 7301853;
	opcode[26] = 49;
	thadd[26] = 0x015E1600;// yeppa

	radd[27] = 8441621;
	opcode[27] = 48;
	thadd[27] = 0x015E1600;// yeppa

	radd[28] = 8446070;
	opcode[28] = 48;
	thadd[28] = 0x015E1600;// yepparadd[25]=0x87b42e;

	biguard = 1;
}

bool checkiguard(unsigned int nerdengeldi, unsigned char command) {
	if (!biguard) iGuardLoad();
	if (nerdengeldi > 0x900000) return false;
	if (command == 0x2b || command == 0x31 || command == 0x30 || command == 0x68 || command == 0x48 || command == 0x06
		|| command == 0x04) {
		for (int i = 0; i < 29; i++) {
			if (radd[i] == nerdengeldi && opcode[i] == command) return true;
		} return false;
	}
	return true;
}