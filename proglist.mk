# The list of programs.
# Draw programs are listed last since they are the most likely to cause
# compilation or linking problems.
#
PROGS = clique codml consense contml contrast dnacomp dnadist dnainvar dnaml \
        dnamlk dnamove dnapars dnapenny dolmove dollop dolpenny factor fitch \
        gendist kitsch mix move neighbor pars penny proml promlk protdist protpars \
        restdist restml retree seqboot threshml treedist drawgram drawtree

SHARED := -shared

# These for now, until we get this stuff debugged.
BUILD_FAILURES := codml contml contrast dnamlk dnamove kitsch neighbor pars proml promlk protpars restml retree
PROGS := $(filter-out $(BUILD_FAILURES),$(PROGS))

ifeq ($(UNAME),Darwin)
  SHARED :=
else ifeq ($(UNAME),Linux)
  DFLAGS := -DX $(CFLAGS)
  DLIBS :=
else
  $(error Only confirmed builds on OS/X and Linux, not $(UNAME))
endif


