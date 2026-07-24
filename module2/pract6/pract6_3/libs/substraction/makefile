CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -std=c11 -fPIC

NAME = substraction
SO = lib$(NAME).so
OBJ = $(NAME).o
SOURCE = $(NAME).c

$(SO): $(OBJ)
	$(CC) $(CFLAGS) -c $(SOURCE)
	$(CC) -shared -o $(SO) $(OBJ)
