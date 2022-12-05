#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

using namespace std;
struct instruction{

    uint8_t opcode = 0;
    string opcode_str = "null";
    string opcode_remain = "null";
    uint8_t rs = 0;
    uint8_t rt = 0;
    uint8_t rd = 0;
    int16_t offset_addr = 0;
    bool branch;
    bool memRead;
    bool memWrite;
    bool ALUsrc;
    bool memToReg;
    bool regDst;
    bool regWrite;
    bool ALUopcode;

    int32_t reg1 = 0;
    int32_t reg2 = 0;
    int32_t ALUresult = 0;
    int32_t memReadValue = 0;
    int32_t regWriteValue = 0;

};
void init(int* reg , int* memory);
instruction IF(int32_t *PC , vector<string>::iterator asm_code_list_begin , vector<string>::iterator asm_code_list_end );
instruction ID(instruction is , int32_t * reg);
instruction EX(instruction is);
instruction MEM(instruction is , int32_t * mem);
instruction WB(instruction is , int32_t * reg);
void I_format(instruction *ret );
void R_format(instruction *ret );
void branch(instruction *ret );
int stringToInt(string &s , bool is_reg);
void print_stats(int cycles , instruction *is);
void readAsmCode(vector<string>&asm_code_list);


// add $1, $2, $3
int main()
{
    cin.tie(0);
    ios::sync_with_stdio(false);

    int32_t Register[32];
    int32_t Memory[32];
    vector<string> asm_code_list;
    int32_t PC = 0;

    init(Register , Memory);
    readAsmCode(asm_code_list);


    // decode("add $4, $8, $9");
    // decode("lw $4, 20($8)");
    // instruction is;
    // string IF_ID;

    instruction stage[5];
    instruction &IF_ID = stage[0];
    instruction &ID_EX = stage[1];
    instruction &EX_MEM = stage[2];
    instruction &MEM_WB = stage[3];
    instruction &WB_END = stage[4];


    

    for(int cycle = 1; cycle <= asm_code_list.size()+4; cycle++){
        
        
        WB_END = WB(MEM_WB , Register);
        MEM_WB = MEM(EX_MEM , Memory);
        EX_MEM = EX(ID_EX);
        ID_EX = ID(IF_ID , Register);
        IF_ID = IF(&PC, asm_code_list.begin() , asm_code_list.end());
        
        

        print_stats(cycle , stage);
        
    }

    // asm_code = IF(&PC, asm_code_list.begin());
    // is = ID(asm_code, Register);
    // is = EX(is);
    // is = MEM(is, Memory);
    // is = WB(is, Register);
    // cout << asm_code << endl;

    return 0;
}

void init(int32_t* reg , int32_t* memory)
{
    for(int i = 1; i < 32; i++)
       reg[i] = 1 , memory[i] = 1;

    reg[0] = 0;

    return;

}

void readAsmCode(vector<string> &asm_code_list)
{
    string asm_code;
    while( getline(cin , asm_code ) )
        asm_code_list.push_back(asm_code);
    
    return;
}

instruction IF(int32_t *PC , vector<string>::iterator asm_code_list_begin , vector<string>::iterator asm_code_list_end )
{
    instruction ret;

    if( (asm_code_list_begin + *PC) == asm_code_list_end )
    return ret;

    string asm_code = *(asm_code_list_begin + *PC);
    *PC += 1;

    stringstream ss;
    string operation , code;
    

    ss << asm_code; 
    ss >> ret.opcode_str;
    getline(ss , ret.opcode_remain , '\n');

    return ret;
    
}

instruction ID(instruction is , int* reg)
{


    

    if( is.opcode_str == "lw" || is.opcode_str == "sw")  I_format(&is);
    else if ( is.opcode_str == "add" || is.opcode_str == "sub" ) R_format(&is);
    else branch( &is );


    is.reg1 = reg[is.rs];
    is.reg2 = reg[is.rt];


    // print_stats("ID" , ret);


    // cout<< (int)ret.rs <<" "<< (int)ret.rt <<" "<< (int)ret.rd <<" "<< (int)ret.offset_addr <<endl;

    return is;
}

void I_format(instruction *ret)
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

void R_format( instruction *ret )
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
    getline(ss , tmp , ',');
    ret->rt = (uint8_t)stringToInt(tmp , true);

    
}

void branch( instruction *ret )
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
    getline(ss , tmp , ',');
    ret->offset_addr = (int16_t)stringToInt(tmp , true);
}

int stringToInt(string &s , bool is_reg)
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

void print_stats(int cycles, instruction *is)
{
    cout<<"Cycle "<< cycles <<" :\n";
    
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

instruction EX(instruction is)
{

   
    int32_t a = is.reg1 , b = is.reg2;

    if( is.ALUsrc == 1 ) b = is.offset_addr;
    if( is.ALUopcode == 1 ) b = -b;

    is.ALUresult = a + b;


    // print_stats("EX", is);

    return is;
}

instruction MEM(instruction is , int32_t* mem)
{

    if( is.memRead ) is.memReadValue = mem[is.ALUresult];
    if( is.memWrite ) mem[is.ALUresult] = is.reg2;
    


    // print_stats("MEM", is);

    return is;
}


instruction WB(instruction is , int32_t* reg)
{
    
    if( is.memToReg == 1 ) is.regWriteValue = is.ALUresult;
    else is.regWriteValue = is.memReadValue;


    uint8_t writeReg;
    if( is.memToReg == 0) writeReg = is.rt;
    else writeReg = is.rd;

    if( is.regWrite ) reg[writeReg] = is.regWriteValue;
    
    


    // print_stats("WB", is);

    return is;
}
