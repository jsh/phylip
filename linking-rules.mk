# Rules for linking


$(OTHER_PROGS) : % : %.o | libphylip.a
	$(LINK.c) $^ -L. $(LDLIBS) -o $@
	
$(DRAW_PROGS) : % : %.o | libphylip.a
	$(LINK.draw) $^ -L. $(LDLIBS) -o $@
