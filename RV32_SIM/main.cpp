#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include <iomanip>
#include <cstdint>
#include <initializer_list>
#include <algorithm>


using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

enum instrName {
  LUI = 0,
  AUIPC = 1,
  JAL = 2,
  JALR = 3,
  BEQ = 4,
  BNE = 5,
  BLT = 6,
  BGE = 7,
  BLTU = 8,
  BGEU = 9,
  LB = 10,
  LH = 11,
  LW = 12,
  LD = 13,
  LBU = 14,
  LHU = 15,
  SB = 16,
  SH = 17,
  SW = 18,
  SD = 19,
  ADDI = 20,
  SLTI = 21,
  SLTIU = 22,
  XORI = 23,
  ORI = 24,
  ANDI = 25,
  SLLI = 26,
  SRLI = 27,
  SRAI = 28,
  ADD = 29,
  SUB = 30,
  SLL = 31,
  SLT = 32,
  SLTU = 33,
  XOR = 34,
  SRL = 35,
  SRA = 36,
  OR = 37,
  AND = 38,
  ECALL = 39,
  HALT = 40,
  UNKNOWN = -1
};


struct IFStruct {
    bitset<32>  PC = 0;
	bitset<32>  Instr = 0;
    bool        nop = false;  
};

struct IDStruct {
	bitset<32>  PC = 0;
    bitset<32>  Instr = 0;
    bool        nop = false;  
};

struct EXStruct {
	bitset<32>  PC = 0;
	bitset<32>  Instr;
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<12>  immI;
	bitset<12>  immS;
	bitset<12>  immB;
	bitset<20>  immU;
	bitset<20>  immJ;
	bitset<32>  imm;
    bitset<5>   rs1;
    bitset<5>   rs2;
	bitset<5>   rd;
    bitset<5>   Wrt_reg_addr;
	instrName instruction = UNKNOWN;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop = false;  
	bool		branchFound = false;
	bool		branchTaken = false;
};

struct MEMStruct {
	bitset<32>  PC = 0;
    bitset<32> Instr = 0;
    bitset<32> ALUresult = 0;
    bitset<32> Store_data = 0;
    bitset<5> rd = 0;
    bitset<5> rs1 = 0;
    bitset<5> rs2 = 0;
    bitset<5> Wrt_reg_addr = 0;
    bool rd_mem = false;
    bool wrt_mem = false;
    bool wrt_enable = false;
    bool nop = false;
    instrName instruction = UNKNOWN;
};

struct WBStruct {
	bitset<32>  PC = 0;
	instrName instruction;
	bitset<32>  Instr;
    bitset<32>  Wrt_data;
    bitset<5>   rs1;
    bitset<5>   rs2;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop = false;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class InsMem
{
	public:
		string id, ioDir;
        InsMem(string name, string ioDir) {       
			id = name;
			IMem.resize(MemSize);
            ifstream imem;
			string line;
			int i=0;
			imem.open(ioDir + "/imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					// cout<<IMem[i]<<endl;
					i++;
				}                    
			}
            else cout<<"Unable to open IMEM input file.\n";
			imem.close();                     
		}

