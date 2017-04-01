
# find the OS
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

# Compile flags for linux / OSX
ifeq ($(uname_S),Linux)
        SHOBJ_CFLAGS ?=  -fno-common -g -ggdb
        SHOBJ_LDFLAGS ?= -shared
else
        SHOBJ_CFLAGS ?= -dynamic -fno-common -g -ggdb
        SHOBJ_LDFLAGS ?= -bundle -undefined dynamic_lookup
endif

CFLAGS = -fPIC -std=gnu99 -c -o
LDFLAGS = -shared -Bsymbolic -lc
CC=gcc
.SUFFIXES: .c .so .o

REDIS_MODULES = extzsets

all: $(REDIS_MODULES)

$(REDIS_MODULES): %: %.o
	$(LD) -o $@.so $< $(SHOBJ_LDFLAGS)

clean:
	rm -rf *.so *.o
