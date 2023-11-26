#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct string *string_create(const char *str, size_t size)
{
    struct string *new_string = malloc(sizeof(struct string));
    if (new_string == NULL)
    {
        perror("Failed to allocate memory for string");
        exit(EXIT_FAILURE);
    }

    new_string->size = size;
    new_string->data = malloc(size + 1); // +1 for the null terminator
    if (new_string->data == NULL)
    {
        perror("Failed to allocate memory for string data");
        free(new_string);
        exit(EXIT_FAILURE);
    }

    memcpy(new_string->data, str, size);
    new_string->data[size] = '\0'; // Null-terminate the string

    return new_string;
}

int string_compare_n_str(const struct string *str1, const char *str2, size_t n)
{
    if (!str1 || !str2)
    {
        return -1;
    }
    for (size_t i = 0; i < n; i++)
    {
        if (str1->data[i] != str2[i])
        {
            return str1->data[i] - str2[i];
        }
    }
    return 0;
}

void string_concat_str(struct string *str, const char *to_concat, size_t size)
{
    size_t new_size = str->size + size;
    str->data = realloc(str->data, new_size + 1); // +1 for the null terminator
    if (str->data == NULL)
    {
        perror("Failed to reallocate memory for string data");
        exit(EXIT_FAILURE);
    }

    memcpy(str->data + str->size, to_concat, size);
    str->size = new_size;
    str->data[new_size] = '\0'; // Null-terminate the string
}

void string_destroy(struct string *str)
{
    if (!str)
    {
        return;
    }
    free(str->data);
    free(str);
}
