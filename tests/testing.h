/* Header file for our tests */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tagbase.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/* This replaces the normal assert but does what we want and doesn't go away in production code */
#define xassert(expr)  do {  \
        if(!(expr)) { \
            fprintf(stderr, "Test Fail (%s) - %s:%d\n",#expr, __FILENAME__, __LINE__); \
            abort(); \
        } \
    } while(0)

/* This is a debug print statement that will not be compiled away when DEBUG is defined */
#define TP(...)  do {   \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, " : %s:%d --> %s()\n", __FILENAME__, __LINE__, __func__); \
} while(0)


/* We put this here since really this is an internal structure exposed by an opaque pointer
   but we still need to make sure it gets built properly in many of these tests so we need
   it here. */
struct  tag_handle_t {
    tag_t *ptr;            /*!< Pointer to the Tag */
    char *str;             /*!< This string is the string that created the handle*/
    uint32_t count;        /*!< The number of items represented by the handle */
    uint32_t byte;         /*!< The byte offset where the data block starts */
    unsigned char bit;     /*!< The bit offset */
    uint32_t size;         /*!< The total size of the data block in bytes */
    tag_type type;         /*!< The data type of the block */
    unsigned int sentinel; /*!< Used to detect when tags have been deleted */
};
