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


#define CLASS(ch, chars) \
    { \
        .c = ch, \
        .letters = chars, \
        .size = sizeof(chars) - 1, \
    },
struct grammar_class {
    char c;
    char *letters;
    int size;
};
struct grammar_class classes[] = {
    CLASSES
};
int const classes_n = sizeof(classes) / sizeof(classes[0]);


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

char grammar_buf[STATIC_BUFFER_SIZE + 1] = DEFAULT_GRAMMAR;
char password_buf[STATIC_BUFFER_SIZE + 1];

char *build_grammar(int triplets, int specials, int numbers, int *grammar_size)
{
    *grammar_size = triplets * 3 + specials + numbers;

    char *build = grammar_buf;
    if (*grammar_size > STATIC_BUFFER_SIZE) {
        build = malloc(*grammar_size + 1);
        if (build == NULL) {
            perror("malloc");
            return NULL;
        }
    }
    build[*grammar_size] = 0;

    if (triplets > 0) {
        memcpy(build, "Cvc", 3);
        for (int i = 1; i < triplets; ++i)
            memcpy(build + (i * 3), "cvc", 3);
    }
    memset(build + (triplets * 3), '!', specials);
    memset(build + (triplets * 3) + specials, '#', numbers);
    return build;
}

int main(int argc, char *argv[])
{
    bool err = false;
    char *grammar = grammar_buf;
    int grammar_size = sizeof(DEFAULT_GRAMMAR) - 1;
    char *password = password_buf;

    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            printf("\
passgen - a small, customisable password generator\n\
Usage: %s\n\
       %s <grammar>\n\
       %s <#triplets ('Cvc')> <#symbols> <#numbers>\n\
Hints:\n\
  - No arguments generates with the default grammar.\n\
  - If three numbers are provided, a password will be generated in the 'standard form', <triplets><symbols><numbers>.\n\
Compile-time options:\n\
  - Default grammar: %s\n", argv[0], argv[0], argv[0], grammar_buf);
            return 0;
        }
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
            fprintf(stderr, "ERROR: Could not initialise memory for password grammar.\n");
            err = true;
            goto cleanup;
        }
    }

    if (grammar_size > STATIC_BUFFER_SIZE) {
        password = malloc(grammar_size + 1);
        if (password == NULL) {
            perror("malloc");
            err = true;
            goto cleanup;
        }
    }
    password[grammar_size] = 0;

    if (!init_rng()) {
        fprintf(stderr, "ERROR: Could not initialise RNG.\n");
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

        struct grammar_class *class = NULL;
        for (int j = 0; j < classes_n; ++j) {
            if (c == classes[j].c) {
                class = &classes[j];
                break;
            }
        }
        if (class == NULL) {
            fprintf(stderr, "ERROR: Invalid grammar character '%c'.\n", c);
            err = true;
            goto cleanup;
        }

        do {
            password[i] = class->letters[get_rng() % class->size] - (caps ? 'a' - 'A' : 0);
        } while (i != 0 && password[i] == password[i - 1]);
    }

    printf("%s\n", password);

cleanup:
#ifdef USE_WINCRYPT
    CryptReleaseContext(win_rng, 0);
#endif
    if (grammar != grammar_buf)
        free(grammar);
    if (password != password_buf)
        free(password);

    if (err)
        return 1;

    return 0;
}
