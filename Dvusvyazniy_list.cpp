#include "The List.h"



void ListTextDump(List_t thou, FILE* stream)
{
    fprintf(stream, "name       = %s\n", thou.name);
    fprintf(stream, "count      = %u\n", thou.count);
    fprintf(stream, "capacity   = %u\n", thou.capacity);
    fprintf(stream, "free       = %u\n", thou.free);
    fprintf(stream, "left_edge  = %u\n", thou.left_edge);
    fprintf(stream, "right_edge = %u\n", thou.right_edge);
    fprintf(stream, "sorted     = %s\n", thou.sorted ? "true" : "false");
    fprintf(stream, " ind item prev next\n");
    for (size_t i = 0; i < thou.capacity; i++)
    {
        fprintf(stream, "[%2u] %4d %4u %4u\n", i, thou.buffer[i].item, thou.buffer[i].prev_item_ind, thou.buffer[i].next_item_ind);
    }
}

void ListVerify(List_t* thou, int line)
{
    #define PRINTF(message, ...) { fprintf(log, "[%s:%p:ERROR:line %d] " message "\n", thou->name, thou, line, __VA_ARGS__); error = 1; }

    assert(thou);

    int left_edge_cracked = 0;
    int buf_ptr_cracked = 0;
    int error = 0;

    size_t isfree = 1;
    size_t pos = thou->left_edge;
    size_t occupied = 1;


    char unnamed = '\0';

    FILE* log = NULL;
    if (fopen_s(&log, LOGFILE, "a"))
    {
         printf("ERROR: log file not found\n");
         abort();
    }

    if (thou->name == NULL) {
        fprintf(log, "[%p:ERROR:line %d] buffer has no name\n", thou, line);
        thou->name = &unnamed;
    }

    if (thou->buffer == NULL)
    {
        PRINTF("buffer is NULL")
        buf_ptr_cracked = 1;
    }

    if (thou->left_edge >= thou->capacity)
    {
        left_edge_cracked = 1;
        PRINTF("left_edge is too big")
    }

    if (thou->right_edge >= thou->capacity)
        PRINTF("right_edge is too big")


    if (buf_ptr_cracked) goto end;

    pos = thou->left_edge;

    if (!left_edge_cracked && pos != thou->buffer[pos].prev_item_ind && thou->count > 0)
        PRINTF("left_edge is not self-linking")

    occupied = 1;

    for (; occupied < thou->capacity; occupied++)
    {
        if (pos == thou->right_edge) break;
        if (pos >= thou->capacity) {
            PRINTF("[pos=%u] too big occupied index value", pos)
            continue;
        }

        if (thou->buffer[pos].prev_item_ind == -1)
            PRINTF("[pos=%u] prev_item_ind is free\'s poison", pos)
        if (thou->buffer[thou->buffer[pos].next_item_ind].prev_item_ind != pos)
            PRINTF("[pos=%u] prev of next is not pos", pos)

        pos = thou->buffer[pos].next_item_ind;
    }
    if (pos != thou->right_edge)
        PRINTF("list has a cycle")
    if (thou->buffer[pos].prev_item_ind == -1)
    {
        occupied = 0;
    }
    if (pos != thou->buffer[pos].next_item_ind && thou->count > 0)
        PRINTF("right_edge is not self-linking")

        isfree = 1;
    pos = thou->free;
    if (pos > thou->capacity && pos != -1) {
        PRINTF("free is too big")
    }
    else if (pos != -1)
    {
        for (; isfree < thou->capacity; isfree++, pos = thou->buffer[pos].next_item_ind)
        {
            if (pos >= thou->capacity) {
                PRINTF("[pos=%u] too big free index value", pos)
                continue;
            }
            if (thou->buffer[pos].next_item_ind == pos) break;
            if (thou->buffer[pos].prev_item_ind != -1)
                PRINTF("[pos=%u] prev of free is not -1", pos)

        }
    }
    else
    {
        isfree = 0;
    }
    if (isfree + occupied != thou->capacity)
        PRINTF("isfree + occupied != thou.capacity")

    if (!thou->sorted) goto end;

    if (thou->count == 0)
    {
        if (thou->left_edge != 0 || thou->right_edge != 0 || thou->free != 0)
        {
            PRINTF("buffer is not sorted")
        }
    }
    else {
        if (thou->left_edge != 0 || thou->right_edge != thou->count - 1 || thou->free != thou->count)
        {
            PRINTF("buffer is not sorted")
        }
        else if (thou->count > 1)
        {
            for (pos = 1; pos < thou->count - 1; pos++)
            {
                if (thou->buffer[pos].next_item_ind != pos + 1
                    || thou->buffer[pos].prev_item_ind != pos - 1)
                {
                    PRINTF("buffer is not sorted");
                    break;
                }
            }
            if (thou->buffer[0].next_item_ind != 1
                || thou->buffer[thou->count - 1].prev_item_ind != thou->count - 2)
                PRINTF("buffer is not sorted");
        }
    }


    end:

    if (error)
    {
        ListTextDump(*thou, log);
    }

    fclose(log);
    return;


    #undef PRINTF
}

