#ifndef _PIPLINE_CPU_SIMULATOR_H_
#define _PIPLINE_CPU_SIMULATOR_H_
#include <vector>

using namespace std;

struct instruction{

    uint8_t opcode = 0;             // 
    string opcode_str = "null";     // Asm opcode Ex. lw / beq
    string opcode_remain = "null";  // Asm 剩下的 Ex. $2, 8($0) / $1,$2,$3
    uint8_t rs = 0;                 // Decode出來的rs
    uint8_t rt = 0;                 // Decode出來的rt
    uint8_t rd = 0;                 // Decode出來的rd
    int16_t offset_addr = 0;        // Decode出來的offset/ addr
    uint8_t WB_reg = 0;             // 要寫回的Reg編號
    bool branch = 0;                // 是否為branch
    bool memRead= 0;                // 是否要讀取mem
    bool memWrite= 0;               // 是否要寫入mem
    bool ALUsrc= 0;                 // ALU 要選擇rt還是offset/addr
    bool memToReg= 0;               // 要寫回Reg的資料為memRead還是ALUreslut
    bool regDst= 0;                 // 要寫入的Reg編號
    bool regWrite= 0;               // 是否要寫入Reg
    bool ALUopcode= 0;              // ALU的運算方式
    bool freezePC = 0;              // 是否要凍結PC
    bool changePC = 0;              // 是否要改變PC

    int32_t reg1 = 0;               // 從rs讀出來的值
    int32_t reg2 = 0;               // 從rt讀出來的值
    int32_t ALUresult = 0;          // ALU運算的結果
    int32_t memReadValue = 0;       // mem讀出來的值
    int32_t regWriteValue = 0;      // 要寫回到Reg的值
    int32_t PC = 0;                 // 這個指令在IF時的PC(已經+4)

};

class pipelineCPUSimulator{

    private:
        
        int32_t Register[32];           // Reg 有32個Word 所以用int32的陣列
        int32_t Memory[32];             // Mem 有32個Word 所以用int32的陣列
        vector<string> asm_code_list;   // 放Asm Code的vector
        int32_t PC = 0;                 // 目前的PC (正常Mips CPU PC一次+4 但在我們的模擬中一次+1)
        
        void init();    // 對Mem及Reg做初始化的動作
        instruction IF(instruction ID_EX_is, instruction IF_ID_is); // Pipeline中的IF執行階段
        instruction ID(instruction is , instruction ID_EX_is , instruction EX_MEM_is , instruction MEM_WB_is);  // Pipeline中的ID執行階段
        instruction EX(instruction is , instruction previous1_is , instruction previous2_is);   // Pipeline中的EX執行階段
        instruction MEM(instruction is);    // Pipeline中的MEM執行階段
        instruction WB(instruction is);     // Pipeline中的WB執行階段
        void I_format(instruction *ret );   // I format指令Decode
        void R_format(instruction *ret );   // R format指令Decode
        void branch(instruction *ret );     // branch指令Decode
        int stringToInt(string &s , bool is_reg);   // 將String轉為int
        
        //void readAsmCode(vector<string>&asm_code_list);



    public:
        pipelineCPUSimulator();                             // 沒有給路徑的建構子 用stdin來輸入測資
        pipelineCPUSimulator(string filename);              // 有給路徑的建構子 用讀檔來輸入測資
        void print_stats(int cycles , instruction *is );    // 將目前的狀態印出來 傳入所有紀錄狀態的Instruction變數
        void print_reg_memory();                            // 印出Reg及Mem的值
        void startSimulation();                             // Instruction主要執行的地方



};

#endif