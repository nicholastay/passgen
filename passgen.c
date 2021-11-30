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

int main()
{
  char *grammar = DEFAULT_GRAMMAR;
  int grammar_size = sizeof(DEFAULT_GRAMMAR)-1;
  
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
      return 1;
    }

    do {
      password[i] = class[rand() % class_size] - (caps ? 'a' - 'A' : 0);
    } while (i != 0 && password[i] == password[i-1]);
  }

  printf("%s\n", password);

  return 0;
}
