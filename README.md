# MIPS_cpu_pipeline_simulate
### Use C++ to simulate MIPS CPU with five Stage Pipeline and Forwarding 
### ![](https://i.imgur.com/NEIUa9u.png)
## Run Code

### Clone Repository And Use gcc Compile
```bash
git clone https://github.com/Yui-Arthur/MIPS_cpu_pipeline_simulate.git
cd MIPS_cpu_pipeline_simulate
gcc main.cpp pipeline_CPU_simulator.cpp -o main
```

### Use File as Input

```bash
./main ${FilePath}
./main sample_input/input1.txt
```
or

```bash
./main << ${FilePath}
./main << sample_input/input1.txt
```

### Use Stdin as Input

```bash
./main 
# input Mips Asm Code
# Ex. lw $2, 8($0)
```
