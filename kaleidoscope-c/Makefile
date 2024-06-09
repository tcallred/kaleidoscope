CC := clang
SRCDIR := src
BUILDDIR := build
TARGET := $(BUILDDIR)/kaleidoscopec

CFLAGS := -Wall -Wextra -g

all:
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SRCDIR)/main.c -o $(TARGET)

clean:
	@$(RM) -r $(BUILDDIR)

.PHONY: clean
