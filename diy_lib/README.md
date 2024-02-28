# DiyTool

## Diy File Format

```
// category of new ops
<number of categories of new ops>
<latency of new op 1> <latency of new op 2> ... <latency of new op k>
// founded pattern in llvm (the number of new ops in bb is larger than or equal to the number of categories of new ops)
<number of new ops in bb>
// new op 1
<category of new op 1>
<enum id for func of new op 1> <enum id for basic block of new op 1>
<number of instructions for new op 1>
<enum id for instruction 1> <enum id for instruction 2> ... <enum id for instruction k>
// new op 2
...... (same as new op 1)
```

`//` means comments, should not appear in Diy File.

## Diy File Example

[TODO]