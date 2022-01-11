#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if    defined (__linux__) \
    || defined (__APPLE__) \
    || defined (__FreeBSD__) \
    || defined (__OpenBSD__)
#   define USE_GETENTROPY 1
#endif

/* getentropy() vs rand()+time()+getpid() */
#ifdef USE_GETENTROPY
#if defined (__linux__) || defined (__APPLE__)
#    include <sys/random.h>
#else
#    include <unistd.h>
#endif
#else
#include <time.h>

/* getpid() on Windows */
#if defined (_WIN32) && ! defined (__MINGW32__)
#include <io.h>
#else
#include <unistd.h>
#endif

#endif


#define CLASS(c, chars) \
    case c: \
        class = chars; \
        class_size = sizeof(chars)-1; \
        break;


void init_rng(void)
{
    /* For now, this is only needed on rand() */
#ifndef USE_GETENTROPY
    /*
     * TODO: seed better RNG
     * this isn't very good, but it's enough(?) for now
     *
     * on linux we use the `getrandom` api which is supposedly
     * what crypto uses to generate. potentially on win we
     * can use the crypto thing (but it's more complicated
     * to invoke).
     *
     * anything else we could use on posix systems?
     */
    srand(time(NULL) + getpid() % 420 - 69);
#endif
}

int get_rng(void)
{
#ifdef USE_GETENTROPY
    int r;
    getentropy(&r, sizeof(r));
    return r;
#else
    return rand();
#endif
}

char *build_grammar(int triplets, int specials, int numbers)
{
    if (triplets < 1) {
        fprintf(stderr, "ERROR: Cannot have less than one triplet.");
        return NULL;
    }

    int grammar_size = triplets * 3 + specials + numbers;
    char *build = malloc(grammar_size + 1);
    if (build == NULL) {
        perror("malloc");
        return NULL;
    }
    build[grammar_size] = 0;

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

    if (argc == 2) {
        /* Take first argument as the grammar */
        grammar = argv[1];
        grammar_size = strlen(grammar);
    } else if (argc == 4) {
        /* 
         * Take arguments as triplets, specials, numbers
         * atoi might be scuffed but so be it (it just goes = 0 if invalid input)
         */
        grammar = build_grammar(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
        if (grammar == NULL) {
            err = true;
            goto cleanup;
        }
        custom_grammar = true;
    }

    char *password = malloc(grammar_size + 1);
    if (password == NULL) {
        perror("malloc");
        err = true;
        goto cleanup;
    }
    password[grammar_size] = 0;

    init_rng();

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
    if (custom_grammar)
        free(grammar);
    if (password)
        free(password);

    if (err)
        return 1;

    return 0;
}
