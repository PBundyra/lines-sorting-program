#include "sorts.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "append.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static bool cmp_num_tabs(long double tab1[], long double tab2[], int tabs_len)
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

static int strcmp_sort(const void *x, const void *y)
{
    char *first = *(char **)x;
    char *second = *(char **)y;
    return strcmp(first, second);
}

static bool cmp_str_tabs(char **tab1, char **tab2, int tabs_len)
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

static int sort_nums(const void *x, const void *y)
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

static int sort_by_nums(corr_line first, corr_line second)
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

static int sort_by_nans(corr_line first, corr_line second)
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