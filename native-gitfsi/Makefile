PWD := $(shell pwd)
SHELL = /bin/sh
BIN_DIR = $(PWD)/bin/
OBJS_DIR = $(PWD)/objs/
SOURCE_DIR = $(PWD)/src/
INCLUDE_DIR = $(PWD)/include/
RM = rm -rf
CC = clang++

$(shell mkdir -p ${BIN_DIR})
$(shell mkdir -p ${OBJS_DIR})

SOURCES = $(patsubst %.cc,%,$(notdir $(wildcard $(SOURCE_DIR)*.cc)))
LINKS = z
OUT_LIBRARY = libgitfsi.so

all: $(OUT_LIBRARY)

$(OUT_LIBRARY): $(SOURCES)
	$(CC) $(SOURCES:%=$(OBJS_DIR)%.o) -shared -fPIC -o $(BIN_DIR)$(OUT_LIBRARY) $(LINKS:%=-l%)

$(SOURCES):
	$(CC) -std=c++11 -g -fPIC -c $(SOURCE_DIR)$@.cc -I $(INCLUDE_DIR) -o $(OBJS_DIR)$@.o

clean:
	$(RM) $(BIN_DIR) $(OBJS_DIR)

