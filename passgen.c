#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define VERSION "0.1.1"

#ifndef NO_PLATFORM_CRYPT
#if    defined (__linux__) \
    || defined (__APPLE__) \
    || defined (__FreeBSD__) \
    || defined (__OpenBSD__)
#   define USE_GETENTROPY
#elif defined (_WIN32) \
    || defined(__CYGWIN__)
#   define USE_WINCRYPT
#endif
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
#   include <windows.h>
#   include <wincrypt.h>
#else
#   include <time.h>
#   include <unistd.h>
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
HCRYPTPROV win_rng;
#endif
bool init_rng(void)
{
#ifdef USE_WINCRYPT
    if (!CryptAcquireContext(
        &win_rng,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT | CRYPT_SILENT
    ))
        return false;
#elif ! defined (USE_GETENTROPY) && ! defined (USE_WINCRYPT)
#if (defined(__GNUC__) || defined(__clang__))
#pragma message "Using fallback insecure RNG seeding!"
#endif
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
    if (!CryptGenRandom(win_rng, sizeof(r), (BYTE *) &r)) {
        perror("CryptGenRandom");
        exit(EXIT_FAILURE);
    }
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
    char *grammar = grammar_buf;
    int grammar_size = sizeof(DEFAULT_GRAMMAR) - 1;
    char *password = password_buf;

    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            printf("\
passgen v"VERSION" - a small, customisable password generator\n\
Usage: %s\n\
       %s <grammar>\n\
       %s <#triplets ('Cvc')> <#symbols> <#numbers>\n\
       %s --help\n\
Hints:\n\
  - No arguments generates with the default grammar.\n\
  - If three numbers are provided, a password will be generated in the 'standard form', <triplets><symbols><numbers>.\n\
Compile-time options (edit `config.h` to customise!):\n\
  - Default grammar: %s\n", argv[0], argv[0], argv[0], argv[0], grammar_buf);
            printf("  - Grammar mappings:\n");
            for (int i = 0; i < classes_n; ++i) {
                printf("    - '%c' => \"%s\"\n", classes[i].c, classes[i].letters);
            }
            return EXIT_SUCCESS;
        } else if (strcmp(argv[1], "--version") == 0) {
		printf("passgen v"VERSION"\n");
		return EXIT_SUCCESS;
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
	    exit(EXIT_FAILURE);
        }
    }

    if (grammar_size > STATIC_BUFFER_SIZE) {
        password = malloc(grammar_size + 1);
        if (password == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }
    password[grammar_size] = 0;

    if (!init_rng()) {
        fprintf(stderr, "ERROR: Could not initialise RNG.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < grammar_size; ++i) {
        char c = grammar[i];
        bool caps = c >= 'A' && c <= 'Z';
        if (caps)
            c += 'a' - 'A';

        struct grammar_class *class = NULL;
        for (int j = 0; j < classes_n; ++j) {
            if (c == classes[j].c) {
                class = &classes[j];
                break;
            }
        }
        if (class == NULL) {
            fprintf(stderr, "ERROR: Invalid grammar character '%c'.\n", c);
	    exit(EXIT_FAILURE);
        }

        do {
            password[i] = class->letters[get_rng() % class->size];
            if (caps && password[i] >= 'a' && password[i] <= 'z')
                password[i] -= 'a' - 'A';
        } while (i != 0 && password[i] == password[i - 1]);
    }

    printf("%s\n", password);
}
