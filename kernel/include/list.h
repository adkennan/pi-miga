#ifndef __LIST_H__
#define __LIST_H__

#include "type.h"
#include "exec.h"

// Initializes a new List structure.
List_t* NewList(NodeType_t type, ListFlag_t flags);

void InitList(List_t* list, NodeType_t type, ListFlag_t flags);

Node_t* NewNode(int8 priority, const char* name);

void InitNode(Node_t* node, int8 priority, const char* name);

// Inserts a new node into the list before next. 
// If next is NULL node is appended to the end of the
// list.
// If the list flags include LF_SORTED the new node
// will be inserted according to its priority.
void Insert(List_t* list, Node_t* node, Node_t* next);

// Removes the node from the list. The caller is 
// responsible for freeing the memory it used.
void RemoveNode(List_t* list, Node_t* node);

// Returns the node at the head of the list and 
// removes it from the list. The caller is responsible
// for freeing the memory it used.
Node_t* RemoveHead(List_t* list);

// Returns the node at the tail of the list and
// removes it from the list. The caller is responsible
// for freeing the memory it used.
Node_t* RemoveTail(List_t* list);

// Finds a node by name.
Node_t* FindNode(List_t* list, const char* name);

#endif // __LIST_H__
