CC = gcc
AR = ar
LIB_DIR = libs/phonebook
LIB_SOURCE_DIR = $(LIB_DIR)/source
LIB_OBJ_DIR = $(LIB_DIR)/objects
LDFLAGS = -L${LIB_DIR} -Wl,-rpath,$(LIB_DIR)
CFLAGS = -Wall -Wextra -std=c11 -I$(LIB_SOURCE_DIR) -fPIC

SO = $(LIB_DIR)/libpract6_2lib.so

OBJ = $(LIB_OBJ_DIR)/library.o \
	$(LIB_OBJ_DIR)/phonebook.o \
	$(LIB_OBJ_DIR)/stringlist.o \
	$(LIB_OBJ_DIR)/tests.o

TARGET = main
MAIN_OBJ = main.o

all: $(TARGET)

$(SO): $(OBJ)
	$(CC) -shared -o $(SO) $(OBJ)

$(TARGET): $(MAIN_OBJ) $(SO)
	$(CC) $(MAIN_OBJ) $(LDFLAGS) -lpract6_2lib -o $(TARGET)

$(MAIN_OBJ): main.c $(LIB_SOURCE_DIR)/library.h
	$(CC) $(CFLAGS) -c main.c

$(LIB_OBJ_DIR)/%.o: $(LIB_SOURCE_DIR)/%.c | $(LIB_OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_OBJ_DIR):
	mkdir -p $(LIB_OBJ_DIR)

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.so" -type f -delete
	find . -name "*.a" -type f -delete
	rm $(TARGET)
