# The list of programs.

# Draw programs are singled out because they're compiled and linked in special ways,
# and are the most likely to cause compilation or linking problems.

DRAW_PROGS := drawgram drawtree
PROGS := clique codml consense contml contrast dnacomp dnadist dnainvar dnaml \
        dnamlk dnamove dnapars dnapenny dolmove dollop dolpenny factor fitch \
        gendist kitsch mix move neighbor pars penny proml promlk protdist protpars \
        restdist restml retree seqboot threshml treedist $(DRAW_PROGS)

# These don't quite build yet
BUILD_FAILURES := codml contml contrast dnamlk dnamove kitsch neighbor pars proml promlk protpars restml retree
PROGS := $(filter-out $(BUILD_FAILURES),$(PROGS))
OTHER_PROGS := $(filter-out $(DRAW_PROGS),$(PROGS))
