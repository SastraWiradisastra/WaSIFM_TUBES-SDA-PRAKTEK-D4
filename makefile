# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g -I. -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lncurses

# Source files
SRCS = main.c UI.c fsimplement.c urimplement.c structure.c helper.c
OBJS = $(SRCS:.c=.o)

# Program name
TARGET = wasifm

# Installation directories
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	@echo "Installed $(TARGET) to $(BINDIR)"

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	@echo "Removed $(TARGET) from $(BINDIR)"