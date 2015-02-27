
#include "exec.h"
#include "kernel.h"
#include "library.h"
#include "mem.h"
#include "msgport.h"
#include "signal.h"
#include "type.h"
#include "device.h"

IExec_t* IExec = (IExec_t*)0x4000;

void InitExecLibrary(void) {

	IExec_t* iface = IExec;

	Interface_t* defIface = (Interface_t*)iface;
	defIface->Open = NULL;
	defIface->Close = NULL;
	defIface->Init = NULL;
	defIface->Expunge = NULL;

	iface->AllocMem = AllocMem;
	iface->FreeMem = FreeMem;

	iface->AllocSignal = AllocSignal;
	iface->DeallocSignal = DeallocSignal;
	iface->Signal = Signal;
	iface->GetSignals = GetSignals;
	iface->SetSignalMask = SetSignalMask;
	iface->GetSignalMask = GetSignalMask;
	iface->Wait = Wait;

	iface->CreatePort = CreatePort;
	iface->FindPort = FindPort;
	iface->RemovePort = RemovePort;
	iface->AllocMessage = AllocMessage;
	iface->GetUserData = GetUserData;
	iface->SetUserData = SetUserData;
	iface->FreeMessage = FreeMessage;
	iface->WaitMessage = WaitMessage;
	iface->GetMessage = GetMessage;
	iface->SendMessage = SendMessage;
	iface->ReplyMessage = ReplyMessage;

	iface->CreateLibrary = CreateLibrary;
	iface->AddLibrary = AddLibrary;
	iface->OpenLibrary = OpenLibrary;
	iface->CloseLibrary = CloseLibrary;

	iface->CreateTask = CreateTask;
	iface->FindTask = FindTask;
	iface->StartTask = StartTask;

	iface->CreateDevice = CreateDevice;
	iface->CreateIORequest = CreateIORequest;
	iface->DeleteIORequest = DeleteIORequest;
	iface->OpenDevice = OpenDevice;
	iface->CloseDevice = CloseDevice;
	iface->DoIO = DoIO;
	iface->SendIO = SendIO;
	iface->CheckIO = CheckIO;
	iface->WaitIO = WaitIO;
	iface->AbortIO = AbortIO;
}

