CFLAGS = -std=c11 -fPIC

LIB = libpract6_2lib.dll
SRC = library.c phonebook.c stringlist.c
OBJ = $(SR:.c=.o)

all: $(LIB)

$(LIB): $(OBJ)
	gcc -shared -o $(LIB) $(OBJ)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@