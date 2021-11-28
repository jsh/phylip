# The list of programs.
# Draw programs are listed last since they are the most likely to cause
# compilation or linking problems.
#
PROGS = clique codml consense contml contrast dnacomp dnadist dnainvar dnaml \
        dnamlk dnamove dnapars dnapenny dolmove dollop dolpenny factor fitch \
        gendist kitsch mix move neighbor pars penny proml promlk protdist protpars \
        restdist restml retree seqboot threshml treedist drawgram drawtree

DYLIBS := libclique.so libconsense.so libcontml.so libdnacomp.so libdnadist.so libdnainvar.so
DYLIBS += lidnaml.so libdnapars.so libdnapenny.so libdollop.so libdolpenny.so libdrawgram.so
DYLIBS += libdrawtree.so libfactor.so libgendist.so libmix.so libneighbor.so libpars.so libpenny.so
DYLIBS += libprotpars.so libproml.so libprotdist.so librestdist.so libseqboot.so libtreedist.so

SHARED := -shared

# These for now, until we get this stuff debugged.
BUILD_FAILURES := codml contml contrast dnamlk dnamove kitsch neighbor pars proml promlk protpars restml retree
PROGS := $(filter-out $(BUILD_FAILURES),$(PROGS))
DYLIBS :=

ifeq ($(UNAME),Darwin)
  SHARED :=
else ifeq ($(UNAME),Linux)
  DFLAGS := -DX $(CFLAGS)
  DLIBS :=
else
  $(error Only know how to build for OS/X and Linux, not $(UNAME))
endif


