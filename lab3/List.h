#pragma once
#include "unistd.h"
#include <sys/types.h>

extern int AMOUNT_LIST_NODE;

typedef struct List
{
    pid_t data;
    struct List *next;
    struct List *prev;

} List;

void add_last(List** head, List** tail, pid_t data);
void add_first(List** head, List** tail, pid_t data);
void remove_n(List** head, List** tail, int number);

void remove_last(List** head, List** tail);
void remove_first(List** head, List** tail);

void add_by(List **head, List **tail, pid_t data, int (*comparator)(const void* first, const void* second));
void free_list(List *head);

void print_list(List* head);
