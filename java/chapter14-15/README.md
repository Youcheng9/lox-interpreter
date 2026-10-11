# Chapter 14: Compressing Line Information

## The problem

Every bytecode instruction needs to remember the source line that produced it.
The original implementation stored one `int` per instruction. That is wasteful
because a source line usually produces several consecutive instructions.

Instead of storing repeated values such as:

```text
instructions:  0  1  2  3  4  5  6
line numbers:  7  7  7  7  8  8  9
```

we store only where the line changes:

```text
{ offset: 0, line: 7 }
{ offset: 4, line: 8 }
{ offset: 6, line: 9 }
```

Each `LineStart` marks the first instruction for a source line. Following
instructions belong to that line until the next `LineStart`.

## Implementation approach

1. `Chunk` stores a dynamic array of `LineStart` records instead of one line
   number for every instruction.
2. `writeChunk()` adds a new `LineStart` only when the line changes.
3. `getLine(chunk, instruction)` finds the last line start whose offset is at
   or before the instruction using binary search.
4. Disassembly and runtime-error reporting call `getLine()`.

The compiler emits instructions in source order, so line-start offsets are
monotonically increasing. The lookup is used only for debugging and runtime
errors, not normal execution, so its small cost is acceptable.

## Files changed

- `c/chunk.h` — defines `LineStart` and declares `getLine()`.
- `c/chunk.c` — maintains the compressed table and implements lookup.
- `c/debug.c` — uses `getLine()` while disassembling.
- `c/vm.c` — uses `getLine()` in runtime errors.

## Presentation summary

> We replaced repeated line numbers with a list of line changes. Writing stays
> constant-time in the common same-line case, while error reporting finds the
> most recent line start.

## Exercise 2: large constants

The answer key in `external/craftinginterpreters/note/answers/chapter14_chunks/2.md`
uses two instruction sizes:

- `OP_CONSTANT` keeps its one-byte operand for constant indexes `0` through
  `255`.
- `OP_CONSTANT_LONG` stores a 24-bit constant index in three little-endian
  operand bytes.

`writeConstant()` adds the value first, then chooses the compact or long form.
The disassembler reconstructs the 24-bit index and prints the referenced value.

The tradeoffs are a slightly more complex interpreter, one fewer available
opcode, and potentially a small instruction-cache cost. In return, common
small constant tables keep their compact two-byte instruction format while
large chunks can address up to 16,777,216 constants.

## Exercise 3: dynamically growing the VM stack

The stack-growth solution is in `c/vm.h` and `c/vm.c`:

- `VM` stores a heap-allocated `Value* stack` and `stackCapacity`.
- `push()` calls `growStack()` when the current capacity is full.
- `growStack()` allocates a larger array, copies the values, and repairs
  `stackTop`, call-frame slot pointers, and open-upvalue pointers.
- `freeVM()` releases the dynamically allocated stack.

## Exercise 4: in-place stack operations

`c/vm.c` updates stack-height-preserving operations directly in their existing
slots. `OP_NEGATE` and `OP_NOT` rewrite the top value, while equality,
comparisons, and numeric arithmetic write the result into the left operand's
slot and decrement `stackCount` once. This avoids unnecessary pop/push pairs.
