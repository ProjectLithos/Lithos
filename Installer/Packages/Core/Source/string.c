#include <string.h>

void *memcpy(void *destination, const void *source, size_t count) {
    unsigned char *output = (unsigned char *)destination;
    const unsigned char *input = (const unsigned char *)source;
    for (size_t index = 0; index < count; ++index) output[index] = input[index];
    return destination;
}

void *memmove(void *destination, const void *source, size_t count) {
    unsigned char *output = (unsigned char *)destination;
    const unsigned char *input = (const unsigned char *)source;
    if (output < input) {
        for (size_t index = 0; index < count; ++index) output[index] = input[index];
    } else if (output > input) {
        while (count != 0) { --count; output[count] = input[count]; }
    }
    return destination;
}

void *memset(void *destination, int value, size_t count) {
    unsigned char *output = (unsigned char *)destination;
    for (size_t index = 0; index < count; ++index) output[index] = (unsigned char)value;
    return destination;
}

int memcmp(const void *left, const void *right, size_t count) {
    const unsigned char *a = (const unsigned char *)left;
    const unsigned char *b = (const unsigned char *)right;
    for (size_t index = 0; index < count; ++index) {
        if (a[index] != b[index]) return a[index] < b[index] ? -1 : 1;
    }
    return 0;
}

size_t strlen(const char *text) {
    size_t length = 0;
    while (text[length] != '\0') ++length;
    return length;
}

int strcmp(const char *left, const char *right) {
    while (*left != '\0' && *left == *right) { ++left; ++right; }
    return (unsigned char)*left - (unsigned char)*right;
}

int strncmp(const char *left, const char *right, size_t count) {
    for (size_t index = 0; index < count; ++index) {
        unsigned char a = (unsigned char)left[index];
        unsigned char b = (unsigned char)right[index];
        if (a != b) return a < b ? -1 : 1;
        if (a == 0) return 0;
    }
    return 0;
}

char *strcpy(char *destination, const char *source) {
    char *result = destination;
    do { *destination++ = *source; } while (*source++ != '\0');
    return result;
}
