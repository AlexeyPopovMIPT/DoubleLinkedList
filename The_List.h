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


/* ���� ������ ������ thou � ������ ��� ����� � ����� stream*/
void ListTextDump(List_t thou, FILE* stream);

/* ��������� ������ �� ������, �������� ��������� �� ������� *thou � LOGFILE,
   ����� � ������ �� ������� �������� ListTextDump*/
void ListVerify(List_t* thou, int line);

/*  ��� �������� ������ ������ *thou � ��������� >=start ���������� ����������.
    ������� ���� *thou ��� ���� �� ����������*/
void setasfree(List_t* thou, size_t start);

void swap(void* v1, void* v2, size_t bytecnt);

/*  ������� ��� �������� ������ *thou � �������� ������� ����������� �������
    ������*/
int ListFlush(List_t* thou);

/*  ������� List_t �� ������ thou � ������ name.
    name = nullptr ������ �������� ������ � ������ �� ���������.
    �� ������ thou ������ ���� ���������� ���������� ����� ��� �������� List_t*/
int MakeList(List_t* thou, const char* name);

int ListGraphDump(List_t thou);

void ListDistruct(List_t* thou);

/*  �������� ������ ������ *buffer � ����������� �� ����, ��� ����������� ����������
    �� ������ *capacity * elemsize � ������� ������� � ����� count * elemsize:
    *capacity == count: ������ ������ �������������;
    *capacity / 4 + *capacity / 8 >= count: ������ ������ �����������;
    �����: ������ ������ �� ����������
    \Note ������ ������ �� ����� ��������� max_capacity ��� ����� ������ min_capacity
*/
int Reallocate(void** buffer, size_t count, size_t elemsize,
    size_t* capacity, size_t min_capacity, size_t max_capacity);

/*  ������������ ������ ������ ������ *thou, ������� Reallocate � ������� �����������
    (min_capacity = MIN_CAPACITY, max_capacity = MAX_CAPACITY), ��� ��������� �������
    ������ �������� ������ ������� ��������� ������
    \Note ���������������, ��� ���������� ������ ��������� ��������� � �����������,
    ���� ��� ������� ������*/
int OptimizeCapacity(List_t* thou);

/*  ��������� ������ �� �������� ����������� ������� ���������� ���������� "����������
    �����" by alekseypopov10 �� ����� O(thou->capacity) � � �������������� ������� O(1)*/
int EnableFastIndexation(List_t* thou);

/* \returns ������ �������� � ������ �� ��� ����������� ������� � ������*/
size_t GetPhysInd(List_t* thou, size_t ind);

/* \returns ��������� �� �������� �������� �� ��� ����������� ������� � ������*/
int* GetByInd(List_t* thou, size_t ind);

/* \returns ��������� �� �������� �������� �� ��� ����������� ������� � ������.
   ���� �� ph_ind �������� ���, nullptr*/
int* GetByPhInd(List_t* thou, size_t ph_ind);

/* \param ph_ind - ���������� ������ ��������, ��������� �� ������� ��������� �����
   \returns ���������� ������ ���������� ��������. ���� ���������� ���, ph_ind.
   ���� ph_ind �����������, -1*/
size_t GetNext(List_t* thou, size_t ph_ind);

/* \returns: ��������� �� ��������� ��������� �� *elem �������. ���� ���������� ���, elem.
   ���� ��������� �����������, nullptr*/
int* GetNextPtr(List_t* thou, int* elem);

/* \param ph_ind - ���������� ������ ��������, ���������� ��� �������� ��������� �����
/* \returns ���������� ������ ����������� ��������. ���� ����������� ���, ph_ind.
   ���� ph_ind �����������, -1*/
size_t GetPrev(List_t* thou, size_t ph_ind);

/* \returns: ��������� �� ��������� ���������� ��� *elem �������. ���� ����������� ���, elem.
   ���� ��������� �����������, nullptr*/
int* GetPrevPtr(List_t* thou, int* elem);

/* ����� �������� �� ��� �������� val
   \param edge - 0 ��� ������ � ������, 1 ��� ������ � �����
   \returns ���������� ������ ��������, ���� �� �������, � -2 � ��������� ������
*/
size_t FindInd(List_t* thou, int val, int edge);

/* ����� �������� �� ��� �������� val
   \param edge - 0 ��� ������ � ������, 1 ��� ������ � �����
   \returns ��������� �� �������, ���� �� �������, � nullptr � ��������� ������
*/
int* FindPtr(List_t* thou, int val, int edge);

size_t GetCount(List_t* thou);

int IfSorted(List_t* thou);

/* ������� ������� � ������ ��� � ����� � ��� �������� ���������� �� ������ dest
   \param edge - 0 ��� �������� � ������, 1 ��� �������� � �����
*/
int RemoveFromEdge(List_t* thou, int* dest, int edge);

int RemoveFromStart(List_t* thou, int* dest);

int RemoveFromEnd(List_t* thou, int* dest);

/*������� ������� �� ���������� ������� ind � ���������� ��� �������� �� ������ dest*/
int RemoveAtPos(List_t* thou, size_t ind, int* dest);

/* ��������� ������� �� ��������� value � ������ ��� � ����� ������
   \param edge - 0 ��� ���������� � ������, 1 ��� ���������� � �����
*/
int AddToEdge(List_t* thou, int value, int edge);

int AddToStart(List_t* thou, int val);

int AddToEnd(List_t* thou, int val);

/* ��������� ������� �� ��������� value � ������ ���, ��� ��� ���������� ������ ����������
   ������ ind � ������� ������� �� ���������
*/
int AddToPos(List_t* thou, size_t ind, int val);

#endif /*INC_APTLIST*/
