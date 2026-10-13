# LLVM Compiler Optimization Toolkit

![CI](https://github.com/Rakshit9702/compiler-optimization/actions/workflows/ci.yml/badge.svg)

A small, modular LLVM new-pass-manager plugin demonstrating constant folding,
basic-block common subexpression elimination, and dead code elimination. The
project builds and tests in GitHub Actions; no compiler development tools are
needed on your computer.

The CI workflow is `.github/workflows/ci.yml`.

## What it does

| Pass | Pipeline name | Behavior |
| --- | --- | --- |
| Constant folding | `toolkit-const-fold` | Replaces instructions LLVM can evaluate from constant operands with their constant result, using the module's data layout. |
| Local CSE | `toolkit-local-cse` | Reuses an identical earlier, side-effect-free, non-memory instruction in the same basic block. |
| Dead code elimination | `toolkit-dce` | Repeatedly removes trivially dead instructions. Side-effecting calls, stores, volatile loads, and instructions whose results are live are retained. |

The combined module pipeline, `llvm-opt-toolkit`, runs the function passes in
constant-folding, local-CSE, and dead-code-elimination order. Each pass reports
preserved analyses accurately: it preserves all analyses when unchanged and
invalidates them after a transformation.

## Layout

- `include/LLVMOptimizationToolkit/Passes.h` declares the function passes.
- `src/` contains the independent pass implementations and plugin/pipeline
  registration.
- `tests/*.ll` are LLVM IR inputs; corresponding `tests/*.check` files describe
  the expected transformed IR. The Python test runner loads the **compiled
  plugin into `opt`** for each individual pass and for the combined pipeline.
- `examples/*.c` are small source-level illustrations of the optimization
  patterns.
- `.github/workflows/ci.yml` installs LLVM 18, builds the plugin, and runs the
  executable IR regression tests.

## Example

Input:

```llvm
define i32 @fold_constants() {
entry:
  %sum = add i32 19, 23
  %answer = mul i32 %sum, 2
  ret i32 %answer
}
```

After `toolkit-const-fold`:

```llvm
define i32 @fold_constants() {
entry:
  ret i32 84
}
```

The regression suite also checks duplicate-expression reuse and verifies that
dead arithmetic is removed without removing a volatile load, a call, or a
store.

## Build and test

CI uses **LLVM 18** on `ubuntu-24.04` (`clang-18`, `llvm-18-dev`, and
`llvm-18-tools` from the runner's Ubuntu package repositories). It configures
with CMake and Ninja, builds the plugin, then runs CTest. The workflow is
triggered by pushes, pull requests, and manual dispatches.

To reproduce the CI build on a machine that already has that LLVM 18 toolchain:

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_DIR=/path/to/llvm-18/lib/cmake/llvm \
  -DLLVM_TOOLS_BINARY_DIR=/path/to/llvm-18/bin
cmake --build build
ctest --test-dir build --output-on-failure
```

On the GitHub-hosted runner, these steps are automated. The repository itself
does not download LLVM or invoke compiler-development tools locally.

To load the built plugin in `opt` directly:

```sh
opt -load-pass-plugin=build/LLVMOptimizationToolkit.so \
  -passes=llvm-opt-toolkit -S tests/constant-fold.ll
```

The plugin filename uses the platform's shared-library suffix; on macOS or
Windows, substitute the generated `.dylib` or `.dll` filename.

## Push to GitHub

The configured remote repository is
[`Rakshit9702/compiler-optimization`](https://github.com/Rakshit9702/compiler-optimization).
If you have not created it yet, create an empty repository with that name.
Then, from this workspace, run:

```sh
git init
git add .
git commit -m "Create LLVM Compiler Optimization Toolkit"
git branch -M main
git remote add origin https://github.com/Rakshit9702/compiler-optimization.git
git push -u origin main
```

If Git prompts for authentication, sign in using Git Credential Manager or use
an SSH remote for an already configured SSH key. The first push triggers GitHub
Actions. The badge will report a real result after the workflow runs; no remote
build has been run from this workspace.
