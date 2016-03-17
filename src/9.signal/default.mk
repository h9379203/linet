#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX_DIR	= ../..

OUT		= sig_basic \
		  sig_siginfo \
		  sig_nodefer \
		  sig_nocldstop \
		  sig_pgid \
		  sig_pending \
		  pthread_sigwait \
		  segv_sigaltstack

#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------

CC			= gcc
CFLAGS		= -Wall -g

#CPPFLAGS	= -D_POSIX_PTHREAD_SEMANTICS -I$(PREFIX_DIR)/include
CPPFLAGS	= -I$(PREFIX_DIR)/include
FPICFLAGS	= -fPIC 
LDSOFLAGS	= -G -dy

LOADLIBES	= -L$(PREFIX_DIR)/lib
LDLIBS		= -Wl,--export-dynamic -lpthread 


#----------------------------------------------------------------------------
# user defined macros
#----------------------------------------------------------------------------
SRCS        = $(wildcard *.c)
OBJS        = $(patsubst %.c,%.o,$(SRCS))
DEPENDENCY  = dep.mk

#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------
.PHONY: all clean dep

all: $(OUT)

#----------------------------------------------------------------------------
# Suffix rules
#----------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .c .o
.PRECIOUS: .o

#----------------------------------------------------------------------------
# Build binaries
#----------------------------------------------------------------------------

sig_nodefer : sig_basic.c
	$(CC) -DWITH_SA_NODEFER $(CFLAGS) $^ -o $@


#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------
clean:
	rm -f *.o core core.* $(DEPENDENCY) $(OUT)

-include $(DEPENDENCY)
