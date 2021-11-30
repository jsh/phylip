# Building PHYLIP

The PHYLIP 4.0 build assumes you are using GNU make.

For the things you can build from this distribution, type `make`
The default build should just work if you type `make all`.

`darwin.mk` holds settings for Darwin (MacOS) builds,
and `linux.mk` holds settings for Linux builds.
To to build for some other kind of system -- Microsoft Windows, for example --
you'll want to create a `_system-name_.mk` file,
where _system-name_ is the name returned by `uname`.

If you just want to tweak your build, say to pass a different set of flags to the C compiler,
you may want to modify `vars.mk` .

# Distributing the PHYLIP you build.

The compressed tar archive phylip.tar.Z when uncompressed and extracted
puts the source code into a directory ./src, and also makes two other
directories ./exe and ./doc for the final executables and the documentation
files.

If you want something different from that structure, change EXEDIR in `vars.mk`
