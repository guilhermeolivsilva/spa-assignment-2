set -e

echo "ENVIRONMENT"
echo "Path to LLVM include/: $LLVM_INCLUDE"
echo "Path to LLVM bin/: $LLVM_BIN"
echo "Library extension format: $LIB_EXTENSION"

echo ""
echo "Building the LLVM pass..."

make clean
cmake -DLLVM_INCLUDE_DIRS="$LLVM_INCLUDE/llvm" -G "Unix Makefiles" -B build/ .
cmake --build build -j 4

echo ""
echo "Compiling tests and running the pass..."

TESTS=(
    "test_1"
    "test_2"
    "test_3"
)

for file in "${TESTS[@]}"
do
    echo "Running the pass for file $file"

    "$LLVM_BIN/clang++" -Xclang -disable-O0-optnone -S -emit-llvm tests/"$file.cpp" -o tests/"$file.ll"

    # First optimization to enable the Range Analysis (otherwise, it will return [-inf, inf] for all the tested ranges)
    "$LLVM_BIN/opt" -instnamer -mem2reg -break-crit-edges tests/"$file.ll" -S -o tests/"$file.ll"

    # Run the Dead Code Elimination pass
    "$LLVM_BIN/opt" -load "build/libDeadCodeElimination.$LIB_EXTENSION" -vssa -dead-code-elimination -stats tests/"$file".ll -o tests/"$file"_optimized.ll
done
