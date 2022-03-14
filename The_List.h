/*
** Created by alekseypopov10
** 09.11.2020
*/


#pragma once
#ifndef INC_APTLIST

#define INC_APTLIST
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*------------------------------------------------------------------------------*/
/*------------------------------Assertion definitions---------------------------*/

#ifdef NDEBUG
#define VERIFY if(0)
#else
#define VERIFY ;
#endif

#define ASSERTION_PACK                            \
          MINI_ASSERTION_PACK                     \
          VERIFY ListVerify(thou, __LINE__);

#define MINI_ASSERTION_PACK                       \
          assert(thou);                           \
          assert(thou->buffer);                   \
          assert(thou->count <= thou->capacity);  \
          assert(thou->capacity <= MAX_CAPACITY); 


/*-----------------------------------------------------------------------------*/
/*----------------------------List and item structures-------------------------*/

struct Item
{
    int item;
    size_t prev_item_ind;
    size_t next_item_ind;
};

struct List
{
    struct Item* buffer;
    size_t capacity;
    size_t free;
    size_t left_edge;
    size_t right_edge;
    size_t count;
    const char* name;
    int sorted;
};

typedef struct List List_t;

/*-----------------------------------------------------------------------------*/
/*----------------------------------Return codes-------------------------------*/

enum ERRORS {
    ERR_INC_PARAM = -1,
    ERR_NO_MEM = 1,
    ERR_LIST_EMPTY = 2,
    NO_ENTRIES = -2
};

enum SIZE_CHANGE {
    UP = 10,
    EQUAL = 0,
    DOWN = -10
};

/*------------------------------------------------------------------------------*/
/*---------------------------------Other constants------------------------------*/

const size_t MIN_CAPACITY = 10;
const size_t MAX_CAPACITY = 0xFFFFFFFD;
const char* LOGFILE = "debug.log";
const char* DEFAULT_NAME = "unnamed";


/*------------------------------------------------------------------------------*/
/*-----------------------------Functions' declarations--------------------------*/


/* Дамп буфера списка thou и данных его полей в поток stream*/
void ListTextDump(List_t thou, FILE* stream);

/* Проверяет список на ошибки, печатает сообщения об ошибках *thou в LOGFILE,
   также в случае их наличия вызывает ListTextDump*/
void ListVerify(List_t* thou, int line);

/*  Все элементы буфера списка *thou с индексами >=start становятся свободными.
    Никакие поля *thou при этом не изменяются*/
void setasfree(List_t* thou, size_t start);

void swap(void* v1, void* v2, size_t bytecnt);

/*  Удаляет все элементы списка *thou и вызывает функцию оптимизации размера
    буфера*/
int ListFlush(List_t* thou);

/*  Создает List_t по адресу thou с именем name.
    name = nullptr влечёт создание списка с именем по умолчанию.
    По адресу thou должно быть достаточно свободного места для создания List_t*/
int MakeList(List_t* thou, const char* name);

int ListGraphDump(List_t thou);

void ListDistruct(List_t* thou);

/*  Изменяет размер буфера *buffer в зависимости от того, как соотносятся занимаемая
    им память *capacity * elemsize и занятая данными её часть count * elemsize:
    *capacity == count: размер буфера увеличивается;
    *capacity / 4 + *capacity / 8 >= count: размер буфера уменьшается;
    иначе: размер буфера не изменяется
    \Note размер буфера не может превысить max_capacity или стать меньше min_capacity
*/
int Reallocate(void** buffer, size_t count, size_t elemsize,
    size_t* capacity, size_t min_capacity, size_t max_capacity);

/*  Оптимизирует размер буфера списка *thou, вызывая Reallocate с нужными параметрами
    (min_capacity = MIN_CAPACITY, max_capacity = MAX_CAPACITY), при изменении размера
    буфера изменяет нужным образом незанятые ячейки
    \Note подразумевается, что физические адреса элементов совпадают с логическими,
    либо все позиции заняты*/
int OptimizeCapacity(List_t* thou);

/*  Сортирует список по значению логического индекса уникальным алгоритмом "Сортировка
    Васей" by alekseypopov10 за время O(thou->capacity) и с дополнительной памятью O(1)*/
int EnableFastIndexation(List_t* thou);

/* \returns индекс элемента в буфере по его логическому индексу в списке*/
size_t GetPhysInd(List_t* thou, size_t ind);

/* \returns указатель на значение элемента по его логическому индексу в списке*/
int* GetByInd(List_t* thou, size_t ind);

/* \returns указатель на значение элемента по его физическому индексу в буфере.
   Если по ph_ind элемента нет, nullptr*/
int* GetByPhInd(List_t* thou, size_t ph_ind);

/* \param ph_ind - физический индекс элемента, следующий за которым требуется найти
   \returns физический индекс следующего элемента. Если следующего нет, ph_ind.
   Если ph_ind некорректен, -1*/
size_t GetNext(List_t* thou, size_t ph_ind);

/* \returns: указатель на логически следующий за *elem элемент. Если следующего нет, elem.
   Если указатель некорректен, nullptr*/
int* GetNextPtr(List_t* thou, int* elem);

/* \param ph_ind - физический индекс элемента, предыдущий для которого требуется найти
/* \returns физический индекс предыдущего элемента. Если предыдущего нет, ph_ind.
   Если ph_ind некорректен, -1*/
size_t GetPrev(List_t* thou, size_t ph_ind);

/* \returns: указатель на логически предыдущий для *elem элемент. Если предыдущего нет, elem.
   Если указатель некорректен, nullptr*/
int* GetPrevPtr(List_t* thou, int* elem);

/* Поиск элемента по его значению val
   \param edge - 0 для поиска с начала, 1 для поиска с конца
   \returns логический индекс элемента, если он нашёлся, и -2 в противном случае
*/
size_t FindInd(List_t* thou, int val, int edge);

/* Поиск элемента по его значению val
   \param edge - 0 для поиска с начала, 1 для поиска с конца
   \returns указатель на элемент, если он нашёлся, и nullptr в противном случае
*/
int* FindPtr(List_t* thou, int val, int edge);

size_t GetCount(List_t* thou);

int IfSorted(List_t* thou);

/* Удаляет элемент с начала или с конца и его значение записывает по адресу dest
   \param edge - 0 для удаления с начала, 1 для удаления с конца
*/
int RemoveFromEdge(List_t* thou, int* dest, int edge);

int RemoveFromStart(List_t* thou, int* dest);

int RemoveFromEnd(List_t* thou, int* dest);

/*Удаляет элемент на логической позиции ind и записывает его значение по адресу dest*/
int RemoveAtPos(List_t* thou, size_t ind, int* dest);

/* Добавляет элемент со значением value в начало или в конец списка
   \param edge - 0 для добавления в начало, 1 для добавление в конец
*/
int AddToEdge(List_t* thou, int value, int edge);

int AddToStart(List_t* thou, int val);

int AddToEnd(List_t* thou, int val);

/* Добавляет элемент со значением value в список так, что его логический индекс становится
   равным ind и никакой элемент не удаляется
*/
int AddToPos(List_t* thou, size_t ind, int val);

#endif /*INC_APTLIST*/
