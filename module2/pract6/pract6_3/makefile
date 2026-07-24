CC = gcc
AR = ar
CFLAGS = -Wall -Wextra
LIBS_DIR = libs

LIB_NAMES = $(notdir $(wildcard $(LIBS_DIR)/*))

INCLUDES = $(foreach l,$(LIB_NAMES),-I$(LIBS_DIR)/$(l))
LIBPATHS = $(foreach l,$(LIB_NAMES),-L$(LIBS_DIR)/$(l))
LINKLIBS = $(foreach l,$(LIB_NAMES),-l$(l))
RPATH = $(foreach l,$(LIB_NAMES),-Wl,-rpath,$(CURDIR)/$(LIBS_DIR)/$(l))

.PHONY: libs

all: libs main

libs:
	for d in $(LIB_NAMES); do $(MAKE) -C $(LIBS_DIR)/$$d; done

main: main.c 
	$(CC) $(CFLAGS) -o $@ $< $(INCLUDES) $(LIBPATHS) $(LINKLIBS) $(RPATH)

clean:
	find . -name "*.o" -delete
	find . -name "*.so" -delete
	rm -f main
