CC = "C:\Program Files\JetBrains\CLion 2026.2\bin\mingw\bin\gcc.exe"
AR = "C:\Program Files\JetBrains\CLion 2026.2\bin\mingw\bin\ar.exe"
CFLAGS = -Wall -Wextra
LIBS_DIR = libs

LIB_NAMES = $(notdir $(wildcard $(LIBS_DIR)/*))

INCLUDES = $(foreach l,$(LIB_NAMES),-I$(LIBS_DIR)/$(l))
LIBPATHS = $(foreach l,$(LIB_NAMES),-L$(LIBS_DIR)/$(l))
LINKLIBS = $(foreach l,$(LIB_NAMES),-l$(l))

.PHONY: libs

all: libs main.exe

libs:
	for %%d in ($(LIB_NAMES)) do $(MAKE) -C $(LIBS_DIR)/%%d

main.exe: main.c libs
	$(CC) $(CFLAGS) -o main.exe main.c $(INCLUDES) $(LIBPATHS) $(LINKLIBS)
	for %%d in ($(LIB_NAMES)) do copy $(LIBS_DIR)\%%d\lib%%d.dll .

clean:
	del /s *.o *.a *.exe *.dll