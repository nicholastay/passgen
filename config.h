#define DEFAULT_GRAMMAR "Cvccvc!##"

/* Use some static buffers to avoid malloc */
#define STATIC_BUFFER_SIZE 24

/*
 * For vowels:
 *   i, o excluded due to potentially confusing 1/l/i + 0/o
 *   y included as a vowel because it kinda is one
 */
#define CLASSES \
    CLASS('v', "aeuy") \
    CLASS('c', "bcdfghkmnprstvwxz") \
    CLASS('#', "1234567890") \
    CLASS('!', "@#$%^&*_-+=()[]{}")
