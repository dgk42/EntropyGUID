UNAME := $(shell uname)

RM = rm -f

CC = gcc
CPPFLAGS = -DTEST1
CCFLAGS = -Wall -Wextra -Os
ifeq ($(UNAME), Linux)
  LDFLAGS = -lcrypt
endif
ifeq ($(UNAME), Darwin)
  LDFLAGS =
endif

MCS = dmcs
MCSFLAGS =

JAVAC = javac
JAVACFLAGS =

BINS = entropy_guid EntropyGUID.class EntropyGUID.exe


.PHONY = all clean distclean rebuild


all: $(BINS)

clean:
	$(RM) $(BINS)

distclean: clean

rebuild: clean all


entropy_guid: entropy_guid.c
	$(CC) $(CPPFLAGS) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

EntropyGUID.class: EntropyGUID.java
	$(JAVAC) $(JAVACFLAGS) $^

EntropyGUID.exe: EntropyGUID.cs
	$(MCS) $(MCSFLAGS) $^
