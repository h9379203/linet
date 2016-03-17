#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX_DIR	= ../..

OUT		= io_select \
		  io_poll io_poll_OOB io_poll_nb \
		  io_poll_LT \
		  io_epoll io_epoll_OOB io_epoll_LT io_epoll_ET

#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------
CC			= gcc
CFLAGS		= -Wall -g

CPPFLAGS	= -I$(PREFIX_DIR)/include -DUSE_GETNAMEINFO
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

all: $(OUT)

#----------------------------------------------------------------------------
# Suffix rules
#----------------------------------------------------------------------------

.SUFFIXES: 
.SUFFIXES: .c .o

#----------------------------------------------------------------------------
# Static pattern rule
#----------------------------------------------------------------------------
OBJ_OUT_DERIVED1 = $(addsuffix .o, $(filter %_OOB, $(OUT)))
$(OBJ_OUT_DERIVED1): %_OOB.o: %.c
	$(CC) -c -DENABLE_MSG_OOB $(CFLAGS) $(CPPFLAGS) $< -o $@

#----------------------------------------------------------------------------
# Build binaries
#----------------------------------------------------------------------------

io_epoll_LT : io_epoll_trigger.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

io_epoll_ET : io_epoll_trigger.c
	$(CC) -DENABLE_EPOLLET $(CFLAGS) $(CPPFLAGS) $< -o $@

#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------
clean:
	rm -f *.o core core.* $(DEPENDENCY) $(OUT)

-include $(DEPENDENCY)