		bitset<32> readInstr(bitset<32> ReadAddress) {//read the next 4 bytes from the base    
			int addr = (int)(ReadAddress.to_ulong());

			if(addr%4 != 0){cout<<"Invalid readInstr at 0d"<<addr<<endl;return 0;}
			bitset<32> instruction;
			for (int i = 0; i < 4; i++) {
				instruction = (instruction << 8)|bitset<32>(IMem[addr + i].to_ulong());
			}
			return instruction;
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public: 
		string id, opFilePath, ioDir;
        DataMem(string name, string ioDir) : id{name}, ioDir{ioDir} {
            DMem.resize(MemSize);
			opFilePath = ioDir + "/" + name + "_DMEMResult.txt";
            ifstream dmem;
            string line;
            int i=0;
            dmem.open(ioDir + "/dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open DMEM input file.\n";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address) {	

			int addr = (int)(Address.to_ulong());

			if(addr%4 != 0){cout<<"Invalid readData at 0d"<<addr<<endl;return 0;}
			bitset<32> dMem;
			for (int i = 0; i < 4; i++) {
				dMem = (dMem << 8)|bitset<32>(DMem[addr + i].to_ulong());
			}
			return dMem;
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData) {
			// write into memory
			int addr = (int)(Address.to_ulong());
			
			
			if(addr%4 != 0){cout<<"Invalid writeData at 0d"<<addr<<endl;return;}
			DMem[addr]=bitset<8>((WriteData.to_ulong()>>24) & 0xFF);
			DMem[addr+1]=bitset<8>((WriteData.to_ulong()>>16) & 0xFF);
			DMem[addr+2]=bitset<8>((WriteData.to_ulong()>>8) & 0xFF);
			DMem[addr+3]=bitset<8>((WriteData.to_ulong()) & 0xFF);

 		// 	cout << "Write into 0x" << hex << addr << " data 0x" << hex << WriteData.to_ulong() << endl
		//  << "bytes: " 
        //  << hex <<"addr "<<addr<<" "<< DMem[addr] << " " 
        //  << hex <<"addr "<<addr+1<<" "<< DMem[addr + 1] << " " 
        //  << hex <<"addr "<<addr+2<<" "<< DMem[addr + 2] << " " 
        //  << hex <<"addr "<<addr+3<<" "<< DMem[addr + 3] << endl;
        }   
                     
        void outputDataMem() {
            ofstream dmemout;
            dmemout.open(opFilePath, ios_base::trunc);
            if (dmemout.is_open()) {
                for (int j = 0; j< 1000; j++)
                {    
					
                    dmemout << DMem[j]<<endl;
					// cout << "DEBUG: DMem[" << j << "] = " << DMem[j] << endl;
                }
                     
            }
            else cout<<"Unable to open "<<id<<" DMEM result file.\n" << endl;
            dmemout.close();
        }             

    private:
		vector<bitset<8> > DMem;      
};

class RegisterFile
{
    public:
		string outputFile;
     	RegisterFile(string ioDir): outputFile {ioDir + "RFResult.txt"} {
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr) {   
            // Fill in
			return Registers[Reg_addr.to_ulong()];
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data) {
            // Fill in
			if(Reg_addr.to_ulong() != 0){
				Registers[Reg_addr.to_ulong()]=Wrt_reg_data;
			}
			
        }
		 
		void outputRF(int cycle) {
			ofstream rfout;
			if (cycle == 0)
				rfout.open(outputFile, ios_base::trunc);
			else 
				rfout.open(outputFile, ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"----------------------------------------------------------------------\nState of RF after executing cycle:"<<cycle<<endl;
				for (int j = 0; j<32; j++)
				{
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open RF output file.\n"<<endl;
			rfout.close();               
		} 
			
	private:
		vector<bitset<32>>Registers;
};

struct Instruction {
	int bits;
	instrName instruction;
    bitset<7> opcode;  
	bitset<3> funct3;   
	bitset<7> funct7; 
    bitset<5> rs1;          
    bitset<5> rs2;          
    bitset<5> rd;           
	bitset<12> immI;  
	bitset<12> immS;  
	bitset<12> immB;  
	bitset<20> immU;  
	bitset<20> immJ;  
    bitset<26> address; 
         
    bool isR = false; 
	bool isI = false;         
    bool isS = false;     
	bool isB = false;
	bool isU = false;
	bool isJ = false;     

    Instruction(bitset<32> instr) {
		bits = instr.to_ulong();
        opcode = bitset<7>(instr.to_ulong() & 0x7F); 

        if (opcode.to_ulong() == 51) { 
            isR = true;
			rd     = bitset<5>((bits>>7) & 0x1F);
			funct3 = bitset<3>((bits>>12)&0x7);
			rs1    = bitset<5>((bits>>15)&0x1F);
			rs2    = bitset<5>((bits>>20)&0x1F);
			funct7 = bitset<7>((bits>>25)&0x7F);

			bitset<10> funct = (funct7.to_ulong()<<3) | funct3.to_ulong();

			switch (funct.to_ulong())
			{
			case 0:
				instruction = ADD;
				break;	
			case 256:
				instruction = SUB;
				break;
			case 4:
				instruction = XOR;
				break;
			case 6:
				instruction = OR;
				break;
			case 7:
				instruction = AND;
				break;
			default:
				instruction = UNKNOWN;
				break;
			}

        } else if (opcode.to_ulong() == 19) { 
            isI    = true;
			rd     = bitset<5>((bits>>7) & 0x1F);
			funct3 = bitset<3>((bits>>12)&0x7);
			rs1    = bitset<5>((bits>>15)&0x1F);
			immI   = bitset<12>((bits>>20)&0xFFF);

			switch (funct3.to_ulong())
			{
			case 0:
				instruction = ADDI;
				break;	
			case 4:
				instruction = XORI;
				break;	
			case 6:
				instruction = ORI;
				break;	
			case 7:
				instruction = ANDI;
				break;	
			default:
				instruction = UNKNOWN;
				break;
			}
        } else if (opcode.to_ulong() == 3){
			isI    = true;
			rd     = bitset<5>((bits>>7) & 0x1F);
			funct3 = bitset<3>((bits>>12)&0x7);
			rs1    = bitset<5>((bits>>15)&0x1F);
			immI   = bitset<12>((bits>>20)&0xFFF);

			switch (funct3.to_ulong())
			{
			//TODO
			case 2:
				instruction = LW;
				break;	
			default:
				instruction = UNKNOWN;
				break;
			}

		} else if (opcode.to_ulong() == 35) { 
            isS = true;

			funct3 = bitset<3>((bits>>12)&0x7);
			rs1    = bitset<5>((bits>>15)&0x1F);
			rs2    = bitset<5>((bits>>20)&0x1F);
			immS = bitset<12>(((bits>>7)&0x1F)|
								((bits>>20)&0xFE0));
			switch (funct3.to_ulong())
			{
			case 2:
				instruction = SW;
				break;	
			default:
				instruction = UNKNOWN;
				break;
			}


        } else if (opcode.to_ulong() == 99) { 
            isB = true;

			funct3 = bitset<3>((bits>>12)&0x7);
			rs1    = bitset<5>((bits>>15)&0x1F);
			rs2    = bitset<5>((bits>>20)&0x1F);
			immB = ((bits >> 31) & 0x1) << 12|
				((bits >> 25) & 0x3F) << 5|
				((bits >> 8) & 0xF) << 1|
				((bits >> 7) & 0x1) << 11;


			switch (funct3.to_ulong())
			{
			case 0:
				instruction = BEQ;
				break;
			case 1:
				instruction = BNE;
				break;		
			default:
				instruction = UNKNOWN;
				break;
			}

        } else if (opcode.to_ulong() == 55 | opcode.to_ulong() == 23) { 
            isU = true;
        } else if (opcode.to_ulong() == 111) { 
            isJ = true;
			rd = bitset<5>((bits>>7) & 0x1F);
			immJ = ((bits>>31)&0x1)<<20|
					((bits>>21)&0x3FF)<<1 |
					((bits>>20)&0x1)<<11|
					((bits>>12)&0xFF)<<12 ;
			instruction = JAL;
        } else if (instr == bitset<32>(0xFFFFFFFF)){
			instruction = HALT;
		}else{
			// cout<<"instruction type undefined: "<<instr<<endl;
		}


    }
};


class Core {
	protected:
		bool memToFirst;
		bool memToSecond;
		bool exToFirst;
		bool exToSecond;
		bool pipeline;
	public:
		RegisterFile myRF;
		uint32_t cycle = 0;
		uint32_t instructionCount = 0;
		bool halted = false;
		string ioDir;
		struct stateStruct state, nextState;
		InsMem ext_imem;
		DataMem& ext_dmem;
		uint32_t halted_cycles = 0;
		bool debugMode = false;
		bool developerMode = false;
		
		Core(string ioDir, InsMem &imem, DataMem &dmem): myRF(ioDir), ioDir{ioDir}, ext_imem {imem}, ext_dmem {dmem} {}

		virtual void step() {}

		virtual void printState() {}

		virtual void decodeInstruction(Instruction instr) {
			string assembly;

			switch (instr.instruction) {
				case ADD:
					assembly = "ADD x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", x" + to_string(instr.rs2.to_ulong());
					break;
				case SUB:
					assembly = "SUB x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", x" + to_string(instr.rs2.to_ulong());
					break;
				case XOR:
					assembly = "XOR x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", x" + to_string(instr.rs2.to_ulong());
					break;
				case OR:
					assembly = "OR x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", x" + to_string(instr.rs2.to_ulong());
					break;
				case AND:
					assembly = "AND x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", x" + to_string(instr.rs2.to_ulong());
					break;
				case ADDI:
					assembly = "ADDI x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immI.to_ulong() | (instr.immI[11] ? 0xFFFFF000 : 0)));
					break;
				case XORI:
					assembly = "XORI x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immI.to_ulong() | (instr.immI[11] ? 0xFFFFF000 : 0)));
					break;
				case ORI:
					assembly = "ORI x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immI.to_ulong() | (instr.immI[11] ? 0xFFFFF000 : 0)));
					break;
				case ANDI:
					assembly = "ANDI x" + to_string(instr.rd.to_ulong()) +
							", x" + to_string(instr.rs1.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immI.to_ulong() | (instr.immI[11] ? 0xFFFFF000 : 0)));
					break;
				case LW:
					assembly = "LW x" + to_string(instr.rd.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immI.to_ulong() | (instr.immI[11] ? 0xFFFFF000 : 0))) +
							"(x" + to_string(instr.rs1.to_ulong()) + ")";
					break;
				case SW:
					assembly = "SW x" + to_string(instr.rs2.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immS.to_ulong() | (instr.immS[11] ? 0xFFFFF000 : 0))) +
							"(x" + to_string(instr.rs1.to_ulong()) + ")";
					break;
				case JAL:
					assembly = "JAL x" + to_string(instr.rd.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immJ.to_ulong() | (instr.immJ[19] ? 0xFFF00000 : 0)));
					break;
				case BEQ:
					assembly = "BEQ x" + to_string(instr.rs1.to_ulong()) +
							", x" + to_string(instr.rs2.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immB.to_ulong() | (instr.immB[11] ? 0xFFFFF000 : 0)));
					break;
				case BNE:
					assembly = "BNE x" + to_string(instr.rs1.to_ulong()) +
							", x" + to_string(instr.rs2.to_ulong()) +
							", " + to_string(static_cast<int32_t>(instr.immB.to_ulong() | (instr.immB[11] ? 0xFFFFF000 : 0)));
					break;
				case HALT:
					assembly = "HALT";
					break;
				default:
					assembly = "UNKNOWN";
					break;
			}

