# passgen

Just a password generator that makes [passwds.ninja](https://passwds.ninja/)-style passwords.

Heavily inspired by a generator I used before at work, just that this time in C. (This is my first C project, please be gentle...)

## Compiling

```
# *nix / macOS / mingw (on Windows)
$ make

# Windows, in Developer CMD
$ cl /std:c11 passgen.c /link Advapi32.lib
```

## Licence

Zlib

See root of repo for full text.
