/************************************************************************************************************************************************************************************
Taylor Mitchell
Mano's Computer Simulator

I decided to keep all of the class files in this one file for compactness, but that made it fairly long.  I wrote this in C++ because I was feeling kind of rusty in 
it and I wanted to make sure that it stayed fresh in my mind.  It ended up being more procedural rather than object oriented, although there are a couple of classes that I 
used.  The weather program will be much more O-O, I promise.  The output is kind of bare-bones, but I think it gets the job done.  Next time I will make it show what its doing in 3D 
and only compatible with a VR headset. 

And without further ado, here it is!
**************************************************************************************************************************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <bitset>

const int MEM_SIZE = 4096;

unsigned createMask(unsigned, unsigned);

void outputLine(int);

void indirect(std::string&);

void t0();
void t1();
void t2();
void t3();

void CLA();
void CLE();
void CMS();
void CME();
void CIR();
void CIL();
void INC();
void SPA();
void SNA();
void SZA();
void SZE();
void HLT();
void AND();
void ADD();
void LDA();
void STA();
void BUN();
void BSA();
void ISZ();
void IO();

struct Int4 {
	/*****************************************************************************************************************************************************
	This is just a quick struct that holds an unsigned int that only takes up 4 bits	
	******************************************************************************************************************************************************/
	unsigned int val : 4;
};

class Int16 {
	/*****************************************************************************************************************************************************
	This class holds a 16-bit int and can split the int up into 4 4-bit ints
	******************************************************************************************************************************************************/
	Int4 hx1;
	Int4 hx2;
	Int4 hx3;
	Int4 hx4;

	int val;

public:
	Int16(Int4 h1, Int4 h2, Int4 h3, Int4 h4);
	Int16();
	Int16(int v);
	Int16(const Int16*);
	Int16& operator=(const Int16&);
	Int16& operator=(int);

	int getVal() { return val; };
	Int4 getHex1() { return hx1; };
	Int4 getHex2() { return hx2; };
	Int4 getHex3() { return hx3; };
	Int4 getHex4() { return hx4; };

	void print();
};

Int16::Int16() {
	/*****************************************************************************************************************************************************
	Constructor
	******************************************************************************************************************************************************/
	hx1 = { 0x0 };
	hx2 = { 0x0 };
	hx3 = { 0x0 };
	hx4 = { 0x0 };
	val = 0;
}

Int16::Int16(const Int16* rhs) {
	/*****************************************************************************************************************************************************
	Copy Constructor
	******************************************************************************************************************************************************/
	hx1 = rhs->hx1;
	hx2 = rhs->hx2;
	hx3 = rhs->hx3;
	hx4 = rhs->hx4;
	val = rhs->val;
}

Int16::Int16(Int4 h1, Int4 h2, Int4 h3, Int4 h4) {
	/*****************************************************************************************************************************************************
	Constructs a new Int16 using 4 Int4's and ensures that each of the Int4's get into the right spot 
	******************************************************************************************************************************************************/
	hx1 = h1;
	hx2 = h2;
	hx3 = h3;
	hx4 = h4;

	val = (hx1.val << 12) | (hx2.val << 8) | (hx3.val << 4) | hx4.val;
}

Int16::Int16(int v) {
	/*****************************************************************************************************************************************************
	Constructs a new Int16 using a regular int.  It splits the int up into 4 and assigns them to the correct Int4's.
	******************************************************************************************************************************************************/
	unsigned mask = createMask(0, 16);
	val = mask & v;

	mask = createMask(12, 16);

	hx1 = { (val & mask) >> 12 };

	mask = createMask(8, 12);
	hx2 = { (val & mask) >> 8 };

	mask = createMask(4, 8);
	hx3 = { (val & mask) >> 4 };

	mask = createMask(0, 28);
	hx4 = { val & mask };
}

Int16& Int16::operator=(const Int16& rhs) {
	/*****************************************************************************************************************************************************
	I think this is how the generic '=' operator works anyway but I put it here just to be safe.
	******************************************************************************************************************************************************/
	hx1 = rhs.hx1;
	hx2 = rhs.hx2;
	hx3 = rhs.hx3;
	hx4 = rhs.hx4;
	val = rhs.val;

	return *this;
}

