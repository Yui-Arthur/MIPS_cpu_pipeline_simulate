#ifndef _PIPLINE_CPU_SIMULATOR_H_
#define _PIPLINE_CPU_SIMULATOR_H_
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
    uint8_t WB_reg = 0;
    bool branch = 0;
    bool memRead= 0;
    bool memWrite= 0;
    bool ALUsrc= 0;
    bool memToReg= 0;
    bool regDst= 0;
    bool regWrite= 0;
    bool ALUopcode= 0;
    bool freezePC = 0;
    bool changePC = 0;

    int32_t reg1 = 0;
    int32_t reg2 = 0;
    int32_t ALUresult = 0;
    int32_t memReadValue = 0;
    int32_t regWriteValue = 0;
    int32_t PC = 0;

};

class pipelineCPUSimulator{

    private:

        int32_t Register[32];
        int32_t Memory[32];
        vector<string> asm_code_list;
        int32_t PC = 0;
        




        void init();
        instruction IF(instruction ID_EX_is, instruction IF_ID_is);
        instruction ID(instruction is , instruction ID_EX_is , instruction EX_MEM_is , instruction MEM_WB_is);
        instruction EX(instruction is , instruction previous1_is , instruction previous2_is);
        instruction MEM(instruction is);
        instruction WB(instruction is);
        void I_format(instruction *ret );
        void R_format(instruction *ret );
        void branch(instruction *ret );
        int stringToInt(string &s , bool is_reg);
        
        //void readAsmCode(vector<string>&asm_code_list);



    public:
        pipelineCPUSimulator();
        pipelineCPUSimulator(string filename);
        void print_stats(int cycles , instruction *is );
        void print_reg_memory();
        void startSimulation();



};

#endif