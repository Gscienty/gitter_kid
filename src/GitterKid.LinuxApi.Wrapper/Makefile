PWD := $(shell pwd)
SHELL = /bin/bash
BIN_DIR = $(PWD)/bin/
OBJS_DIR = $(PWD)/objs/
SOURCE_DIR = $(PWD)/src/
INCLUDE_DIR = $(PWD)/include/
RM = rm -rf
CC = gcc
SHARED = -shared -o
FPIC = -fPIC

$(shell mkdir -p ${BIN_DIR})
$(shell mkdir -p ${OBJS_DIR})

SOURCES = $(SOURCE_DIR)fs_access.c
INCLUDES = $(INCLUDE_DIR)fs_access.h $(INCLUDE_DIR)define.h
OBJECTS = fs_access.o

LIBRARY = libgkwrapper.so

all: $(LIBRARY)
	echo complete.

$(LIBRARY): $(OBJECTS)
	$(CC) $(OBJECTS:%=$(OBJS_DIR)%) $(SHARED) $(FPIC) -o $(BIN_DIR)$@

$(OBJECTS):%.o: $(SOURCES)
	$(CC) $(FPIC) -c  $(SOURCES) -I $(INCLUDE_DIR) -o $(OBJS_DIR)$@

clean:
	$(RM) $(BIN_DIR) $(OBJS_DIR)
