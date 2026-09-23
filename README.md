# lox-interpreter

Practice of language and systems exercises from *Crafting Interpreters*.

## What is here

- `c/` - a C doubly linked list example (`linkedList.c`)
- `java/` - a minimal Java hello world example
- `lox/` - a few `.lox` sample programs / edge-case experiments
- `external/craftinginterpreters/` - repository as a git submodule

## Quick start

### Lox files

Compile the Lox interpreter:

```powershell
javac -d .\build\jlox .\external\craftinginterpreters\java\com\craftinginterpreters\lox\*.java
```

Run a Lox file:

```powershell
java -cp .\build\jlox com.craftinginterpreters.lox.Lox .\lox\example.lox
```

Recompile after changing any Java source files.

### C example

```powershell
gcc .\c\linkedList.c -o .\build\linkedList.exe
.\build\linkedList.exe
```

### Java hello-world example

```powershell
javac .\java\helloworld.java
java -cp .\java helloworld
```
