CC = "C:\Program Files\JetBrains\CLion 2026.2\bin\mingw\bin\gcc.exe"
AR = "C:\Program Files\JetBrains\CLion 2026.2\bin\mingw\bin\ar.exe"
CFLAGS = -Wall -Wextra -std=c11 -fPIC

NAME = multiplication
DLL = lib$(NAME).dll
IMPLIB = $(DLL).a
OBJ = $(NAME).o
SOURCE = $(NAME).c

$(DLL): $(OBJ)
	$(CC) $(CFLAGS) -c $(SOURCE)
	$(CC) -shared -o $(DLL) $(OBJ) -Wl,--out-implib,$(IMPLIB)