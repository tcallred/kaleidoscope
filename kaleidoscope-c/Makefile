CC := clang
SRCDIR := src
BUILDDIR := build
TARGET := $(BUILDDIR)/kaleidoscopec

SRCEXT := c
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS := -Wall -Wextra -g

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILDDIR)
	$(CC) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) -r $(BUILDDIR)

.PHONY: clean
