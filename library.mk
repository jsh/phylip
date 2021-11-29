# how to make our library
libphylip.a: Slist.o phylip.o
	ar -rcs $@ $^
