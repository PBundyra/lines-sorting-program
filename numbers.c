#include "numbers.h"
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include "allocs.h"
#include "append.h"
#include <string.h>

#define ASCII_0 48
#define ASCII_7 55
#define ASCII_9 57
#define ASCII_DIFF_BETWEEN_A_AND_10 87
#define ASCII_LOWERCASE_A 97
#define ASCII_LOWERCASE_F 102
#define OCT_SYSTEM_BASE 8
#define HEX_SYSTEM_BASE 16
#define LEN_OF_MAX_OCT_NUM 23

static long double hex_to_dec(char *hex_num, int word_len)
{
    if (word_len == 2)
    {
        return 0;
    }

    long double dec_val = 0;
    unsigned long long int base = 1;

    for (int i = word_len - 1; i >= 2; i--)
    {
        if (hex_num[i] >= '0' && hex_num[i] <= '9')
        {
            dec_val += (hex_num[i] - ASCII_0) * base;
        }
        else
        {
            dec_val += (hex_num[i] - ASCII_DIFF_BETWEEN_A_AND_10) * base;
        }
        base *= HEX_SYSTEM_BASE;
    }

    return dec_val;
}

static bool is_hex_char(char c)
{
    return ((ASCII_0 <= c && c <= ASCII_9) || (ASCII_LOWERCASE_A <= c && c <= ASCII_LOWERCASE_F));
}

static bool is_hex_num(char *word, int word_len)
{
    if (word[0] != '0' || word[1] != 'x' || word_len > 18)
    {
        return false;
    }
    for (int i = 2; i < word_len; i++)
    {
        if (!is_hex_char(word[i]))
        {
            return false;
        }
    }

    return true;
}

static long double oct_to_dec(char oct_num[], int word_len)
{
    long double dec_val = 0, base = 1;
    for (int i = word_len - 1; i > 0; i--)
    {
        dec_val += (oct_num[i] - ASCII_0) * base;
        base *= OCT_SYSTEM_BASE;
    }

    return dec_val;
}

static bool is_oct_char(char c)
{
    return (ASCII_0 <= c && c <= ASCII_7);
}

static bool is_oct_num(char *word, int word_len)
{
    if (word[0] != '0' || word_len > LEN_OF_MAX_OCT_NUM - 1)
    {
        if (!(word[0] == '0' && word_len == LEN_OF_MAX_OCT_NUM && word[1] == '1'))
        {
            return false;
        }
    }

    for (int i = 0; i < word_len; i++)
    {
        if (!(is_oct_char(word[i])))
        {
            return false;
        }
    }

    return true;
}

static bool is_dec_num(char word[], int word_len)
{
    if (word_len >= 2)
    {
        if ((word[0] == '+' || word[0] == '-') && (word[2] == 'x'))
        {
            return false;
        }
    }

    char *str_end;
    strtold(word, &str_end);

    if (errno == ERANGE)
    {
        return false;
    }
    else if (word == str_end || *str_end != 0)
    {
        return false;
    }

    return true;
}

bool is_num(char *word, int word_len)
{
    if (is_hex_num(word, word_len) || is_oct_num(word, word_len) || is_dec_num(word, word_len))
    {
        return true;
    }

    return false;
}

long double map_word_to_num(char word[], int word_len)
{
    long double val;

    if (is_hex_num(word, word_len))
    {
        val = hex_to_dec(word, word_len);
    }
    else if (is_oct_num(word, word_len))
    {
        val = oct_to_dec(word, word_len);
    }
    else
    {
        val = strtold(word, NULL);
    }

    return val;
}