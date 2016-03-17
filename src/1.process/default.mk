#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX_DIR	= ../..

OUT		= fork_process \
			fork_omit_switch \
			execl \
			forkexec_parent forkexec_child \
			forkexec_parent_fdcloexec \
			pspawn1 pspawn2 pspawn3 \
			pspawn4 pspawn4_child \
			pspawn5 pspawn5_child_x pspawn5_child_y \

#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------
CC			= gcc
CFLAGS		= -Wall -g

CPPFLAGS	= -I$(PREFIX_DIR)/include
FPICFLAGS	= -fPIC 
LDSOFLAGS	= -G -dy

LOADLIBES	= -L$(PREFIX_DIR)/lib
LDLIBS		= 

MAKE		= make

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

#----------------------------------------------------------------------------
# Static pattern rule
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Build binaries
#----------------------------------------------------------------------------
fork_omit_switch : fork_process.c
	$(CC) $(CFLAGS) -DOMIT_SWITCH $^ -o $@

forkexec_parent_fdcloexec: CPPFLAGS+=-DAPPLY_FD_CLOEXEC
forkexec_parent_fdcloexec: forkexec_parent.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LOADLIBES) $(LDLIBS) -o $@ $<


#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------
clean: ; rm -f *.o core core.* *.log $(DEPENDENCY) $(OUT)

-include $(DEPENDENCY)
