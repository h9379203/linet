#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX  = ../..
OBJ_DIR = .obj/
LIB_DIR	= $(PREFIX)/lib

OUT		= mmap_ex1 \
			mmap_ran

#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------
CC			= gcc
CFLAGS 		= -Wall -g
CPPFLAGS	= -I$(PREFIX)/include

CPICFLAGS	= -fPIC 
LDSOFLAGS	= -G -dy
ARFLAGS		= ruv

LOADLIBES	= -L$(PREFIX)/lib
LDLIBS		= -lrt -lpthread

#----------------------------------------------------------------------------
# user defined macros
#----------------------------------------------------------------------------
SRCS        = $(wildcard *.c)
OBJS        = $(patsubst %.c,%.o,$(SRCS))
DEPENDENCY  = dep.mk

#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------

all: $(OUT)
	$(MAKE) -C xsi_sysv
	$(MAKE) -C posix

#----------------------------------------------------------------------------
# Suffix rules
#----------------------------------------------------------------------------

.SUFFIXES: 
.PRECIOUS: .o

%: %.o
	$(CC) $< -o $@ $(LOADLIBES) $(LDLIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

#----------------------------------------------------------------------------
# Build binaries
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------
clean:
	rm -f *.o core core.* *.dat $(DEPENDENCY) $(OUT)
	$(MAKE) clean -C xsi_sysv
	$(MAKE) clean -C posix

-include $(DEPENDENCY)