Int16& Int16::operator=(int v) {
	/*****************************************************************************************************************************************************
	This is so I can make a quick conversion from an int with an already made Int16.  It does the same thing as the int Constructor above.
	******************************************************************************************************************************************************/
	unsigned mask = createMask(0, 16);
	val = mask & v;

	mask = createMask(12, 16);
	
	hx1 = { (val & mask) >> 12 };

	mask = createMask(8, 12);
	hx2 = { (val & mask) >> 8 };

	mask = createMask(4, 8);
	hx3 = { (val & mask) >> 4 };

	mask = createMask(0, 28);
	hx4 = { val & mask };

	return *this;
}

void Int16::print() {
	/*****************************************************************************************************************************************************
	I'll bet you can guess...
	******************************************************************************************************************************************************/
	std::cout << "Decimal Value - " << std::dec << val << std::endl;
	std::cout << "Hex Value - " << std::hex << hx1.val << hx2.val << hx3.val << hx4.val << std::endl;
}

class Register {
	/*****************************************************************************************************************************************************
	Just a Register class that holds an Int16 and other register signals
	******************************************************************************************************************************************************/
	Int16 val;
	std::string name;
	std::string busAddress;
	bool load, incr, clear;

public:
	Register(std::string, std::string);

	Register(Register*);
	
	Register();

	void increment();

	Int16 getVal() { return val; };
	int getInt() { return val.getVal(); };
	std::string getName() { return name; };
	std::string getBusAddress() { return busAddress; };
	void changeBusAddress(std::string);
	bool getLD() { return load; };
	bool getINR() { return incr; };
	bool getCLR() { return clear; };
	void clearSignals() { load = false; incr = false; clear = false; };
	void clr() { clear = true; loadVal(0); };

	void loadVal(int);
	void loadVal(Int16);

	Register& operator=(const Register&);

	void print();
};

Register::Register() {
	/*****************************************************************************************************************************************************
	Constructor
	******************************************************************************************************************************************************/
	val = 0;
	name = "";
	busAddress = "000";
	load = false;
	incr = false;
	clear = false;
}

Register::Register(Register* in) {
	/*****************************************************************************************************************************************************
	Copy Constructor
	******************************************************************************************************************************************************/
	val = in->getVal();
	name = in->getName();
	busAddress = in->getBusAddress();
	load = in->getLD();
	incr = in->getINR();
	clear = in->getCLR();
}

Register::Register(std::string n, std::string bA) {
	/*****************************************************************************************************************************************************
	Constructor with a name and a bus address
	******************************************************************************************************************************************************/
	name = n;
	busAddress = bA;
	load = false;
	incr = false;
	clear = false;
}

Register& Register::operator=(const Register& rhs) {
	/*****************************************************************************************************************************************************
	Again, added this just in case
	******************************************************************************************************************************************************/
	val = rhs.val;
	name = rhs.name;
	busAddress = rhs.busAddress;
	load = rhs.load;
	incr = rhs.incr;
	clear = rhs.clear;

	return *this;
}

void Register::increment() {
	/*****************************************************************************************************************************************************
	Uses the '=' operator defined above to increment the Int16
	******************************************************************************************************************************************************/
	val = val.getVal() + 1;
	incr = true;
}

void Register::loadVal(int v) {
	/*****************************************************************************************************************************************************
	Loads a value from an int.  
	******************************************************************************************************************************************************/
	val = v;
}

void Register::loadVal(Int16 v) {
	/*****************************************************************************************************************************************************
	Loads a value from an Int16
	******************************************************************************************************************************************************/
	val = v;
	load = true;
}

void Register::print() {
	/*****************************************************************************************************************************************************
	Prints all values...
	******************************************************************************************************************************************************/
	std::cout << name << std::endl;
	val.print();
	std::cout << std::endl;
}

void Register::changeBusAddress(std::string bA) {
	/*****************************************************************************************************************************************************
	Changes the bus address.  This is really only for the BUS register.
	******************************************************************************************************************************************************/
	busAddress = bA;
}

