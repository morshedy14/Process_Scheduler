/* double linked list */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pq.h"

struct List
{
   struct MNode *head;
   struct MNode *tail;
   struct MNode *tail_pred;
};

struct MNode
{
   process data;
   struct MNode *succ;
   struct MNode *pred;
};

typedef struct MNode *NODE;
typedef struct List *LIST;

/*
** LIST l = newList()
** create (alloc space for) and initialize a list
*/
LIST newList(void);

/*
** int isEmpty(LIST l)
** test if a list is empty
*/
int isEmpty(LIST);

/*
** NODE n = getTail(LIST l)
** get the tail node of the list, without removing it
*/
NODE getTail(LIST);

/*
** NODE n = getHead(LIST l)
** get the head node of the list, without removing it
*/
NODE getHead(LIST);

/*
** NODE rn = addTail(LIST l, NODE n)
** add the node n to the tail of the list l, and return it (rn==n)
*/
void addTail(LIST, process);

/*
** NODE rn = addHead(LIST l, NODE n)
** add the node n to the head of the list l, and return it (rn==n)
*/
void addHead(LIST, process);

/*
** NODE n = remHead(LIST l)
** remove the head node of the list and return it
*/
NODE remHead(LIST);

/*
** NODE n = remTail(LIST l)
** remove the tail node of the list and return it
*/
NODE remTail(LIST);

/*
** NODE rn = insertAfter(LIST l, NODE r, NODE n)
** insert the node n after the node r, in the list l; return n (rn==n)
*/
NODE insertAfter(LIST, NODE, NODE);

/*
** NODE rn = removeNode(LIST l, NODE n)
** remove the node n (that must be in the list l) from the list and return it (rn==n)
*/
NODE removeNode(LIST, NODE);

LIST newList(void)
{
   LIST tl = (LIST)malloc(sizeof(struct List));
   if (tl != NULL)
   {
      tl->tail_pred = (NODE)&tl->head;
      tl->tail = NULL;
      tl->head = (NODE)&tl->tail;
      return tl;
   }
   return NULL;
}

int isEmpty(LIST l)
{
   return (l->head->succ == 0);
}

NODE getHead(LIST l)
{
   return l->head;
}

NODE getTail(LIST l)
{
   return l->tail;
}

void addTail(LIST l, process n)
{
   //  n->succ = (NODE)&l->tail;
   struct MNode *node = (NODE)malloc(sizeof(struct MNode));
   node->data.ID = n.ID;
   node->data.ArrTime = n.ArrTime;
   node->data.PID = n.PID;
   node->data.Priority = n.Priority;
   node->data.RemaingTime = n.RemaingTime;
   node->data.RunTime = n.RunTime;
   node->data.Stoped = n.Stoped;
   node->data.WatingTime = n.WatingTime;
   node->data.ID = n.ID;
   node->data.memsize =n.memsize;
   node->data.memindx =n.memindx;
   if (l->tail == NULL)
   {
      l->head = node;
      l->tail = node;
      l->tail->pred = NULL;
      l->tail->succ = NULL;
   }
   else
   {
      NODE P = l->tail;
      node->pred = l->tail;
      P->succ = node;
      l->tail = node;
      node->succ = NULL;
   }
}

void addHead(LIST l, process n)
{
   struct MNode *node = (NODE)malloc(sizeof(struct MNode));
   node->data.ID = n.ID;
   node->data.ArrTime = n.ArrTime;
   node->data.PID = n.PID;
   node->data.Priority = n.Priority;
   node->data.RemaingTime = n.RemaingTime;
   node->data.RunTime = n.RunTime;
   node->data.Stoped = n.Stoped;
   node->data.WatingTime = n.WatingTime;
   node->data.ID = n.ID;
   node->data.memsize = n.memsize;
   node->data.memindx = n.memindx;

   if (isEmpty(l))
   {
      l->head = node;
      l->tail = node;
   }

   else
   {
      NODE P = l->head;
      node->succ = P;
      P->pred = node;
      l->head = node;
   }
}

NODE remHead(LIST l)
{
   NODE h;
   h = l->head;
   l->head = l->head->succ;
   l->head->pred = (NODE)&l->head;
   return h;
}

NODE remTail(LIST l)
{
   NODE t;
   t = l->tail_pred;
   l->tail_pred = l->tail_pred->pred;
   l->tail_pred->succ = (NODE)&l->tail;
   return t;
}

NODE insertAfter(LIST l, NODE r, NODE n)
{
   n->pred = r;
   n->succ = r->succ;
   n->succ->pred = n;
   r->succ = n;
   return n;
}

NODE removeNode(LIST l, NODE n)
{
   if (n->pred == NULL && n->succ == NULL)
   {
      l->head = NULL;
      l->tail = NULL;
      free(n);
      return NULL;
   }
   else if (n->succ == NULL)
   {
      l->tail = n->pred;
      n->pred = NULL;
      l->tail->succ = NULL;
      free(n);
      return l->tail;
   }
   else if (n->pred == NULL)
   {
      l->head = n->succ;
      l->head->pred = NULL;
      n->succ = NULL;
      free(n);
      return l->head;
   }
   else
   {
      n->succ->pred = n->pred;
      n->pred->succ = n->succ;
      NODE P = n->succ;
      free(n);
      return P;
   }
}