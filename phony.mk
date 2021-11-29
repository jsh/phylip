#
# General commands.
#

# These targets do not produce a file of their namesake, so tell make not
# to bother looking:
.PHONY .SILENT: help all introduce put clean install distclean

#
#  The first target it executed if you just type "make".  It tells you how to use the Makefile.
#
help:
	echo
	echo " To use the PHYLIP v4.0 Makefile, type:"
	echo
	echo "     make install           to compile the whole package and install"
	echo "                            the executables in $(EXEDIR), and then"
	echo "                            remove the object files to save space."
	echo
	echo "     make all               to compile the whole package but not install it"
	echo "                            or remove the object files."
	echo
	echo "     make put               to move the executables into $(EXEDIR)."
	echo
	echo "     make clean             to remove all object files and executables from the"
	echo "                            current directory."
	echo
	echo "     make dnaml             to compile and link one program, (in this example,"
	echo "                            Dnaml) and leave the executable and object files"
	echo "                            in the current directory (where the source code is)."
	echo
	echo "                            You will have to move the executable into the"
	echo "                            executables directory (e.g. "mv dnaml $(EXEDIR)")"
	echo "                            Note that the program name should be lower case."

all:    introduce $(PROGS) $(DYLIBS)
	echo
	echo "Successfully built programs:"
	printf "\t$(PROGS)\n"
	echo
	echo "Did not try to build programs:"
	printf "\t$(BUILD_FAILURES)\n"

all-objects: $(OBJS)
	echo
	echo "Successfully built object files:"
	printf "\t$(OBJS)\n"
	echo
	echo "Did not try to build object files:"
	printf "\t$(COMPILE_FAILURES)\n"

introduce:
	echo
	echo "Building PHYLIP version 4.0."

install: pre-clean all put clean
	echo
	echo "Done."

put:
	echo
	echo "Installing PHYLIP v4.0 binaries in $(EXEDIR)."
	mkdir -p $(EXEDIR)
	mv $(PROGS) $(EXEDIR)
	echo
	echo "Installing Hershey font files in $(EXEDIR)."
	cp font* $(EXEDIR)
	echo
	echo "Finished installation."

pre-clean:
	echo
	echo "Removing preexisting executables."
	rm -rf $(EXEDIR)/*
	echo
	echo "Ready for new executables."

clean:
	echo
	echo "Removing object files and libraries to save space."
	rm -f *.[oa]
	echo
	echo "Finished removing object files.  Now will remove"
	echo "executable files from the current directory, but not from the"
	echo "executables directory.  (If some are not here, the makefile"
	echo "may terminate with an error message but this is not a problem.)"
	echo
	echo "Removing executables from this directory."
	rm -f $(PROGS)
	echo
	echo "Finished cleanup."

distclean:
	git clean -dfx
	git reset --hard -q
