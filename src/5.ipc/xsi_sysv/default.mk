#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX  = ../../..
LIB_DIR = $(PREFIX)/lib

OUT		= libsysvipc.a \
			sysv_shm \
			sysv_sem sysv_sem_waitforzero \
			sysv_msg  sysv_msg_regex sysv_msg_info \
			sysv_sem_counting sysv_sem_graph

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
LDLIBS		= -lsysvipc

#----------------------------------------------------------------------------
# user defined macros
#----------------------------------------------------------------------------
SRCS        = $(wildcard *.c)
OBJS        = $(wildcard *.o)
DEPENDENCY  = dep.mk

#----------------------------------------------------------------------------
# Sources
#----------------------------------------------------------------------------
SYSVIPC_FILES = lib_sysv_sem lib_sysv_shm lib_sysv_msg

SYSVIPC_SRC      = $(addsuffix .c,       $(SYSVIPC_FILES))
SYSVIPC_OBJ      = $(addsuffix .o,       $(SYSVIPC_FILES))

#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------
all: $(OUT)

#----------------------------------------------------------------------------
# Suffix rules
#----------------------------------------------------------------------------

# remove all built-in suffix rule
.SUFFIXES:
.PRECIOUS: .o

%: %.o
	$(CC) $< $(LOADLIBES) $(LDLIBS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

#----------------------------------------------------------------------------
# Static pattern rule
#----------------------------------------------------------------------------

# Library Compilation (Position Independent Code)
$(SYSVIPC_OBJ): %.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(CPICFLAGS) $< -o $@

#----------------------------------------------------------------------------
# Build library
#----------------------------------------------------------------------------

libsysvipc.a : $(SYSVIPC_OBJ)
	@echo "++ Building SysV IPC Wrapper library... ($@)"
	$(AR) $(ARFLAGS) $@ $(SYSVIPC_OBJ)
	@echo "++ Moving to $(LIB_DIR)"
	@if [ ! -d $(LIB_DIR) ]; then \
		mkdir $(LIB_DIR); \
	fi
	@ranlib $@
	@mv $@ $(LIB_DIR)
	@rm $^
	@echo ""

libsysvipc.so : $(SYSVIPC_OBJ)
	@echo "++ Building SysV IPC Wrapper library... ($@)"
	$(CC) -shared -o $@ $(SYSVIPC_OBJ)
	@echo "++ Moving to $(LIB_DIR)"
	@mv $@ $(LIB_DIR)
	@echo ""

#----------------------------------------------------------------------------
# Build sample program
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------

clean:
	-rm -f *.o core core.* output*.txt $(DEPENDENCY) $(OUT)

-include $(DEPENDENCY)
