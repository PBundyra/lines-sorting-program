#include "allocs.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "append.h"

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