#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX_DIR	= ../..

OUT		= sum_strnum \
		  pthread_hello \
		  mutex_default \
		  mutex_normal \
		  mutex_recursive \
		  mutex_errchk \
		  cond_var \
		  cond_var_mt \
		  mutex_pshared \
		  barrier \
		  spinlock \
		  sum_strnum_mutex \
		  sum_strnum_tls \
		  sum_strnum_tls_single \
		  sum_strnum_tls_gcc \
		  mutex_robust \
		  cleanup_mutex \
		  pi_num_integration \
		  non_omp_task_linkedlist \
		  non_omp_false_sharing \
		  pthread_pi_cputime
OUT_OPENMP	= omp_hello \
			  omp_loop1 \
			  omp_loop2 \
			  omp_loop3 \
			  omp_pi_num_integration \
			  omp_loop_sched \
			  omp_loop_sched_icv \
			  omp_loop_ordered \
			  omp_sections \
			  omp_single \
			  omp_task_linkedlist \
			  omp_lock \
			  omp_master \
			  omp_nested_loop1 \
			  omp_nested_loop2 \
			  omp_nested_loop3 \
			  omp_sum_strnum_tls \
			  omp_sum_strnum_tls2 \
			  omp_tls_copy \
			  omp_false_sharing \
			  omp_avoid_false_sharing1 \
			  omp_avoid_false_sharing2 \
			  omp_pi_cputime
			

#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------

CC			= gcc
CFLAGS		= -Wall -g -std=c99

CPPFLAGS	= -I$(PREFIX_DIR)/include
FPICFLAGS	= -fPIC 
LDSOFLAGS	= -G -dy

LOADLIBES	= -L$(PREFIX_DIR)/lib
LDLIBS		= -lpthread -lrt 

MAKE		= make

#----------------------------------------------------------------------------
# user defined macros 
#----------------------------------------------------------------------------
SRCS        = $(wildcard *.c)
OBJS        = $(patsubst %.c,%.o,$(SRCS))
DEPENDENCY  = dep.mk

OBJ_OUT_OPENMP	= $(addsuffix .o, $(OUT_OPENMP))

#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------
.PHONY: all clean dep

all: $(OUT) $(OUT_OPENMP)

#----------------------------------------------------------------------------
# Suffix rules 
#----------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .c .o
.PRECIOUS: .o

#----------------------------------------------------------------------------
# Static pattern rule
#----------------------------------------------------------------------------

$(OUT_OPENMP): %: %.c
	$(CC) -fopenmp $(CFLAGS) $(CPPFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@


#----------------------------------------------------------------------------
# Build binaries
#----------------------------------------------------------------------------

mutex_default.o : mutex_type.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

mutex_normal.o : mutex_type.c
	$(CC) -c -DNORMAL_MUTEX $(CFLAGS) $(CPPFLAGS) $< -o $@

mutex_recursive.o : mutex_type.c
	$(CC) -c -DRECURSIVE_MUTEX $(CFLAGS) $(CPPFLAGS) $< -o $@

mutex_errchk.o : mutex_type.c
	$(CC) -c -DERRORCHECK_MUTEX $(CFLAGS) $(CPPFLAGS) $< -o $@

mutexattr_default : mutexattr.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@ 

mutexattr_errchk : mutexattr.c
	$(CC) -DUSE_MUTEXATTR $(CFLAGS) $(CPPFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@ 

cond_var_mt : cond_var.c
	$(CC) -DMULTI_THREAD $(CFLAGS) $(CPPFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@ 

non_omp_% : omp_%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@ 

#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------
clean:
	-rm -f *.o core core.* $(DEPENDENCY) $(OUT) $(OUT_OPENMP)

-include $(DEPENDENCY)