    		cout << assembly << endl;
		}

		virtual void cleanNop(){
			if(!halted){
				state.IF.nop = false;
				state.ID.nop = false;
				state.EX.nop = false;
				state.MEM.nop = false;
				state.WB.nop = false;
			}
			
		}

		virtual void IFStage(){ //pc up, instruction fetch
			if(state.IF.nop == true){
				if (debugMode) {printf("IF >> nop >> "); decodeInstruction(ext_imem.readInstr(state.IF.PC));}
				return;}

			if (debugMode) {
				printf("IF >> ");
				decodeInstruction(ext_imem.readInstr(state.IF.PC));
				if (developerMode) cout << "IF >> Instruction fetched: 0x" << hex << uppercase << setw(8) << setfill('0') << ext_imem.readInstr(state.IF.PC).to_ulong() << endl;
				}

			nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong()+4);
			state.ID.Instr = ext_imem.readInstr(state.IF.PC);
			state.ID.PC = state.IF.PC;
			nextState.IF.nop = false;

			
				
		}

		virtual void IDStage(){

			if(state.ID.nop == true){
				if (debugMode) {printf("ID >> nop >> "); decodeInstruction(state.ID.Instr);}
				return;}

			if (debugMode) {
				printf("ID >> ");
				decodeInstruction(state.ID.Instr);
				}
				
			state.EX.Instr = state.ID.Instr;
			Instruction instr(state.ID.Instr);


			if(instr.isB){
				state.EX.rs1 = instr.rs1;
				state.EX.rs2 = instr.rs2;
				state.EX.immB = instr.immB;
				state.EX.instruction = instr.instruction;

			}else if(instr.isI){
				state.EX.rd = instr.rd;
				state.EX.rs1 = instr.rs1;
				state.EX.immI = instr.immI;
				state.EX.instruction = instr.instruction;

			}else if(instr.isJ){
				state.EX.immJ = instr.immJ;
				state.EX.rd = instr.rd;
				state.EX.instruction = instr.instruction;
			}else if(instr.isR){
				state.EX.rd = instr.rd;
				state.EX.rs1 = instr.rs1;
				state.EX.rs2 = instr.rs2;
				state.EX.instruction = instr.instruction;
			}else if(instr.isS){
				state.EX.immS= instr.immS;
				state.EX.rs1 = instr.rs1;
				state.EX.rs2 = instr.rs2;
				state.EX.instruction = instr.instruction;
			}else if(instr.isU){
				//TODO
			}



			if (developerMode) {
				printf("ID >> PC: 0x%08lX, Next PC: 0x%08lX\n", state.IF.PC.to_ulong(), nextState.IF.PC.to_ulong());
				printf("ID >> Instruction Name: %d\n", instr.instruction);
				bitset<32> binary(instr.bits);
				printf("ID >> Binary: %s\n", binary.to_string().c_str());
				printf("ID >> Opcode: 0x%02lX, Funct3: 0x%lX, Funct7: 0x%lX\n",
					instr.opcode.to_ulong(),
					instr.funct3.to_ulong(),
					instr.funct7.to_ulong());
				printf("ID >> rd: x%ld, rs1: x%ld, rs2: x%ld\n",
					instr.rd.to_ulong(),
					instr.rs1.to_ulong(),
					instr.rs2.to_ulong());

				if (instr.isR) {
					printf("ID >> Type: R-type\n");
				} else if (instr.isI) {
					printf("ID >> Type: I-type, immI: 0x%lX\n", instr.immI.to_ulong());
				} else if (instr.isS) {
					printf("ID >> Type: S-type, immS: 0x%lX\n", instr.immS.to_ulong());
				} else if (instr.isB) {
					printf("ID >> Type: B-type, immB: 0x%lX\n", instr.immB.to_ulong());
				} else if (instr.isU) {
					printf("ID >> Type: U-type\n");
				} else if (instr.isJ) {
					printf("ID >> Type: J-type, immJ: 0x%lX\n", instr.immJ.to_ulong());
				}
			}

			state.EX.Read_data1 = myRF.readRF(state.EX.rs1);
			state.EX.Read_data2 = myRF.readRF(state.EX.rs2);
			state.EX.PC = state.ID.PC;
		
		}

		virtual void EXStage() {

    if (state.EX.nop == true) {
        if (debugMode) {
            printf("EX >> nop >> ");
            decodeInstruction(state.EX.Instr);
        }
        return;
    }

    if (debugMode) {
        printf("EX >> ");
        decodeInstruction(state.EX.Instr);
    }

    state.MEM.Instr = state.EX.Instr;

    int32_t immI;
    int32_t immS;
    int32_t immJ;
    int32_t immB;



    int32_t rs1Value = static_cast<int32_t>(state.EX.Read_data1.to_ulong());
    int32_t rs2Value = static_cast<int32_t>(state.EX.Read_data2.to_ulong());
    int32_t result = 0;

    if (state.EX.immI[11] | state.EX.immB[11] | state.EX.immS[11] | state.EX.immJ[19]) {
        immI = static_cast<int32_t>(state.EX.immI.to_ulong() | 0xFFFFF000);
        immS = static_cast<int32_t>(state.EX.immS.to_ulong() | 0xFFFFF000);
        immJ = static_cast<int32_t>(state.EX.immJ.to_ulong() | 0xFFFFF000);
        immB = static_cast<int32_t>(state.EX.immB.to_ulong() | 0xFFFFF000);
    } else {
        immI = static_cast<int32_t>(state.EX.immI.to_ulong());
        immS = static_cast<int32_t>(state.EX.immS.to_ulong());
        immJ = static_cast<int32_t>(state.EX.immJ.to_ulong());
        immB = static_cast<int32_t>(state.EX.immB.to_ulong());
    }

    switch (state.EX.instruction) {
        case ADD:
            result = rs1Value + rs2Value;
            state.EX.imm = 0;
            if (developerMode) {
                printf("EX >> ADD: %d + %d = %d\n", rs1Value, rs2Value, result);
            }
            break;
        case SUB:
            result = rs1Value - rs2Value;
            state.EX.imm = 0;
            if (developerMode) {
                printf("EX >> SUB: %d - %d = %d\n", rs1Value, rs2Value, result);
            }
            break;
        case XOR:
            result = rs1Value ^ rs2Value;
            state.EX.imm = 0;
            if (developerMode) {
                printf("EX >> XOR: %d ^ %d = %d\n", rs1Value, rs2Value, result);
            }
            break;
        case OR:
            result = rs1Value | rs2Value;
            state.EX.imm = 0;
            if (developerMode) {
                printf("EX >> OR: %d | %d = %d\n", rs1Value, rs2Value, result);
            }
            break;
        case AND:
            result = rs1Value & rs2Value;
            state.EX.imm = 0;
            if (developerMode) {
                printf("EX >> AND: %d & %d = %d\n", rs1Value, rs2Value, result);
            }
            break;
        case ADDI:
            result = rs1Value + immI;
            state.EX.imm = bitset<32>(static_cast<uint32_t>(immI));
            if (developerMode) {
                printf("EX >> ADDI: %d + %d = %d\n", rs1Value, immI, result);
            }
            break;
		case XORI:
			result = rs1Value ^ immI;
            state.EX.imm = bitset<32>(static_cast<uint32_t>(immI));
            if (developerMode) {
                printf("EX >> XORI: %d ^ %d = %d\n", rs1Value, immI, result);
            }
            break;
		case ORI:
			result = rs1Value | immI;
            state.EX.imm = bitset<32>(static_cast<uint32_t>(immI));
            if (developerMode) {
                printf("EX >> ORI: %d | %d = %d\n", rs1Value, immI, result);
            }
            break;
		case ANDI:
			result = rs1Value & immI;
            state.EX.imm = bitset<32>(static_cast<uint32_t>(immI));
            if (developerMode) {
                printf("EX >> XORI: %d & %d = %d\n", rs1Value, immI, result);
            }
            break;
        case LW:
            result = rs1Value + immI;
            state.EX.imm = bitset<32>(static_cast<uint32_t>(immI));
            if (developerMode) {
                printf("EX >> LW Address Calculation: %d + %d = %d\n", rs1Value, immI, result);
            }
            break;
        case SW:
            result = rs1Value + immS;
            state.EX.imm = bitset<32>(static_cast<uint32_t>(immS));
            if (developerMode) {
                printf("EX >> SW Address Calculation: %d + %d = %d\n", rs1Value, immS, result);
            }
            break;
        case BNE:
			state.EX.branchFound = (rs1Value != rs2Value);
            if (rs1Value != rs2Value) {
                result = state.EX.PC.to_ulong() + immB;
                if (developerMode) {
                    printf("EX >> BNE Taken: PC %ld + %d = %d\n",state.EX.PC.to_ulong(), immB, result);
                }
            } else {

                result = state.EX.PC.to_ulong()+4;
                if (developerMode) {
                    printf("EX >> BNE Not Taken\n");
                }
            }
            state.EX.imm = bitset<32>(static_cast<uint32_t>(immB));
            break;
        case BEQ:
			state.EX.branchFound = (rs1Value == rs2Value);
            if (rs1Value == rs2Value) {
                result = state.EX.PC.to_ulong() + immB;
                if (developerMode) {
                    printf("EX >> BEQ Taken: PC + %d = %d\n", immB, result);
                }
            } else {
                result = state.EX.PC.to_ulong()+4;
                if (developerMode) {
                    printf("EX >> BEQ Not Taken\n");
                }
            }
            state.EX.imm = bitset<32>(static_cast<uint32_t>(immB));
            break;
        case JAL:
			state.EX.branchFound = true;
            result = state.EX.PC.to_ulong() + 4;
            state.WB.Wrt_data = bitset<32>(static_cast<uint32_t>(result));
            state.WB.Wrt_reg_addr = state.EX.rd;
			result = state.EX.PC.to_ulong() + immJ;
            if (developerMode) {
                printf("EX >> JAL: PC + 4 = %ld, Writing to rd = x%ld, Jumping to %d\n", state.EX.PC.to_ulong()+4, state.EX.rd.to_ulong(),result);
            }
            break;
        default:
            if (developerMode) {
                printf("EX >> Unknown Instruction\n");
            }
            break;
    }

    state.MEM.instruction = state.EX.instruction;
    state.MEM.ALUresult = bitset<32>(static_cast<uint32_t>(result));
    state.MEM.rd = state.EX.rd;
    state.MEM.rs2 = state.EX.rs2;
    state.MEM.rs1 = state.EX.rs1;
	state.MEM.PC = state.EX.PC;
}

		virtual void MEMStage() {
		if (state.MEM.nop == true) {
			if (debugMode) {
				printf("ME >> nop >> ");
				decodeInstruction(state.MEM.Instr);
			}
			return;
		}

		if (debugMode) {
			printf("ME >> ");
			decodeInstruction(state.MEM.Instr);
		}
		
		state.WB.Instr = state.MEM.Instr;
		state.WB.PC = state.MEM.PC;
		bitset<32> result;

		switch (state.MEM.instruction) {
			// ALU result to rd
			case ADD:
			case SUB:
			case XOR:
			case OR:
			case AND:
			case ADDI:
			case XORI:
			case ORI:
			case ANDI:
				state.WB.Wrt_data = state.MEM.ALUresult;
				state.WB.Wrt_reg_addr = state.MEM.rd;
				if (developerMode) {
					printf("ME >> ALU Result: 0x%08lX, Writing to rd: x%ld\n",
						state.MEM.ALUresult.to_ulong(), state.MEM.rd.to_ulong());
				}
				break;

			case LW:
				result = ext_dmem.readDataMem(state.MEM.ALUresult);
				state.WB.Wrt_data = result;
				state.WB.Wrt_reg_addr = state.MEM.rd;
				if (developerMode) {
					printf("ME >> LW: Loaded data 0x%08lX from address 0x%08lX, Writing to rd: x%ld\n",
						result.to_ulong(), state.MEM.ALUresult.to_ulong(), state.MEM.rd.to_ulong());
				}
				break;

			case SW:
				ext_dmem.writeDataMem(state.MEM.ALUresult, myRF.readRF(state.MEM.rs2));
				state.WB.nop = true;
				if (developerMode) {
					printf("ME >> SW: Stored data 0x%08lX to address 0x%08lX\n",
						myRF.readRF(state.MEM.rs2).to_ulong(), state.MEM.ALUresult.to_ulong());
				}
				break;

			case BNE:
				if (developerMode) {
					printf("ME >> BNE: Branching to address 0x%08lX\n", state.MEM.ALUresult.to_ulong());
				}
				nextState.IF.PC = state.MEM.ALUresult;
				state.WB.nop = true;
				break;
			case BEQ:
				if (developerMode) {
					printf("ME >> BEQ: Branching to address 0x%08lX\n", state.MEM.ALUresult.to_ulong());
				}
				nextState.IF.PC = state.MEM.ALUresult;
				state.WB.nop = true;
				break;
			case JAL:
				if (developerMode) {
					printf("ME >> JAL: Jumping to address 0x%08lX\n", state.MEM.ALUresult.to_ulong());
				}
				nextState.IF.PC = state.MEM.ALUresult;
				break;
			default:
				if (developerMode) {
					printf("ME >> Unknown Instruction\n");
				}
				break;
		}

		state.WB.instruction = state.MEM.instruction;

		// Hazard handling
		if (memToFirst) {
			state.MEM = MEMStruct();
		}
}

		virtual void WBStage(){
			
			if(state.WB.nop == true){
				if (debugMode) {printf("WB >> nop >> "); decodeInstruction(state.WB.Instr);}
				return;}
			
			if (debugMode) {
				printf("WB >> ");
				decodeInstruction(state.WB.Instr);}

			

			switch (state.WB.instruction)
			{
			// wrt data to wrt reg
			case ADD:
			case SUB:
			case XOR:
			case OR:
			case AND:
			case ADDI:
			case XORI:
			case ORI:
			case ANDI:
			case JAL:
			case LW:	
				myRF.writeRF(state.WB.Wrt_reg_addr,state.WB.Wrt_data);
				if (developerMode) {
					cout << "WB >> Writing value 0x" << hex << state.WB.Wrt_data.to_ulong()
						<< " to register x" << dec << state.WB.Wrt_reg_addr.to_ulong() << endl;
				}
				break;

			// case JAL:
			// 	myRF.writeRF(state.WB.Wrt_reg_addr,nextState.IF.PC);
			// 	if (developerMode) {
			// 		cout << "WB >> Writing JAL value 0x" << hex << nextState.IF.PC.to_ulong()
			// 			<< " to register x" << dec << state.WB.Wrt_reg_addr.to_ulong() << endl;
			// 	}
			// 	nextState.IF.PC = state.WB.Wrt_data;
			// 	break;

			default:
				break;
			}


		}
		



};

