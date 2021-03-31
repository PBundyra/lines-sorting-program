#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include <sorts.h>
#include <append.h>
#include <numbers.h>
#include <allocs.h>

#define COM_SIGN '#'
#define FIRST_ASCII_PRINTABLE_CHAR 32
#define LAST_ASCII_PRINTABLE_CHAR 126
#define FIRST_VALID_ASCII_CONTROL_CHAR 9
#define LAST_VALID_ASCII_CONTROL_CHAR 13
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define STALA 12
#define ASCII_DIFF_BETWEEN_CAP_AND_LOW_LETTERS 32
#define ASCII_CAP_A 65
#define ASCII_CAP_Z 90

typedef struct nan_tab
{
    char **nans;
    size_t size;
    int first_free_index;
    bool empty;
} nan_tab;

typedef struct num_tab
{
    long double *number;
    size_t size;
    int first_free_index;
    bool empty;
} num_tab;

typedef struct corr_line
{
    int line_index;
    nan_tab nans_tab;
    num_tab numbers_tab;
    char *whole_line;
} corr_line;

typedef struct incorr_line
{
    char *whole_line;
    int line_index;
} incorr_line;

typedef struct err_tab
{
    size_t size;
    int first_free_index;
    incorr_line *incorr_lines;
} err_tab;

typedef struct lines_tab
{
    size_t size;
    int first_free_index;
    corr_line *corr_lines;
} lines_tab;

size_t safe_getline(char **line, size_t *lenght, FILE *stdin)
{
    int line_len = getline(line, lenght, stdin);
    if (line_len == -1)
    {
        exit(1);
    }

    return (size_t)line_len;
}

void *safe_realloc(void *ptr, size_t size)
{
    errno = 0;
    ptr = realloc(ptr, size);
    if (errno == ENOMEM)
    {
        exit(1);
    }

    return ptr;
}

void *safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr)
    {
        return ptr;
    }

    exit(1);
}

size_t new_size(size_t n)
{
    return (2 * n + 1);
}

void ignore_com(void)
{
    char *line = NULL;
    size_t lenght = 0;
    safe_getline(&line, &lenght, stdin);

    free(line);
}

void initializing_loop(lines_tab *corr_lines_tab, int loop_beg, int loop_end)
{
    for (int i = loop_beg; i < loop_end; i++)
    {
        corr_lines_tab->corr_lines[i].whole_line = NULL;

        corr_lines_tab->corr_lines[i].nans_tab.nans = safe_malloc(sizeof(char *));
        corr_lines_tab->corr_lines[i].nans_tab.first_free_index = 0;
        corr_lines_tab->corr_lines[i].nans_tab.size = 1;
        corr_lines_tab->corr_lines[i].nans_tab.empty = true;

        corr_lines_tab->corr_lines[i].numbers_tab.number = safe_malloc(sizeof(long double));
        corr_lines_tab->corr_lines[i].numbers_tab.first_free_index = 0;
        corr_lines_tab->corr_lines[i].numbers_tab.size = 1;
        corr_lines_tab->corr_lines[i].numbers_tab.empty = true;
    }
}

void initialize_lines_tab(lines_tab *corr_lines_tab, int loop_beg, int loop_end)
{
    corr_lines_tab->corr_lines = safe_malloc(loop_end * sizeof(corr_line));
    corr_lines_tab->first_free_index = 0;
    corr_lines_tab->size = loop_end;
    initializing_loop(corr_lines_tab, loop_beg, loop_end);
}

void initialize_errs(err_tab *errs)
{
    errs->incorr_lines = safe_malloc(sizeof(incorr_line));

    errs->incorr_lines[0].whole_line = NULL;
    errs->first_free_index = 0;
    errs->size = 1;
}

