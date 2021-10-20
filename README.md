# jaz80

**J**ust **A**ssemble **Z80**

This program is a simple assembler that assembles Z80 assembly to Z80 object code.

## Set-up

### Prerequisites

To use jaz80 you need premake5, make and gcc.

### Building and running the assembler

To compile the assembler you must first run the command `premake5 gmake2` in the project directory. After that, run `make`.

The assembler will be compiled to `bin/Release/` but you can move the binary to wherever you like. The assembler only requieres a source filename, which should be a path to a Z80 assembly file.

Example: `./bin/Release/jaz80 asm-tests/test-6.asm -o test.com`

#### Testing the assembler

To test the assmbler you probably want to use a [Z80 emulator](https://github.com/superzazu/z80) and use one of the many tests provided in the [asm-tests](asm-tests/) directory. The tests use some CP/M test functions (such as CP/Ms print function). 

After assembling and running the [test-6.asm](asm-tests/test-6.asm) the Z80 emulator should output:

```
Hello, world!
Hello, world!
Hello, world!
Hello, world!
Hello, world!
Hello, world!
Hello, world!
Hello, world!
test 1 executed succesfully!
test 2 executed succesfully!
test 3 executed succesfully!
testing the bit instruction...
testing the reset instruction...
testing the set instruction...
program exiting...
```
