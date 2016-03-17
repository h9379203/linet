#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX_DIR	= ../../..

OUT		= chk_endian \
			inet_tcp_serv1 \
			inet_udp_rcver \
			inet_udp_bcast \
			unix_udp \
			unix_tcp_serv \
			getaddrinfo \
			inet6_tcp_serv1 \
			if_nameindex \
			inet_tcp_serv1_OOB \
			tcp_cli \
			tcp_cli_nonblock \
			tcp_cli_OOB


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
# Suffix rules
#----------------------------------------------------------------------------

.SUFFIXES: 
.SUFFIXES: .o .c
.PRECIOUS: .o

#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------
.PHONY: all clean dep

all: $(OUT)

#----------------------------------------------------------------------------
# Build binaries
#----------------------------------------------------------------------------
tcp_cli_nonblock: tcp_cli.c
	$(CC) -DENABLE_O_NONBLOCK $(CFLAGS) $(CPPFLAGS) $< -o $@

tcp_cli_OOB: tcp_cli.c
	$(CC) -DENABLE_MSG_OOB $(CFLAGS) $(CPPFLAGS) $< -o $@

#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------
clean:
	rm -f *.o core core.* $(DEPENDENCY) $(OUT)

-include $(DEPENDENCY)
