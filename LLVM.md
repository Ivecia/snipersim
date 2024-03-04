# Sniper with LLVM

# Quick Start

## Preparing Benchmark & Trace

Step 1, compile benchmark to LLVM IR.

```
cd snipersim/trace/test
clang -S -emit-llvm -fno-slp-vectorize -fno-vectorize -fno-unroll-loops -ffp-contract=off matmul.c -pthread
clang -S -emit-llvm -fno-slp-vectorize -fno-vectorize -fno-unroll-loops -ffp-contract=off parallel-add.c -pthread
```

Noted that **any function calls** will be ignored during the simulation of the code.

Step 2, compile the injection code and trace pass, inject code to benchmark, and compile to any platform (like x86), run it. It will output whether a branch will be taken and the addresses of each load and store.

To support multithreading, we use `pthread` model instead of OpenMP or MPI, because it is easy to generate trace and analyse by sniper simulator at LLVM IR level.

```
# Compile injection code
cd snipersim/trace/inject
clang -S -emit-llvm Injection.c -o Injection.ll -pthread
llvm-as Injection.ll
# Compile trace pass
mkdir snipersim/trace/build
cd snipersim/trace/build
cmake ..
make
# Prepare files that sniper needed, and inject code to benchmark
mkdir snipersim/trace/prepare
cd snipersim/trace/prepare
opt -load-pass-plugin ../build/libTraceInfo.so --passes=trace-info ../test/matmul.ll -o matmul.bc
opt -load-pass-plugin ../build/libTraceInfo.so --passes=trace-info ../test/parallel-add.ll -o parallel-add.bc
clang ../inject/Injection.bc matmul.bc -o matmul
clang ../inject/Injection.bc parallel-add.bc -o parallel-add
# Run matmul
./matmul
rm matmul
mkdir matmul
mv trace* matmul
# Run parallel-add
./parallel-add
rm parallel-add
mkdir parallel-add
mv trace* parallel-add
# Remove bitcode of LLVM
rm matmul.bc
rm magic.bc
rm parallel-add.bc
```

Noted that the main thread will generate trace in `trace_main.txt`, and other threads will generate trace in `trace_i.txt` where i is the id of that thread. **Different applications will generate traces with same filenames, so it is important to copy any file to other place between trace generation.**

## Running benchmark in sniper

Step 1, make a dictionary under `snipersim/test/`, like `snipersim/test/magic`, prepare all things which is needed by sniper.

```
mkdir snipersim/test/magic
cd snipersim/test/magic
```

Step 2, copy all files under `prepare` folder, and all related materials.

```
cp -r snipersim/trace/prepare/* snipersim/test/magic
cp snipersim/trace/test/*.ll snipersim/test/magic
# Prepare your own diy file in each folder.
# If you needn't diy, please make sure it only contains a single 0, means no diy at all.
# For example, for parallel-add application, you need to put your diy.txt under snipersim/test/magic/parallel-add/
```

Noted that you should put `diy.txt` as DIY File for each application under its own folder, together with trace.

Step 3, write your own Makefile.

```
# Makefile
TARGET=magic
include ../shared/Makefile.shared

run_$(TARGET):
	../../run-sniper -v -c llvm --llvm-bench=matmul,parallel-add

CLEAN_EXTRA=viz
```

And here are some options:

- `-c llvm`: enable llvm configuration, which is defined in `snipersim/config/llvm.cfg`
- `--llvm-bench=matmul,parallel-add`: using `matmul` and `parallel-add` as the applications for this test.

Step 4, check again. At this time, your dictionary (`snipersim/test/magic` for example) should look like:

```
root@machine:~/snipersim/test/magic# tree
.
|-- Makefile
|-- matmul
|   |-- diy.txt
|   `-- trace_main.txt
|-- matmul.ll
|-- parallel-add
|   |-- diy.txt
|   |-- trace_0.txt
|   |-- trace_1.txt
|   |-- trace_2.txt
|   |-- trace_3.txt
|   |-- trace_4.txt
|   |-- trace_5.txt
|   |-- trace_6.txt
|   |-- trace_7.txt
|   |-- trace_8.txt
|   |-- trace_9.txt
|   `-- trace_main.txt
`-- parallel-add.ll

2 directories, 17 files
```

Step 5, make & run!

```
make run_magic
```

# Design & Implementation

## DIY Toolbox

### Format

DIY format is designed to represent each pattern (merged instruction) appears in benchmark.

DIY format contains several lines:

- Line 1: 1 number, marked as `t`, representing the category of new merged instructions
- Line 2: `t` numbers, representing the latency of each category of new merged instructions
- Line 3: 1 number, marked as `n`, representing the total number of new merged instructions
- The remaining lines are organized by `n` blocks, and each block contains:
  - Line 1: the category of new merged instruction
  - Line 2: enum id for function, and enum id for basic block (under LLVM order), separated by space
  - Line 3: the number of instructions for new merged instruction
  - Line 4: enum id for each instruction, separated by spaces

See `snipersim/test/mat-magic/diy.txt` for example.

### Function

DIY Toolbox provide several functions used in LLVM Reader, LLVM Decoder and Performance Model for LLVM, which will provide necessary information while processing LLVM IR or the latency of LLVM Opcode.

## LLVM Toolbox

LLVM Toolbox contains LLVM Reader, LLVM Decoder and Performance Model for LLVM.

![LLVM Toolbox](/img/1.png)

### LLVM Reader

LLVM Reader is implemented under `snipersim/sift`, which is a part of Sift Reader, responsible for maintaining `pc`, and analyse whether the instruction is part of a merged instruction. It will finally translate LLVM IR to Sift formatted instruction and return to TraceThread as Sift Reader does.

If the reader meet a merged instruction, it will call functions in DIY Toolbox to get any information. The merged instruction will be considered to be appear at the position of the first original instruction.

Noted that the LLVM Reader will disable the original Sift Reader.

### LLVM Decoder

LLVM Decoder is implemented under `snipersim/decoder_lib`, responsible for decode the Sift formatted instruction (which will finally convert to llvm instruction or merged instruction). It will manage the return values related to instructions, like the number of operands, etc.

The address of `llvm::Instruction` will be passed to LLVM Decoder, so that the decoder can use LLVM library to handle different requirements from sniper simulator.

### Performance Model for LLVM

Performance Model for LLVM is responsible for checking the latency of each instruction (both llvm instruction and merged instruction), and provide an interval model for multicore analysis.

The interval model contains ports like `UOP_INT`, `UOP_FLOAT`, `UOP_MEM` and `UOP_ALL` (shared port). For merged instruction, there is another port called `UOP_SPECIAL` (which is not a shared port with all other ports).

The latency for each instruction can be changed under `snipersim/common/performance_model/performance_models/core_model/core_model_llvm.cc`.

## Other Modification

- Support LLVM command line arguments.

# TODO

- Fully multicore support
  - Multicore tracing
  - Multicore Simulating (using fork and join provided in sift_reader.cc)
- Function calls
  - Internal function calls
  - External function calls
- Performance Model
  - ROB Performance Model

# Q&A

## How to disable llvm-fma-intrinsic function like `llvm.fmuladd.f32`?

Using option `-ffp-contract=off` while generating LLVM IR.

## Is that possible to enable a higher level of optimization?

Yes, but some optimizations may lead to unintended errors.