void setasfree(List_t* thou, size_t start)
{
    MINI_ASSERTION_PACK

    for (size_t i = start; i < thou->capacity; i++)
    {
        thou->buffer[i].item = 0;
        thou->buffer[i].prev_item_ind = -1;
        thou->buffer[i].next_item_ind = i + 1;
    }
    thou->buffer[thou->capacity - 1].next_item_ind = thou->capacity - 1;
}

void swap(void* v1, void* v2, size_t bytecnt)
{
    register int* val1 = (int*)v1, *val2 = (int*)v2;
    register int temp = 0;
    while (bytecnt >= sizeof(int)) 
    {
        temp = *val1;
        *val1 = *val2;
        *val2 = temp;
        val1++;
        val2++;
        bytecnt -= sizeof(int);
    }
    register char* cval1 = (char*)val1, * cval2 = (char*)val2;
    register char ctemp = '\0';
    while (bytecnt--)
    {
        ctemp = *cval1;
        *cval1 = *cval2;
        *cval2 = ctemp;
        cval1++;
        cval2++;
    }
}

int ListFlush(List_t* thou)
{
    ASSERTION_PACK

    if (thou->capacity > MIN_CAPACITY)
    {
        void* newbuffer = realloc(thou->buffer, MIN_CAPACITY * sizeof(*thou->buffer));
        if (newbuffer == NULL) return ERR_NO_MEM;

        thou->buffer = (struct Item*)newbuffer;
        thou->capacity = MIN_CAPACITY;
    }

    setasfree(thou, 0);

    thou->count = 0;
    thou->free = 0;
    thou->left_edge = 0;
    thou->right_edge = 0;
    thou->sorted = 1;

    return 0;
}

int MakeList(List_t* thou, const char* name)
{
    assert(thou);
    
    if ((thou->buffer = (struct Item*) calloc(MIN_CAPACITY, sizeof(*thou->buffer))) == NULL)
         return ERR_NO_MEM;

    thou->capacity = MIN_CAPACITY;
    thou->name = name ? name : DEFAULT_NAME;

    setasfree(thou, 0);

    thou->count = 0;
    thou->free = 0;
    thou->left_edge = 0;
    thou->right_edge = 0;
    thou->sorted = 1;

    return 0;

}

int ListGraphDump(List_t thou) 
{
    assert(thou.buffer);
    assert(thou.count <= thou.capacity);
    assert(thou.capacity <= MAX_CAPACITY);

    #define graphvisfile "listgraph.dot"

    FILE* out;
    fopen_s(&out, graphvisfile, "w");
    if (out == NULL)
        return 1;
    fputs("digraph list {\n\trankdir = TB; \n", out);
    for (size_t i = 0; i < thou.capacity; i++) 
    {
        int is_free = thou.buffer[i].prev_item_ind == -1;
        fprintf(out, "\telem%u [shape=record,color=%s,style=filled,fillcolor=%s label=\"<val> %d | { prev: %d | <ind> ind : %u | next: %u }\" ];\n",
            i,
            is_free ? "blue" : "black",
            is_free ? 
                 i==thou.free ? "\"#0BA017\"" : "\"#41FE54\"" 
            : 
                 i == thou.left_edge || i == thou.right_edge ? "\"#820F7E\"" : "\"#FF7BFF\"",
            thou.buffer[i].item, 
            thou.buffer[i].prev_item_ind, 
            i, 
            thou.buffer[i].next_item_ind
        );

        int next_item_ind_correct = i == thou.right_edge 
            ? thou.buffer[i].next_item_ind == i
            : thou.buffer[thou.buffer[i].next_item_ind].prev_item_ind == i;
        if (i < thou.capacity - 1) fprintf(out, "\telem%u:<val> -> elem%u:<val>[color=white];\n", i, i + 1);
        fprintf(out, "\telem%u:<ind> -> elem%u:<ind>[color=%s];\n",
            i, thou.buffer[i].next_item_ind, next_item_ind_correct ? "\"#01740A\"" : is_free ? "\"#958D04\"" : "red");
    }
    fputc('}', out);
    fclose(out);

    system("Graphviz\\bin\\dot.exe -Tpng:cairo \"" graphvisfile "\" >\"res.png\"");
    system("res.png");

    #undef graphvisfile

    return 0;

}

