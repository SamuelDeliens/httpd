#ifndef STRING_H
#define STRING_H

#include <ctype.h>
#include <stddef.h>

struct string
{
    size_t size;
    char *data;
};

/*
 ** @brief Create new string struct from char * and size
 **        Be careful, the argument str will not be deallocated and thus you
 **        can call this function with a static char *
 **
 ** @param str
 ** @param size
 **
 ** @return the newly allocated string
 */
struct string *string_create(const char *str, size_t size);

/*
 ** @brief Act like strncmp(3) but for struct string and char *
 **
 ** @param str1
 ** @param str2
 ** @param n
 **
 ** @return
 */
int string_compare_n_str(const struct string *str1, const char *str2, size_t n);

/*
 ** @brief Concat a char * with its size in a struct string
 **
 ** @param str
 ** @param to_concat
 ** @param size
 */
void string_concat_str(struct string *str, const char *to_concat, size_t size);

void string_destroy(struct string *str);

#endif /* ! STRING_H */
