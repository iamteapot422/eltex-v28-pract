CC = "C:\Program Files\JetBrains\CLion 2026.2\bin\mingw\bin\gcc.exe"
AR = "C:\Program Files\JetBrains\CLion 2026.2\bin\mingw\bin\ar.exe"
LIBDIR = libs\phonebook
LDFLAGS = -L${LIBDIR}
CFLAGS = -Wall -Wextra -std=c11

LIB = libpract6_1lib.a
LIB_OBJ = library.o phonebook.o stringlist.o tests.o

TARGET = main.exe
MAIN_OBJ = main.o

all: $(TARGET)

$(LIB): $(LIB_OBJ)
	$(AR) rcs $(LIB) $(LIB_OBJ)

$(TARGET): $(MAIN_OBJ) $(LIB)
	$(CC) $(MAIN_OBJ) $(LDFLAGS) -lpract6_1lib -o $(TARGET)

$(MAIN_OBJ): main.c $(LIBDIR)\pract6_1lib.h
	$(CC) $(CFLAGS) -c main.c

%.o: %.c:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q *.o *.a *.exe