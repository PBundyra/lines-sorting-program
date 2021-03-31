#ifndef SORTS_H
#define SORTS_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "append.h"

//funkcja porównuje dwa wiersze reprezentowane poprzez zawarte w nich liczby oraz nany
extern bool cmp_lines(lines_tab *corr_lines_tab, int first_line, int second_line);

//funkcja sortująca w rosnącej/leksykograficznej kolejności odpowiednio liczby
//oraz nany zawarte w wierszach
extern void sort_content_of_corr_lines(lines_tab *corr_lines_tab);

/*funkcja sortująca 2 poprawne wiersze w zależności od zawierających liczb i nanów,
* przy czym najpierw porównywane są tablice liczb, jeśli są one równe to funkcja sortuje
* wiersze wedle leksykograficznego porządku zawartych w nich słów
*/
extern int sort_lines_tab(const void *a, const void *b);

#endif //SORTS_H