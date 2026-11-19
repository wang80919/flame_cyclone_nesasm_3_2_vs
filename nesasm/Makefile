#指定命令
RM      := del /F /Q
MD      := mkdir

#定义编译器和编译选项
CC      := gcc
CFLAGS  := -O1# -Wall
WINRES  := windres

#定义源代码目录和目标文件目录
BIN_DIR := Bin
DIR_SRC := source
DIR_OBJ := obj

#指定可执行文件路径
TARGET_NAME  = nesasm_3_2.exe
TARGET_PATH  = $(BIN_DIR)/$(TARGET_NAME)

#根据.c源文件名生成对应的.o目标文件名
SRC     := $(wildcard $(DIR_SRC)/*.c)
OBJS    := $(patsubst $(DIR_SRC)/%.c, $(DIR_OBJ)/%.o, $(SRC)) $(DIR_OBJ)/resource.o

#定义生成可执行文件的规则
all: build_prepare resource out_file
default: build_prepare resource out_file

#为每个.c源文件生成对应的.o目标文件
# $@: 表示所有的目标文件
# $^: 表示目标所依赖的所有文件
# $<: 表示依赖文件中的第一个文件
# $?: 表示所有更新过的依赖文件
$(DIR_OBJ)/%.o : $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

#定义清理规则
clean:
	IF EXIST "$(TARGET_PATH)" ( $(RM) "$(BIN_DIR)\$(TARGET_NAME)" )
	IF EXIST "$(DIR_OBJ)" ( $(RM) "$(DIR_OBJ)\*.*" )
    

#定义编译前准备
build_prepare:
	IF NOT EXIST $(BIN_DIR) ($(MD) $(BIN_DIR))
	IF NOT EXIST $(DIR_OBJ) ($(MD) $(DIR_OBJ))

#定义目标文件规则
out_file:$(OBJS)
	$(CC) $(OBJS) -o $(TARGET_PATH)

#编译资源文件
resource: $(DIR_SRC)/nesasm.rc
	$(WINRES) $< -o $(DIR_OBJ)/resource.o
