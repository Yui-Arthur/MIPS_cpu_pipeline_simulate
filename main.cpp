#include <iostream>
#include "pipeline_CPU_simulator.h"


using namespace std;

int main()
{
    cin.tie(0);
    ios::sync_with_stdio(false);

    pipelineCPUSimulator a("sample_input\\input1.txt");

    a.startSimulation();


    return 0;
}