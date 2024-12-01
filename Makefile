.PHONY: all test clean compile gen mkbuild tidy clean

all: compile

run: compile
	@# cd build && ./app/app

test: compile
	cmake -E chdir build ctest

compile: gen
	cmake --build build/ --target all

gen: mkbuild
	cmake -B build/

mkbuild: | build/
	@# mkdir -p build/

build/:
	mkdir -pv build/

lint: tidy
tidy:
	@# clang-tidy --checks='modernize*, readability*' --fix-errors *.cc -- -std=c++17
	find . -type f -iname '*.cc' -exec clang-tidy --checks='modernize*, readability*' {} -- -std=c++17 \;

clean:
	rm -rf build/