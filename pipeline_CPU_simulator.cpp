#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
#include "pipeline_CPU_simulator.h"

using namespace std;

pipelineCPUSimulator::pipelineCPUSimulator(){

    string asm_code;
    while( getline(cin , asm_code ) )
        asm_code_list.push_back(asm_code);
    

    init();
}

pipelineCPUSimulator::pipelineCPUSimulator(string filename){

    string asm_code;
    ifstream inputFile(filename);
    while( getline( inputFile, asm_code ) )
        asm_code_list.push_back(asm_code);

    init();
}

void pipelineCPUSimulator::startSimulation()
{
    asm_code_list.push_back("");
    asm_code_list.push_back("");
    asm_code_list.push_back("");
    asm_code_list.push_back("");

    instruction stage[5];
    instruction &IF_ID = stage[0];
    instruction &ID_EX = stage[1];
    instruction &EX_MEM = stage[2];
    instruction &MEM_WB = stage[3];
    instruction &WB_END = stage[4];


    int cycles = 1;    

    while(PC < asm_code_list.size()){
        
        
        WB_END = WB(MEM_WB );
        MEM_WB = MEM(EX_MEM);
        EX_MEM = EX(ID_EX , EX_MEM , WB_END);
        ID_EX = ID(IF_ID , ID_EX , MEM_WB , WB_END);
        IF_ID = IF(ID_EX , IF_ID);
        
        print_stats(cycles , stage);
        cycles++;
        // cout<<"PC : "<<PC <<" asm: "<< asm_code_list.size() <<endl;
        // cout<<endl;
        // print_reg_memory(Register, Memory);
    }

    print_reg_memory();

    return;
}


void pipelineCPUSimulator::init()
{
    for(int i = 0; i < 32; i++)
       Register[i] = 1 , Memory[i] = 1;

    Register[0] = 0;

    return;

}

/*
    void pipelineCPUSimulator::readAsmCode(vector<string> &asm_code_list)
    {
        string asm_code;
        while( getline(cin , asm_code ) )
            asm_code_list.push_back(asm_code);
        
        return;
    }
*/

int pipelineCPUSimulator::stringToInt(string &s , bool is_reg)
{
    stringstream ss;
    int ret;
    ss<<s;
    if( is_reg )
    {
        string x;
        getline(ss , x , '$');
        ss>>ret;
    }
    else
        ss>>ret;

    // cout<< ret << endl;
    return ret;
    
}

void pipelineCPUSimulator::print_stats(int cycles, instruction *is)
{
    cout<<"Cycle "<< cycles <<" :\n";
    cout<<"PC = "<< PC <<endl;
    
    string state[ ]={"IF" , "ID" , "EX" , "MEM" , "WB"};

    //base info
    for(int i=0; i< 208; i++)
        cout<<"-";
    cout<<endl;

    cout<<"|"<< setw(8) << "Opcode"<<"  |"
        << setw(8) << "Stage" <<"  |"
        << setw(8) << "RegDst" <<"  |"
        << setw(8) << "ALUSrc" <<"  |"
        << setw(8) << "Branch" <<"  |"
        << setw(8) << "MemRead"<<"  |"
        << setw(10) << "MemWrite"<<"  |"
        << setw(10) << "RegWrite"<<"  |"
        << setw(10)<<"MemtoReg"<<"  |"
        << setw(4)<<"rs"<<"  |"
        << setw(4)<<"rt"<<"  |"
        << setw(4)<<"rd"<<"  |"
        << setw(6)<<"reg1"<<"  |"
        << setw(6)<<"reg2"<<"  |"
        << setw(12)<<"Addr/Offset"<<"  |"
        << setw(12)<<"ALUresult"<<"  |"
        << setw(14)<<"ReadmemValue"<<"  |"
        << setw(14)<<"RegWriteValue"<<"  |\n";

    for(int i=0; i< 208; i++)
    cout<<"-";
    cout<<endl;


    for(int i = 0; i < 5; i++)
    {
        
            cout<<"|"<< setw(8) << is[i].opcode_str <<"  |"
            << setw(8) << state[i] <<"  |"
            << setw(8) << is[i].regDst <<"  |"
            << setw(8) << is[i].ALUsrc <<"  |"
            << setw(8) << is[i].branch <<"  |"
            << setw(8) << is[i].memRead<<"  |"
            << setw(10) << is[i].memWrite <<"  |"
            << setw(10) << is[i].regWrite <<"  |"
            << setw(10)<<is[i].memToReg <<"  |"
            << setw(4)<<(int)is[i].rs<<"  |"
            << setw(4)<<(int)is[i].rt<<"  |"
            << setw(4)<<(int)is[i].rd<<"  |"
            << setw(6)<<is[i].reg1<<"  |"
            << setw(6)<<is[i].reg2<<"  |"
            << setw(12)<<(int)is[i].offset_addr<<"  |"
            << setw(12)<<is[i].ALUresult<<"  |"
            << setw(14)<<is[i].memReadValue<<"  |"
            << setw(14)<<is[i].regWriteValue<<"  |";

        cout<<endl;
    }

    for(int i=0; i< 208; i++)
        cout<<"-";
    cout<<endl;


    return;
}

