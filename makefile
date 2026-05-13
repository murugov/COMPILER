CC = clang++

FLAGS = -D _DEBUG -ggdb3 -std=c++17 -O3 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
	    -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline \
	    -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
		-Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
		-Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation \
		-fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla

SANITIZE_FLAGS = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

COMMON_INCLUDES		= -I./COMMON/headers
CONFIG_INCLUDES 	= -I./CONFIG
STK_INCLUDES		= -I./STACK
VEC_INCLUDES		= -I./VECTOR
TREE_INCLUDES		= -I./TREE/headers
HT_INCLUDES			= -I./HASH_TABLE
GEN_INCLUDES		= -I./GENERATOR/headers -I./GENERATOR/src -I./GENERATOR/reports
AST_PARSER_INCLUDES = -I./AST_PARSER/headers
DUMP_INCLUDES		= -I./DUMP/headers
FRONT_INCLUDES		= -I./FRONTEND/headers
MIDD_INCLUDES		= -I./MIDDLEEND/headers
BACK_INCLUDES		= -I./BACKEND/headers


COMMON_FILES     = COMMON/is_bad_ptr.cpp COMMON/logger.cpp
TREE_FILES 	     = TREE/tree_func.cpp
AST_PARSER_FILES = AST_PARSER/ast_reader.cpp AST_PARSER/ast_writer.cpp
DUMP_FILES	  	 = DUMP/dump.cpp
FRONT_FILES	  	 = FRONTEND/lexer.cpp FRONTEND/token.cpp FRONTEND/parser.cpp FRONTEND/read_lines.cpp
MIDD_FILES	  	 = MIDDLEEND/SimplifyNode.cpp MIDDLEEND/CalcExpression.cpp MIDDLEEND/CalcFunc.cpp
BACK_FILES	  	 = BACKEND/translator.cpp

ARGS ?= src/data.txt

all: help

gen: GENERATOR/main_gen.cpp $(COMMON_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o gen_program $(FLAGS) GENERATOR/main_gen.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) $(STK_INCLUDES) $(GEN_INCLUDES) $(COMMON_FILES)
	@echo "-----------------------------------------------------------------------------------------"

front: FRONTEND/main_front.cpp $(COMMON_FILES) $(TREE_FILES) $(DUMP_FILES) $(AST_PARSER_FILES) $(FRONT_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	$(CC) -o front_program $(FLAGS) $(SANITIZE_FLAGS) FRONTEND/main_front.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) \
	$(VEC_INCLUDES) $(STK_INCLUDES) $(TREE_INCLUDES) $(HT_INCLUDES) $(DUMP_INCLUDES) $(GEN_INCLUDES) $(AST_PARSER_INCLUDES) $(FRONT_INCLUDES) \
	$(COMMON_FILES) $(TREE_FILES) $(DUMP_FILES) $(AST_PARSER_FILES) $(FRONT_FILES)
	@echo "-----------------------------------------------------------------------------------------"

# leaks-front:
# 	$(CC) -o front_leaks_version $(FLAGS) -O0 FRONTEND/main_front.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) \
# 	$(VEC_INCLUDES) $(TREE_INCLUDES) $(HT_INCLUDES) $(GEN_INCLUDES) $(FRONT_INCLUDES) \
# 	$(COMMON_FILES) $(TREE_FILES) $(FRONT_FILES)
# 	@echo "--- Running a leak test ---"
# 	leaks --atExit -- ./front_leaks_version

midd: MIDDLEEND/main_midd.cpp $(COMMON_FILES) $(TREE_FILES) $(AST_PARSER_FILES) $(FRONT_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o midd_program $(FLAGS) MIDDLEEND/main_midd.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) \
	$(TREE_INCLUDES) $(GEN_INCLUDES) $(AST_PARSER_INCLUDES) $(MIDD_INCLUDES) \
	$(COMMON_FILES) $(TREE_FILES) $(AST_PARSER_FILES) $(MIDD_FILES)
	@echo "-----------------------------------------------------------------------------------------"

back: BACKEND/main_back.cpp $(COMMON_FILES) $(TREE_FILES) $(DUMP_FILES) $(BACK_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o back_program $(FLAGS) BACKEND/main_back.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) \
	$(STK_INCLUDES) $(TREE_INCLUDES) $(AST_PARSER_INCLUDES) $(HT_INCLUDES) $(DUMP_INCLUDES) $(GEN_INCLUDES) $(BACK_INCLUDES) \
	$(COMMON_FILES) $(TREE_FILES) $(AST_PARSER_FILES) $(DUMP_FILES) $(BACK_FILES)
	@echo "-----------------------------------------------------------------------------------------"

run-gen: gen
	./gen_program

run-front: front
	./front_program $(ARGS)

run-back: back
	./back_program
	
run: run-back

clean:
	rm -f gen_program front_program

help:
	@echo "Available commands:"
	@echo ""
	@echo "  make front                    - compile a frontend"
	@echo "  make run-front                - compile and run frontend"
	@echo ""
	@echo "  make back                     - compile a backend"
	@echo "  make run-back                 - compile and run backend"
	@echo ""
	@echo "  make run                      - compile and run backend"
	@echo ""
	@echo "  make clean                    - remove compiled programs"

.PHONY: gen front back run-gen run-front run-back run clean help