APPNAME = example

DEPDIR  := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CC	= gcc
CFLAGS	= -Wall -Wextra -Wdeclaration-after-statement -Wvla -std=gnu99 -g -O2 \
	  -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -fstack-protector \
	  -fPIE -fexceptions -fvisibility=hidden -pipe
LDFLAGS = -Wl,-z,now,-z,defs,-z,relro,--as-needed -pie
LIBS =
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

COMPILER := $(shell $(CC) --version 2> /dev/null | grep -oE 'GCC|clang')

ifeq "$(COMPILER)" "GCC"
        GCC_MAJOR  := $(shell gcc -dumpfullversion -dumpversion | cut -d . -f 1)
        GCC_MINOR  := $(shell gcc -dumpfullversion -dumpversion | cut -d . -f 2)
        GCC_SUB    := $(shell gcc -dumpfullversion -dumpversion | cut -d . -f 3)
        GCC_VER_OK := $(shell test $(GCC_MAJOR) -ge 5 -a $(GCC_MINOR) -ge 1 \
                                   -a $(GCC_SUB) -ge 1 && echo 1)

        ifneq "$(GCC_VER_OK)" "1"
                # For GCC < 5.1.1
                CFLAGS += -Wno-missing-field-initializers
        endif
endif

sources = $(wildcard *.c)
objects = $(sources:.c=.o)

ifeq ($(ASAN),1)
        override ASAN = -fsanitize=address
endif

v = @
ifeq ($V,1)
	v =
endif

.PHONY: all
all: $(APPNAME)

$(APPNAME): $(objects)
	@echo -e "  LNK\t$@"
	$(v)$(CC) $(LDFLAGS) $(ASAN) -o $@ $(objects) $(LIBS)

%.o: %.c
%.o: %.c $(DEPDIR)/%.d
	@echo -e "  CC\t$@"
	$(v)$(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(sources))))

.PHONY: clean
clean:
	$(v)rm -f $(objects) $(APPNAME)
	$(v)rm -f $(DEPDIR)/*
	$(v)rmdir $(DEPDIR)
