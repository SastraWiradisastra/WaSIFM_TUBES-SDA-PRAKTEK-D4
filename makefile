# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g -O2 -I. -D_XOPEN_SOURCE_EXTENDED
LDFLAGS = -lncurses -ltinfo 

# Project files
SRCS = main.c UI.c fsimplement.c helper.c structure.c urimplement.c
OBJS = $(SRCS:.c=.o)
TARGET = wasifm

# Installation paths
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)/$(TARGET)
	@echo "Installed in $(BINDIR)"
	$(MAKE) clean

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	@echo "Removed from $(BINDIR)"