void pipelineCPUSimulator::print_reg_memory(){

    for(int i=0; i< 188; i++)
        cout<<"-";
    cout<<endl;

    // 0~15
    cout<<"|"<< setw(9) << "Index" <<"  |";
    for (int i=0; i<16; i++)
        cout << setw(8) << i <<"  |";
    cout << endl;

    for(int i=0; i< 188; i++)
        cout<<"-";
    cout<<endl;


    cout<<"|"<< setw(9) << "Register" <<"  |";
    for (int i=0; i<16; i++)
        cout << setw(8) << Register[i] <<"  |";
    cout << endl;

    cout<<"|"<< setw(9) << "Memory" <<"  |";
    for (int i=0; i<16; i++)
        cout << setw(8) << Memory[i] <<"  |";
    cout << endl;

    for(int i=0; i< 188; i++)
        cout<<"-";
    cout<<endl<<endl;


    // 16 ~ 31
    for(int i=0; i< 188; i++)
        cout<<"-";
    cout<<endl;

    cout<<"|"<< setw(9) << "Index" <<"  |";
    for (int i=16; i<32; i++)
        cout << setw(8) << i <<"  |";
    cout << endl;

    for(int i=0; i< 188; i++)
        cout<<"-";
    cout<<endl;


    cout<<"|"<< setw(9) << "Register" <<"  |";
    for (int i=16; i<32; i++)
        cout << setw(8) << Register[i] <<"  |";
    cout << endl;

    cout<<"|"<< setw(9) << "Memory" <<"  |";

    for (int i=16; i<32; i++)
        cout << setw(8) << Memory[i] <<"  |";
    cout << endl;

    for(int i=0; i< 188; i++)
        cout<<"-";
    cout<<endl;
}

instruction pipelineCPUSimulator::IF(instruction ID_EX_is, instruction IF_ID_is)
{
    instruction ret;
    vector<string>::iterator asm_code_list_begin = asm_code_list.begin() , asm_code_list_end = asm_code_list.end();
    
    // load data hazards
    if (ID_EX_is.freezePC)
    {
        // cout<<"IF_ID "<< IF_ID_is.PC <<" "<<IF_ID_is.opcode_str<< endl;
        return IF_ID_is;    
    }

    // check code is end
    if( (asm_code_list_begin + PC) == asm_code_list_end )
    return ret;

    string asm_code = *(asm_code_list_begin + PC);
    PC += 1;

    ret.PC = PC;

    // branch 
    if( ID_EX_is.changePC )
    {
        // cout<<"odd PC: "<<*PC<<endl;
        // cout<< ID_EX_is.opcode_str <<endl;
        PC = ID_EX_is.PC + ID_EX_is.offset_addr;
        // cout<<ID_EX_is.PC<<" "<<ID_EX_is.offset_addr<<endl;
        // cout<<"new PC: "<<*PC<<endl;
        return ret;
    }

    stringstream ss;
    string operation , code;
    

    ss << asm_code; 
    ss >> ret.opcode_str;
    getline(ss , ret.opcode_remain , '\n');


    return ret;
    
}