void ListDistruct(List_t* thou) 
{
    ASSERTION_PACK

    free(thou->buffer);
    thou->buffer = NULL;

    thou->capacity = -1;
    thou->count = -1;
    thou->free = -1;
    thou->left_edge = -1;
    thou->right_edge = -1;
    thou->sorted = -1;
}

int Reallocate(void** buffer, size_t count, size_t elemsize, 
    size_t* capacity, size_t min_capacity, size_t max_capacity)
{
    assert(*capacity >= min_capacity);
    assert(buffer);
    assert(*buffer);
    assert(capacity);
    
    if (count == *capacity)
    {
        void* newbuffer = NULL;
        #define TRYCHANGE(oper)                                                 \
            if (*capacity oper <= max_capacity &&                               \
                 (newbuffer = realloc(*buffer, (*capacity oper) * elemsize)))   \
            {                                                                   \
                *capacity = *capacity oper;                                     \
                *buffer = newbuffer;                                            \
                return UP;                                                       \
            }

        TRYCHANGE(*2)
        TRYCHANGE(*3/2)
        TRYCHANGE(*5/4)
        TRYCHANGE(*7/6)
        TRYCHANGE(+10)
        TRYCHANGE(+3)

        #undef TRYCHANGE

        return EQUAL;
    }

    if (*capacity / 4 + *capacity / 8 >= count && *capacity > min_capacity)
    {
        *capacity = *capacity / 2 < min_capacity ? min_capacity : *capacity / 2;
        void* newbuffer = realloc(*buffer, *capacity * elemsize);
        if (newbuffer == NULL)
            return EQUAL;
        *buffer = newbuffer;
        return DOWN;
    }

    return EQUAL;
}

int OptimizeCapacity(List_t* thou)
{
    ASSERTION_PACK
    assert(thou->count == thou->capacity || thou->sorted);

    int ret = Reallocate ( (void**)&thou->buffer, thou->count,
                             sizeof(*thou->buffer),
                               &thou->capacity, MIN_CAPACITY, MAX_CAPACITY );

    if (ret == UP)
    {
        setasfree(thou, thou->count);
        thou->free = thou->count;
    }
    if (ret == DOWN)
    {
        thou->buffer[thou->capacity - 1].next_item_ind = thou->capacity - 1;
    }

    return ret;
}

int EnableFastIndexation(List_t* thou)
{
    ASSERTION_PACK

    size_t elems_met = 0;
    size_t pos = 0;

    for (pos = thou->left_edge; thou->buffer[pos].next_item_ind != pos; pos = thou->buffer[pos].next_item_ind, elems_met++)
        thou->buffer[pos].prev_item_ind = elems_met;
    thou->buffer[pos].prev_item_ind = elems_met;

    size_t vasya_pos = 0;
    while (1)
    {
        while ( vasya_pos < thou->capacity  && 
                    (  thou->buffer[vasya_pos].prev_item_ind == -1
                       || thou->buffer[vasya_pos].prev_item_ind == vasya_pos  ) )
        {
            vasya_pos++;
        }

        if (vasya_pos == thou->capacity) break;

        while (thou->buffer[vasya_pos].prev_item_ind != -1
            && thou->buffer[vasya_pos].prev_item_ind != vasya_pos)
        {
            swap(thou->buffer + vasya_pos,
                thou->buffer + thou->buffer[vasya_pos].prev_item_ind,
                8U /*поле next_item_ind содержит ненужную более информацию, которая ниже заменяется,
                   поэтому копируем только item и prev_item_ind*/
            );
        }
    }

    for (pos = 0; pos < thou->count; pos++)
    {
        thou->buffer[pos].prev_item_ind = pos - 1;
        thou->buffer[pos].next_item_ind = pos + 1;
    }
    thou->buffer[0].prev_item_ind = 0;
    thou->buffer[thou->count - 1].next_item_ind = thou->count - 1;

    for (; pos < thou->capacity; pos++)
    {
        thou->buffer[pos].next_item_ind = pos + 1;
    }
    thou->buffer[thou->capacity - 1].next_item_ind = thou->capacity - 1;

    thou->left_edge = 0;
    thou->right_edge = thou->count == 0 ? 0 : thou->count - 1;
    thou->free = thou->count == thou->capacity ? -1 : thou->count;
    thou->sorted = 1;

    OptimizeCapacity(thou);

    return 0;
}

