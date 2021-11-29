## Rules for linking

$(OTHER_PROGS) : % : %.o | libphylip.a
	$(LINK.c) $^ -L. $(LDLIBS) -o $@
	
$(DRAW_PROGS) : % : %.o | libphylip.a
	$(LINK.draw) $^ -L. $(LDLIBS) -o $@

## Rules for libraries
%.a:
	ar -rcs $@ $^

## Rules for C-source dependency generation
# This is lifted from http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
