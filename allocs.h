/*Biblioteka allocs.h zawiera funkcje alokujące pamięć w bezpieczny sposób
* tj. jeśli wystąpi błąd krytyczny, następuje exit(1);
* oraz funkcje zwalniające zaalokowaną pamięć
*/
#ifndef ALLOCS_H
#define ALLOCS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "append.h"

extern size_t safe_getline(char **line, size_t *lenght, FILE *stdin);

extern void *safe_realloc(void *ptr, size_t size);

extern void *safe_malloc(size_t size);

extern void free_lines_tab(lines_tab *tab);

extern void free_errs(err_tab *errs);

#endif //ALLOCS_H
