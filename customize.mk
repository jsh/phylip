# ----------------------------------------------------------------------------
# Starting here is the section where you may want to change things.
# ----------------------------------------------------------------------------
#
# These are the statements we have been talking about:
#
# One of the reasons for changing them would be to put the executables
# on a different file system.  The default configuration is to have within
# the overall PHYLIP directory three subdirectories:
#   "src" for source code, "exe" for the executables, and
#   "doc" for the documentation files.
#
# The following specifies the directory where the executables will be placed:
EXEDIR = ../exe
#
# The following fetches the name of the Unix or Linux shell
#
UNAME := $(shell uname)
#
# ----------------------------------------------------------------------------
#
# The following statements set these variables:
#
#    CFLAGS -- compiler directives needed to compile most programs
#    DFLAGS -- compiler directives needed to compile draw programs
#    LIBS   -- non-default system libraries needed to compile most programs
#    DLIBS  -- non-default system libraries needed to compile draw programs
#    CC     -- the name (and, optionally, location) of your C compiler
#    DC     -- the name (and, optionally, location) of your C compiler
#               for draw programs
#
# We've provided a set of possible values for each variable.
#
# The value used is the one without a "#" at the beginning of the line.
#
# To try an alternate value, make sure the one you want has no "#"
# as its first character and that all other possibilities have "#" for
# their first character.
#
# Advanced users may need to further edit one of the alternatives in
# order to correctly compile on their system.
#
# ----------------------------------------------------------------------------
#
# In the following statements (the ones that set CFLAGS, DFLAGS, LIBS
# and DLIBS, CC and DC) you should make sure each is set properly.
# Usually this will simply involve making sure that the proper statement
# has no "#" as its first character and that all other possibilities
# have "#" for their first character.
#
# ----------------------------------------------------------------------------
#
# This is the CFLAGS statement:
#
# If these statements say "-g"  and that causes trouble, remove the "-g".
# If the "-lX11" does not work (X compiling not working), you may want to
# remove that switch or fix it.
#
# Here are some possible CFLAGS statements:
#
# A minimal one:
#CFLAGS  = -g
#
# A basic one for debugging:
CFLAGS  = -g -Wall
#
# For some serious debugging using GNU GCC.  This raises debug warning
#  reporting to highest level possible.
#ifeq ($(MODE), GDB)
#   No optimization; intended for debugging with GDB.  Debug level
#    is "-ggdb3", maximal for GDB.  Originally was "-g".  Maximal debug
#    information preserved (at cost of "-Wuninitialized" warning).
#CFLAGS=-ggdb3 -std=c99 -Wall -Wmain -Wmissing-prototypes -Wreturn-type -Wstrict-prototypes -Wunused -Wextra -pedantic -Wredundant-decls -Waggregate-return -Wcast-align -Wcomment
#else
#   Minimal optimization; intended for debugging via compilation but not
#   using GDB. Debug level is "-g", good for debugging but not maximal for GDB.
#CFLAGS=-g -std=c99 -O -Wall -Wmain -Wmissing-prototypes -Wreturn-type -Wstrict-prototypes -Wunused -Wextra -pedantic -Wuninitialized -Wredundant-decls -Waggregate-return -Wcast-align -Wcomment
#  In case value is not defined (or bogus), give MODE a value to indicate
#   "NORMAL" compilation; used only for display.
#MODE=NORMAL
#endif
#
# An optimized one for gcc:
#CFLAGS  = -O3 -fomit-frame-pointer
#
# For some serious debugging using Gnu gcc:
#CFLAGS=-g -Wall -Wmain -Wmissing-prototypes -Wreturn-type -Wstrict-prototypes -Wunused -Werror -Wredundant-decls -Waggregate-return -Wcast-align -Wcomment
#
# For Digital Alpha systems with Compaq Tru64 Unix
# (however, be aware that this may cause floating-point problems in programs
#  like Dnaml owing to not using IEEE floating point standards):
#CFLAGS = -fast
#
# Optimized one for the IBM Visualage compiler for Linux or AIX on IBM pSeries
#CFLAGS = -O -qmaxmem=-1 -qarch=auto -qtune=auto
#
# ----------------------------------------------------------------------------
#
# This is the DFLAGS statement.  It specifies compiler behavior for the
# programs drawgram and drawtree.  It adds additional information to
# the $(CFLAGS) value so these programs can use X11 graphics.
#
# A minimal one -- drawing programs will run, but will not use X11 graphics.
# If you use this one, you must also use the minimal value for $(DLIBS).
#DFLAGS  = -DX $(CFLAGS)
#
# A basic one for debugging:
#DFLAGS  = -g -DX
#
# For Gnu C++ for runs:
#DFLAGS  = -I/usr/X11R6/include -O3 -DX -fomit-frame-pointer
#
# For Digital Alpha systems with Compaq Tru64 Unix:
#DFLAGS = -DX -fast
#
# For Linux with X Windows development packages installed
# or for MacOS X with X Windows installed:
DFLAGS = -DX $(CFLAGS) -I/usr/X11R6/include
#
# For IBM Visualage compiler on Linux or AIX for the IBM pSeries
#DFLAGS  = -DX $(CFLAGS)
#
# ----------------------------------------------------------------------------
#
# These are the libraries for the CC and DC compiles, respectively:
#
# Use this one to include debugging facility.
#LIBS    = Slist.o phylip.o debug.o
#
LIBS    = Slist.o phylip.o
LDADD   = -lm
#
# If the Xlib library for the X windowing system is somewhere
# unexpected, you may have to change the path /usr/X11R6/lib in this one.
#
# For gcc for Linux with X windows development packages installed
#  or for MacOS X with X windows installed
#  or for IBM pSeries with Linux:
DLIBS= -L/usr/X11R6/lib/ -lX11 -lXt
# was: DLIBS= -L/usr/X11R6/lib/ -lX11 -lXaw -lXt   Athena widgets available? needed?
#
# For IBM Visualage compiler for AIX:
#DLIBS= -L/usr/X11R6/lib/ -lX11 -lXaw -lXt -lXmu -lXext
#
# ----------------------------------------------------------------------------
#
# The next two assignments are the invocations of the compiler for the
# ordinary compiles and the tree-drawing programs, CC and DC:
#
# This one specifies the "cc" C compiler:
#CC        = cc $(CFLAGS)
#
#  To use GCC instead, if it is not the compiler that "cc" invokes:
CC         = gcc $(CFLAGS)
#
# For IBM compiler for Linux or AIX on IBM pSeries
#CC        = xlc $(CFLAGS)
#
# This one specifies the "cc" C compiler for the Draw programs:
#DC        = cc $(DFLAGS)
#
# To use GCC for the Draw programs instead, if it is not the compiler that "cc" invokes:
DC         = gcc $(DFLAGS)
#
# For IBM compiler for the Draw programs for Linux or AIX on IBM pSeries:
#DC        = xlc $(DFLAGS)
#
# ----------------------------------------------------------------------------
# After this point there should not be any reason to change anything.
# ----------------------------------------------------------------------------