size_t GetPhysInd(List_t* thou, size_t ind)
{
    ASSERTION_PACK

    if (ind >= thou->count) return ERR_INC_PARAM;
    
    if (thou->sorted)
        return ind;
    
    size_t pos;
    if (ind < thou->count / 2)
        for (pos = thou->left_edge; ind > 0; ind--, pos = thou->buffer[pos].next_item_ind);
    else
        for (pos = thou->right_edge; thou->count - ind > 1; ind++, pos = thou->buffer[pos].prev_item_ind);

    return pos;
}

int* GetByPhInd(List_t* thou, size_t ph_ind)
{
    ASSERTION_PACK

    if (ph_ind >= thou->capacity || thou->buffer[ph_ind].prev_item_ind == -1) return NULL;

    return (int*)(thou->buffer + ph_ind);
}

int* GetByInd(List_t* thou, size_t ind)
{
    size_t ph_ind = GetPhysInd(thou, ind);

    if (ph_ind == ERR_INC_PARAM) return NULL;

    return &(thou->buffer[ph_ind].item);
}

int* GetNextPtr(List_t* thou, int* elem)
{
    ASSERTION_PACK

    if ((struct Item*)elem > thou->buffer + (thou->capacity - 1) || (struct Item*)elem < thou->buffer) return NULL;
    if (((struct Item*)elem)->prev_item_ind == -1) return NULL;

    return (int*)(thou->buffer + ((struct Item*)elem)->next_item_ind);
}

size_t GetNext(List_t* thou, size_t ph_ind)
{
    ASSERTION_PACK

    if (ph_ind >= thou->capacity) return -1;
    if (thou->buffer[ph_ind].prev_item_ind == -1) return ERR_INC_PARAM;

    return thou->buffer[ph_ind].next_item_ind;
}

int* GetPrevPtr(List_t* thou, int* elem)
{
    ASSERTION_PACK

    if ((struct Item*)elem > thou->buffer + (thou->capacity - 1) || (struct Item*)elem < thou->buffer) return NULL;
    if (((struct Item*)elem)->prev_item_ind == -1) return NULL;

    return (int*)(thou->buffer + ((struct Item*)elem)->prev_item_ind);
}

size_t GetPrev(List_t* thou, size_t ph_ind)
{
    ASSERTION_PACK
    
    if (ph_ind >= thou->capacity) return ERR_INC_PARAM;
    if (thou->buffer[ph_ind].prev_item_ind == -1) return ERR_INC_PARAM;

    return thou->buffer[ph_ind].prev_item_ind;
}

size_t FindInd(List_t* thou, int val, int edge)
{
    ASSERTION_PACK
    assert(edge == 0 || edge == 1);

    size_t pos = (&thou->left_edge)[edge];
    size_t ind = 0;
    for (; 
        thou->buffer[pos].item != val && pos != (&thou->right_edge)[-edge];
        pos = (&(thou->buffer[pos].next_item_ind))[-edge], ind++
        );
    if (thou->buffer[pos].item != val) return NO_ENTRIES;
    return edge ? thou->count - 1 - ind : ind;
}

int* FindPtr(List_t* thou, int val, int edge)
{
    size_t ind = FindInd(thou, val, edge);
    if (ind == -1) return NULL;
    return (int*)(thou->buffer + ind);
}

size_t GetCount(List_t* thou)
{
    ASSERTION_PACK
    return thou->count;
}

int IfSorted(List_t* thou)
{
    ASSERTION_PACK
    return thou->sorted;
}













int RemoveFromEdge(List_t* thou, int* dest, int edge) 
{
    ASSERTION_PACK
    assert(edge == 0 || edge == 1);

    if (thou->count == 0) return ERR_LIST_EMPTY;

    if (dest) *dest = thou->buffer[(&thou->left_edge)[edge]].item;

    size_t remove_index = (&thou->left_edge)[edge];

    (&thou->left_edge)[edge] = (&(thou->buffer[remove_index].next_item_ind))[-edge];

    thou->buffer[remove_index].prev_item_ind = -1;
    thou->buffer[remove_index].item = 0;
    thou->buffer[remove_index].next_item_ind = thou->free;

    if (thou->count > 1) /*выполняется, если после удаления список непуст*/
        (&(thou->buffer[(&thou->left_edge)[edge]].prev_item_ind))[edge] = (&thou->left_edge)[edge];

    thou->free = remove_index;

    thou->count--;

    thou->sorted = thou->sorted & edge;

    return 0;
}

int RemoveFromStart(List_t* thou, int* dest) 
{
    ASSERTION_PACK

    return RemoveFromEdge(thou, dest, 0);
}

int RemoveFromEnd(List_t* thou, int* dest)
{
    int err = RemoveFromEdge(thou, dest, 1);
    if (err) return err;
    if (thou->sorted) OptimizeCapacity(thou);
    return 0;
}

