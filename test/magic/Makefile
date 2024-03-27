# Makefile
TARGET=magic
include ../shared/Makefile.shared

run_$(TARGET):
	../../run-sniper -v --power -c llvm --llvm-bench=parallel-add,matmul

CLEAN_EXTRA=viz
