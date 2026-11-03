#include "mymemory.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Fixtures de teste: nao substituem init/alloc da dupla. */
static mymemory_t *fixture(size_t size)
{
    mymemory_t *m = malloc(sizeof(*m));
    assert(m != NULL);
    m->pool = malloc(size);
    assert(m->pool != NULL);
    m->total_size = size;
    m->head = NULL;
    return m;
}

static void add(mymemory_t *m, size_t offset, size_t size)
{
    allocation_t *node = malloc(sizeof(*node));
    assert(node != NULL);
    node->start = (unsigned char *)m->pool + offset;
    node->size = size;
    node->next = m->head;
    m->head = node;
    memset(node->start, 0x5a, size);
}

int main(void)
{
    mymemory_free(NULL, NULL);
    mymemory_cleanup(NULL);
    mymemory_t *m = fixture(1000);
    puts("[vazio]");
    mymemory_stats(m);

    /* Lista fora de ordem: 600 -> 100 -> 300. */
    add(m, 300, 100);
    add(m, 100, 100);
    add(m, 600, 200);
    puts("[fragmentado]");
    mymemory_stats(m);

    allocation_t *head = m->head;
    int external = 0;
    mymemory_free(m, &external);
    mymemory_free(m, NULL);
    mymemory_free(m, (unsigned char *)m->pool + 101);
    assert(m->head == head && head->next->next != NULL);
    void *middle = (unsigned char *)m->pool + 100;
    mymemory_free(m, middle);
    mymemory_free(m, middle); /* Liberacao repetida deve ser ignorada. */
    assert(m->head == head && head->next->start == (unsigned char *)m->pool + 300);
    assert(head->next->next == NULL);
    mymemory_free(m, (unsigned char *)m->pool + 300); /* Cauda. */
    assert(head->next == NULL);
    assert(((unsigned char *)head->start)[0] == 0x5a);
    assert(((unsigned char *)head->start)[199] == 0x5a);
    mymemory_free(m, head->start); /* Cabeca e ultimo nodo. */
    assert(m->head == NULL);
    mymemory_cleanup(m);

    m = fixture(1000);
    add(m, 500, 500);
    add(m, 0, 500);
    puts("[cheio]");
    mymemory_stats(m);
    mymemory_cleanup(m); /* Ainda existem nodos a liberar. */

    m = fixture(1000);
    for (size_t i = 0; i < 50; i++)
        add(m, i * 20, 1 + i % 19);
    puts("[50_blocos]");
    mymemory_stats(m);
    for (size_t i = 0; i < 50; i += 2)
        mymemory_free(m, (unsigned char *)m->pool + i * 20);
    for (size_t i = 1; i < 50; i += 2)
        mymemory_free(m, (unsigned char *)m->pool + i * 20);
    assert(m->head == NULL);
    mymemory_cleanup(m);
    puts("OK");
    return 0;
}
