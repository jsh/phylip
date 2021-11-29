# Rules for linking

$(DRAW_PROGS) : % : %.o
	$(DC) $(DLIBS) $^ $(LDADD) -o $@

$(OTHER_PROGS) : % : %.o
	$(CC) $^ $(LDADD) -o $@

