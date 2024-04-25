#include <stdio.h>
#include <stdlib.h>

typedef struct //Data_Struc that use of each process
{
    int ID;
    int ArrTime;
    int RunTime;
    int Priority;
    int PID;
    int RemaingTime;
    int WatingTime;
    int Stoped;
    int Running;
    int memsize;
    int memindx;
} process;

typedef struct
{
    int priority;
    process data;
} node_t;

typedef struct
{
    node_t *nodes;
    int len;
    int size;
} heap_t;

void push(heap_t *h, int priority, process *data)
{
    if (h->len + 1 >= h->size)
    {
        h->size = h->size ? h->size * 2 : 4;
        h->nodes = (node_t *)realloc(h->nodes, h->size * sizeof(node_t));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority)
    {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority = priority;
    h->nodes[i].data.ID = data->ID;
    h->nodes[i].data.ArrTime = data->ArrTime;
    h->nodes[i].data.RunTime = data->RunTime;
    h->nodes[i].data.Priority = data->Priority;
    h->nodes[i].data.PID = data->PID;
    h->nodes[i].data.WatingTime = data->WatingTime;
    h->nodes[i].data.RemaingTime = data->RemaingTime;
    h->nodes[i].data.Stoped = data->Stoped;
    h->nodes[i].data.memsize = data->memsize;
    h->nodes[i].data.memindx = data->memindx;
    h->len++;
}

process *pop(heap_t *h)
{
    int i, j, k;
    if (!h->len)
    {
        return NULL;
    }
    process *pdata = (process*)malloc(sizeof(process));
    pdata->ID = h->nodes[1].data.ID;
    pdata->ArrTime = h->nodes[1].data.ArrTime;
    pdata->RunTime = h->nodes[1].data.RunTime;
    pdata->Priority = h->nodes[1].data.Priority;
    pdata->PID = h->nodes[1].data.PID;
    pdata->WatingTime = h->nodes[1].data.WatingTime;
    pdata->RemaingTime  = h->nodes[1].data.RemaingTime;
    pdata->Stoped  = h->nodes[1].data.Stoped;
    pdata->memsize  = h->nodes[1].data.memsize;
    pdata->memindx  = h->nodes[1].data.memindx;
    h->nodes[1] = h->nodes[h->len];

    h->len--;

    i = 1;
    while (i != h->len + 1)
    {
        k = h->len + 1;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority)
        {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority)
        {
            k = j + 1;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    return pdata;
}

process *top(heap_t *h)
{
    if (h->len <= 0)
    {
        return NULL;
    }
    return &h->nodes[1].data;
}