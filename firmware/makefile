TARGET_EXEC ?= clock.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
INC_DIRS ?= ./inc

PLATFORM := NO_PLATFORM

SRCS := $(shell find $(SRC_DIRS) -name *.c)
INCS := $(shell find $(INC_DIRS) -name *.h)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
DEF_FLAGS := -D$(PLATFORM)

CC := gcc
FORMATTER := clang-format
LINTER := cpplint

TOOLS := $(CC) $(FORMATTER) $(LINTER)

FORMAT_OPTIONS := -style=google -i
LINTER_OPTIONS := --filter=-legal/copyright,-build/include_subdir

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
	$(FORMATTER) $(FORMAT_OPTIONS) $(SRCS) $(INCS)
	
# run linter on source files
lint:
	$(LINTER) $(LINTER_OPTIONS) $(SRCS) $(INCS)

# set up development environment
env-install:
	sudo apt-get install $(TOOLS)

# removed installed tools
env-clean: 
	sudo apt-get remove $(TOOLS)

# remove binaries
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p