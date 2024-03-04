TARGET=magic
include ../shared/Makefile.shared

run_$(TARGET):
	../../run-sniper -v -c llvm --llvm-bench=magic.ll,magic.ll --diy=diy.txt,diy.txt --traces=trace.txt,trace.txt

CLEAN_EXTRA=viz
