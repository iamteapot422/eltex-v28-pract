CC = gcc
AR = ar
LIB_DIR = libs/phonebook
LIB_SOURCE_DIR = $(LIB_DIR)/source
LIB_OBJ_DIR = $(LIB_DIR)/objects
LDFLAGS = -L${LIB_DIR}
CFLAGS = -Wall -Wextra -std=c11

LIB = $(LIB_DIR)/libpract6_1lib.a
OBJ = $(LIB_OBJ_DIR)/library.o \
	$(LIB_OBJ_DIR)/phonebook.o \
	$(LIB_OBJ_DIR)/stringlist.o \
	$(LIB_OBJ_DIR)/tests.o

TARGET = main
MAIN_OBJ = main.o

all: $(TARGET)

$(LIB): $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

$(TARGET): $(MAIN_OBJ) $(LIB)
	$(CC) $(MAIN_OBJ) $(LDFLAGS) -lpract6_1lib -o $(TARGET)

$(MAIN_OBJ): main.c $(LIB_SOURCE_DIR)/pract6_1lib.h
	$(CC) $(CFLAGS) -c main.c

$(LIB_OBJ_DIR)/%.o: $(LIB_SOURCE_DIR)/%.c | $(LIB_OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_OBJ_DIR):
	mkdir -p $(LIB_OBJ_DIR)

clean:
	find . -name "*.a" -type f -delete
	rm -f $(TARGET)
