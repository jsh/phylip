UNAME := $(shell uname)
SYSTEM_MAKEFILE := $(shell uname | tr [:upper:] [:lower:]).mk
include $(SYSTEM_MAKEFILE) # darwin.mk or linux.mk
