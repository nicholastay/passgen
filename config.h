#define DEFAULT_GRAMMAR "Cvccvc!##"

/* Use some static buffers to avoid malloc */
#define STATIC_BUFFER_SIZE 24

/*
 * For vowels:
 *   i, o excluded due to potentially confusing 1/l/i + 0/o
 *   y included as a vowel because it kinda is one
 * The real vowels are a separate class, 'x' just in case
 */
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"
#define NUMBERS "1234567890"
#define SYMBOLS "@#$%^&*_-+=()[]{}"
#define CLASSES \
    CLASS('v', "aeuy") \
    CLASS('c', "bcdfghkmnprstvwxz") \
    CLASS('#', NUMBERS) \
    CLASS('!', SYMBOLS) \
    CLASS('l', ALPHABET) \
    CLASS('a', ALPHABET NUMBERS) \
    CLASS('b', ALPHABET NUMBERS SYMBOLS) \
    CLASS('x', "aeiou")

/*
 * Rationale for the combinations:
 *   l => 'letters'
 *   a => 'alphanumeric'
 *   b => 'all', but 'a' already taken so 'b'
 */
