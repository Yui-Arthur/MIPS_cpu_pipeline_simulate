#include <iostream>
#include "pipeline_CPU_simulator.h"


using namespace std;

int main(int argc, char *argv[])
{
    cin.tie(0);
    ios::sync_with_stdio(false);

    // 帶有參數的就用讀檔處理 
    // 並且可以根據參數的數量重複執行
    if (argc > 1 )
    {
        for (int i = 1; i < argc ; i++)
        {
            cout<<"\n\n-------------------------------------------------------- "<<"Test Data "<<i<<" Start --------------------------------------------------------\n\n";
            // 讀取第i個Test Data Path
            string filename(argv[i]);
            cout<<filename<<"\n\n";
            // 放入pipelineCPUSimulator的建構子中
            pipelineCPUSimulator a(filename);
            // 開始模擬
            a.startSimulation();

            cout<<"\n\n-------------------------------------------------------- "<<"Test Data "<<i<<" End --------------------------------------------------------\n\n";
        }
    }
    // 未帶有參數就使用一般的Stdin處理
    else
    {
        pipelineCPUSimulator a;
        // 開始模擬
        a.startSimulation();
    }



    return 0;
}