/*****************************************************************************************************************************************************
I decided to make MEM[AR] and E into full registers just for ease and simplicity.  Even though the MEM[AR] register isn't actually the same exact memory
location as the value it represents in memory, I made sure to keep it updated correctly for all cases.
******************************************************************************************************************************************************/
Register AR = new Register("AR", "001");
Register PC = new Register("PC", "010");
Register DR = new Register("DR", "011");
Register AC = new Register("AC", "100");
Register IR = new Register("IR", "101");
Register TR = new Register("TR", "110");
Register MEM_AR = new Register("MEM[AR]", "111");
Register E = new Register("E", "000");
Register BUS = new Register("BUS", "000");

std::vector<Int16> memory;

void(*instruction)();

bool halt = false;
bool direct = false;

std::string arrow = "\t||\n\t\\/\n";
std::string line = "--------------------\n";

int main() {
	srand(time(NULL));

	memory.reserve(MEM_SIZE);

	for (int i = 0; i < MEM_SIZE; i++) {
		/*****************************************************************************************************************************************************
		This for statement populates the memory according to the project guidelines.
		******************************************************************************************************************************************************/
		Int4 h1 = { rand() % 0xF };
		Int4 h2;
		Int4 h3;
		Int4 h4;

		if (h1.val == 0x7) {
			switch (rand() % 12) {
			case 0:
				h2 = { 0x8 };
				h3 = { 0x0 };
				h4 = { 0x0 };
				break;
			case 1:
				h2 = { 0x4 };
				h3 = { 0x0 };
				h4 = { 0x0 };
				break;
			case 2:
				h2 = { 0x2 };
				h3 = { 0x0 };
				h4 = { 0x0 };
				break;
			case 3:
				h2 = { 0x1 };
				h3 = { 0x0 };
				h4 = { 0x0 };
				break;
			case 4:
				h2 = { 0x0 };
				h3 = { 0x8 };
				h4 = { 0x0 };
				break;
			case 5:
				h2 = { 0x0 };
				h3 = { 0x4 };
				h4 = { 0x0 };
				break;
			case 6:
				h2 = { 0x0 };
				h3 = { 0x2 };
				h4 = { 0x0 };
				break;
			case 7:
				h2 = { 0x0 };
				h3 = { 0x1 };
				h4 = { 0x0 };
				break;
			case 8:
				h2 = { 0x0 };
				h3 = { 0x0 };
				h4 = { 0x8 };
				break;
			case 9:
				h2 = { 0x0 };
				h3 = { 0x0 };
				h4 = { 0x4 };
				break;
			case 10:
				h2 = { 0x0 };
				h3 = { 0x0 };
				h4 = { 0x2 };
				break;
			case 11:
				h2 = { 0x0 };
				h3 = { 0x0 };
				h4 = { 0x1 };
				break;
			}
		}
		else {
			h2 = { (unsigned int)(rand() % 0xF) };
			h3 = { (unsigned int)(rand() % 0xF) };
			h4 = { (unsigned int)(rand() % 0xF) };
		}

		memory.push_back(new Int16(h1, h2, h3, h4));
	}

	for (int j = 0; j < 2; j++) {
		PC.loadVal(rand() % (MEM_SIZE - 5));

		int count = 0;

		while (!halt && count < 5) {
			/*****************************************************************************************************************************************************
			Main Program Loop
			******************************************************************************************************************************************************/
			std::cout << "Instruction #" << count + 1 << std::endl;
			t0();
			t1();
			t2();
			t3();
			count++;

			std::cout << "Press enter to execute next instruction.\n";
			std::cin.ignore();
		}

		std::cout << "--New random PC value--\n";
	}
}

unsigned createMask(unsigned start, unsigned finish) {
	/*****************************************************************************************************************************************************
	This is just a simple bitmask function to set certain bits of a number to 0's
	******************************************************************************************************************************************************/
	unsigned toReturn = 0;

	for (unsigned i = start; i < finish; i++) {
		toReturn |= 1 << i;
	}

	return toReturn;
}

void t0() {
	/*****************************************************************************************************************************************************
	Performs all of the actions at t0 in the instruction cycle.  The display of all of these instructions is a little clunky, but I don't know a better way
	without building a window.
	******************************************************************************************************************************************************/
	BUS.changeBusAddress(PC.getBusAddress());
	BUS.loadVal(PC.getVal());

	AR.loadVal(BUS.getVal());

	MEM_AR.loadVal(memory.at(AR.getInt()));

	outputLine(0);
}

