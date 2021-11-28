
# Dynamic libraries (.so)

libclique.so:   clique.o disc.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libclique.so $(SHARED) -fPIC clique.c disc.c phylip.c

libcodml.so:    codml.o ml.o seq.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libcodml.o $(SHARED) -fPIC codml.c ml.c seq.c phylip.c Slist.c

libconsense.so:	consense.o cons.o phylip.o Slist.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libconsense.so $(SHARED) -fPIC consense.c cons.c phylip.c Slist.c

libcontml.so:   contml.o cont.o ml.o phylip.o phylip.h ml.h Slist.h $(LIBS)
	$(CC) $(CFLAGS) -o libcontml.so $(SHARED) -fPIC contml.c cont.c ml.c phylip.c Slist.c

libdnacomp.so: dnacomp.o seq.o parsimony.o dnaparsimony.o phylip.o $(LIBS)
	$(CC) $(CFLAGS) -o libdnacomp.so $(SHARED) -fPIC dnacomp.c seq.c dnaparsimony.c parsimony.c phylip.c Slist.c

libdnadist.so:	dnadist.o pmatrix.o seq.o phylip.o phylip.h seq.h  $(LIBS)
	$(CC) $(CFLAGS) -o libdnadist.so $(SHARED) -fPIC dnadist.c seq.c phylip.c pmatrix.c matrixd.c Slist.c

libdnainvar.so: dnainvar.o seq.o phylip.o $(LIBS)
	$(CC) $(CFLAGS) -o libdnainvar.so $(SHARED) -fPIC dnainvar.c seq.c phylip.c Slist.c

libdnaml.so:	dnaml.o seq.o ml.o $(LIBS)
	$(CC) $(CFLAGS) -o libdnaml.so $(SHARED) -fPIC dnaml.c seq.c ml.c phylip.c Slist.c -shared -fPIC

libdnapars.so: dnapars.o seq.o parsimony.o dnaparsimony.o phylip.o $(LIBS)
	$(CC) $(CFLAGS) -o libdnapars.so $(SHARED) -fPIC dnapars.c seq.c dnaparsimony.c parsimony.c phylip.c Slist.c

libdnapenny.so: dnapenny.o seq.o phylip.o $(LIBS)
	$(CC) $(CFLAGS) -o libdnapenny.so $(SHARED) -fPIC dnapenny.c seq.c phylip.c Slist.c

libdollop.so:    dollop.o disc.o dollo.o wagner.o dollo.h phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libdollop.so $(SHARED) -fPIC dollop.c dollo.c disc.c wagner.c phylip.c Slist.c

libdolpenny.so:    dolpenny.o disc.o wagner.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libdolpenny.so $(SHARED) -fPIC dolpenny.c disc.c wagner.c phylip.c Slist.c

libdrawgram.so:   drawgram.o draw.o draw2.o drawinit.o $(LIBS)
	$(CC) $(CFLAGS) -o libdrawgram.so $(SHARED) -fPIC drawgram.c draw.c draw2.c drawinit.c Slist.c phylip.c

libdrawtree.so:   drawtree.o draw.o draw2.o drawinit.o $(LIBS)
	$(CC) $(CFLAGS) -o libdrawtree.so $(SHARED) -fPIC drawtree.c draw.c draw2.c drawinit.c Slist.c phylip.c

libfactor.so:   factor.o phylip.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libfactor.so $(SHARED) -fPIC factor.c phylip.c

libfitch.so:	fitch.o phylip.o dist.o ml.o $(LIBS)
	$(CC) $(CFLAGS) -o libfitch.so $(SHARED) -fPIC fitch.c phylip.c ml.c dist.c Slist.c

libgendist.so:  gendist.o phylip.o pmatrix.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libgendist.so $(SHARED) -fPIC gendist.c pmatrix.c phylip.c Slist.c

libmix.so:    mix.o disc.o wagner.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libmix.so $(SHARED) -fPIC mix.c disc.c wagner.c phylip.c Slist.c

libneighbor.so: neighbor.o dist.o phylip.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libneighbor.so $(SHARED) -fPIC neighbor.c dist.c phylip.c Slist.c

libpars.so: pars.o parsimony.o discreteparsimony.o phylip.o $(LIBS)
	$(CC) $(CFLAGS) -o libpars.so $(SHARED) -fPIC pars.c discreteparsimony.c parsimony.c phylip.c Slist.c

libpenny.so:    penny.o disc.o wagner.o phylip.h $(LIBS)
	$(CC) $(CFLAGS) -o libpenny.so $(SHARED) -fPIC penny.c disc.c wagner.c phylip.c Slist.c

libproml.so:	proml.o seq.o ml.c prom_common.c $(LIBS)
	$(CC) $(CFLAGS) -o libproml.so $(SHARED) -fPIC proml.c prom_common.c seq.c ml.c phylip.c Slist.c -shared -fPIC

libprotdist.so:  protdist.o pmatrix.o seq.o $(LIBS)
	$(CC) $(CFLAGS) -o libprotdist.so $(SHARED) -fPIC protdist.c seq.c pmatrix.c phylip.c Slist.c

libprotpars.so: protpars.o seq.o parsimony.o dnaparsimony.o phylip.o $(LIBS)
	$(CC) $(CFLAGS) -o libprotpars.so $(SHARED) -fPIC protpars.c seq.c dnaparsimony.c parsimony.c phylip.c Slist.c

librestdist.so:  restdist.o rest_common.o seq.o pmatrix.o $(LIBS)
	$(CC) $(CFLAGS) -o librestdist.so $(SHARED) -fPIC restdist.c rest_common.c seq.c pmatrix.c phylip.c Slist.c

libseqboot.so:  seqboot.o seq.o $(LIBS)
	$(CC) $(CFLAGS) -o libseqboot.so $(SHARED) -fPIC seqboot.c seq.c phylip.c Slist.c

libtreedist.so: treedist.o cons.o $(LIBS)
	$(CC) $(CFLAGS) -o libtreedist.so $(SHARED) -fPIC treedist.c cons.c phylip.c Slist.c

