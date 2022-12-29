#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
#include "pipeline_CPU_simulator.h"

using namespace std;

// 沒有給路徑的建構子 用stdin來輸入測資
pipelineCPUSimulator::pipelineCPUSimulator(){

    string asm_code;
    while( getline(cin , asm_code ) )
        // asm_code_list用來存放所有Instruction
        asm_code_list.push_back(asm_code);
    

    init();
}

// 有給路徑的建構子 用讀檔來輸入測資
pipelineCPUSimulator::pipelineCPUSimulator(string filename){

    string asm_code;
    ifstream inputFile(filename);
    while( getline( inputFile, asm_code ) )
        // asm_code_list用來存放所有Instruction
        asm_code_list.push_back(asm_code);

    init();
}

// Instruction主要執行的地方
void pipelineCPUSimulator::startSimulation()
{
    // 將asm_code_list新增四個空Instruction 方便後續處理
    asm_code_list.push_back("");
    asm_code_list.push_back("");
    asm_code_list.push_back("");
    asm_code_list.push_back("");

    // 用來存放pipeline各stage的值
    instruction stage[5];
    // 用referance的寫法 方便檢查
    instruction &IF_ID = stage[0];
    instruction &ID_EX = stage[1];
    instruction &EX_MEM = stage[2];
    instruction &MEM_WB = stage[3];
    instruction &WB_END = stage[4];

    // 計算總cycles的變數
    int cycles = 1;    

    // Instruction主要執行的地方 前面塞入了四個空指令 所以只需判斷PC是否在asm_code_list的長度內
    while(PC < asm_code_list.size()){
        
        /*
         * pipeline主要執行的地方
         * 每一個Cycle Instruction會依序由最後(WB)的地方開始執行 
         * 如此一來前面的結果就不會被蓋掉  所以最後執行的是(IF) 
         * 當每個Instruction執行完後 會有對應的變數來存放結果(IF_ID , ID_EX ...)
         * WB_END並沒有在實際上的pipeline架構中 是我們為了確認結果才放進來的 實際上並不需要
         */

        WB_END = WB(MEM_WB );
        MEM_WB = MEM(EX_MEM);
        EX_MEM = EX(ID_EX , EX_MEM , WB_END);
        ID_EX = ID(IF_ID , ID_EX , MEM_WB , WB_END);
        IF_ID = IF(ID_EX , IF_ID);
        
        // 當每個Cycle結束後 會印出目前的狀態
        print_stats(cycles , stage);
        // 接著將Cycle+1
        cycles++;

        // print_reg_memory(Register, Memory);
    }

    // 全部Instruction結束後 將Mem及Reg的值印出來
    print_reg_memory();

    return;
}

