
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define ListDef(TYPE)                                                     \
    typedef struct ListOf##TYPE                                           \
    {                                                                     \
        TYPE *items;                                                      \
        ssize_t count, cap;                                               \
    } ListOf##TYPE;                                                       \
    void TYPE##_ListPush(ListOf##TYPE *list, TYPE item);                  \
    TYPE TYPE##_ListPop(ListOf##TYPE *list);                              \
    void TYPE##_ListRemoveAt(ListOf##TYPE *list, size_t idx);             \
    void TYPE##_ListInsertAt(ListOf##TYPE *list, size_t idx, TYPE value); \
    size_t TYPE##_ListLength(ListOf##TYPE *list);                         \
    void TYPE##_ListFree(ListOf##TYPE *list);                             \
    TYPE *TYPE##_ListAt(ListOf##TYPE *list, size_t idx)

#define ListImpl(TYPE)                                                            \
    void TYPE##_ListPush(ListOf##TYPE *list, TYPE item)                           \
    {                                                                             \
        if (list->count >= list->cap)                                             \
        {                                                                         \
            list->cap = list->cap ? list->cap * 2 : 4;                            \
            list->items = (TYPE *)realloc(list->items, list->cap * sizeof(TYPE)); \
            assert(list->items);                                                  \
        }                                                                         \
        list->items[list->count++] = item;                                        \
    }                                                                             \
                                                                                  \
    TYPE TYPE##_ListPop(ListOf##TYPE *list)                                       \
    {                                                                             \
        assert(list->count > 0);                                                  \
        return list->items[--list->count];                                        \
    }                                                                             \
                                                                                  \
    void TYPE##_ListRemoveAt(ListOf##TYPE *list, size_t idx)                      \
    {                                                                             \
        assert(idx < list->count);                                                \
        for (size_t i = idx; i < list->count - 1; ++i)                            \
        {                                                                         \
            list->items[i] = list->items[i + 1];                                  \
        }                                                                         \
        --list->count;                                                            \
    }                                                                             \
                                                                                  \
    void TYPE##_ListInsertAt(ListOf##TYPE *list, size_t idx, TYPE value)          \
    {                                                                             \
        assert(idx <= list->count);                                               \
        if (list->count >= list->cap)                                             \
        {                                                                         \
            list->cap = list->cap ? list->cap * 2 : 4;                            \
            list->items = (TYPE *)realloc(list->items, list->cap * sizeof(TYPE)); \
            assert(list->items);                                                  \
        }                                                                         \
        for (size_t i = list->count; i > idx; --i)                                \
        {                                                                         \
            list->items[i] = list->items[i - 1];                                  \
        }                                                                         \
        list->items[idx] = value;                                                 \
        ++list->count;                                                            \
    }                                                                             \
                                                                                  \
    size_t TYPE##_ListLength(ListOf##TYPE *list)                                  \
    {                                                                             \
        return list->count;                                                       \
    }                                                                             \
                                                                                  \
    void TYPE##_ListFree(ListOf##TYPE *list)                                      \
    {                                                                             \
        free(list->items);                                                        \
        list->items = NULL;                                                       \
        list->count = list->cap = 0;                                              \
    }                                                                             \
    TYPE *TYPE##_ListAt(ListOf##TYPE *list, size_t idx)                           \
    {                                                                             \
        assert(idx < list->count);                                                \
        return &list->items[idx];                                                 \
    }



    