# passgen

Just a password generator that makes [passwds.ninja](https://passwds.ninja/)-style passwords.

Heavily inspired by a generator I used before at work (in Python), just that this time in C.

As such, it utilises 'grammars' of character classes, which can be efficiently be mixed and matched on the fly, and/or the grammars themselves being able to be easily customised at compile-time.

See `passgen --help` for details.

(This is my first C project, please be gentle... some of the decisions I made may look weird or seem like I'm going out of my way to do something - some of these are done intentionally as a way for me to familiarise myself more with the features of the language.)

## Compiling

```
# *nix / macOS / mingw (on Windows)
$ make

# Windows, in Developer CMD
$ build_msvc.bat
```

NOTE: Compilation with mingw will use the fallback RNG for password generation. This utilises `rand()` in C, seeded with the time and PID - may be unsafe! On Windows with `cl`, \*nix, macOS, Free/OpenBSD, the relevant system calls will be utilised to get better quality randomness. See the `_rng` functions for details.

## Licence

Zlib

See root of repo for full text.
