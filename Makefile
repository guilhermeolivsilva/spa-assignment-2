all:
	cmake -DLLVM_INCLUDE_DIRS=${LLVM_INCLUDE_DIRS} -G "Unix Makefiles" -B build/ .
	cmake --build build

clean:
	rm -rf build/
	rm -f tests/*.ll
