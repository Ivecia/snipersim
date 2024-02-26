TARGET=magic
include ../shared/Makefile.shared

run_$(TARGET):
	../../run-sniper -v -c llvm --llvm-bench=$(shell pwd)/magic.ll --traces=trace.txt,trace.txt,trace.txt

CLEAN_EXTRA=viz
