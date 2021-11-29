#
# Link the individual programs.
#
#progs: all
#	for i in $(RUNS); do yes Q | ./$$i &> /dev/null || echo $$i fails; done

clique:       clique.o disc.o 
codml:        codml.o ml.o seq.o 
consense:     consense.o cons.o 
contml:       contml.o cont.o ml.o 
contrast:     contrast.o cont.o 
dnacomp:      dnacomp.o seq.o parsimony.o dnaparsimony.o 
dnadist:      dnadist.o pmatrix.o matrixd.o seq.o 
dnainvar:     dnainvar.o seq.o 
dnaml:        dnaml.o seq.o ml.o 
dnamlk:       dnamlk.o seq.o ml.o 
dnamove:      dnamove.o seq.o moves.o parsimony.o dnaparsimony.o 
dnapars:      dnapars.o seq.o parsimony.o dnaparsimony.o 
dnapenny:     dnapenny.o seq.o parsimony.o dnaparsimony.o 
dolmove:      dolmove.o disc.o moves.o dollo.o 
dollop:       dollop.o disc.o dollo.o 
dolpenny:     dolpenny.o disc.o dollo.o 
drawgram:     drawgram.o draw.o draw2.o drawinit.o 
drawtree:     drawtree.o draw.o draw2.o drawinit.o 
factor:       factor.o disc.o 
fitch:        fitch.o dist.o ml.o 
gendist:      gendist.o pmatrix.o 
kitsch:       kitsch.o dist.o ml.o 
mix:          mix.o disc.o wagner.o 
move:         move.o disc.o moves.o wagner.o 
neighbor:     neighbor.o dist.o 
pars:         pars.o discreteparsimony.o parsimony.o 
penny:        penny.o disc.o wagner.o 
proml:        proml.o prom_common.o seq.o ml.o 
promlk:       promlk.o prom_common.o seq.o ml.o 
protdist:     protdist.o seq.o pmatrix.o 
protpars:     protpars.o seq.o parsimony.o 
restdist:     restdist.o rest_common.o seq.o pmatrix.o 
restml:       restml.o rest_common.o seq.o ml.o 
retree:       retree.o moves.o 
seqboot:      seqboot.o seq.o 
threshml:     threshml.o cont.o 
treedist:     treedist.o cons.o 
