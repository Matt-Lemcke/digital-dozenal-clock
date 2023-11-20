TARGET_EXEC ?= clock.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
INCLUDE_DIRS ?= ./inc

PROJECT_DIR 	= $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
TEST_DIR 		= $(PROJECT_DIR)tests

PLATFORM := NO_PLATFORM

SRCS := $(shell find $(SRC_DIRS) -name *.c)
INCS := $(shell find $(INCLUDE_DIRS) -name *.h)
TESTS := $(shell find $(TEST_DIR) -maxdepth 1 -name *.c )
TEST_RUNNERS := $(shell find $(TEST_DIR) -maxdepth 1 -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))
DEF_FLAGS := -D$(PLATFORM)

CC := gcc
CPPC := g++
FORMATTER := clang-format
LINTER := cpplint

TOOLS := $(CC) $(FORMATTER) $(LINTER) $(CPPC)

FORMAT_OPTIONS := -style=file:.clang-format -i
LINTER_OPTIONS := --filter=-legal/copyright,-build/include_subdir,-runtime/printf,-whitespace/braces,-whitespace/newline, --exclude=tests

# CppUTest variables
CPPUTEST_DIR 	= $(TEST_DIR)/cpputest
CPPFLAGS 		+= -I$(CPPUTEST_DIR)/include
LD_LIBRARIES 	= -L$(CPPUTEST_DIR)/lib -lCppUTest -lCppUTestExt

# build c source files
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@  $(INC_FLAGS) $(DEF_FLAGS)


.PHONY: clean

# build executable file
build: $(OBJS)
	$(CC) $(OBJS) -o $(BUILD_DIR)/$(TARGET_EXEC)

# run executable
run: $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC)

# format source files
format: $(SRCS) $(INCS)
	$(FORMATTER) $(FORMAT_OPTIONS) $(SRCS) $(INCS) $(TESTS) $(TEST_RUNNERS)
	
# run linter on source files
lint:
	$(LINTER) $(LINTER_OPTIONS) $(SRCS) $(INCS) $(TESTS) $(TEST_RUNNERS)

test:
	make -f MakeCppuTest.mk all

# set up development environment
env-install:
	sudo apt-get install $(TOOLS)

# removed installed tools
env-clean: 
	sudo apt-get remove $(TOOLS)

# remove binaries
clean:
	$(RM) -r $(BUILD_DIR)
	make -f MakeCppuTest.mk clean

-include $(DEPS)

MKDIR_P ?= mkdir -p