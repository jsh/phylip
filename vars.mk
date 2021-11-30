# PHYLIP uses the default compiler toolchain: gcc on Linux, clang on Darwin, GNU Make on both
# The command "make -p" shows the internal Make variables and their default settings
# The following variables are internal to Make, and use default values unless modified in the following section.
#    CFLAGS -- compiler directives for non-draw programs
#    LDLIBS -- libraries needed to link non-draw programs
#    CC     -- the name (and, optionally, location) of your C compiler
#    COMPILE.c -- default command to compile C programs
#    LINK.c -- default command to link C programs

## Modifications to standard make variables
# For COMPILE.c, see compiling-rules.mk
CFLAGS += -g -Wall
LDLIBS += -lm -lphylip

## These variables are particular to PHYLIP

#    EXEDIR -- directory where the executables will be placed
#    DFLAGS -- compiler directives for draw programs
#    DLIBS  -- additional libraries needed to compile draw programs
#               for draw programs
#    COMPILE.draw -- command to compile draw programs
#    LINK.draw -- command to link draw programs

EXEDIR := ../exe
DFLAGS := -DX $(CFLAGS) -I/usr/X11R6/include
DLIBS := -L/usr/X11R6/lib/ -lX11 -lXt
COMPILE.draw := $(COMPILE.c) $(DFLAGS)
LINK.draw := $(LINK.c) $(DFLAGS) $(DLIBS)
