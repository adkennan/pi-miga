
#ifndef __EXEC_DEVICE_H__
#define __EXEC_DEVICE_H__

/** \brief Describes a device
 *
 * A device represents an abstraction of a resource available to the system.
 *
 */
typedef struct {
	Node_t n;
	Task_t* task;	//!< The task providing the device interface.
} Device_t;

/** \brief Describes a device unit
 *
 *
 */
typedef struct {
	MessagePort_t port;   //!< Queue for unprocessed messages.
	uint32 unitNum;	  //!< Unit number.
	uint32 openCount; //!< Count of opens of this unit.
} Unit_t;


/** \brief Standard device commands.
 *
 * These are the standard commands devices are expected to respond to.
 *
 */
typedef enum {
	CMD_INVALID, //!< Invalid command.
	CMD_OPEN,	 //!< Opens the device.
	CMD_CLOSE,	 //!< Closes the device.
	CMD_READ,	 //!< Performs a read from the device.
	CMD_WRITE,	 //!< Performs a write to the device.
	CMD_ABORT,	 //!< Attempt to abort an IO operation.
	CMD_CUSTOM	 //!< A custom IO command.
} IOCommand_t;

/** \brief Base for device IO requests.
 *
 * 
 *
 */
typedef struct {
	Message_t m;
	Device_t* device;
	Unit_t* unit;
	uint32 command;
	uint32 flags;
	uint32 error;
} IORequest_t;

/** \brief A standard IO request.
 *
 *
 *
 */
typedef struct {
	IORequest_t r;
	uint8* data;	//!< Pointer to the data buffer.
	uint32 length;	//!< The length of the buffer.
	uint32 offset;	//!< An offset within the buffer to read or write from.
	uint32 actual;	//!< The actual number of bytes read or written.
} IOStdReq_t;

#endif // __EXEC_DEVICE_H__