char *empty_line(size_t *line_len, lines_tab *corr_lines_tab)
{
    corr_lines_tab->corr_lines[corr_lines_tab->first_free_index].whole_line = NULL;
    size_t lenght = 0;
    *line_len = safe_getline(&corr_lines_tab->corr_lines[corr_lines_tab->first_free_index].whole_line, &lenght, stdin);

    bool empty = true;
    char c;
    for (int i = 0; i < (int)(*line_len); i++)
    {
        c = corr_lines_tab->corr_lines[corr_lines_tab->first_free_index].whole_line[i];
        if (!(0 <= c && c <= FIRST_ASCII_PRINTABLE_CHAR))
        {
            empty = false;
        }
    }

    if (empty)
    {
        free(corr_lines_tab->corr_lines[corr_lines_tab->first_free_index].whole_line);
        corr_lines_tab->corr_lines[corr_lines_tab->first_free_index].whole_line = NULL;
        return NULL;
    }

    return corr_lines_tab->corr_lines[corr_lines_tab->first_free_index].whole_line;
}

bool correct_line(char line[], int line_len)
{
    for (int i = 0; i < line_len; i++)
    {
        char c = line[i];

        if (!((FIRST_VALID_ASCII_CONTROL_CHAR <= c && c <= LAST_VALID_ASCII_CONTROL_CHAR) || (FIRST_ASCII_PRINTABLE_CHAR <= c && c <= LAST_ASCII_PRINTABLE_CHAR)))
        {
            return false;
        }
    }
    return true;
}

void to_lower(char word[], int word_len)
{
    for (int i = 0; i < word_len; i++)
    {
        char c = word[i];
        if (ASCII_CAP_A <= c && c <= ASCII_CAP_Z)
        {
            word[i] = (char)(c + ASCII_DIFF_BETWEEN_CAP_AND_LOW_LETTERS);
        }
    }
}

long double hex_to_dec(char *hex_num, int word_len)
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
            dec_val += (hex_num[i] - 48) * base;
        }
        else
        {
            dec_val += (hex_num[i] - 87) * base;
        }
        base *= 16;
    }

    return dec_val;
}

bool is_hex_char(char c)
{
    return ((48 <= c && c <= 57) || (97 <= c && c <= 102));
}

bool is_hex_num(char *word, int word_len)
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

long double oct_to_dec(char oct_num[], int word_len)
{
    long double dec_val = 0, base = 1;
    for (int i = word_len - 1; i > 0; i--)
    {
        dec_val += (oct_num[i] - 48) * base;
        base *= 8;
    }

    return dec_val;
}

bool is_oct_char(char c)
{
    return (48 <= c && c <= 55);
}

