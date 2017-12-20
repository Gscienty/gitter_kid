PWD := $(shell pwd)
SHELL = /bin/bash
BIN_DIR = $(PWD)/bin/
OBJS_DIR = $(PWD)/objs/
SOURCE_DIR = $(PWD)/src/
INCLUDE_DIR = $(PWD)/include/
TEST_DIR = $(PWD)/test/
RM = rm -rf
CC = gcc
SHARED = -shared
FPIC = -fPIC

LOG_FLAG = -D LOG_FLAG=1

$(shell mkdir -p ${BIN_DIR})
$(shell mkdir -p ${OBJS_DIR})

SOURCES = fs_access repository __pwio user
LIBRARY = libgkid.so

all: $(LIBRARY)

$(LIBRARY): $(SOURCES)
	$(CC) $(SOURCES:%=$(OBJS_DIR)%.o) $(SHARED) $(FPIC) -o $(BIN_DIR)$(LIBRARY)

$(SOURCES):
	$(CC) $(LOG_FLAG) $(FPIC) -c $(SOURCE_DIR)$@.c -I $(INCLUDE_DIR) -o $(OBJS_DIR)$@.o

clean:
	$(RM) $(BIN_DIR) $(OBJS_DIR)
