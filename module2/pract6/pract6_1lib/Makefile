CC = gcc
CFLAGS = -Wall -Wextra -std=c11

LIB = libpract6_1lib.a
LIB_OBJ = library.o phonebook.o stringlist.o

TARGET = main.exe
MAIN_OBJ = main.o

all: $(TARGET)

$(LIB): $(LIB_OBJ)
	ar rcs $(LIB) $(LIB_OBJ)

$(TARGET): $(MAIN_OBJ) $(LIB)
	$(CC) $(MAIN_OBJ) -L. -lpract6_1lib -o $(TARGET)

main.o: main.c pract6_1lib.h
	$(CC) $(CFLAGS) -c main.c

library.o: library.c pract6_1lib.h
	$(CC) $(CFLAGS) -c library.c

phonebook.o: phonebook.c pract6_1lib.h
	$(CC) $(CFLAGS) -c phonebook.c

stringlist.o: stringlist.c pract6_1lib.h
	$(CC) $(CFLAGS) -c stringlist.c

clean:
	del /Q *.o *.a *.exe