#
# Link the individual programs.
#
#progs: all
#	for i in $(RUNS); do yes Q | ./$$i &> /dev/null || echo $$i fails; done

clique:       clique.o disc.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

codml:        codml.o ml.o seq.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

consense:     consense.o cons.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

contml:       contml.o cont.o ml.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

contrast:     contrast.o cont.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dnacomp:      dnacomp.o seq.o parsimony.o dnaparsimony.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dnadist:      dnadist.o pmatrix.o matrixd.o seq.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dnainvar:     dnainvar.o seq.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dnaml:        dnaml.o seq.o ml.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dnamlk:       dnamlk.o seq.o ml.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dnamove:      dnamove.o seq.o moves.o parsimony.o dnaparsimony.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dnapars:      dnapars.o seq.o parsimony.o dnaparsimony.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dnapenny:     dnapenny.o seq.o parsimony.o dnaparsimony.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dolmove:      dolmove.o disc.o moves.o dollo.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dollop:       dollop.o disc.o dollo.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

dolpenny:     dolpenny.o disc.o dollo.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

drawgram:     drawgram.o draw.o draw2.o drawinit.o $(LIBS)
	$(DC) $(DLIBS) $^ $(LDADD) -o $@

drawtree:     drawtree.o draw.o draw2.o drawinit.o $(LIBS)
	$(DC) $(DLIBS) $^ $(LDADD) -o $@

factor:       factor.o disc.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

fitch:        fitch.o dist.o ml.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

gendist:      gendist.o pmatrix.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

kitsch:       kitsch.o dist.o ml.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

mix:          mix.o disc.o wagner.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

move:         move.o disc.o moves.o wagner.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

neighbor:     neighbor.o dist.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

pars:         pars.o discreteparsimony.o parsimony.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

penny:        penny.o disc.o wagner.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

proml:        proml.o prom_common.o seq.o ml.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

promlk:       promlk.o prom_common.o seq.o ml.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

protdist:     protdist.o seq.o pmatrix.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

protpars:     protpars.o seq.o parsimony.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

restdist:     restdist.o rest_common.o seq.o pmatrix.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

restml:       restml.o rest_common.o seq.o ml.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

retree:       retree.o moves.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

seqboot:      seqboot.o seq.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

threshml:     threshml.o cont.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

treedist:     treedist.o cons.o $(LIBS)
	$(CC) $^ $(LDADD) -o $@

