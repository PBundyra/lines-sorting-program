#ifndef APPEND_H
#define APPEND_H
#include <stdbool.h>
#include <stdlib.h>

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

extern void initializing_loop(lines_tab *corr_lines_tab, int loop_beg, int loop_end);

extern void add_num_to_tab(lines_tab *corr_lines_tab, long double num);

extern void add_nan_to_tab(lines_tab *corr_lines_tab, char *word);

extern void add_line_to_errs(err_tab *errs, int line_index, char *curr_line);

//funkcja zwiększa rozmiar przekazywanej tablicy, allokując dodatkową pamięć i inicjalizując ją
extern void resize_if_ness(lines_tab *corr_lines_tab);

#endif //APPEND_H