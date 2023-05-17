set -e

echo "ENVIRONMENT"
echo "Path to LLVM: $LLVM_PATH"
echo "Path to clang: $CLANG_PATH"
echo "Path to opt: $OPT_PATH"
echo "Library extension format: $LIB_EXTENSION"
export LLVM_INCLUDE_DIRS="$LLVM_PATH/include/llvm"

echo ""
echo "Building the LLVM pass..."

make clean
cmake -DLLVM_INCLUDE_DIRS=$LLVM_INCLUDE_DIRS -G "Unix Makefiles" -B build/ .
cmake --build build

echo ""
echo "Compiling examples and running the pass..."

EXAMPLES=(
    "example_1"
    "example_2"
)

for file in "${EXAMPLES[@]}"
do
    echo "Running the pass for file $file"

    $CLANG_PATH -Wno-everything -S -emit-llvm examples/"$file.cpp" -o examples/"$file.ll"
    $OPT_PATH -load "build/libRADeadCodeElimination.$LIB_EXTENSION" -ra-dead-code-elimination -disable-output examples/"$file".ll 
done
