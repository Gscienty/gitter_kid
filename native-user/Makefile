PWD := $(shell pwd)
BIN_DIR = $(PWD)/bin/
OBJS_DIR = $(PWD)/objs/
SOURCES_DIR = $(PWD)/src/
INCLUDE_DIR = $(PWD)/include/
CC = g++
FPIC = -fPIC
SHARE = -shared

$(shell mkdir -p ${BIN_DIR})
$(shell mkdir -p ${OBJS_DIR})

OUT_LIBRARY = libgkiduser.so

SOURCES = $(patsubst %.cpp, %, $(notdir $(wildcard $(PWD)/src/*.cpp)))

all: $(OUT_LIBRARY)

test: $(SOURCES) comp_test
	$(CC) $(SOURCES:%=$(OBJS_DIR)%.o) $(OBJS_DIR)tester.o -o test/tester
	test/tester

$(OUT_LIBRARY): $(SOURCES)
	$(CC) $(SOURCES:%=$(OBJS_DIR)%.o) $(SHARE) $(FPIC) -o $(BIN_DIR)$(OUT_LIBRARY) 

comp_test:
	$(CC) -g -c test/tester.cpp -I $(INCLUDE_DIR) -o $(OBJS_DIR)tester.o

$(SOURCES):
	$(CC) -g $(FPIC) -c $(SOURCES_DIR)$@.cpp -I $(INCLUDE_DIR) -o $(OBJS_DIR)$@.o

clean:
	rm -rf $(BIN_DIR) $(OBJS_DIR)