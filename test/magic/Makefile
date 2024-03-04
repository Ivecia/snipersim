# Makefile
TARGET=magic
include ../shared/Makefile.shared

run_$(TARGET):
	../../run-sniper -v -c llvm --llvm-bench=matmul

CLEAN_EXTRA=viz