void t1() {
	/*****************************************************************************************************************************************************
	Performs all of the actions at t1 in the instruction cycle.
	******************************************************************************************************************************************************/
	PC.increment();

	BUS.changeBusAddress(MEM_AR.getBusAddress());
	BUS.loadVal(MEM_AR.getVal());

	IR.loadVal(BUS.getVal());

	outputLine(1);
}

void t2() {
	/*****************************************************************************************************************************************************
	Since this is the decode phase, it is a little more complicated.  I used a function pointer and large nested switch statements to define the correct
	instruction.
	******************************************************************************************************************************************************/
	AR.loadVal(new Int16({ 0 }, { IR.getVal().getHex2() }, { IR.getVal().getHex3() }, { IR.getVal().getHex4() }));

	MEM_AR.loadVal(memory.at(AR.getInt()));

	switch (IR.getVal().getHex1().val) {
	case 0:
		instruction = &AND;
		direct = true;
		break;
	case 1:
		instruction = &ADD;
		direct = true;
		break;
	case 2:
		instruction = &LDA;
		direct = true;
		break;
	case 3:
		instruction = &STA;
		direct = true;
		break;
	case 4:
		instruction = &BUN;
		direct = true;
		break;
	case 5:
		instruction = &BSA;
		direct = true;
		break;
	case 6:
		instruction = &ISZ;
		direct = true;
		break;
	case 7:
		switch (IR.getInt()) {
		case 0x7800:
			instruction = &CLA;
			break;
		case 0x7400:
			instruction = &CLE;
			break;
		case 0x7200:
			instruction = &CMS;
			break;
		case 0x7100:
			instruction = &CME;
			break;
		case 0x7080:
			instruction = &CIR;
			break;
		case 0x7040:
			instruction = &CIL;
			break;
		case 0x7020:
			instruction = &INC;
			break;
		case 0x7010:
			instruction = &SPA;
			break;
		case 0x7008:
			instruction = &SNA;
			break;
		case 0x7004:
			instruction = &SZA;
			break;
		case 0x7002:
			instruction = &SZE;
			break;
		case 0x7001:
			instruction = &HLT;
			break;
		}
		break;
	case 8:
		instruction = &AND;
		direct = false;
		break;
	case 9:
		instruction = &ADD;
		direct = false;
		break;
	case 10:
		instruction = &LDA;
		direct = false;
		break;
	case 11:
		instruction = &STA;
		direct = false;
		break;
	case 12:
		instruction = &BUN;
		direct = false;
		break;
	case 13:
		instruction = &BSA;
		direct = false;
		break;
	case 14:
		instruction = &ISZ;
		direct = false;
		break;
	case 15:
		instruction = &IO;
		break;
	}

	outputLine(2);
}

void t3() {
	/*****************************************************************************************************************************************************
	Just executes whatever function the function pointer holds.  Since instructions use different phases in the instruction cycle, I let the individual 
	functions print out all of the display
	******************************************************************************************************************************************************/
	instruction();
}

void CLA(){
	/*****************************************************************************************************************************************************
	Clears the AC
	******************************************************************************************************************************************************/
	AC.clr();

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "CLA executed\n";	
}

void CLE(){
	/*****************************************************************************************************************************************************
	Clears E
	******************************************************************************************************************************************************/
	E.loadVal(0);

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "CLE executed\n";
}

void CMS(){
	/*****************************************************************************************************************************************************
	Complements AC
	******************************************************************************************************************************************************/
	AC.loadVal(~AC.getInt());

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "CMS executed\n";
}

void CME() {
	/*****************************************************************************************************************************************************
	Complements E
	******************************************************************************************************************************************************/
	E.loadVal(~E.getInt());

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "CME executed\n";
}

void CIR() {
	/*****************************************************************************************************************************************************
	Circular rotate right on AC and E
	******************************************************************************************************************************************************/
	int temp = E.getInt();
	if (AC.getInt() % 2 == 1) {
		E.loadVal(1);
	}else{
		E.loadVal(0);
	}

	int mask = 1 << (sizeof(int) * CHAR_BIT - 1);
	AC.loadVal(temp ? mask | (AC.getInt() >> 1) : AC.getInt() >> 1);

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "CIR executed\n";
}

