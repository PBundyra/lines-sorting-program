#include "allocs.h"
#include "append.h"
#include <stdlib.h>
#include <stdbool.h>

static size_t new_size(size_t n)
{
    return (2 * n + 1);
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