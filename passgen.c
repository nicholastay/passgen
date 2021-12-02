#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef __linux__
#include <sys/random.h>
#else
#include <time.h>
#include <unistd.h>
#endif

#define CLASS(c, chars) \
  case c: \
    class = chars; \
    class_size = sizeof(chars)-1; \
    break;


int main(int argc, char *argv[])
{
  char *grammar = DEFAULT_GRAMMAR;
  int grammar_size = sizeof(DEFAULT_GRAMMAR)-1;

  if (argc == 2) {
    // Take first argument as the grammar
    grammar = argv[1];
    grammar_size = strlen(grammar);
  } else if (argc == 4) {
    // Take arguments as triplets, specials, numbers
    // atoi might be scuffed but so be it (it just goes = 0 if invalid input)
    int triplets = atoi(argv[1]);
    int specials = atoi(argv[2]);
    int numbers = atoi(argv[3]);

    if (triplets < 1) {
      printf("ERROR: Cannot have less than one triplet.");
      return 1;
    }

    grammar_size = triplets * 3 + specials + numbers;
    grammar = malloc(grammar_size + 1);
    grammar[grammar_size] = 0;

    memcpy(grammar, "Cvc", 3);
    for (int i = 1; i < triplets; ++i)
      memcpy(grammar + (i * 3), "cvc", 3);

    memset(grammar + (triplets * 3), '!', specials);
    memset(grammar + (triplets * 3) + specials, '#', numbers);
    //printf("Custom: %s\n", grammar);
  }

  char password[grammar_size+1];
  password[grammar_size] = 0;

#ifdef __linux__
  // Reduce syscalls by generating a bunch and then doing it
  unsigned int entropy[grammar_size];
  getrandom(&entropy, sizeof(entropy), 0);
#else
  // seed RNG; this isn't very good, but it's enough(?)
  srand(time(NULL) + getpid() % 420 - 69);
#endif
  
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
      printf("ERROR: Invalid grammar character '%c'.\n", c);
      if (grammar != (char*)DEFAULT_GRAMMAR)
        free(grammar);
      return 1;
    }

    do {
#ifdef __linux__
      unsigned int r = entropy[i];
#else
      long r = rand();
#endif
      password[i] = class[r % class_size] - (caps ? 'a' - 'A' : 0);
    } while (i != 0 && password[i] == password[i-1]);
  }

  if (grammar != (char*)DEFAULT_GRAMMAR)
    free(grammar);

  printf("%s\n", password);

  return 0;
}
