# Directories
BINDIR   = build/bin
CCODEDIR = build/ccode
OBJDIR   = build/obj
SRCDIR   = src
BASEDIR  = examples

LIBS = "glib-2.0 gobject-2.0 gee-0.8"
LIBDIR = "/home/emre/Workport/dev/mingw-w64/mingw32/lib/pkgconfig"
PKGCONFIG := $(shell PKG_CONFIG_LIBDIR=$(LIBDIR) pkg-config --cflags --libs $(LIBS))
CC = i686-w64-mingw32-gcc
CFLAGS := -mwindows -static-libgcc -I$(SRCDIR)

# Build targets
SAMPLES = encryptor \
          helloworld
          
EXECUTABLES := $(addprefix $(BINDIR)/,$(addsuffix .exe,$(SAMPLES)))
DEPS := $(notdir $(wildcard src/*.c))

.PHONY: info $(SAMPLES)

info:
	@echo SAMPLES = $(SAMPLES)
	@echo EXECUTABLES = $(EXECUTABLES)
	@echo DEPS = $(DEPS)
	@echo OBJS = $(OBJDIR)/$(DEPS:.c=.o)

$(SAMPLES): %: $(BINDIR)/%.exe
	@echo SAMPLE: $^
	
$(EXECUTABLES): $(BINDIR)/%.exe: $(OBJDIR)/%.o $(addprefix $(OBJDIR)/,$(DEPS:.c=.o)) | $(BINDIR)
	@echo EXECUTABLE: $@ from file\(s\): $^
	@$(CC) $^ $(CFLAGS) $(PKGCONFIG) -o $@

# Generate Object files specific to a example
$(patsubst %,$(OBJDIR)/%.o,$(SAMPLES)): $(OBJDIR)/%.o: $(CCODEDIR)/%.c | $(OBJDIR)
	@echo OBJECT-FILE \(EXAMPLE\): $@ from file: $<
	@$(CC) -c $< $(CFLAGS) $(PKGCONFIG) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h | $(OBJDIR)
	@echo OBJECT-FILE: $@ from file: $<
	@$(CC) -c $< $(CFLAGS) $(PKGCONFIG) -o $@

$(CCODEDIR)/%.c: $(BASEDIR)/%.vala | $(CCODEDIR)
	@echo CCODE-FILE: $@ from file: $<
	@valac -C $< vapi/libwin32.vapi --pkg gee-0.8 -b $(BASEDIR) -d $(CCODEDIR)

$(CCODEDIR):
	mkdir -p $(CCODEDIR)
	
$(BINDIR):
	mkdir -p $(BINDIR)
	
$(OBJDIR):
	mkdir -p $(OBJDIR)

