
#ifndef __MSGPORT_H__
#define __MSGPORT_H__

#include "type.h"
#include "exec.h"

MessagePort_t* CreatePort(const char* name);

MessagePort_t* CreatePortForTask(Task_t* task, const char* name);

MessagePort_t* FindPort(const char *name);

void RemovePort(MessagePort_t* port);

Message_t* AllocMessage(uint32 length);

void* GetUserData(Message_t* message);

void SetUserData(Message_t* msg, void* data, uint32 length);

void FreeMessage(Message_t* message);

Message_t* WaitMessage(MessagePort_t* port);

Message_t* GetMessage(MessagePort_t* port);

void SendMessage(MessagePort_t* port, Message_t* message);

void ReplyMessage(Message_t* message);

#endif //__MSGPORT_H__

