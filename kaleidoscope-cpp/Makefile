CC := clang++
SRCDIR := src
BUILDDIR := build
TARGET := $(BUILDDIR)/kaleidoscopec

CFLAGS := -std=c++20 -Wall -Wextra -g

all:
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SRCDIR)/main.cpp -o $(TARGET)

clean:
	@$(RM) -r $(BUILDDIR)

.PHONY: clean
