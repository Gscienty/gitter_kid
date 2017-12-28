PWD := $(shell pwd)
SHELL = /bin/bash
BIN_DIR = $(PWD)/bin/
OBJS_DIR = $(PWD)/objs/
SOURCE_DIR = $(PWD)/src/
INCLUDE_DIR = $(PWD)/include/
TEST_DIR = $(PWD)/test/
REF_LIBRARY = $(PWD)/lib/
RM = rm -rf
CC = gcc
SHARED = -shared
FPIC = -fPIC

LOG_FLAG = -D DBG=1

$(shell mkdir -p ${BIN_DIR})
$(shell mkdir -p ${OBJS_DIR})

# this module need setup pam-devel - centos OR libpam0g-dev - ubuntu
# this module need setup zlib-devel -centos OR zlib1g.dev - ubuntu

SOURCES = $(patsubst\
	%.c,%,\
	$(notdir \
		$(wildcard $(PWD)/src/*.c) \
	) \
)
LINKS = pam pam_misc z
OUT_LIBRARY = libgkid.so

all: $(OUT_LIBRARY)

test: $(SOURCES)
	$(CC) -c test/tester.c -I $(INCLUDE_DIR) -o $(OBJS_DIR)tester.o
	$(CC) $(SOURCES:%=$(OBJS_DIR)%.o) $(OBJS_DIR)tester.o -o test/tester $(LINKS:%=-l%)
	test/tester

$(OUT_LIBRARY): $(SOURCES)
	$(CC) $(SOURCES:%=$(OBJS_DIR)%.o) $(SHARED) $(FPIC) -o $(BIN_DIR)$(OUT_LIBRARY) $(LINKS:%=-l%)

$(SOURCES):
	$(CC) $(LOG_FLAG) $(FPIC) -c $(SOURCE_DIR)$@.c -I $(INCLUDE_DIR) -o $(OBJS_DIR)$@.o

clean:
	$(RM) $(BIN_DIR) $(OBJS_DIR)
