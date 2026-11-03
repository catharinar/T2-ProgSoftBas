#include "mymemory.h"

#include <stdio.h>
#include <stdlib.h>

mymemory_t *mymemory_init(size_t size) {
    if (size == 0) {
        return NULL;
    }

    mymemory_t *memory = malloc(sizeof *memory);
    if (memory == NULL) {
        return NULL;
    }

    memory->pool = malloc(size);
    if (memory->pool == NULL) {
        free(memory);
        return NULL;
    }

    memory->total_size = size;
    memory->head = NULL;

    return memory;
}

void *mymemory_alloc(mymemory_t *memory, size_t size) {
    if (memory == NULL || memory->pool == NULL || size == 0) {
        return NULL;
    }

    unsigned char *pool = memory->pool;
    size_t offset = 0;
    allocation_t *previous = NULL;
    allocation_t *current = memory->head;

    while (current != NULL) {
        size_t start = (unsigned char *)current->start - pool;

        if (start - offset >= size) {
            break;
        }

        offset = start + current->size;
        previous = current;
        current = current->next;
    }

    if (current == NULL && memory->total_size - offset < size) {
        return NULL;
    }

    allocation_t *allocation = malloc(sizeof *allocation);
    if (allocation == NULL) {
        return NULL;
    }

    allocation->start = pool + offset;
    allocation->size = size;
    allocation->next = current;

    if (previous == NULL) {
        memory->head = allocation;
    } else {
        previous->next = allocation;
    }

    return allocation->start;
}

/* Item 3: retira somente o nodo de controle; o pool continua existindo. */
void mymemory_free(mymemory_t *memory, void *ptr)
{
    if (memory == NULL || ptr == NULL)
        return;

    allocation_t **link = &memory->head;
    while (*link != NULL) {
        allocation_t *current = *link;
        if (current->start == ptr) {
            *link = current->next;
            free(current);
            return;
        }
        link = &current->next;
    }
}

void mymemory_display(mymemory_t *memory) {
    if (memory == NULL) {
        puts("Memoria nao inicializada.");
        return;
    }

    if (memory->head == NULL) {
        puts("Nenhuma alocacao ativa.");
        return;
    }

    size_t index = 1;

    for (allocation_t *current = memory->head;
         current != NULL;
         current = current->next) {
        printf("Alocacao %zu: inicio = %p, tamanho = %zu bytes\n",
               index++, current->start, current->size);
    }
}

/* Item 5: pressupoe blocos validos, sem sobreposicao, dentro do pool.
 * A lista pode estar em qualquer ordem. Os offsets sao medidos em bytes. */
void mymemory_stats(mymemory_t *memory)
{
    if (memory == NULL) {
        printf("Memoria nao inicializada.\n");
        return;
    }

    size_t count = 0;
    size_t used = 0;
    size_t first = memory->total_size;
    size_t largest = 0;
    size_t fragments = 0;
    const unsigned char *base = memory->pool;

    for (allocation_t *node = memory->head; node != NULL; node = node->next) {
        size_t start = (size_t)((const unsigned char *)node->start - base);
        count++;
        used += node->size;
        if (start < first)
            first = start;

        /* Procura o bloco seguinte no pool, nao necessariamente na lista. */
        size_t next_start = memory->total_size;
        for (allocation_t *other = memory->head; other != NULL; other = other->next) {
            size_t offset = (size_t)((const unsigned char *)other->start - base);
            if (offset > start && offset < next_start)
                next_start = offset;
        }

        size_t gap = next_start - (start + node->size);
        if (gap > 0) {
            fragments++;
            if (gap > largest)
                largest = gap;
        }
    }

    /* Inclui o espaco antes do primeiro bloco ou todo o pool vazio. */
    if (first > 0) {
        fragments++;
        if (first > largest)
            largest = first;
    }

    printf("Numero total de alocacoes: %zu\n", count);
    printf("Memoria total alocada: %zu bytes\n", used);
    printf("Memoria total livre: %zu bytes\n", memory->total_size - used);
    printf("Maior bloco contiguo livre: %zu bytes\n", largest);
    printf("Numero de fragmentos livres: %zu\n", fragments);
}

/* Item 6: libera nodos, pool e estrutura principal, nessa ordem. */
void mymemory_cleanup(mymemory_t *memory)
{
    if (memory == NULL)
        return;

    allocation_t *node = memory->head;
    while (node != NULL) {
        allocation_t *next = node->next;
        free(node);
        node = next;
    }
    free(memory->pool);
    free(memory);
}
