#include "common.h"

struct _ilnklist {
    int used;
    int next;
    int prev;
};

void ilnkListInit(_ilnklist *list, int n, int *head)
{
    if (n > 0) {
        do {
            list->used = 0;
            list->prev = -1;
            list->next = -1;
            list++;
        } while (--n);
    }
    *head = -1;
}
void ilnkListFill(_ilnklist *list, int n, int *head, bool used)
{
    int i;

    n--;
    for (i = 0; i < n; i++, list++) {
        list->prev = i - 1;
        list->used = used;
        list->next = i + 1;
    }
    list->used = used;
    list->prev = i - 1;
    list->next = -1;
    *head = i;
}
void ilnkListAdd(_ilnklist *list, int idx, int *head)
{
    _ilnklist *e = &list[idx];

    if (e->used == 0) {
        if (*head != -1) {
            list[*head].next = idx;
            e->prev = *head;
            e->next = -1;
        }
        *head = idx;
        e->used = 1;
    }
}
void ilnkListRem(_ilnklist *list, int idx, int *head)
{
    _ilnklist *e = &list[idx];

    if (e->used == 1) {
        if (e->prev != -1)
            list[e->prev].next = e->next;
        if (idx == *head)
            *head = e->prev;
        else
            list[e->next].prev = e->prev;
        e->used = 0;
        e->prev = -1;
        e->next = -1;
    }
}