class SingleStageCore : public Core {
	public:
		SingleStageCore(string ioDir, InsMem &imem, DataMem &dmem): Core(ioDir + "/SS_", imem, dmem), opFilePath(ioDir + "/StateResult_SS.txt") {}

		void step() {
			/* Your implementation*/

			if (debugMode){
				printf("---------------Cycle %d --------------- \n",cycle);
			}

			haltCheck();
			
			if (halted){
				myRF.outputRF(cycle);
				printState(nextState, cycle);
				return;
			}
			
			IFStage();			
			IDStage();		
			EXStage();	
			MEMStage();
			WBStage();

			myRF.outputRF(cycle); // dump RF
			
			printState(nextState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
			cleanNop();	
			state = nextState; // The end of the cycle and updates the current state with the values calculated in this cycle
			cycle++;
			instructionCount ++;

	
		}

		void printState(stateStruct state, int cycle) {
    		ofstream printstate;
			if (cycle == 0)
				printstate.open(opFilePath, ios_base::trunc);
			else 
    			printstate.open(opFilePath, ios_base::app);
    		if (printstate.is_open()) {
    		    printstate<<"----------------------------------------------------------------------\nState after executing cycle: "<<cycle<<endl; 

    		    printstate<<"IF.PC: "<<state.IF.PC.to_ulong()<<endl;
    		    printstate << "IF.nop: " << ((state.IF.nop) ? "True" : "False") << endl;
    		}
    		else cout<<"Unable to open SS StateResult output file.\n" << endl;
    		printstate.close();
		}

		void outputPerformanceMetrics() {
			string filePath = ioDir.substr(0, ioDir.length() - 3) + "PerformanceMetrics.txt";
			ofstream metricsFile(filePath, ios_base::trunc); 
			
			uint32_t cycleCount = cycle + 1;
			double cpi = static_cast<double>(cycleCount) / instructionCount;
			double ipc = static_cast<double>(instructionCount) / cycleCount;

			if (metricsFile.is_open()) {
				metricsFile << "Performance of Single Stage:\n";
				metricsFile << "#Cycles -> " << cycleCount << "\n";
				metricsFile << "#Instructions -> " << instructionCount << "\n";
				metricsFile << "CPI -> " << setprecision(17) << cpi << "\n";
				metricsFile << "IPC -> " << setprecision(16) << ipc << "\n";
			} else {
				cout << "Unable to open PerformanceMetrics.txt file. Path: " << filePath << endl;
			}
			
			metricsFile.close();
		}

		void haltCheck(){

			if(static_cast<int32_t>(ext_imem.readInstr(state.IF.PC).to_ulong()) == -1) {
					if(halted_cycles >= 1) {
						halted = true;
					}else{
						halted_cycles ++;
						nextState.IF.nop = true;
					}
					state.IF.nop = true;			
					state.ID.nop = true;
					state.EX.nop = true;
					state.MEM.nop = true;
					state.WB.nop = true;
					
				}
		}
	private:
		string opFilePath;
};

class FiveStageCore : public Core{
	public:
		