// 對Mem及Reg做初始化的動作
void pipelineCPUSimulator::init()
{
    // 0號 Reg為0 其他皆為1 
    // Mem全部都為1
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
/*
 * 在Decode過程中 用來將$reg($1)轉為Int的函式
 * 也可以將 Offest轉為Int
 * 透過 is_reg 來判斷要轉的是reg還是offset
 */ 
int pipelineCPUSimulator::stringToInt(string &s , bool is_reg)
{
    stringstream ss;
    int ret;
    ss<<s;
    // 如果要轉的是reg 就用$切割
    if( is_reg )
    {
        string x;
        getline(ss , x , '$');
        ss>>ret;
    }
    else
    // 否則就用一般的轉換
        ss>>ret;

    ;
    return ret;
    
}

// 將目前的狀態印出來 傳入所有紀錄狀態的Instruction變數
void pipelineCPUSimulator::print_stats(int cycles, instruction *is)
{
    //印出PC及Cycle
    cout<<"Cycle "<< cycles <<" :\n";
    cout<<"PC = "<< PC <<endl;
    
    string state[ ]={"IF" , "ID" , "EX" , "MEM" , "WB"};

    // 印出的格式
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

    // IF為下一個要執行的指令 直接利用PC取的下一個Instruction
    
    if (PC < asm_code_list.size())
    cout<<"|"<<setw(19)<<asm_code_list[PC]<<endl;


    //印出各個Stage的狀況
    for(int i = 0; i < 4; i++)
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

// 印出Reg及Mem的值
void pipelineCPUSimulator::print_reg_memory(){

    // 單純排版及輸出數值
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

/*
 * IF Stage
 * 將Asm Code拿出來
 * 並將PC+4
 * 特別要注意如果是Hazard發生
 * 會將PC給Freeze
 * 
 * 需要傳入這個Cycle的ID結果 ID_EX_is (因ID比IF早執行結束所以會放在ID_EX)
 * 以及上個Cycle的IF結果 IF_ID_is (因這個Cycle的IF還未執行 所以IF_ID的值是上個Cycle的)
 */ 
instruction pipelineCPUSimulator::IF(instruction ID_EX_is, instruction IF_ID_is)
{
    // 要回傳的指令 也就是新的IF_ID
    instruction ret;
    vector<string>::iterator asm_code_list_begin = asm_code_list.begin() , asm_code_list_end = asm_code_list.end();
    
    // 如果在ID中的Hazard Detection Unit發現有Hazard發生
    // 會將PC給freeze 並且回傳上次的IF結果
    if (ID_EX_is.freezePC)
    {
        // cout<<"IF_ID "<< IF_ID_is.PC <<" "<<IF_ID_is.opcode_str<< endl;
        return IF_ID_is;    
    }

    // 確認是否沒有下個Asm Code
    if( (asm_code_list_begin + PC) == asm_code_list_end )
    return ret;

    // 取得Asm Code
    string asm_code = *(asm_code_list_begin + PC);
    // 將PC+1
    PC += 1;

    // 將目前的PC記錄進變數中 如果是branch指令有可能會用到
    ret.PC = PC;

    // 如果branch指令在ID執行後發現要taken 
    // changePC會設為1 並且將PC改為 ID指令存下的PC加上offset
    // 同時這次拿到的指令就取消
    if( ID_EX_is.changePC )
    {
        // cout<<"odd PC: "<<*PC<<endl;
        // cout<< ID_EX_is.opcode_str <<endl;
        // cout<<ID_EX_is.PC<<" "<<ID_EX_is.offset_addr<<endl;
        // cout<<"new PC: "<<*PC<<endl;
        PC = ID_EX_is.PC + ID_EX_is.offset_addr;
        return ret;
    }

    stringstream ss;
    string operation , code;
    
    // 將Asm Code放入變數中 用來在ID解碼及方便Debug
    ss << asm_code; 
    ss >> ret.opcode_str;
    getline(ss , ret.opcode_remain , '\n');


    return ret;
    
}

/*
 * ID Stage
 * 將Asm Code進行Decode
 * 將用到的Reg拿出來
 * 並檢查Hazard
 * 特別要注意如果是Hazard發生
 * 會將PC給Freeze
 * 
 * 需要傳入這個Cycle的ID_EX、EX_MEM、MEM_WB用於判斷Hazard
 * 因為有執行順序的問題 所以其實傳入的是這個Cycle MEM WB的執行結果
 * 以及傳入上個Cycle的IF_ID並接續執行
 */ 
instruction pipelineCPUSimulator::ID(instruction is , instruction ID_EX_is , instruction EX_MEM_is , instruction MEM_WB_is)
{
    // 判斷opcode的種類 進入不同的func解碼
    // 解碼會將Asm Code 解析成 rs rt rd offset等等 並將Control Signal填好
    if( is.opcode_str == "lw" || is.opcode_str == "sw")  I_format(&is);
    else if ( is.opcode_str == "add" || is.opcode_str == "sub" ) R_format(&is);
    else branch( &is );

    // 根據rs及rt取出對應暫存器的值放入reg1 & 2
    is.reg1 = Register[is.rs];
    is.reg2 = Register[is.rt];


    // cout<< ID_EX_is.memRead <<" " << (int)ID_EX_is.rt <<" "<<(int)is.rs<<" "<<(int)is.rt <<endl;

    /* 
     * 檢查 Load Data Hazard
     * 如果上個指令有讀取Mem(lw)且rt為這個指令的rs or rt 
     * 則必須Stall一個Cycle
     * 所以會回傳一個全部都是0的指令到ID_EX
     * 並且要凍結PC
     * 且防止這個Cycle的IF_ID被寫入
     * Forwarding中只有除了Branch以外
     * 只有Load Data Hazard需要Stall
     * 其他都可以用Forwarding解決
     */
    if( ID_EX_is.memRead && ( ID_EX_is.rt == is.rs || ID_EX_is.rt == is.rt ))
    {
        instruction ret;
        // 凍結PC的Signal設為1
        ret.freezePC = 1;
        return ret;
    }


    // branch data hazard
    /*
     * 檢查branch data hazard
     * branch data hazard在Forwarding有三種情況
     * 1. 當上個指令寫入的暫存器 為branch指令的rs or rt
     * 2. 當上上個讀取Mem指令(lw)寫入的暫存器 為branch指令的rs or rt
     * 
     * 上個指令為lw的情況已經被上面的 Load Data Hazard給過濾掉了 
     */     

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

    /*
     * Branch Forwarding的部分
     * 兩種情況
     * 1. 上上個指令寫入的暫存器 為branch指令的rs or rt
     * 2. 上上上個指令寫入的暫存器 為branch指令的rs or rt
     * 如果同時發生以最近的為主
     */

    // 用來判斷是reg1 & reg2是否已經foward
    bool reg1_forward = false, reg2_forward = false;
    if(is.regDst) is.WB_reg = is.rd;
    else is.WB_reg = is.rt;

    // branch source = 上上個指令 (EX_MEM) 寫入的暫存器
    if( is.opcode_str == "beq" && EX_MEM_is.regWrite && EX_MEM_is.WB_reg != 0 && (EX_MEM_is.WB_reg == is.rs || EX_MEM_is.WB_reg == is.rt)){

        if (EX_MEM_is.WB_reg == is.rs) is.reg1 = EX_MEM_is.ALUresult , reg1_forward = true;
        if (EX_MEM_is.WB_reg == is.rt) is.reg2 = EX_MEM_is.ALUresult , reg2_forward = true;

    }

    // branch source = 上上上個 (MEM_WB) 寫入的暫存器 並且還沒forward

    if( is.opcode_str == "beq" && MEM_WB_is.regWrite && MEM_WB_is.WB_reg != 0 && ( MEM_WB_is.WB_reg == is.rs || MEM_WB_is.WB_reg == is.rt )){

        if(MEM_WB_is.WB_reg == is.rs && !reg1_forward) is.reg1 = MEM_WB_is.regWriteValue;
        if(MEM_WB_is.WB_reg == is.rt && !reg2_forward) is.reg2 = MEM_WB_is.regWriteValue;

    }

    // 如果reg1 == reg2 代表 taken 需要改變PC並且取消下一個指令

    if( is.opcode_str == "beq" && is.reg1 == is.reg2)
        is.changePC = 1;
    
    





    // print_stats("ID" , ret);


    // cout<< (int)ret.rs <<" "<< (int)ret.rt <<" "<< (int)ret.rd <<" "<< (int)ret.offset_addr <<endl;

    return is;
}

/*
 * EX Stage
 * 進行運算
 * 有Forwarding所以需傳入EX_MEM(上個指令)及MEM_WB(上上個指令)
 * 以及傳入上個Cycle的ID_EX並接續執行
 * 
 */ 
instruction pipelineCPUSimulator::EX(instruction is , instruction EX_MEM_is , instruction MEM_WB_is)
{



    // 確認寫入的暫存器是哪個 放到WB_reg中
    if(is.regDst) is.WB_reg = is.rd;
    else is.WB_reg = is.rt;


    /*
     * 因為有Forwarding
     * 所以ALU的來源可能是
     * 1. 原本指令的reg1 & reg2
     * 2. 上個指令的結果
     * 3. 上上個指令的結果
     * 
     * 所以有兩種情況
     * 1. 上個指令寫入的暫存器 為目前指令的rs or rt
     * 2. 上上個指令寫入的暫存器 為目前指令的rs or rt 
     * 如果同時發生以最近的為主
     */

    bool reg1_forward = false, reg2_forward = false;

    // ALU Source = 上個指令 (EX_MEM)寫入的暫存器
    if( EX_MEM_is.regWrite && EX_MEM_is.WB_reg != 0 && (EX_MEM_is.WB_reg == is.rs || EX_MEM_is.WB_reg == is.rt)){

        if (EX_MEM_is.WB_reg == is.rs) is.reg1 = EX_MEM_is.ALUresult , reg1_forward = true;
        if (EX_MEM_is.WB_reg == is.rt) is.reg2 = EX_MEM_is.ALUresult , reg2_forward = true;

    }

    // ALU Source = 上上個指令 (MEM_WB)寫入的暫存器 並且還沒forward
    if( MEM_WB_is.regWrite && MEM_WB_is.WB_reg != 0 && ( MEM_WB_is.WB_reg == is.rs || MEM_WB_is.WB_reg == is.rt )){

        if(MEM_WB_is.WB_reg == is.rs && !reg1_forward) is.reg1 = MEM_WB_is.regWriteValue;
        if(MEM_WB_is.WB_reg == is.rt && !reg2_forward) is.reg2 = MEM_WB_is.regWriteValue;

    }


    // a b 為 ALU的 Source
    int32_t a = is.reg1 , b = is.reg2;
    
    // 如果ALUsrc == 1 代表是要對offset/addr進行運算 所以將b設為offset/addr
    if( is.ALUsrc == 1 ) b = is.offset_addr;
    // 如果ALUopcode == 1 代表是減法 
    if( is.ALUopcode == 1 ) b = -b;

    //進行ALU運算 放入 ALUresult
    is.ALUresult = a + b;


    return is;
}

/*
 * MEM Stage
 * 進行Mem寫入或讀取
 * 傳入上個Cycle的EX_MEM並接續執行
 * 
 */ 
instruction pipelineCPUSimulator::MEM(instruction is)
{
    // 如果 memRead ==1 代表要讀取Mem 所以讀取ALUresult/4 
    // (因為Mem是 Word Addr 而 ALUresult是 Byte Addr 所以需除四)
    if( is.memRead ) is.memReadValue = Memory[is.ALUresult/4];
    // 如果 memWrite ==1 代表要寫入Mem 所以寫入ALUresult/4 
    // (因為Mem是 Word Addr 而 ALUresult是 Byte Addr 所以需除四)
    if( is.memWrite ) Memory[is.ALUresult/4] = is.reg2;
    
    return is;
}

/*
 * WB Stage
 * 進行Reg寫入
 * 傳入上個Cycle的MEM_WB並接續執行
 * 
 */ 
instruction pipelineCPUSimulator::WB(instruction is)
{
    // 判斷要寫入的值是memRead or ALUresult
    if( is.memToReg == 1 ) is.regWriteValue = is.ALUresult;
    else is.regWriteValue = is.memReadValue;

    // 確認有要寫入Reg 且寫入的Reg不為0號
    if( is.regWrite && is.WB_reg != 0) Register[is.WB_reg] = is.regWriteValue;
    

    return is;
}


// I Format指令Decode
void pipelineCPUSimulator::I_format(instruction *ret)
{
    if( ret->opcode_str =="lw" )
    {
        ret->opcode = 35;   // lw
        ret->memRead = 1;   // 需要讀取mem
        ret->memWrite = 0;  // 沒有要寫入mem
        ret->memToReg = 0;  // 寫入暫存器的值為mem讀出的
        ret->regDst = 0;    // 寫入暫存器為rt
        ret->regWrite = 1;  // 要寫入Reg
        ret->ALUsrc = 1;    // 是對Offset做運算
        ret->branch = 0; 
    }
    else
    {
        ret->opcode = 43;   // sw
        ret->memRead = 0;   // 沒有要讀取mem
        ret->memWrite = 1;  // 要寫入mem
        ret->memToReg = 0;  // Don't Care
        ret->regDst = 0;    // Don't Care
        ret->regWrite = 0;  // 沒有要寫入Reg
        ret->ALUsrc = 1;    // 是對Offset做運算
        ret->branch = 0;
    }

    stringstream ss;
    // 將除了Opcode之外的Asm指令做Decode
    // Ex. $2, 8($0)
    ss << ret->opcode_remain;

    string tmp;  
    
    // 第一個為rt 利用,切
    getline(ss , tmp , ',');
    // 放入轉換函式
    ret->rt = (uint8_t)stringToInt(tmp , true);

    // 第二個為offset 利用(切
    getline(ss , tmp , '(');
    // 放入轉換函式
    ret->offset_addr = (int16_t)stringToInt(tmp , false);

    // 第三個為rs 利用)切
    getline(ss , tmp , ')');
    // 放入轉換函式
    ret->rs = (uint8_t)stringToInt(tmp , true);


    return;

    

}

void pipelineCPUSimulator::R_format( instruction *ret )
{
    ret->opcode = 0;    // add or sub
    ret->memRead = 0;   // 沒有要讀取Reg
    ret->memWrite = 0;  // 沒有要寫入Reg
    ret->memToReg = 1;  // 放入Reg的值為ALU result
    ret->regDst = 1;    // 放入的Reg為rd
    ret->regWrite = 1;  // 有要寫入Reg
    ret->ALUsrc = 0;    // ALU來源為rt
    ret->branch = 0;    
    ret->ALUopcode = 0; // ALU運算預設為加法

    // 如果指令為sub代表是做減法 所以ALU運算設為減法
    if( ret->opcode_str == "sub" )
        ret->ALUopcode = 1;

    stringstream ss;
    // 將除了Opcode之外的Asm指令做Decode
    // Ex. $1 , $2, 8
    ss << ret->opcode_remain;

    string tmp;  
    
    // 第一個為rd 利用,切
    getline(ss , tmp , ',');
    // 放入轉換函式
    ret->rd = (uint8_t)stringToInt(tmp , true);
    
    // 第二個為rs 利用,切
    getline(ss , tmp , ',');
    // 放入轉換函式
    ret->rs = (uint8_t)stringToInt(tmp , true);

    // 第三個為rt 
    getline(ss , tmp );
    // 放入轉換函式
    ret->rt = (uint8_t)stringToInt(tmp , true);

    
}

void pipelineCPUSimulator::branch( instruction *ret )
{
    ret->opcode = 4;    // beq
    ret->memRead = 0;   // 沒有要讀取mem
    ret->memWrite = 0;  // 沒有要寫入mem
    ret->memToReg = 0;  // Don't care
    ret->regDst = 0;    // Don't care
    ret->regWrite = 0;  // 沒有要寫入reg
    ret->ALUsrc = 0;    // ALU來源為rt
    ret->branch = 1;    // Branch
    ret->ALUopcode = 0; // Don't care

    stringstream ss;
    // 將除了Opcode之外的Asm指令做Decode
    // Ex. $4, $1, -2
    ss << ret->opcode_remain;

    string tmp;  
    // 第一個為rs 利用,切
    getline(ss , tmp , ',');
    ret->rs = (uint8_t)stringToInt(tmp , true);
    
    // 第二個為rt 利用,切
    getline(ss , tmp , ',');
    ret->rt = (uint8_t)stringToInt(tmp , true);

    // 第三個為addr 
    getline(ss , tmp );
    ret->offset_addr = (int16_t)stringToInt(tmp , false);

}
