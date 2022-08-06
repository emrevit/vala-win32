# Directories
BINDIR   = build/bin
CCODEDIR = build/ccode
OBJDIR   = build/obj
SRCDIR   = src
RESDIR   = res
BASEDIR  = examples

# Update the LIBDIR variable to your library path
LIBDIR =
LIBS   = "glib-2.0 gobject-2.0 gee-0.8"
PKGCONFIG := $(shell PKG_CONFIG_LIBDIR="$(LIBDIR)/mingw32/lib/pkgconfig" pkg-config --cflags --libs $(LIBS))
CC = i686-w64-mingw32-gcc
CFLAGS := -mwindows -static-libgcc -I$(SRCDIR)

# Build targets
SAMPLES = encryptor
          
EXECUTABLES := $(addprefix $(BINDIR)/,$(addsuffix .exe,$(SAMPLES)))
DEPS := $(notdir $(wildcard $(SRCDIR)/*.c))

.PHONY: clean $(SAMPLES)

default: encryptor

$(SAMPLES): %: $(BINDIR)/%.exe
	@echo SAMPLE BUILT: $^
	
$(EXECUTABLES): $(BINDIR)/%.exe: $(OBJDIR)/%.o $(addprefix $(OBJDIR)/,$(DEPS:.c=.o)) | $(BINDIR)
	$(CC) $^ $(CFLAGS) $(PKGCONFIG) -o $@

$(patsubst %,$(OBJDIR)/%.o,$(SAMPLES)): $(OBJDIR)/%.o: $(CCODEDIR)/%.c $(SRCDIR)/vala-win32.h | $(OBJDIR)
	$(CC) -c $< $(CFLAGS) $(PKGCONFIG) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h | $(OBJDIR)
	$(CC) -c $< $(CFLAGS) $(PKGCONFIG) -o $@

$(CCODEDIR)/%.c: $(BASEDIR)/%.vala vapi/libwin32.vapi | $(CCODEDIR)
	valac -C $< vapi/libwin32.vapi --pkg gee-0.8 -b $(BASEDIR) -d $(CCODEDIR)

$(CCODEDIR):
	mkdir -p $(CCODEDIR)
	
$(BINDIR):
	mkdir -p $(BINDIR)
	
$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	$(RM) $(BINDIR)/*.exe $(OBJDIR)/*.o $(CCODEDIR)/*.c

