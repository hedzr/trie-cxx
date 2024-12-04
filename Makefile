#
# Makefile
#
# 2024-11: We are running under g++-14, v14.2.0 / llvm 19 / apple clang 16
#

-include ../mk/detect-env.mk	# setup COS, ARCH
-include ../mk/detect-cc.mk		# setup NASM, LLD, NASM_FMT, NASM_FMT_SUFFIX, CC, CXX, ....
-include _env.mk				# setup NASM_DEBUG_OPTS, ...
-include _env.local.mk			#    your local tuning

-include ../mk/help.mk			# targets: i info help list



SRC_DIR   ?= $(PWD)
BUILD_DIR ?= ./build
OUTDIR    ?= ./bin
GEN_DIR   ?= $(BUILD_DIR)/gen

PROGS      = ask1x




YACC ?= bison
LEX  ?= flex
AS   ?= as
CC   ?= gcc
CXX  ?= g++


LLVM_CONFIG  ?= llvm-config
COMMON_FLAGS  = -Wall -Wextra -fPIC
LDFLAGS      += $(shell $(LLVM_CONFIG) --ldflags)
CFLAGS       += $(COMMON_FLAGS) $(shell $(LLVM_CONFIG) --cflags)
CXXFLAGS     += -std=c++20 $(COMMON_FLAGS) $(shell $(LLVM_CONFIG) --cxxflags)
CPPFLAGS     += $(shell $(LLVM_CONFIG --cppflags) -I$(SRC_DIR)/include)





ifndef VERBOSE
	QUIET := @
endif
ifeq ($(COS),OSX)
	# Use brew version instead of system bundled for the higher version
	BREW := $(shell brew --prefix)
	LEX  := $(BREW)/opt/flex/bin/flex
	YACC := $(BREW)/opt/bison/bin/bison
	CC   := clang
	CXX  := clang++
	LDFLAGS  += -L$(BREW)/opt/flex/lib -L$(BREW)/opt/bison/lib
	CPPFLAGS += -I$(BREW)/opt/flex/include
	CFLAGS   += -I$(BREW)/opt/flex/include
endif



$(GEN_DIR)/%.cc $(GEN_DIR)/%.hh $(GEN_DIR)/%.html $(GEN_DIR)/%.gv: %.y
	$(QUIET)$(YACC) $(YFLAGS) --html=$(GEN_DIR)/$*.html --graph=$(GEN_DIR)/$*.gv -o $(GEN_DIR)/$*.cc $<

$(GEN_DIR)/%.yy.cc: %.l
	$(QUIET)$(LEX) $(LFLAGS) -o $(GEN_DIR)/$@ $<
	@cp $(GEN_DIR)/$@ lex-copy.yy.cc.log

$(BUILD_DIR)/%.o: %.cc
	@echo Compiling $*.cc ...
	$(QUIET)$(CXX) $(CXXFLAGS) -c $(CPPFLAGS) -o$@ $<

$(BUILD_DIR)/%.o: %.c
	@echo Compiling $*.c ...
	$(QUIET)$(CC) $(CFLAGS) -c $(CPPFLAGS) -o$@ $<







.PHONY: all test clean compile gen mkbuild tidy clean

all: info
	@echo ALL END.

build: $(PROGS)
build-all: compile

$(OUTDIR):
	[ -d $@ ] || mkdir $@

info:
	@echo COS = $(COS), ARCH = $(ARCH)
	@echo LEX/FLEX = $(LEX)
	@echo YACC/BISON = $(YACC)

$(OUTDIR)/$(PROGS): $(BUILD_DIR)/lex.yy.o
	$(CXX) $(CXXFLAGS) -o$@ $<
	@echo $(PROGS) END.

clean:
	-rm -f $(BUILD_DIR)/a.out *.log *.yy.{c,o} *.tab.{c,o} $(PROGS:=.cc) $(OUTDIR)/$(PROGS)



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