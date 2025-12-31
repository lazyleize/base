include ../Makefile.conf
ARGET=libaps.base.a
LIB_TO_BUILD = libaps.base.so
TARGET = $(LIB_TO_BUILD) $(ARGET)

#basic options
#----------------------
LIBCOMPILE += -std=c++11 -I. -fPIC
# C 编译选项（用于 zlib 的 .c 文件，不使用 C++11，但需要 -fPIC 用于共享库）
CLIBCOMPILE += -I. -fPIC

#external paths
# aps
INCLUDEDIR  += -I..
LIBLINK += -L$(PRODUCT_PATH)/lib 

#all source files - 分别处理 .cpp 和 .c 文件
#----------------------
CPPSRC := $(wildcard *.cpp json/*.cpp zlib/*.cpp zip/*.cpp)
CSRC := $(wildcard zlib/*.c)
ALLSRC := $(CPPSRC) $(CSRC)

# 分别生成 .cpp 和 .c 的目标文件
CPPOBJ := $(patsubst %.cpp, $(BUILD_CACHE_DIR)/%.o, $(CPPSRC))
COBJ := $(patsubst %.c, $(BUILD_CACHE_DIR)/%.o, $(CSRC))
ALLOBJ := $(CPPOBJ) $(COBJ)

.PHONY:all
all: createDir $(TARGET)

a:$(ARGET)

createDir:
	@mkdir -p $(BUILD_CACHE_DIR)/src 2>/dev/null
	@mkdir -p $(BUILD_CACHE_DIR)/json 2>/dev/null
	@mkdir -p $(BUILD_CACHE_DIR)/zlib 2>/dev/null
	@mkdir -p $(BUILD_CACHE_DIR)/zip 2>/dev/null

$(LIB_TO_BUILD): $(ALLOBJ)
	$(CC) $(ALLOBJ) -shared -o $@
#	$(MOVEFILE) $@ $(PRODUCT_LIB_PATH)
	
$(ARGET): $(ALLOBJ)
	$(AR) $(ARFLAGS)  $@ $^
#	$(MOVEFILE) $@ $(PRODUCT_LIB_PATH)

#----------------------------------------------------
# C++ 文件编译规则
$(BUILD_CACHE_DIR)/%.o:%.cpp
	$(CC) $(LIBCOMPILE) $(INCLUDEDIR) -c $< -o $@

# C 文件编译规则（不使用 C++11 标准，但需要 -fPIC 用于共享库）
$(BUILD_CACHE_DIR)/%.o:%.c
	$(CC) $(CLIBCOMPILE) $(INCLUDEDIR) -c $< -o $@

clean:
	-rm -rf $(BUILD_CACHE_DIR)

