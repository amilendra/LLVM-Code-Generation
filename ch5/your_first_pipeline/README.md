# Build your first pass pipeline #

In this exercise you will practice how to create your first pass pipeline with both the legacy and the new pass manager.

Go to `yourTurn/passPipelineWithLegacyPM.cpp` and `your_turn/passPipelineWithNewPM.cpp` to implement the pass pipeline for the legacy pass manager and the new pass manager respectively.

For this exercise, you will need to implement a pass pipeline that consists of the three following optimizations, in this order:
- The mem2reg pass (called PromoteMemoryToRegister)
- The instcombine pass
- The always inliner pass

## Configuring your build environment ##

```bash
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=$TOOLS_DIR/LLVM-20.1.1-Linux-X64/lib/cmake/llvm -Bbuild .
```

This will initialize your build directory in `build` (the `-B` option) with Ninja (`-G` option).

You must have a version of LLVM installed at `$TOOLS_DIR/LLVM-20.1.1-Linux-X64` for this to succeed.
Either build and install your own llvm (with the `install` target from your LLVM build) or install an [official LLVM release](https://releases.llvm.org/) package.

## Build ##

```bash
ninja -C build
```

This builds the default target in the build directory.

This should produce in the `build` directory a binary named `your_first_pipeline`.

## Run ##

```bash
./build/your_first_pass [input.ll|.bc]
```

This will run both the reference implementation and your implementation on `input.ll` if specified or the default input if not.

Check that the output look similar for both your and the reference pipelines.

## Solution ##

Look into the `solution` directory to see how to implement the connection of your optimization with both pass manager.

## Producing an input ##

Using the following command line, you can produce from a C file an input to give to your program:
```bash
clang -o - -S -emit-llvm test.c -O0 | sed -e 's#optnone##g' | <path/to/llvm/build>/bin/opt -S -passes=mem2reg,instnamer > input.ll
```

optnone => remove the attribute that prevents optimizations
mem2reg => get rid of stack accesses / build SSA
instnamer => get rid of the implicit variables
