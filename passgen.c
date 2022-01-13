#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if    defined (__linux__) \
    || defined (__APPLE__) \
    || defined (__FreeBSD__) \
    || defined (__OpenBSD__)
#   define USE_GETENTROPY
#elif defined (_WIN32) && ! defined (__MINGW32__)
#   define USE_WINCRYPT
#endif


/* getentropy() vs rand()+time()+getpid() */
#ifdef USE_GETENTROPY
#if defined (__linux__) || defined (__APPLE__)
#   include <sys/random.h>
#else
#   include <unistd.h>
#endif
#elif defined (USE_WINCRYPT)
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   include <wincrypt.h>
#else
#   include <time.h>
/* getpid() on Windows */
#   if defined (_WIN32) && ! defined (__MINGW32__)
#       include <io.h>
#   else
#       include <unistd.h>
#   endif
#endif


#define CLASS(c, chars) \
    case c: \
        class = chars; \
        class_size = sizeof(chars)-1; \
        break;


#ifdef USE_WINCRYPT
HCRYPTPROV win_rng = NULL;
#endif
bool init_rng(void)
{
#ifdef USE_WINCRYPT
    CryptAcquireContext(
        &win_rng,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT | CRYPT_SILENT
    );
    if (!win_rng)
        return false;
#elif ! defined (USE_GETENTROPY) && ! defined (USE_WINCRYPT)
    /*
     * TODO: seed better RNG
     * this isn't very good, but it's enough(?) for now
     *
     * anything else we could use on general posix systems?
     * perhaps /dev/urandom is a better fallback before doing this
     */
    srand(time(NULL) + getpid() % 420 - 69);
#endif
    return true;
}

unsigned int get_rng(void)
{
    unsigned int r;
#ifdef USE_GETENTROPY
    getentropy(&r, sizeof(r));
#elif defined (USE_WINCRYPT)
    /* TODO: This could fail. Figure out how to handle */
    CryptGenRandom(win_rng, sizeof(r), (BYTE *) &r);
#else
    r = rand();
#endif
    return r;
}

char *build_grammar(int triplets, int specials, int numbers, int *grammar_size)
{
    if (triplets < 1) {
        fprintf(stderr, "ERROR: Cannot have less than one triplet.");
        return NULL;
    }

    *grammar_size = triplets * 3 + specials + numbers;
    char *build = malloc(*grammar_size + 1);
    if (build == NULL) {
        perror("malloc");
        return NULL;
    }
    build[*grammar_size] = 0;

    memcpy(build, "Cvc", 3);
    for (int i = 1; i < triplets; ++i)
        memcpy(build + (i * 3), "cvc", 3);

    memset(build + (triplets * 3), '!', specials);
    memset(build + (triplets * 3) + specials, '#', numbers);
    return build;
}

int main(int argc, char *argv[])
{
    bool err = false;
    bool custom_grammar = false;
    char *grammar = DEFAULT_GRAMMAR;
    int grammar_size = sizeof(DEFAULT_GRAMMAR) - 1;
    char *password = NULL;

    if (argc == 2) {
        /* Take first argument as the grammar */
        grammar = argv[1];
        grammar_size = strlen(grammar);
    } else if (argc == 4) {
        /*
         * Take arguments as triplets, specials, numbers
         * atoi might be scuffed but so be it (it just goes = 0 if invalid input)
         */
        grammar = build_grammar(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), &grammar_size);
        if (grammar == NULL) {
            err = true;
            goto cleanup;
        }
        custom_grammar = true;
    }

    password = malloc(grammar_size + 1);
    if (password == NULL) {
        perror("malloc");
        err = true;
        goto cleanup;
    }
    password[grammar_size] = 0;

    if (!init_rng()) {
        fprintf(stderr, "ERROR: Could not initialise RNG.");
        err = true;
        goto cleanup;
    }

    for (int i = 0; i < grammar_size; ++i) {
        char c = grammar[i];

        bool caps = false;
        if (c >= 'A' && c <= 'Z') {
            caps = true;
            c += 'a' - 'A';
        }

        char *class;
        int class_size = 0;
        switch (c) {
            CLASSES
        default:
            fprintf(stderr, "ERROR: Invalid grammar character '%c'.\n", c);
            err = true;
            goto cleanup;
        }

        do {
            password[i] = class[get_rng() % class_size] - (caps ? 'a' - 'A' : 0);
        } while (i != 0 && password[i] == password[i - 1]);
    }

    printf("%s\n", password);

cleanup:
#ifdef USE_WINCRYPT
    CryptReleaseContext(win_rng, 0);
#endif
    if (custom_grammar)
        free(grammar);
    if (password)
        free(password);

    if (err)
        return 1;

    return 0;
}
