PWD := $(shell pwd)
SHELL = /bin/bash
BIN_DIR = $(PWD)/bin/
OBJS_DIR = $(PWD)/objs/
SOURCE_DIR = $(PWD)/src/
INCLUDE_DIR = $(PWD)/include/
RM = rm -rf
CC = gcc
SHARED = -shared
FPIC = -fPIC

$(shell mkdir -p ${BIN_DIR})
$(shell mkdir -p ${OBJS_DIR})

SOURCES = fs_access.c repository.c
OBJECTS = fs_access.o repository.o

LIBRARY = libgkid.so

all: $(LIBRARY)
	$(shell cp ${BIN_DIR}${LIBRARY} ../GitterKid.LinuxApi/)

$(LIBRARY): $(OBJECTS)
	$(CC) $(OBJS_DIR)$< $(SHARED) $(FPIC) -o $(BIN_DIR)$@

$(OBJECTS):%.o:$(SOURCES)
	$(CC) $(FPIC) -c $(SOURCE_DIR)$< -I $(INCLUDE_DIR) -o $(OBJS_DIR)$@

$(SOURCES):

clean:
	$(RM) $(BIN_DIR) $(OBJS_DIR)
