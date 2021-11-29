# The list of programs.
# Draw programs are listed last since they are the most likely to cause
# compilation or linking problems.

DRAW_PROGS := drawgram drawtree
PROGS := clique codml consense contml contrast dnacomp dnadist dnainvar dnaml \
        dnamlk dnamove dnapars dnapenny dolmove dollop dolpenny factor fitch \
        gendist kitsch mix move neighbor pars penny proml promlk protdist protpars \
        restdist restml retree seqboot threshml treedist $(DRAW_PROGS)

# These don't quite work yet
BUILD_FAILURES := codml contml contrast dnamlk dnamove kitsch neighbor pars proml promlk protpars restml retree
PROGS := $(filter-out $(BUILD_FAILURES),$(PROGS))

# Partition into drawing programs and "other"
OTHER_PROGS := $(filter-out $(DRAW_PROGS),$(PROGS))

ifeq ($(UNAME),Darwin)
  SHARED :=
else ifeq ($(UNAME),Linux)
  DFLAGS := -DX $(CFLAGS)
  DLIBS :=
else
  $(error Only confirmed builds on OS/X and Linux, not $(UNAME))
endif
