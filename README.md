# passgen

Just a password generator that makes [passwds.ninja](https://passwds.ninja/)-style passwords.

Heavily inspired by a generator I used before at work (in Python), just that this time in C99.

As such, it utilises 'grammars' of character classes, which can be efficiently be mixed and matched on the fly, and/or the grammars themselves being able to be easily customised at compile-time.

See `passgen --help` for details.

(This is my first C project, please be gentle... some of the decisions I made may look weird or seem like I'm going out of my way to do something - some of these are done intentionally as a way for me to familiarise myself more with the features of the language.)

## Install

passgen is officially packaged for Fedora Linux and Homebrew, as these are the platforms I use.

```
# Fedora (COPR)
$ sudo dnf copr enable nicholastay/nexpkg
$ sudo dnf install passgen

# Homebrew (custom tap)
$ brew tap nicholastay/tap
$ brew install passgen
```

As it is a fairly simple, portable C99 application, it should be easy enough to follow the steps below for the specific platform with just a standard C compiler.

## Compiling

```
# *nix / macOS / Windows-MinGW
$ make
# (optionally to install into prefix, default target is ~/.local/bin/)
$ make install

# Windows with MSVC, in MSVC Developer CMD
$ build_msvc.bat

# Cross-compile for Win32 via MinGW on Linux
$ make passgen.exe
# (for Win64)
$ make passgen.exe CC_WIN32=x86_64-w64-mingw32-gcc

# Cross-compile for DOS 16-bit Real Mode via OpenWatcom on Linux
$ export WATCOM=<path to watcom install>
$ export PATH=$WATCOM/binl64:$WATCOM/binl:$PATH; export EDPATH=$WATCOM/eddat; export INCLUDE=$WATCOM/h
$ make passgen.com
```

NOTE: Compilation under certain modes will fallback to a weaker RNG source. This utilises `rand()` in C, seeded with the time and PID - may be quite insecure! A modern compiler should warn when compiled in this mode. On most modern systems (Windows XP+ with `cl`/`mingw`, \*nix, macOS, Free/OpenBSD), the relevant system calls will be utilised to get better quality randomness. See the `_rng` functions for details.

## Licence

Zlib

See root of repo for full text.