instruction pipelineCPUSimulator::ID(instruction is , instruction ID_EX_is , instruction EX_MEM_is , instruction MEM_WB_is)
{

    if( is.opcode_str == "lw" || is.opcode_str == "sw")  I_format(&is);
    else if ( is.opcode_str == "add" || is.opcode_str == "sub" ) R_format(&is);
    else branch( &is );

    is.reg1 = Register[is.rs];
    is.reg2 = Register[is.rt];

    



    // cout<< ID_EX_is.memRead <<" " << (int)ID_EX_is.rt <<" "<<(int)is.rs<<" "<<(int)is.rt <<endl;

    // load / store hazard
    if( ID_EX_is.memRead && ( ID_EX_is.rt == is.rs || ID_EX_is.rt == is.rt ))
    {
        instruction ret;
        ret.freezePC = 1;
        return ret;
    }


    // branch data hazard


    bool reg1_forward = false, reg2_forward = false;

    if(is.regDst) is.WB_reg = is.rd;
    else is.WB_reg = is.rt;


    // branch source = 上個 ID_EX 寫入的暫存器 
    if( is.opcode_str == "beq" && ID_EX_is.regWrite && !ID_EX_is.memRead && ID_EX_is.rd !=0 && (ID_EX_is.rd == is.rs || ID_EX_is.rt == is.rt))
    {
        
        instruction ret;
        ret.freezePC = 1;
        return ret;
        
    }   

    // branch source = 上上個 EX_MEM 讀取的記憶體 暫存器
    if ( is.opcode_str == "beq" && EX_MEM_is.memRead && ( EX_MEM_is.rt == is.rs || EX_MEM_is.rt == is.rt))
    {
        
        instruction ret;
        ret.freezePC = 1;
        return ret;
        
    }  


    // branch source = 上上個 EX_MEM or 上上上個 MEM_WB 寫入的暫存器

    if( is.opcode_str == "beq" && EX_MEM_is.regWrite && EX_MEM_is.WB_reg != 0 && (EX_MEM_is.WB_reg == is.rs || EX_MEM_is.WB_reg == is.rt)){

        if (EX_MEM_is.WB_reg == is.rs) is.reg1 = EX_MEM_is.ALUresult , reg1_forward = true;
        if (EX_MEM_is.WB_reg == is.rt) is.reg2 = EX_MEM_is.ALUresult , reg2_forward = true;

    }

    if( is.opcode_str == "beq" && MEM_WB_is.regWrite && MEM_WB_is.WB_reg != 0 && ( MEM_WB_is.WB_reg == is.rs || MEM_WB_is.WB_reg == is.rt )){

        if(MEM_WB_is.WB_reg == is.rs && !reg1_forward) is.reg1 = MEM_WB_is.regWriteValue;
        if(MEM_WB_is.WB_reg == is.rt && !reg2_forward) is.reg2 = MEM_WB_is.regWriteValue;

    }

    if( is.opcode_str == "beq" && is.reg1 == is.reg2)
        is.changePC = 1;
    
    





    // print_stats("ID" , ret);


    // cout<< (int)ret.rs <<" "<< (int)ret.rt <<" "<< (int)ret.rd <<" "<< (int)ret.offset_addr <<endl;

    return is;
}


instruction pipelineCPUSimulator::EX(instruction is , instruction EX_MEM_is , instruction MEM_WB_is)
{

   

    if(is.regDst) is.WB_reg = is.rd;
    else is.WB_reg = is.rt;

    bool reg1_forward = false, reg2_forward = false;

    if( EX_MEM_is.regWrite && EX_MEM_is.WB_reg != 0 && (EX_MEM_is.WB_reg == is.rs || EX_MEM_is.WB_reg == is.rt)){

        if (EX_MEM_is.WB_reg == is.rs) is.reg1 = EX_MEM_is.ALUresult , reg1_forward = true;
        if (EX_MEM_is.WB_reg == is.rt) is.reg2 = EX_MEM_is.ALUresult , reg2_forward = true;

    }

    if( MEM_WB_is.regWrite && MEM_WB_is.WB_reg != 0 && ( MEM_WB_is.WB_reg == is.rs || MEM_WB_is.WB_reg == is.rt )){

        if(MEM_WB_is.WB_reg == is.rs && !reg1_forward) is.reg1 = MEM_WB_is.regWriteValue;
        if(MEM_WB_is.WB_reg == is.rt && !reg2_forward) is.reg2 = MEM_WB_is.regWriteValue;

    }

    int32_t a = is.reg1 , b = is.reg2;

    if( is.ALUsrc == 1 ) b = is.offset_addr;
    if( is.ALUopcode == 1 ) b = -b;

    is.ALUresult = a + b;


    // print_stats("EX", is);

    return is;
}

