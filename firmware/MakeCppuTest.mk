#Set this to @ to keep the makefile quiet
ifeq (1,${V})
	SILENCE =
else
	SILENCE = @  
endif

#---- Outputs ----#
COMPONENT_NAME = project
TARGET_LIB = \
	lib/lib$(COMPONENT_NAME).a
	
TEST_TARGET = \
	$(COMPONENT_NAME)_tests

#--- Inputs ----#
PROJECT_HOME_DIR = .
CPPUTEST_HOME = ./tests/cpputest
CPP_PLATFORM = Gcc 

CPPUTEST_USE_EXTENSIONS = Y
CPPUTEST_USE_MEM_LEAK_DETECTION = Y

CPPUTEST_WARNINGFLAGS += -Wall 
CPPUTEST_WARNINGFLAGS += -Wextra
#CPPUTEST_WARNINGFLAGS += -Werror 
CPPUTEST_WARNINGFLAGS += -Wswitch-default 
CPPUTEST_WARNINGFLAGS += -Wswitch-enum  
CPPUTEST_CFLAGS += -std=gnu99 
CPPUTEST_CFLAGS += -Wextra 
#CPPUTEST_CFLAGS += -pedantic 
CPPUTEST_CFLAGS += -Wstrict-prototypes
CPPUTEST_CFLAGS += -DSYSTEM_UNIT_TEST=1
CPPUTEST_CFLAGS += -g3
CPPUTEST_CFLAGS += -O0 
CPPUTEST_CPPFLAGS += -DSYSTEM_UNIT_TEST=1

SRC_DIRS = \
	src \

TEST_SRC_DIRS = \
	tests \
		
INCLUDE_DIRS =\
  $(CPPUTEST_HOME)/include/ \
  inc \
  test
  
include $(CPPUTEST_HOME)/build/MakefileWorker.mk