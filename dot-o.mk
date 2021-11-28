SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)
COMPILE_FAILURES := codml0.o codml.o contml.o contrast.o debug.o discreteparsimony.o dnamlk.o dnamove.o
COMPILE_FAILURES += dumptree.o interface.o kitsch.o macface.o neighbor.o newmove.o pars.o pool.o
COMPILE_FAILURES += prom_common.o proml.o promlk.o protpars.o restml.o restore.o retree.o usertree.o
OBJS := $(filter-out $(COMPILE_FAILURES),$(OBJS))