		FiveStageCore(string ioDir, InsMem &imem, DataMem &dmem)
		: Core(ioDir + "/FS_", imem, dmem), opFilePath(ioDir + "/StateResult_FS.txt") {}

		void step() {
			// pipeline = true;
			if (debugMode){
				printf("--------------- Cycle %d --------------- \n",cycle);
			}

			WBHaltCheck();
			
			if (halted) {
				if(debugMode) printf("--------------- HALTED ---------------\n");
				myRF.outputRF(cycle);
				return;
			}

			initialPipeline();

			handleDataHazard();

			WBStage();		
			MEMStage();		
			EXStage();

			if (!state.EX.nop && state.EX.instruction!=-1 && state.EX.instruction!=40)	instructionCount ++;
			// printf("branch found >> %s\n", state.EX.branchFound ? "true" : "false");
			if(state.EX.branchFound) {
				handleBranchHazard();
			}else{
				
				IDStage();
				IFStage();			
			}

			


			memToSecond = false;
			memToFirst = false;
			exToSecond = false;
			exToFirst = false;;

            myRF.outputRF(cycle); // dump RF
			printState(nextState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
			cleanNop();

			state.IF.PC = nextState.IF.PC;//The end of the cycle and updates the current state with the values calculated in this cycle
			cycle++;

			// printf("S.PC >> %ld\n", state.IF.PC.to_ulong());
			// printf("N.PC >> %ld\n", nextState.IF.PC.to_ulong());
			
			
		}

		

		void printState(stateStruct state, int cycle) {
		    ofstream printstate;
			if (cycle == 0)
				printstate.open(opFilePath, ios_base::trunc);
			else 
		    	printstate.open(opFilePath, ios_base::app);
		    if (printstate.is_open()) {

				printstate << "----------------------------------------------------------------------" << endl;
				printstate << "State after executing cycle: " << cycle << endl;
				printstate << "IF.nop: " << (state.IF.nop ? "True" : "False") << endl;
				printstate << "IF.PC: " << state.IF.PC.to_ulong()  << endl;

				printstate << "ID.nop: " << (state.ID.nop ? "True" : "False") << endl;
				// printstate << "ID.Instr: " << state.ID.Instr << endl;
				printstate << ((state.ID.Instr.to_ulong() != 0) ? ("ID.Instr: " + state.ID.Instr.to_string()) : "ID.Instr:") << endl;

				printstate << "EX.nop: " << (state.EX.nop ? "True" : "False") << endl;
				printstate << ((state.ID.Instr.to_ulong() != 0) ? ("EX.instr: " + state.ID.Instr.to_string()) : "EX.instr:") << endl;
				printstate << "EX.Read_data1: " << state.EX.Read_data1 << endl;
				printstate << "EX.Read_data2: " << state.EX.Read_data2 << endl;
				printstate << "EX.Imm: " << state.EX.imm << endl;
				printstate << "EX.Rs: " << state.EX.rs1 << endl;
				printstate << "EX.Rt: " << state.EX.rs2 << endl;
				printstate << "EX.Wrt_reg_addr: " << state.EX.Wrt_reg_addr << endl;
				printstate << "EX.is_I_type: " << state.EX.is_I_type << endl;
				printstate << "EX.rd_mem: " << state.EX.rd_mem << endl;
				printstate << "EX.wrt_mem: " << state.EX.wrt_mem << endl;
				printstate << "EX.alu_op: " << state.EX.alu_op << endl;
				printstate << "EX.wrt_enable: " << state.EX.wrt_enable << endl;
				
				printstate << "MEM.nop: " << (state.MEM.nop ? "True" : "False") << endl;
				printstate << "MEM.ALUresult: " << state.MEM.ALUresult << endl;
				printstate << "MEM.Store_data: " << state.MEM.Store_data << endl;
				printstate << "MEM.Rs: " << state.MEM.rs1 << endl;
				printstate << "MEM.Rt: " << state.MEM.rs2 << endl;
				printstate << "MEM.Wrt_reg_addr: " << state.MEM.Wrt_reg_addr << endl;
				printstate << "MEM.rd_mem: " << state.MEM.rd_mem << endl;
				printstate << "MEM.wrt_mem: " << state.MEM.wrt_mem << endl;
				printstate << "MEM.wrt_enable: " << state.MEM.wrt_enable << endl;
				
				printstate << "WB.nop: " << (state.WB.nop ? "True" : "False") << endl;
				printstate << "WB.Wrt_data: " << state.WB.Wrt_data << endl;
				printstate << "WB.Rs: " << state.WB.rs1 << endl;
				printstate << "WB.Rt: " << state.WB.rs2 << endl;
				printstate << "WB.Wrt_reg_addr: " << state.WB.Wrt_reg_addr << endl;
				printstate << "WB.wrt_enable: " << state.WB.wrt_enable << endl;
				
			}


		    else cout<<"Unable to open FS StateResult output file.\n" << endl;
		    printstate.close();
		}

		void handleBranchHazard(){

			if (debugMode) {printf("!! Branch Hazards\n");}
			state.EX = EXStruct();
			state.ID = IDStruct();
			state.IF = IFStruct();
			state.EX.nop = true;
			state.ID.nop = true;
			state.IF.nop = true;

			nextState.IF.PC = state.MEM.ALUresult;

			// state.IF.PC = bitset<32>(state.IF.PC.to_ulong() - 4);

			// nextState.IF.PC = bitset<32>(nextState.IF.PC.to_ulong() - 4);

			// if(state.WB.instruction == BEQ || state.WB.instruction == BNE){
			// 	// state.IF.PC = nextState.IF.PC;
			// 	printf("branchtaken\n");
			// 	state.EX.branchFound = false;
			// 	state.EX.branchTaken = true;
			// }

			// if(state.WB.instruction == JAL){
			// 	// state.IF.PC = nextState.IF.PC;
			// 	state.EX.branchFound = false;
			// 	state.EX.branchTaken = true;
			// }

		}

		void handleDataHazard(){
			//data hazard 
			switch (state.EX.instruction){
				case SW:
				case JAL:
				case HALT:
					return;
					break;
				default:
					break;
			}
				

			//mem to 1st
			if ((state.MEM.instruction == LW) &&
				(state.MEM.rd.to_ulong() != 0) && 
    			((state.EX.rs1 == state.MEM.rd) || (state.EX.rs2 == state.MEM.rd))){

				if (debugMode) {printf("!! MEM to 1st >> nop added\n");}

				state.EX.nop = true;
				state.ID.nop = true;
				state.IF.nop = true;
				memToFirst = true;
				// return;
			}

			//mem to 2rd
			if ((state.WB.instruction == LW) &&
				(state.WB.Wrt_reg_addr.to_ulong() != 0) && 
				((state.EX.rs1 == state.WB.Wrt_reg_addr) || (state.EX.rs2 == state.WB.Wrt_reg_addr))) {

				if (debugMode) printf("!! MEM to 2st >> wb forwarding\n");

				if (state.EX.rs1 == state.WB.Wrt_reg_addr) {
					state.EX.Read_data1 = state.WB.Wrt_data;
					// myRF.writeRF(state.EX.rs1, state.WB.Wrt_data);
					if (developerMode) {
						printf("Forwarding WB data to EX stage >> ");
						printf("EX.rs1 (x%ld) matched WB.Wrt_reg_addr (x%ld) >> ", 
							state.EX.rs1.to_ulong(), state.WB.Wrt_reg_addr.to_ulong());
						printf("Writing WB.Wrt_data (0x%08lX) to EX.rs1 (x%ld)\n", 
							state.WB.Wrt_data.to_ulong(), state.EX.rs1.to_ulong());
					}
				}

				if (state.EX.rs2 == state.WB.Wrt_reg_addr) {
					state.EX.Read_data2 = state.WB.Wrt_data;
					// myRF.writeRF(state.EX.rs2, state.WB.Wrt_data);
					if (developerMode) {
						printf("Forwarding WB data to EX stage >> ");
						printf("EX.rs2 (x%ld) matched WB.Wrt_reg_addr (x%ld) >> ", 
							state.EX.rs2.to_ulong(), state.WB.Wrt_reg_addr.to_ulong());
						printf("Writing WB.Wrt_data (0x%08lX) to EX.rs2 (x%ld)\n", 
							state.WB.Wrt_data.to_ulong(), state.EX.rs2.to_ulong());
					}
				}
				return;
			}

			// ex to 1st
			if ((state.MEM.rd.to_ulong() != 0) && 
    			((state.EX.rs1 == state.MEM.rd) || (state.EX.rs2 == state.MEM.rd))){

				if (debugMode) {printf("!! EX to 1st >> \n");}

				if (state.EX.rs1 == state.MEM.rd) {
					state.EX.Read_data1 = state.MEM.ALUresult;
					// myRF.writeRF(state.EX.rs1, state.MEM.ALUresult);
					if (developerMode) {
						printf("Forwarding MEM ALUresult to EX stage >> ");
						printf("EX.rs1 (x%ld) matched MEM (x%ld) >> ", 
							state.EX.rs1.to_ulong(), state.MEM.rd.to_ulong());
						printf("Writing MEM (0x%08lX) to EX.rs1 (x%ld)\n", 
							state.MEM.rd.to_ulong(), state.EX.rs1.to_ulong());
					}
				}

				if (state.EX.rs2 == state.MEM.rd) {
					state.EX.Read_data2 = state.MEM.ALUresult;
					// myRF.writeRF(state.EX.rs2, state.MEM.ALUresult);
					if (developerMode) {
						printf("Forwarding MEM ALUresult to EX stage >> ");
						printf("EX.rs2 (x%ld) matched MEM (x%ld) >> ", 
							state.EX.rs2.to_ulong(), state.MEM.rd.to_ulong());
						printf("Writing MEM (0x%08lX) to EX.rs2 (x%ld)\n", 
							state.MEM.rd.to_ulong(), state.EX.rs2.to_ulong());
					}
				}
				return;
			}

			// ex to 2rd
			if ((state.WB.Wrt_reg_addr.to_ulong() != 0) && 
    			((state.EX.rs1 == state.WB.Wrt_reg_addr) || (state.EX.rs2 == state.WB.Wrt_reg_addr))){

				if (debugMode) {printf("!! EX to 2rd >> \n");}

				if (state.EX.rs1 == state.WB.Wrt_reg_addr) {
					state.EX.Read_data2 = state.WB.Wrt_data;
					// myRF.writeRF(state.EX.rs1, state.WB.Wrt_data);
					if (developerMode) {
						printf("Forwarding WB ALUresult to EX stage >> ");
						printf("EX.rs1 (x%ld) matched WB (x%ld) >> ", 
							state.EX.rs1.to_ulong(), state.WB.Wrt_reg_addr.to_ulong());
						printf("Writing WB (0x%08lX) to EX.rs1 (x%ld)\n", 
							state.WB.Wrt_reg_addr.to_ulong(), state.EX.rs1.to_ulong());
					}
				}

				if (state.EX.rs2 == state.WB.Wrt_reg_addr) {
					state.EX.Read_data2 = state.WB.Wrt_data;
					// myRF.writeRF(state.EX.rs2, state.WB.Wrt_data);
					if (developerMode) {
						printf("Forwarding MWBEM ALUresult to EX stage >> ");
						printf("EX.rs2 (x%ld) matched WB (x%ld) >> ", 
							state.EX.rs2.to_ulong(), state.WB.Wrt_reg_addr.to_ulong());
						printf("Writing WB (0x%08lX) to EX.rs2 (x%ld)\n", 
							state.WB.Wrt_reg_addr.to_ulong(), state.EX.rs2.to_ulong());
					}
				}
				return;
			}



		}
	
		void WBHaltCheck(){
			
			if(state.WB.Instr == -1) {
						halted = true;			
				}
		}
	
		void initialPipeline(){
			if (cycle == 0) {  
				state.WB.nop = true;
				state.MEM.nop = true;
				state.EX.nop = true;
				state.ID.nop = true;
			}else if (cycle == 1) {  
				state.WB.nop = true;
				state.MEM.nop = true;
				state.EX.nop = true;
			}else if (cycle == 2) {  
				state.WB.nop = true;
				state.MEM.nop = true;
			}else if (cycle == 3) {  
				state.WB.nop = true;
			}
		}

		void outputPerformanceMetrics() {
			string filePath = ioDir.substr(0, ioDir.length() - 3) + "PerformanceMetrics.txt";
			ofstream metricsFile(filePath, ios_base::app);

			uint32_t cycleCount = ++ cycle ;
			double cpi = static_cast<double>(cycleCount) / instructionCount;
			double ipc = static_cast<double>(instructionCount) / cycleCount;

			if (metricsFile.is_open()) {
				metricsFile << "\nPerformance of Five-Stage:\n";
				metricsFile << "#Cycles -> " << cycleCount << "\n";
				metricsFile << "#Instructions -> " << -- instructionCount  << "\n";
				metricsFile << "CPI -> " << setprecision(17) << cpi << "\n";
				metricsFile << "IPC -> " << setprecision(16) << ipc << "\n";
			} else {
				cout << "Unable to open PerformanceMetrics.txt file. Path: " << filePath << endl;
			}

			metricsFile.close();
		}



	private:
		string opFilePath;
		int count;
};

int main(int argc, char* argv[]) {
	
	string ioDir = "";

    if (argc == 1) {
        cout << "Enter path containing the memory files: ";
        cin >> ioDir;
    }
    else if (argc > 2) {
        cout << "Invalid number of arguments. Machine stopped." << endl;
        return -1;
    }
    else {
        ioDir = argv[1];
        cout << "IO Directory: " << ioDir << endl;
    }

    // ioDir = "/home/ruihan11/myProject/RV32_SIM/input";

	
    InsMem imem = InsMem("Imem", ioDir);

    DataMem dmem_ss = DataMem("SS", ioDir);
	SingleStageCore SSCore(ioDir, imem, dmem_ss);
	DataMem dmem_fs = DataMem("FS", ioDir);
	FiveStageCore FSCore(ioDir, imem, dmem_fs);

    


	// for(int i=0;i<30;i++){
   	// 	SSCore.debugMode = true;
	// 	// SSCore.developerMode = true;
	// 	SSCore.step();
	// }


	while (1) {
		if (!SSCore.halted){
			// FSCore.debugMode = true;
			// FSCore.developerMode = true;
			SSCore.step();
		}
		if (!FSCore.halted){
			// FSCore.debugMode = true;
			// FSCore.developerMode = true;
			FSCore.step();
		}
		if (SSCore.halted && FSCore.halted)
			break;
    }


    



	dmem_ss.outputDataMem();
	SSCore.outputPerformanceMetrics(); 
	dmem_fs.outputDataMem();
	FSCore.outputPerformanceMetrics();
	
	return 0;
}