
#ifndef __EXEC_LIST_H__
#define __EXEC_LIST_H__

/*! \brief Enumeration of list node types. 
 *
 * These values indicate the type of node contained by a list.
 */
typedef enum {
	NT_MEMORY, 	//!< A node in a memory list.
	NT_MSGPORT, //!< A message port.
	NT_MESSAGE, //!< A message.
	NT_TASK,	//!< A task.
	NT_TIMER,	//!< A timer.
	NT_IRQ,		//!< An IRQ handler.
	NT_LIBRARY,	//!< A library.
	NT_DEVICE	//!< A device.
} NodeType_t;

/*! \brief Flags controlling list behaviour.
 *
 */
typedef enum {
	LF_NONE = 0, 	//!< None.
	LF_SORTED = 0x1	//!< The list is sorted by node priority.
} ListFlag_t;

typedef struct Node_t Node_t;

/*! \brief A node in a doubly linked list.
 *
 * Lists form the basis for most structures used by exec. Most structs include
 * a node as a member so they can be included in a list.
 *
 */
struct Node_t {
	Node_t* prev; 		//!< The previous node in the list.
	Node_t* next; 		//!< The next node in the list.
	int8 priority;		//!< The priority of the node, used for sorting lists.
	const char* name;	//!< The name of the node, used for finding nodes.
};

/*! \brief A doubly linked list.
 *
 * Exec uses this type to manage collections of data. 
 *
 * Depending on the flags a List_t could be used to implement a number of 
 * different algorithms such as lists, queues and stacks.
 *
 */
typedef struct List_t {
	Node_t* head;		//!< The first node in the list.
	Node_t* tail;		//!< The last node in the list.
	NodeType_t type;	//!< The type of nodes in the list.
	ListFlag_t flags;	//!< Flags controlling the behaviour of the list.
} List_t;

#endif // __EXEC_LIST_H__

