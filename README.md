# Project assignment 2 - Range Analysis based Dead Code Elimination

## Students

Guilherme O. Silva - 2023671528

Henrique F. Scheid - 2017014898

## Environment

Requirements:

* LLVM 8.0.1
* CMake 3.16.3

Tested with

* Ubuntu 20.04.4

## Setup

### Setup environment variables

This project requires two environment variables to be set before running our `run.sh` script. You must set

* `LLVM_PATH`: path to the root directory of your LLVM distribution (i.e., the one that contains the `include/` directory).
* `LIB_EXTENSION`: set it to `so` if running Linux, or `dylib` if running macOS.

> You can set these variables together with the bash script run call, with `LLVM_PATH=... LIB_EXTENSION=... bash run.sh`.

### Run `run.sh`

From a terminal window pointed to the root of this project, simply call

```
bash run.sh
```

This file will

1. Create the `build/` directory, to contain the compiled pass.
2. Build the LLVM pass and output the compiled library to `build/libRADeadCodeElimination.{so|dylib}`.
3. Compile the test cases (`tests/*.cpp`) and generate LLVM Intermediate Representations for each source, in `.ll` format, with `clang++`.
4. Apply some optimizations to the IR files – namely `-instnamer`, `-mem2reg`, and `-break-crit-edges` – to ensure the pass can eliminate instructions.
5. Run the pass on the optimized IRs, output its final versions to `tests/..._optimized.ll` files, and print the collected statistics on the terminal.

> Notice that this project assumes that the `clang++` and `opt` that comes first in your PATH are the ones associated to LLVM 8. If not, adjust the bash script accordingly.
