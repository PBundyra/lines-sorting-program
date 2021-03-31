/* Patryk Bundyra
* Moje rozwiązanie opiera się na wykorzystaniu structur które zawierają odpowiednie tablice,
* pamietąją ich rozmiar, oraz indeks w który można wpisać kolejną wartość.
* Każdy wiersz jest początkowo reprezentowany jako tablica, która następnie zostaje odpowiednio parsowana
* oraz podzielona na tablicę liczb oraz tablicę nanów.
* W dalszej części programu wszystkie wiersze zostają posortowane w zależności od swojej zawartości.
* Na koniec wiersze podobne są umieszczone w tablicy ich indeksów w kolejności rosnącej,
* która zostaje posortowana oraz wypisana.
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include "sorts.h"
#include "append.h"
#include "numbers.h"
#include "allocs.h"

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

void ignore_com(void)
{
    char *line = NULL;
    size_t lenght = 0;
    safe_getline(&line, &lenght, stdin);

    free(line);
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

//funkcja zwracająca NULL jeśli wiersz zawiera tylko białe znaki, lub nie ma ich w ogóle
//wpp zwraca sprawdzany wiersz oraz (wykorzystując wskaźnik) ilość zawartych w nim znaków.
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

//funkcja wczytuje poprawny wiersz i dzieli jego zawartość na tablice typu char* i long double
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

//funkcja sprawdza czy wiersz jest poprawny i w zależności od wyniku dodaje wiersz do tablicy
//błędnych linii lub konwertuje wiersz do tablic wykorzystując funkcję map_line_to_tabs
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

//funkcja iteruje po posortowanej tablicy corr_lines_tab sprawdza czy dwa kolejne wiersze
//są do siebie podobne - jeśli tak wpisuje ich indeksy do tablicy
void map_sim_lines_to_tab(lines_tab *corr_lines_tab, lines_tab *sim_lines)
{
    for (int j = 0; j < (int)corr_lines_tab->first_free_index; j++)
    {
        resize_if_ness(sim_lines);
        add_num_to_tab(sim_lines, (long double)corr_lines_tab->corr_lines[j].line_index);
        if (j + 1 != corr_lines_tab->first_free_index)
        {

            int k = j + 1;

            while (cmp_lines(corr_lines_tab, j, k))
            {
                add_num_to_tab(sim_lines, (long double)corr_lines_tab->corr_lines[k].line_index);
                k++;
            }
            j = k - 1;
        }

        sim_lines->first_free_index++;
    }
}

/*Funkcja sortuje tablice liczb oraz nanów zawartych corr_lines_tab a następnie wykorzystując je
* sortuje tablice corr_lines_tab porównując tablice liczb oraz w sposób leksykograficzny poprzez tablice nanów
* następnie indeksy podobnych wierszy zostają wpisane do odpowiedniej tablicy,
* na końcu tablica z indeksami podobnych wierszy zostaje posortowana analogicznie do poprzedniej.
* Po zakończeniu wywołania posortowana tablica sim_lines zawiera posortowane indeksy podobnych wierszy, a całość
* ma złożoność O(n log n) w zależności od liczby wierszy.
*/
void sort_lines(lines_tab *corr_lines_tab, lines_tab *sim_lines)
{
    sort_content_of_corr_lines(corr_lines_tab);
    qsort(corr_lines_tab->corr_lines, corr_lines_tab->first_free_index, sizeof(corr_line), sort_lines_tab);

    map_sim_lines_to_tab(corr_lines_tab, sim_lines);
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
    initialize_lines_tab(corr_lines_tab, 0, STALA);
    initialize_lines_tab(sim_lines, 0, STALA);
    initialize_errs(errs);
}

void free_mem(lines_tab *corr_lines_tab, lines_tab *sim_lines, err_tab *errs)
{
    free_lines_tab(corr_lines_tab);
    free_lines_tab(sim_lines);
    free_errs(errs);
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
    print_sim_lines(&sim_lines);
    print_errs(errs);

    free_mem(&corr_lines_tab, &sim_lines, &errs);

    return 0;
}