instruction pipelineCPUSimulator::MEM(instruction is)
{

    if( is.memRead ) is.memReadValue = Memory[is.ALUresult/4];
    if( is.memWrite ) Memory[is.ALUresult/4] = is.reg2;
    


    // print_stats("MEM", is);

    return is;
}

instruction pipelineCPUSimulator::WB(instruction is)
{
    
    if( is.memToReg == 1 ) is.regWriteValue = is.ALUresult;
    else is.regWriteValue = is.memReadValue;




    if( is.regWrite ) Register[is.WB_reg] = is.regWriteValue;
    
    


    // print_stats("WB", is);

    return is;
}


void pipelineCPUSimulator::I_format(instruction *ret)
{
    if( ret->opcode_str =="lw" )
    {
        ret->opcode = 35;
        ret->memRead = 1;
        ret->memWrite = 0;
        ret->memToReg = 0;
        ret->regDst = 0;
        ret->regWrite = 1;
        ret->ALUsrc = 1;
        ret->branch = 0;
    }
    else
    {
        ret->opcode = 43;
        ret->memRead = 0;
        ret->memWrite = 1;
        ret->memToReg = 0;
        ret->regDst = 0;
        ret->regWrite = 0;
        ret->ALUsrc = 1;
        ret->branch = 0;
    }

    stringstream ss;
    ss << ret->opcode_remain;

    string tmp;  
    
    // rt
    getline(ss , tmp , ',');
    ret->rt = (uint8_t)stringToInt(tmp , true);

    // offset
    getline(ss , tmp , '(');
    ret->offset_addr = (int16_t)stringToInt(tmp , false);

    // rs
    getline(ss , tmp , ')');
    ret->rs = (uint8_t)stringToInt(tmp , true);


    return;

    

}

void pipelineCPUSimulator::R_format( instruction *ret )
{
    ret->opcode = 0;
    ret->memRead = 0;
    ret->memWrite = 0;
    ret->memToReg = 1;
    ret->regDst = 1;
    ret->regWrite = 1;
    ret->ALUsrc = 0;
    ret->branch = 0;
    ret->ALUopcode = 0;

    if( ret->opcode_str == "sub" )
        ret->ALUopcode = 1;

    stringstream ss;
    ss << ret->opcode_remain;

    string tmp;  
    
    // rd
    getline(ss , tmp , ',');
    ret->rd = (uint8_t)stringToInt(tmp , true);
    
    // rs
    getline(ss , tmp , ',');
    ret->rs = (uint8_t)stringToInt(tmp , true);

    // rt
    getline(ss , tmp );
    ret->rt = (uint8_t)stringToInt(tmp , true);

    
}

void pipelineCPUSimulator::branch( instruction *ret )
{
    ret->opcode = 4;
    ret->memRead = 0;
    ret->memWrite = 0;
    ret->memToReg = 0;
    ret->regDst = 0;
    ret->regWrite = 0;
    ret->ALUsrc = 0;
    ret->branch = 1;
    ret->ALUopcode = 0;

    stringstream ss;
    ss << ret->opcode_remain;

    string tmp;  
    // rs
    getline(ss , tmp , ',');
    ret->rs = (uint8_t)stringToInt(tmp , true);
    
    // rt
    getline(ss , tmp , ',');
    ret->rt = (uint8_t)stringToInt(tmp , true);

    // addr
    getline(ss , tmp );
    ret->offset_addr = (int16_t)stringToInt(tmp , false);

}
