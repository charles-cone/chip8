//
// Created by Charlie Cone on 2/8/21.
//

#include "stack.h"

struct stack* initStack(int size) {
    struct stack* s = (struct stack*)malloc(sizeof(struct stack));
    s->size = size;
    s->len = 0;
    s->buf = (int *)malloc(sizeof(int)*size);
    return s;
}
void insertVal(struct stack* st, int val) {
    if(st->size == st->len) {
        st->size *= 2;
        st->buf = (int *)realloc(st->buf, sizeof(int)*st->size);
    }
    st->buf[st->len] = val;
    st->len++;
}

void deleteStack(struct stack* st) {
    free(st->buf);
    free(st);
}

int popVal(struct stack* st) {
    int val = st->buf[--st->len];
    if(st->len < st->size/2) {
        st->size /= 2;
        st->buf = (int *)realloc(st->buf, sizeof(int)*st->size);
    }
    return val;
}

int comparer(const void* num1, const void* num2) {
    int val = *(int*)num1;
    int val2 = *(int*)num2;
    return val - val2;
}

void sortContents(struct stack* st) {
    qsort(st->buf, st->len, sizeof(int), comparer);
}

void printStack(struct stack* st) {
    for(int i = 0; i < st->len; i++) {
        printf("%d ", st->buf[i]);
    }
    printf("\n");
}