void CIL() {
	/*****************************************************************************************************************************************************
	Circular rotate left on AC and E
	******************************************************************************************************************************************************/
	int temp = E.getInt();
	if (AC.getVal().getHex1().val > 7) {
		E.loadVal(1);
	}
	else {
		E.loadVal(0);
	}

	int mask = 1;
	AC.loadVal(temp ? mask | (AC.getInt() << 1) : AC.getInt() << 1);

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "CIL executed\n";
}

void INC() {
	/*****************************************************************************************************************************************************
	Increments the AC
	******************************************************************************************************************************************************/
	AC.increment();

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "INC executed\n";
}

void SPA() {
	/*****************************************************************************************************************************************************
	Increments PC if AC is positive
	******************************************************************************************************************************************************/
	if (AC.getInt() >= 0) {
		PC.increment();
	}

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "SPA executed\n";
}

void SNA() {
	/*****************************************************************************************************************************************************
	Increments PC if AC is negative
	******************************************************************************************************************************************************/
	if (AC.getInt() < 0) {
		PC.increment();
	}

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "SNA executed\n";
}

void SZA() {
	/*****************************************************************************************************************************************************
	Increments PC if AC is zero
	******************************************************************************************************************************************************/
	if (AC.getInt() == 0) {
		PC.increment();
	}

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "SZA executed\n";
}

void SZE() {
	/*****************************************************************************************************************************************************
	Increments PC if E is zero
	******************************************************************************************************************************************************/
	if (E.getInt() == 0) {
		PC.increment();
	}

	outputLine(3);
	outputLine(4);
	outputLine(5);
	std::cout << "SZE executed\n";
}

void HLT() {
	/*****************************************************************************************************************************************************
	Stops the program
	******************************************************************************************************************************************************/
	halt = true;

	std::cout << "Program has been ended\n";
}

void AND() {
	/*****************************************************************************************************************************************************
	Gets the appropriate value from memory and ands it with the value in AC
	******************************************************************************************************************************************************/
	std::string inst = "AND-";
	indirect(inst);

	BUS.changeBusAddress(MEM_AR.getBusAddress());
	BUS.loadVal(MEM_AR.getVal());	

	DR.loadVal(BUS.getVal());

	outputLine(4);

	AC.loadVal(AC.getInt() & DR.getInt());	
	
	outputLine(5);
	std::cout << inst + "executed\n";
}

void ADD() {
	/*****************************************************************************************************************************************************
	Gets the appropriate value from memory and adds it with the value in AC
	******************************************************************************************************************************************************/
	std::string inst = "ADD-";
	indirect(inst);

	BUS.changeBusAddress(MEM_AR.getBusAddress());
	BUS.loadVal(MEM_AR.getVal());	

	DR.loadVal(BUS.getVal());

	outputLine(4);

	int sum = DR.getInt() + AC.getInt();
	if (sum >= 0xffff) {
		unsigned mask = createMask(0, 16);
		sum = sum & mask;

		E.loadVal(1);
	}

	AC.loadVal(sum);

	outputLine(5);
	std::cout << inst + "executed\n";
}

void LDA() {
	/*****************************************************************************************************************************************************
	Gets the appropriate value from memory and loads it into AC
	******************************************************************************************************************************************************/
	std::string inst = "LDA-";
	indirect(inst);

	BUS.changeBusAddress(MEM_AR.getBusAddress());
	BUS.loadVal(MEM_AR.getVal());

	DR.loadVal(BUS.getVal());

	outputLine(4);

	AC.loadVal(DR.getVal());

	outputLine(5);
	std::cout << inst + "executed\n";
}

void STA() {
	/*****************************************************************************************************************************************************
	Stores the value of AC into memory
	******************************************************************************************************************************************************/
	std::string inst = "STA-";
	indirect(inst);

	BUS.changeBusAddress(AC.getBusAddress());
	BUS.loadVal(AC.getVal());

	memory.at(AR.getInt()) = BUS.getInt();
	MEM_AR.loadVal(memory.at(AR.getInt()));
	outputLine(4);
	outputLine(5);
	std::cout << inst + "executed\n";
}

