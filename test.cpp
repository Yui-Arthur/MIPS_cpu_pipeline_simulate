#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
struct instruction{

    uint8_t opcode = 0;
    string opcode_str;
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
    

};
void init(int* reg , int* memory);
string IF();
instruction ID(string asm_code , int32_t * reg);
void I_format(string &operation , string code , instruction *ret );
void R_format(string &operation , string code , instruction *ret );
void branch(string &operation , string code , instruction *ret );
int stringToInt(string &s , bool is_reg);
void print_stats(string state , instruction is);



int main()
{
    cin.tie(0);
    ios::sync_with_stdio(false);

    int32_t Register[32];
    int32_t Memory[32];

    init(Register , Memory);

    // decode("add $4, $8, $9");
    // decode("lw $4, 20($8)");
    instruction is;
    string asm_code;
    asm_code = IF();
    is = ID(asm_code, Register);


    return 0;
}

void init(int* reg , int* memory)
{
    for(int i = 1; i < 32; i++)
       reg[i] = 1 , memory[i] = 1;

    reg[0] = 0;

    return;

}

string IF()
{
    string asm_code;
    // getline(cin, asm_code);
    // return asm_code;

    return "lw $4, 20($8)";
    
}

instruction ID(string asm_code , int* reg)
{
    stringstream ss;
    string operation , code;
    instruction ret;

    ss << asm_code; 
    ss >> operation;

    getline(ss , code , '\n');
    // ss >> code;

    // cout<<operation<<endl;
    // cout<<code<<endl;

    

    if( operation == "lw" || operation == "sw")  I_format(operation , code , &ret);
    else if ( operation == "add" || operation == "sub" ) R_format(operation , code , &ret);
    else branch(operation , code , &ret );


    ret.opcode_str = operation; 
    ret.reg1 = reg[ret.rs];
    ret.reg2 = reg[ret.rt];


    print_stats("ID" , ret);


    // cout<< (int)ret.rs <<" "<< (int)ret.rt <<" "<< (int)ret.rd <<" "<< (int)ret.offset_addr <<endl;

    return ret;
}

void I_format(string &operation , string code , instruction *ret)
{
    if( operation=="lw" )
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
    ss << code;

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

void R_format(string &operation , string code , instruction *ret )
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

    if( operation == "sub" )
        ret->ALUopcode = 1;

    stringstream ss;
    ss << code;

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

void branch(string &operation , string code , instruction *ret )
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
    ss << code;

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

void print_stats(string state , instruction is)
{

    cout<<"|"<< setw(8) << "Opcode"<<"  |"
            << setw(8) << "Stage" <<"  |"
            << setw(8) << "RegDst" <<"  |"
            << setw(8) << "ALUSrc" <<"  |"
            << setw(8) << "Branch" <<"  |"
            << setw(8) << "MemRead"<<"  |"
            << setw(10) << "MemWrite"<<"  |"
            << setw(10) << "RegWrite"<<"  |"
            << setw(10)<<"MemtoReg"<<"  |\n";

    cout<<"|"<< setw(8) << is.opcode_str <<"  |"
            << setw(8) << state <<"  |"
            << setw(8) << is.regDst <<"  |"
            << setw(8) << is.ALUsrc <<"  |"
            << setw(8) << is.branch <<"  |"
            << setw(8) << is.memRead<<"  |"
            << setw(10) << is.memWrite <<"  |"
            << setw(10) << is.regWrite <<"  |"
            << setw(10)<<is.memToReg <<"  |\n";

    return;
}