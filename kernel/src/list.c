
#include "mem.h"
#include "list.h"
#include "string.h"

List_t* NewList(NodeType_t type, ListFlag_t flags) {

	List_t* list = (List_t*)AllocMem(sizeof(List_t));
	if( list != NULL ) {
		InitList(list, type, flags);
	}
	return list;
}

void InitList(List_t* list, NodeType_t type, ListFlag_t flags) {

	list->head = NULL;
	list->tail = NULL;
	list->type = type;
	list->flags = flags;
}

Node_t* NewNode(int8 priority, const char *name) {

	Node_t* node = (Node_t*)AllocMem(sizeof(Node_t));
	if( node != NULL ) {
		InitNode(node, priority, name);
	}
	return node;
}

void InitNode(Node_t* node, int8 priority, const char *name) {
	
	node->prev = NULL;
	node->next = NULL;
	node->priority = priority;
	node->name = name;
}

void Insert(List_t *list, Node_t* node, Node_t* next) {
	
	if( list->head == NULL ) {
		list->head = node;
		list->tail = node;
	} 
	else if( list->flags & LF_SORTED ) {

		Node_t* curr = list->head;
		Node_t* prev = NULL;
		while( curr != NULL && curr->priority <= node->priority ) {
			prev = curr;
			curr = curr->next;
		}

		node->prev = prev;
		node->next = curr;
		if( prev == NULL ) {
			list->head = node;
		} else {
			prev->next = node;
		}
		if( curr != NULL ) {
			curr->prev = node;
		}
	} 
	else if( next == NULL ) {
		node->prev = list->tail;
	} else {
		node->prev = next->prev;
		node->next = next;
	}

	if( node->prev == NULL ) {
		list->head = node;
	} else {
		node->prev->next = node;
	}

	if( node->next == NULL ) {
		list->tail = node;
	} else {
		node->next->prev = node;
	}
}

void RemoveNode(List_t* list, Node_t* node) {

	if( node->prev == NULL ) {
		list->head = node->next;
	} else {
		node->prev->next = node->next;
	}

	if( node->next == NULL ) {
		list->tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}
}

Node_t* RemoveHead(List_t* list) {
	
	Node_t* head = list->head;

	RemoveNode(list, head);

	return head;
}

Node_t* RemoveTail(List_t* list) {

	Node_t* tail = list->tail;

	RemoveNode(list, tail);

	return tail;
}

Node_t* FindNode(List_t* list, const char* name) {

	Node_t* curr = list->head;
	while( curr != NULL ) {
		if( Compare(curr->name, name) == 0 ) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