bool is_oct_num(char *word, int word_len)
{
    if (word[0] != '0' || word_len > 22)
    {
        if (!(word[0] == '0' && word_len == 23 && word[1] == '1'))
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

bool is_dec_num(char word[], int word_len)
{
    if (word_len >= 2)
    {
        if ((word[0] == '+' || word[0] == '-') && (word[2] == 'x'))
        {
            return false;
        }
    }

    char *str_end;
    errno = 0;
    long double num = strtold(word, &str_end);

    if (errno == ERANGE)
    {
        return false;
    }
    else if (word == str_end || *str_end != 0)
    {
        return false;
    }

    if (num != num)
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

void add_num_to_tab(lines_tab *corr_lines_tab, long double num)
{
    int line_ind = corr_lines_tab->first_free_index;
    int num_ind = corr_lines_tab->corr_lines[line_ind].numbers_tab.first_free_index;
    int num_tab_size = corr_lines_tab->corr_lines[line_ind].numbers_tab.size;

    if (num_ind >= num_tab_size)
    {
        corr_lines_tab->corr_lines[line_ind].numbers_tab.number = safe_realloc(corr_lines_tab->corr_lines[line_ind].numbers_tab.number, new_size(num_tab_size) * sizeof(long double));

        for (int i = (int)num_tab_size; i < (int)new_size(num_tab_size); i++)
        {
            corr_lines_tab->corr_lines[line_ind].numbers_tab.number[i] = 0;
        }

        corr_lines_tab->corr_lines[line_ind].numbers_tab.size = new_size(num_tab_size);
    }

    corr_lines_tab->corr_lines[line_ind].numbers_tab.empty = false;
    corr_lines_tab->corr_lines[line_ind].numbers_tab.number[num_ind] = num;
    (corr_lines_tab->corr_lines[line_ind].numbers_tab.first_free_index)++;
}

void add_nan_to_tab(lines_tab *corr_lines_tab, char *word)
{
    int line_ind = corr_lines_tab->first_free_index;
    int nan_ind = corr_lines_tab->corr_lines[line_ind].nans_tab.first_free_index;
    int nan_tab_size = (int)corr_lines_tab->corr_lines[line_ind].nans_tab.size;

    if (nan_ind == nan_tab_size)
    {
        corr_lines_tab->corr_lines[line_ind].nans_tab.nans = safe_realloc(corr_lines_tab->corr_lines[line_ind].nans_tab.nans, new_size(nan_tab_size) * sizeof(char *));
        for (int i = (int)nan_tab_size; i < (int)new_size(nan_tab_size); i++)
        {
            corr_lines_tab->corr_lines[line_ind].nans_tab.nans[i] = NULL;
        }

        corr_lines_tab->corr_lines[line_ind].nans_tab.size = new_size(nan_tab_size);
    }

    corr_lines_tab->corr_lines[line_ind].nans_tab.empty = false;
    corr_lines_tab->corr_lines[line_ind].nans_tab.nans[nan_ind] = word;
    (corr_lines_tab->corr_lines[line_ind].nans_tab.first_free_index)++;
}

void add_line_to_errs(err_tab *errs, int line_index, char *curr_line)
{
    if (errs->first_free_index == (int)errs->size)
    {
        errs->incorr_lines = safe_realloc(errs->incorr_lines, new_size(errs->size) * sizeof(incorr_line));

        for (int i = (int)errs->size; i < (int)new_size(errs->size); i++)
        {
            errs->incorr_lines[i].whole_line = NULL;
        }

        errs->size = new_size(errs->size);
    }

    errs->incorr_lines[errs->first_free_index].line_index = line_index;
    errs->incorr_lines[errs->first_free_index].whole_line = curr_line;

    (errs->first_free_index)++;
}

void resize_if_ness(lines_tab *corr_lines_tab)
{
    if (corr_lines_tab->first_free_index == (int)corr_lines_tab->size)
    {

        corr_lines_tab->corr_lines = safe_realloc(corr_lines_tab->corr_lines, new_size(corr_lines_tab->size) * sizeof(corr_line));

        initializing_loop(corr_lines_tab, (int)corr_lines_tab->size, (int)new_size(corr_lines_tab->size));

        corr_lines_tab->size = new_size(corr_lines_tab->size);
    }
}

void map_line_to_tabs(lines_tab *corr_lines_tab, char curr_line[])
{
    char *word = strtok(curr_line, " \n");
    while (word)
    {
        int word_len = (int)strlen(word);
        to_lower(word, word_len);
        if (is_num(word, word_len))
        {
            long double num = map_word_to_num(word, word_len);
            add_num_to_tab(corr_lines_tab, num);
        }
        else
        {
            add_nan_to_tab(corr_lines_tab, word);
        }

        word = strtok(NULL, " \n");
    }
}

void parse_line(lines_tab *corr_lines_tab, int curr_line_index, err_tab *errs, size_t line_len)
{
    char *curr_line = corr_lines_tab->corr_lines[corr_lines_tab->first_free_index].whole_line;
    if (!(correct_line(curr_line, (int)line_len)))
    {
        add_line_to_errs(errs, curr_line_index, curr_line);
    }

    else
    {
        corr_lines_tab->corr_lines[corr_lines_tab->first_free_index].line_index = curr_line_index;
        map_line_to_tabs(corr_lines_tab, curr_line);
        (corr_lines_tab->first_free_index)++;
    }
}

bool cmp_num_tabs(long double tab1[], long double tab2[], int tabs_len)
{
    for (int i = 0; i < tabs_len; i++)
    {
        if (tab1[i] != tab2[i])
        {
            return false;
        }
    }

    return true;
}

int strcmp_sort(const void *x, const void *y)
{
    char *first = *(char **)x;
    char *second = *(char **)y;
    return strcmp(first, second);
}

bool cmp_str_tabs(char **tab1, char **tab2, int tabs_len)
{
    for (int i = 0; i < tabs_len; i++)
    {
        int res = strcmp_sort(tab1, tab2);
        if (res != 0)
        {
            return false;
        }
    }
    return true;
}

bool cmp_lines(lines_tab *corr_lines_tab, int first_line, int second_line)
{
    int num_tab_fill_1 = corr_lines_tab->corr_lines[first_line].numbers_tab.first_free_index;
    int num_tab_fill_2 = corr_lines_tab->corr_lines[second_line].numbers_tab.first_free_index;
    int nans_tab_fill_1 = corr_lines_tab->corr_lines[first_line].nans_tab.first_free_index;
    int nans_tab_fill_2 = corr_lines_tab->corr_lines[second_line].nans_tab.first_free_index;

    if (num_tab_fill_1 != num_tab_fill_2 || nans_tab_fill_1 != nans_tab_fill_2)
    {
        return false;
    }

    long double *nums_1 = corr_lines_tab->corr_lines[first_line].numbers_tab.number;
    long double *nums_2 = corr_lines_tab->corr_lines[second_line].numbers_tab.number;
    char **nans_1 = corr_lines_tab->corr_lines[first_line].nans_tab.nans;
    char **nans_2 = corr_lines_tab->corr_lines[second_line].nans_tab.nans;

    return (cmp_num_tabs(nums_1, nums_2, num_tab_fill_1) && cmp_str_tabs(nans_1, nans_2, nans_tab_fill_1));
}

int sort_nums(const void *x, const void *y)
{
    long double first_num = *(long double *)x;
    long double second_num = *(long double *)y;

    if (first_num < second_num)
    {
        return -1;
    }
    if (first_num > second_num)
    {
        return 1;
    }
    return 0;
}

void sort_content_of_corr_lines(lines_tab *corr_lines_tab)
{
    for (int i = 0; i < (int)corr_lines_tab->first_free_index; i++)
    {
        long double *nums_to_sort = corr_lines_tab->corr_lines[i].numbers_tab.number;
        size_t arr_len = corr_lines_tab->corr_lines[i].numbers_tab.first_free_index;
        qsort(nums_to_sort, arr_len, sizeof(long double), sort_nums);

        char **nans_to_sort = corr_lines_tab->corr_lines[i].nans_tab.nans;
        arr_len = corr_lines_tab->corr_lines[i].nans_tab.first_free_index;
        qsort(nans_to_sort, arr_len, sizeof(char *), strcmp_sort);
    }
}

int sort_by_nums(corr_line first, corr_line second)
{
    if (first.numbers_tab.empty == false && first.numbers_tab.empty == false)
    {
        int arr_len = MIN(first.numbers_tab.first_free_index, second.numbers_tab.first_free_index);

        for (int i = 0; i < arr_len; i++)
        {
            int first_num = first.numbers_tab.number[i], second_num = second.numbers_tab.number[i];
            if (first_num != second_num)
            {
                if (first_num > second_num)
                {
                    return 1;
                }

                return -1;
            }
        }

        if (first.numbers_tab.first_free_index > second.numbers_tab.first_free_index)
        {
            return 1;
        }
        else if (first.numbers_tab.first_free_index < second.numbers_tab.first_free_index)
        {
            return -1;
        }
    }
    else if (first.numbers_tab.empty == true && second.numbers_tab.empty == false)
    {
        return -1;
    }
    else if (first.numbers_tab.empty == false && second.numbers_tab.empty == true)
    {
        return 1;
    }
    return 0;
}

int sort_by_nans(corr_line first, corr_line second)
{
    if (first.nans_tab.empty == false && first.nans_tab.empty == false)
    {
        int arr_len = MIN(first.nans_tab.first_free_index, second.nans_tab.first_free_index);

        for (int i = 0; i < arr_len; i++)
        {
            char *first_str = first.nans_tab.nans[i], *second_str = second.nans_tab.nans[i];
            int res = strcmp(first_str, second_str);
            if (res != 0)
            {
                return res;
            }
        }

        if (first.nans_tab.first_free_index > second.nans_tab.first_free_index)
        {
            return 1;
        }
        else if (first.nans_tab.first_free_index < second.nans_tab.first_free_index)
        {
            return -1;
        }
    }
    else if (first.nans_tab.empty == true && second.nans_tab.empty == false)
    {
        return -1;
    }
    else if (first.nans_tab.empty == false && second.nans_tab.empty == true)
    {
        return 1;
    }

    return 0;
}

int sort_lines_tab(const void *a, const void *b)
{
    corr_line first = *(corr_line *)a;
    corr_line second = *(corr_line *)b;

    int num_res = sort_by_nums(first, second);
    if (num_res != 0)
    {
        return num_res;
    }

    int nan_res = sort_by_nans(first, second);
    if (nan_res != 0)
    {
        return nan_res;
    }

    return 0;
}

void free_lines_tab(lines_tab *corr_lines_tab)
{
    for (int i = 0; i < (int)corr_lines_tab->size; i++)
    {
        free(corr_lines_tab->corr_lines[i].nans_tab.nans);
        free(corr_lines_tab->corr_lines[i].numbers_tab.number);
        free(corr_lines_tab->corr_lines[i].whole_line);
    }
    free(corr_lines_tab->corr_lines);
}

void free_errs(err_tab *errs)
{
    for (int i = 0; i < (int)errs->size; i++)
    {
        free(errs->incorr_lines[i].whole_line);
    }
    free(errs->incorr_lines);
}

void map_sim_lines_to_tab(lines_tab *corr_lines_tab, lines_tab *sim_lines)
{
    for (int j = 0; j < (int)corr_lines_tab->first_free_index; j++)
    {
        resize_if_ness(&sim_lines);
        add_num_to_tab(&sim_lines, (long double)corr_lines_tab->corr_lines[j].line_index);
        if (j + 1 != corr_lines_tab->first_free_index)
        {

            int k = j + 1;

            while (cmp_lines(corr_lines_tab, j, k))
            {
                add_num_to_tab(&sim_lines, (long double)corr_lines_tab->corr_lines[k].line_index);
                k++;
            }
            j = k - 1;
        }

        sim_lines->first_free_index++;
    }
}

void sort_lines(lines_tab *corr_lines_tab, lines_tab *sim_lines)
{
    sort_content_of_corr_lines(corr_lines_tab);
    qsort(corr_lines_tab->corr_lines, corr_lines_tab->first_free_index, sizeof(corr_line), sort_lines_tab);

    map_sim_lines_to_tab(corr_lines_tab, &sim_lines);
    qsort(sim_lines->corr_lines, (ssize_t)sim_lines->first_free_index, sizeof(corr_line), sort_lines_tab);
}

void print_sim_lines(lines_tab *sim_lines)
{
    for (int i = 0; i < sim_lines->first_free_index; i++)
    {
        fprintf(stdout, "%d", (int)sim_lines->corr_lines[i].numbers_tab.number[0]);

        for (int j = 1; j < (int)sim_lines->corr_lines[i].numbers_tab.first_free_index; j++)
        {
            fprintf(stdout, " %d", (int)sim_lines->corr_lines[i].numbers_tab.number[j]);
        }

        fprintf(stdout, "\n");
    }
}

void print_errs(err_tab errs)
{
    for (int i = 0; i < (int)errs.first_free_index; i++)
    {
        fprintf(stderr, "ERROR %d\n", errs.incorr_lines[i].line_index);
    }
}

void initialize_tabs(lines_tab *corr_lines_tab, lines_tab *sim_lines, err_tab *errs)
{
    initialize_lines_tab(&corr_lines_tab, 0, STALA);
    initialize_lines_tab(&sim_lines, 0, STALA);
    initialize_errs(&errs);
}

void free_mem(lines_tab *corr_lines_tab, lines_tab *sim_lines, err_tab *errs)
{
    free_lines_tab(&corr_lines_tab);
    free_lines_tab(&sim_lines);
    free_errs(&errs);
}

int main(void)
{
    int curr_line_index = 1;
    lines_tab corr_lines_tab, sim_lines;
    err_tab errs;
    initialize_tabs(&corr_lines_tab, &sim_lines, &errs);

    char c;
    while ((c = getchar()) != EOF)
    {
        if (c == COM_SIGN)
        {
            ignore_com();
        }
        else
        {
            ungetc(c, stdin);
            size_t line_len;
            resize_if_ness(&corr_lines_tab);
            if (empty_line(&line_len, &corr_lines_tab) != NULL)
            {
                parse_line(&corr_lines_tab, curr_line_index, &errs, line_len);
            }
        }
        curr_line_index++;
    }
    sort_lines(&corr_lines_tab, &sim_lines);
    print_sim_lines(&corr_lines_tab);
    print_errs(errs);

    free_mem(&corr_lines_tab, &sim_lines, &errs);

    return 0;
}