#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

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
    int32_t ALUresult = 0;
    int32_t memReadValue = 0;
    int32_t regWriteValue = 0;

};
void init(int* reg , int* memory);
string IF(int32_t *PC , vector<string>::iterator asm_code_list);
instruction ID(string asm_code , int32_t * reg);
instruction EX(instruction is);
instruction MEM(instruction is , int32_t * mem);
instruction WB(instruction is , int32_t * reg);
void I_format(string &operation , string code , instruction *ret );
void R_format(string &operation , string code , instruction *ret );
void branch(string &operation , string code , instruction *ret );
int stringToInt(string &s , bool is_reg);
void print_stats(string state , instruction is);
void readAsmCode(vector<string>&asm_code_list);



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
    vector<instruction> is( asm_code_list.size() );
    vector<string> asm_code( asm_code_list.size() );

    // decode("add $4, $8, $9");
    // decode("lw $4, 20($8)");
    // instruction is;
    for(int i=0; i<asm_code_list.size()+4; i++){
        cout<<endl << "cycle" << i<<endl;

        if(i == 0){
            asm_code[i] = IF(&PC, asm_code_list.begin());

        }else if(i == 1){
            is[i - 1] = ID(asm_code[i - 1], Register);
            asm_code[i] = IF(&PC, asm_code_list.begin());

        }else if(i == 2){
            is[i - 2] = EX(is[i - 2]);
            is[i - 1] = ID(asm_code[i - 1], Register);
            asm_code[i] = IF(&PC, asm_code_list.begin());

        }else if(i == 3){
            is[i - 3] = MEM(is[i], Memory);
            is[i - 2] = EX(is[i - 2]);
            is[i - 1] = ID(asm_code[i - 1], Register);
            asm_code[i] = IF(&PC, asm_code_list.begin());

        }else{
            is[i - 4] = WB(is[i], Register);
            is[i - 3] = MEM(is[i], Memory);
            is[i - 2] = EX(is[i - 2]);
            is[i - 1] = ID(asm_code[i - 1], Register);
            asm_code[i] = IF(&PC, asm_code_list.begin());
        }
        
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

string IF(int32_t *PC , vector<string>::iterator asm_code_list)
{
    string asm_code = *(asm_code_list + *PC);
    *PC += 1;

    return asm_code ;
    
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
            << setw(10)<<"MemtoReg"<<"  |";

    if(state == "ID")
        cout<< setw(4)<<"rs"<<"  |"
            << setw(4)<<"rt"<<"  |"
            << setw(4)<<"rd"<<"  |"
            << setw(6)<<"reg1"<<"  |"
            << setw(6)<<"reg2"<<"  |"
            << setw(14)<<"Addr/Offset"<<"  |";

    else if(state == "EX")
        cout<< setw(6)<<"reg1"<<"  |"
            << setw(6)<<"reg2"<<"  |"
            << setw(14)<<"Addr/Offset"<<"  |"
            << setw(14)<<"ALUresult"<<"  |";

    else if(state == "MEM")
        cout<< setw(14)<<"ALUresult"<<"  |"
            << setw(14)<<"memValue"<<"  |";
    else if(state == "WB")
        cout<< setw(18)<<"regWriteValue"<<"  |";



    cout<<endl;


    cout<<"|"<< setw(8) << is.opcode_str <<"  |"
            << setw(8) << state <<"  |"
            << setw(8) << is.regDst <<"  |"
            << setw(8) << is.ALUsrc <<"  |"
            << setw(8) << is.branch <<"  |"
            << setw(8) << is.memRead<<"  |"
            << setw(10) << is.memWrite <<"  |"
            << setw(10) << is.regWrite <<"  |"
            << setw(10)<<is.memToReg <<"  |";

    if(state == "ID")
        cout<< setw(4)<<(int)is.rs<<"  |"
            << setw(4)<<(int)is.rt<<"  |"
            << setw(4)<<(int)is.rd<<"  |"
            << setw(6)<<is.reg1<<"  |"
            << setw(6)<<is.reg2<<"  |"
            << setw(14)<<(int)is.offset_addr<<"  |";

    else if(state == "EX")
        cout<< setw(6)<<is.reg1<<"  |"
            << setw(6)<<is.reg2<<"  |"
            << setw(14)<<(int)is.offset_addr<<"  |"
            << setw(14)<<is.ALUresult<<"  |";

    else if(state == "MEM")
        cout<< setw(14)<<is.ALUresult<<"  |"
            << setw(14)<<is.memReadValue<<"  |";

    else if(state == "WB")
        cout<< setw(18)<<is.regWriteValue<<"  |";

    cout<<endl;

    

    return;
}

instruction EX(instruction is)
{

   
    int32_t a = is.reg1 , b = is.reg2;

    if( is.ALUsrc == 1 ) b = is.offset_addr;
    if( is.ALUopcode == 1 ) b = -b;

    is.ALUresult = a + b;


    print_stats("EX", is);

    return is;
}

instruction MEM(instruction is , int32_t* mem)
{

    if( is.memRead ) is.memReadValue = mem[is.ALUresult];
    if( is.memWrite ) mem[is.ALUresult] = is.reg2;
    


    print_stats("MEM", is);

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
    
    


    print_stats("WB", is);

    return is;
}