void BUN() {
	/*****************************************************************************************************************************************************
	Moves the PC to the value in AR
	******************************************************************************************************************************************************/
	std::string inst = "BUN-";
	indirect(inst);

	PC.loadVal(AR.getVal());

	outputLine(4);
	outputLine(5);	
	std::cout << inst + "executed\n";
}

void BSA() {
	/*****************************************************************************************************************************************************
	Saves the PC value into memory and moves the PC to the value in AR
	******************************************************************************************************************************************************/
	std::string inst = "BSA-";
	indirect(inst);

	BUS.changeBusAddress(PC.getBusAddress());
	BUS.loadVal(PC.getVal());

	memory.at(AR.getInt()) = BUS.getInt();
	MEM_AR.loadVal(memory.at(AR.getInt()));

	AR.increment();	

	MEM_AR.loadVal(memory.at(AR.getInt()));

	outputLine(4);

	PC.loadVal(AR.getVal());

	outputLine(5);
	std::cout << inst + "executed\n";
}

void ISZ() {
	/*****************************************************************************************************************************************************
	Increments DR and incrments PC if DR = 0
	******************************************************************************************************************************************************/
	std::string inst = "ISZ-";
	indirect(inst);

	BUS.changeBusAddress(PC.getBusAddress());
	BUS.loadVal(MEM_AR.getVal());

	DR.loadVal(BUS.getVal());

	outputLine(4);

	DR.increment();

	outputLine(5);

	BUS.changeBusAddress(DR.getBusAddress());
	BUS.loadVal(DR.getVal());

	memory.at(AR.getInt()) = BUS.getInt();
	MEM_AR.loadVal(memory.at(AR.getInt()));

	if (DR.getInt() == 0) {
		PC.increment();
	}	

	outputLine(6);
	std::cout << inst + "executed\n";
}

void IO() {
	/*****************************************************************************************************************************************************
	This is just a filler function for all input/output instructions
	******************************************************************************************************************************************************/
	std::cout << "INPUT/OUTPUT INSTRUCTION HERE\n";
}

void indirect(std::string &inst) {
	if (!direct) {
		inst += "Indirect ";
		BUS.changeBusAddress(MEM_AR.getBusAddress());
		BUS.loadVal(MEM_AR.getVal());

		AR.loadVal(new Int16({ 0 }, { BUS.getVal().getHex2() }, { BUS.getVal().getHex3() }, { BUS.getVal().getHex4() }));

		MEM_AR.loadVal(memory.at(AR.getInt()));
	}
	else {
		inst += "Direct ";
	}

	outputLine(3);
}

void outputLine(int t) {
	if (t == 0) {
		std::cout << "t(x) | AC   - LD - INR - CLR | DR   - LD - INR - CLR | PC   - LD - INR - CLR | AR   - LD - INR - CLR | M[AR] | IR   - LD | E | S(0-2)\n";
		std::cout << "---------------------------------------------------------------------------------------------------------------------------------------\n";
	}
	std::cout << "t(" << t << ") | ";
	std::cout << std::left << std::hex << std::setw(4) << AC.getInt() << " -  " << AC.getLD() << " -   " << AC.getINR() << " -   " << AC.getCLR() << " | ";
	std::cout << std::setw(4) << DR.getInt() << " -  " << DR.getLD() << " -   " << DR.getINR() << " -   " << DR.getCLR() << " | ";
	std::cout << std::setw(4) << PC.getInt() << " -  " << PC.getLD() << " -   " << PC.getINR() << " -   " << PC.getCLR() << " | ";
	std::cout << std::setw(4) << AR.getInt() << " -  " << AR.getLD() << " -   " << AR.getINR() << " -   " << AR.getCLR() << " | ";
	std::cout << std::setw(5) << MEM_AR.getInt() << " | ";
	std::cout << std:: setw(4) << IR.getInt() << " -  " << IR.getLD() << " | ";
	std::cout << E.getInt() << " | ";
	std::cout << BUS.getBusAddress() << std::endl;

	AR.clearSignals();
	PC.clearSignals();
	AC.clearSignals();
	DR.clearSignals();
	IR.clearSignals();
	BUS.changeBusAddress("000");
}