int RemoveAtPos(List_t* thou, size_t ind, int* dest)
{
    ASSERTION_PACK

    if (ind >= thou->count) return ERR_INC_PARAM;
    
    if (thou->free == -1) return ERR_LIST_EMPTY;

    if (ind == 0) return RemoveFromStart(thou, dest);

    if (ind == thou->count - 1) return RemoveFromEnd(thou, dest);

    size_t pos = GetPhysInd(thou, ind);

    if (dest) *dest = thou->buffer[pos].item;
    thou->buffer[pos].item = 0;

    thou->buffer[thou->buffer[pos].next_item_ind].prev_item_ind = thou->buffer[pos].prev_item_ind;
    thou->buffer[thou->buffer[pos].prev_item_ind].next_item_ind = thou->buffer[pos].next_item_ind;

    thou->buffer[pos].prev_item_ind = -1;
    thou->buffer[pos].next_item_ind = thou->free;
    thou->free = pos;
    thou->count--;
    thou->sorted = 0;

    return 0;

}


















int AddToEdge(List_t* thou, int value, int edge)
{
    ASSERTION_PACK
    assert(edge == 0 || edge == 1);

    if (thou->free == -1) {
        if (OptimizeCapacity(thou) != UP)
        {
            return ERR_NO_MEM;
        }
    }

    size_t new_elem_ind = thou->free;

    thou->buffer[thou->free].item = value;

    thou->free = thou->buffer[thou->free].next_item_ind;
    (&(thou->buffer[new_elem_ind].next_item_ind))[-edge] = (&thou->left_edge)[edge];

    if (thou->free == new_elem_ind) thou->free = -1;

    (&(thou->buffer[new_elem_ind].prev_item_ind))[edge] = new_elem_ind;
    (&(thou->buffer[(&thou->left_edge)[edge]].prev_item_ind))[edge] = new_elem_ind;
    (&thou->left_edge)[edge] = new_elem_ind;

    thou->count++;
    thou->sorted = thou->sorted & edge;

    return 0;
}

int AddToStart(List_t* thou, int val)
{
    ASSERTION_PACK

    return AddToEdge(thou, val, 0);

}

int AddToEnd(List_t* thou, int val)
{
    return AddToEdge(thou, val, 1);
}

int AddToPos(List_t* thou, size_t ind, int val) 
{
    ASSERTION_PACK

    if (ind > thou->count) 
        return ERR_INC_PARAM;

    if (ind == 0) 
        return AddToStart(thou, val);

    if (ind == thou->count) 
        return AddToEnd(thou, val);

    if (thou->free == -1) {
        if (OptimizeCapacity(thou) != UP)
            return ERR_NO_MEM;
    }

    size_t new_elem_ind = thou->free;
    thou->free = thou->count + 1 == thou->capacity ? -1 : thou->buffer[thou->free].next_item_ind;

    size_t pos = GetPhysInd(thou, ind);

    thou->buffer[new_elem_ind].item = val;
    thou->buffer[new_elem_ind].next_item_ind = pos;
    thou->buffer[new_elem_ind].prev_item_ind = thou->buffer[pos].prev_item_ind;

    thou->buffer[thou->buffer[pos].prev_item_ind].next_item_ind = new_elem_ind;
    thou->buffer[pos].prev_item_ind = new_elem_ind;

    thou->count++;
    thou->sorted = 0;

    return 0;

}

void DebugFlush()
{
    FILE* fict;
    fopen_s(&fict, LOGFILE, "w");
    if (fict) fclose(fict);
}









int main()
{
    DebugFlush();
    List_t list = { NULL, 0xFFFFFFFF, 0xFFFFFFFF };

    MakeList(&list, "Victor");

    AddToEnd(&list, 0);
    AddToEnd(&list, 10);
    AddToEnd(&list, 15);
    AddToEnd(&list, 20);
    AddToEnd(&list, 30);
    AddToEnd(&list, 40);
    RemoveAtPos(&list, 2, NULL);
    AddToEnd(&list, 50);

    int k;
    /*int* l;
    int* ptr = GetByInd(&list, 0);;
    do
    {
        printf("%d ", *ptr);
        l = ptr;
    } while ((ptr = GetNextPtr(&list, ptr)) != l);*/
    ListTextDump(list, stdout);
    for (k = 0; k < 70; k += 10) 
    {
        printf(",%d %u", k, FindInd(&list, k, 1));
    }
    EnableFastIndexation(&list);
    ListGraphDump(list);
    ListDistruct(&list);
    return 0;
}
