#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX_DIR	= ../..

OUT		= cond_var_rts \
		  io_rts \
		  dir_notify \
		  clock_getres \
		  rt_timer \
		  cputime_process \
		  aio_basic \
		  aio_listio \
		  aio_listio_async \
		  aio_listio_sig \
		  sched

#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------

CC			= gcc
CFLAGS		= -Wall -g

CPPFLAGS	= -I$(PREFIX_DIR)/include
FPICFLAGS	= -fPIC 
LDSOFLAGS	= -G -dy

LOADLIBES	= -L$(PREFIX_DIR)/lib
LDLIBS		= -lrt
#----------------------------------------------------------------------------
# user defined macros
#----------------------------------------------------------------------------
SRCS		= $(wildcard *.c)
OBJS		= $(wildcard *.o)
DEPENDENCY  = dep.mk

#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------
.PHONY: all clean dep

all: dep $(OUT)

#----------------------------------------------------------------------------
# Suffix rules
#----------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .c .o .h
.PRECIOUS: .o

#----------------------------------------------------------------------------
# Static pattern rule
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Build binaries
#----------------------------------------------------------------------------
cond_var_rts: LDLIBS += -lpthread

sig_nomask : sig_basic.c
	$(CC) -DWITH_SA_NOMASK $(CFLAGS) $^ -o $@

aio_listio_async: aio_listio.c
	$(CC) -DASYNCHRONIZED_IO $(CPPFLAGS) $(CFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@

#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------
clean:
	-rm -f core core.* *.o $(DEPENDENCY) $(OUT)

-include $(DEPENDENCY)
