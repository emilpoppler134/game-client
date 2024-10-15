#include "common.h"

typedef struct string_t
{
    u32 length;
    byte *c_string;
} string_t;

/*
@return length of string
*/
int string_length(char *string)
{
    int i = 0;
    while (string[i++] != '\0')
        ;
    return i;
}

/*
@note will ALWAYS insert '\0' to ensure string ends. Prevents problems with other string functions!
*/
char *string_get_substring(char *string, int start, int length)
{
    char *substr = (char *)malloc(sizeof(char) * (length + 1));

    for (int i = start; i < start + length; i++)
    {
        substr[i - start] = string[i];
    }

    // forces the last character to be
    substr[length - 1] = '\0';

    return substr;
}

/*
Creates a string_t
*/
string_t string_create(char *string, u32 length)
{
    return (string_t){
        .length = length,
        .c_string = (byte *)malloc(length),
    };
}

/*
Frees a string_t from memory
*/
void string_free(string_t *str)
{
    if (str->c_string)
    {
        free(str->c_string);
        str->c_string = NULL;
        str->length = 0;
    }
}

/*
@param str1 first string
@param str2 second string
@return true on match
*/
bool string_compare(char *str1, char *str2)
{
    i64 i = 0;
    while (str1[i] == str2[i])
    {
        if (str1[i] == '\0' && str2[i] == '\0')
        {
            return true;
        }
        i++;
    }
    return false;
}