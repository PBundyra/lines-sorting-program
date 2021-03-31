#ifndef NUMBERS_H
#define NUMBERS_H
#include <stdbool.h>

//funkcja określa czy wejściowe słowo powinno być interpretowane jako liczba
extern bool is_num(char *word, int word_len);

//funkcja zwracająca wartość liczbową w systemie dziesiętnym wejściowego słowa
extern long double map_word_to_num(char word[], int word_len);

#endif //NUMERS_H