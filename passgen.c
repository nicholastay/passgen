#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_GRAMMAR "Cvccvc!##"

// i, o excluded due to potentially confusing 1/l/i + 0/o
#define VOWELS "aeuy"
#define CONSONANTS "bcdfghkmnprstvwxz"
#define NUMBERS "1234567890"
#define SYMBOLS "@#$%^&*_-+=()[]{}"

int main(int argc, char *argv[])
{
  char *grammar = DEFAULT_GRAMMAR;
  int grammar_size = sizeof(DEFAULT_GRAMMAR)-1;

  if (argc == 4) {
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

  char password[grammar_size];

  // seed RNG; this isn't very good, but it's enough (for now)
  srand(time(NULL) + getpid() % 420 - 69);
  
  for (int i = 0; i < grammar_size; ++i) {
    char c = grammar[i];

    bool caps = false;
    if (c >= 'A' && c <= 'Z') {
      caps = true;
      c += 'a' - 'A';
    }

    char *class;
    int class_size = 0;
#define setClass(cl) \
    class = cl; \
    class_size = sizeof(cl)-1; \
    break;

    switch (c) {
    case 'c':
      setClass(CONSONANTS);
    case 'v':
      setClass(VOWELS);
    case '!':
      setClass(SYMBOLS);
    case '#':
      setClass(NUMBERS);
    default:
      printf("ERROR: Invalid grammar character '%c'.\n", c);
      if (grammar != (char*)DEFAULT_GRAMMAR)
        free(grammar);
      return 1;
    }

    do {
      password[i] = class[rand() % class_size] - (caps ? 'a' - 'A' : 0);
    } while (i != 0 && password[i] == password[i-1]);
  }

  if (grammar != (char*)DEFAULT_GRAMMAR)
    free(grammar);

  printf("%s\n", password);

  return 0;
}
