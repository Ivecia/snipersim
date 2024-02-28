TARGET=magic
include ../shared/Makefile.shared

run_$(TARGET):
	../../run-sniper -v -c llvm --llvm-bench=magic.ll --diy=diy.txt --traces=trace.txt

CLEAN_EXTRA=viz
