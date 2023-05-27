set -e

echo "ENVIRONMENT"
echo "Path to LLVM: $LLVM_PATH"
echo "Library extension format: $LIB_EXTENSION"
export LLVM_INCLUDE_DIRS="$LLVM_PATH/include/llvm"

# if clang++ and opt are not set to llvm8, uncomment
# and edit if necessary
# alias c++=PATH_TO_c++
# alias opt=PATH_TO_opt

echo ""
echo "Building the LLVM pass..."

make clean
cmake -DLLVM_INCLUDE_DIRS=$LLVM_INCLUDE_DIRS -G "Unix Makefiles" -B build/ .
cmake --build build -j 4

echo ""
echo "Compiling tests and running the pass..."

TESTS=(
    "test_1"
    "test_2"
)

for file in "${TESTS[@]}"
do
    echo "Running the pass for file $file"

    clang++ -Xclang -disable-O0-optnone -S -emit-llvm tests/"$file.cpp" -o tests/"$file.ll"

    # First optimization to enable the Range Analysis (otherwise, it will return [-inf, inf] for all the tested ranges)
    opt -instnamer -mem2reg -break-crit-edges tests/"$file.ll" -S -o tests/"$file.ll"

    # Run the Dead Code Elimination pass
    opt -load "build/libDeadCodeElimination.$LIB_EXTENSION" -vssa -dead-code-elimination -stats tests/"$file".ll -o tests/"$file"_optimized.ll
done
