
#ifndef __EXEC_MSGPORT_H__
#define __EXEC_MSGPORT_H__


/*! \brief Facilitates passing of messages between tasks.
 *
 * A Message Port is the core of the IPC mechanism used for communication
 * between tasks.
 *
 * When a message is received by a port the task will be signalled. If the
 * task is sleeping it will be made runnable and can process the queued 
 * messages.
 */
typedef struct {
	Node_t n;			
	SignalBit_t signal;	//!< The signal associated with this message port.
	Task_t* owner;		//!< The task that owns this message port.
	List_t messages;	//!< This list of messages queued for processing.
} MessagePort_t;

/*! \brief A message to be passed to a message port.
 *
 * Messages are objects that are passed between tasks. The message includes
 * a length parameter indicating the number of bytes of data following the
 * message structure. No copying of data takes place.
 *
 */
typedef struct {
	Node_t n;
	MessagePort_t* destPort;	//!< The port the message will be sent to.
	MessagePort_t* replyPort;	//!< The port to send a reply to. 
	uint32 length;				//!< The number of bytes of data following the message structure.
} Message_t;

#endif // __EXEC_MSGPORT_H__

