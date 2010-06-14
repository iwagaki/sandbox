OBJ_DIR         = ./obj
DEPEND_FILE     = Makefile.d

CXXFLAGS        += -c -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE
# Bruce Evans' BDE options
CXXFLAGS        += -Wall -W -Wno-format-y2k -Wpointer-arith -Wreturn-type -Wcast-qual -Wwrite-strings
CXXFLAGS        += -Wswitch -Wshadow -Wcast-align -Wuninitialized -Wformat=2
CFLAGS          += -Wstrict-prototypes -Wmissing-prototypes

RELEASE_FLAGS   = -O2 -DNDEBUG
DEBUG_FLAGS     = -O2 -g -DDEBUG

TARGET          = ./a.out

LDFLAGS         += -lstdc++
#LDFLAGS         += -Wl,--version-script,version.map 
INCLUDE_FLAGS   = -I common

#CMD_PREFIX      = arm-none-linux-gnueabi-
CMD_PREFIX      =
STRIP           = $(CMD_PREFIX)strip
CC              = $(CMD_PREFIX)gcc
CXX             = $(CMD_PREFIX)g++
AR              = $(CMD_PREFIX)ar
MAKE            = $(CMD_PREFIX)make
VPATH           = $(OBJ_DIR)

ifeq ($(strip $(RELEASE)), y)
 CXXFLAGS       += $(RELEASE_FLAGS)
else
 CXXFLAGS       += $(DEBUG_FLAGS) 
endif

CXXFLAGS        += $(INCLUDE_FLAGS)
CFLAGS          += $(CXXFLAGS)

CXX_SRCS        = $(shell find -name "*.cpp") $(shell find -name "*.cc")
CC_SRCS         = $(shell find -name "*.c")
OBJS            = $(subst .cpp,.o,$(subst .cc,.o,$(CXX_SRCS)))

.PHONY: all
all: depend $(OBJS)
	$(CXX) $(addprefix $(OBJ_DIR)/, $(OBJS)) $(LDFLAGS) -o $(TARGET)

.PHONY: release
release:
	@$(MAKE) -f Makefile RELEASE=y all

.PHONY: clean
clean: depend_clean
	@rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: depend
depend: depend_clean
ifneq ($(strip $(CXX_SRCS)),)
	@$(CXX) $(CXXFLAGS) -M $(CXX_SRCS) >> $(DEPEND_FILE)
endif
ifneq ($(strip $(CC_SRCS)),)
	@$(CC) $(CFLAGS) -M $(CC_SRCS) >> $(DEPEND_FILE)
endif

.PHONY: depend_clean
depend_clean:
	@rm -rf ./$(DEPEND_FILE)

.SUFFIXES: .c .cpp .cc .o
.cpp.o:
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $(OBJ_DIR)/$@

.cc.o:
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $(OBJ_DIR)/$@

.c.o:
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	$(CC) $(CFLAGS) $< -o $(OBJ_DIR)/$@

-include $(DEPEND_FILE)
