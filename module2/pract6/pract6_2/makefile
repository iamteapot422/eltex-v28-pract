CC = "C:\Program Files\JetBrains\CLion 2026.2\bin\mingw\bin\gcc.exe"
AR = "C:\Program Files\JetBrains\CLion 2026.2\bin\mingw\bin\ar.exe"
LIB_DIR = libs/phonebook
LIB_SOURCE_DIR = $(LIB_DIR)/source
LIB_OBJ_DIR = $(LIB_DIR)/objects
LDFLAGS = -L${LIB_DIR}
CFLAGS = -Wall -Wextra -std=c11 -Ilibs/phonebook/source -fPIC

DLL = libpract6_2lib.dll
IMPLIB = libpract6_2lib.dll.a

OBJ = $(LIB_OBJ_DIR)/library.o \
	$(LIB_OBJ_DIR)/phonebook.o \
	$(LIB_OBJ_DIR)/stringlist.o \
	$(LIB_OBJ_DIR)/tests.o

TARGET = main.exe
MAIN_OBJ = main.o

all: $(TARGET)

$(DLL): $(OBJ)
	$(CC) -shared -o $(DLL) $(OBJ) -Wl,--out-implib,$(IMPLIB)

$(TARGET): $(MAIN_OBJ) $(DLL)
	$(CC) $(MAIN_OBJ) $(LDFLAGS) -lpract6_2lib -o $(TARGET)

$(MAIN_OBJ): main.c $(LIB_SOURCE_DIR)/library.h
	$(CC) $(CFLAGS) -c main.c

$(LIB_OBJ_DIR)/%.o: $(LIB_SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /s *.o *.a *.exe *.dll