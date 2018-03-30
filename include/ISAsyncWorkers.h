#ifndef __ISASYNCWORKERS_H
#define __ISASYNCWORKERS_H

#include <ISEngine.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_ASYNCWORKERS
	#define ASYNCWORKERS_EXTERN_FUNC		__declspec(dllexport)
#else
	#define ASYNCWORKERS_EXTERN_FUNC		__declspec(dllimport)
#endif
#define JOB_FINISH				NULL // Marks the end of a process


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum AwsProcState
	{APS_RUNNING, APS_STOPPED, APS_IDLE, APS_CLOSING};


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
struct IAwsProcess;
class IAwsJob;
typedef IAwsProcess* (__stdcall* CREATEAWSPROCESS_CALLBACK)();
typedef IAwsJob* (__stdcall* AWSJOB_CALLBACK)(IAwsProcess* process, AwsProcState* state, bool* isdone);


//-----------------------------------------------------------------------------
// Name: IAwsProcess
// Desc: Interface to user defined process ("job chain") structure.
//		 Contains user defined variables, as well as create and release function prototypes for initiating and freeing those variables.
//-----------------------------------------------------------------------------
typedef struct IAwsProcess
{
	virtual void Create(LPVOID user) = 0;
	virtual void Release() = 0;
}* LPAWSPROCESS;

//-----------------------------------------------------------------------------
// Name: IAwsJob
// Desc: Interface to the AwsJob class
//-----------------------------------------------------------------------------
typedef class IAwsJob
{
}* LPAWSJOB;

//-----------------------------------------------------------------------------
// Name: IAsyncWorkers
// Desc: Interface to the AsyncWorkers class
//-----------------------------------------------------------------------------
typedef class IAsyncWorkers
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init(const CREATEAWSPROCESS_CALLBACK creator, LPVOID user, const LPAWSJOB firstjob, DWORD& numworkers) = 0;
	virtual Result CreateJob(AWSJOB_CALLBACK func, bool ismutex, IAwsJob** job) = 0;
	virtual Result CreateCounter(IAwsJob** masters, DWORD nummasters, IAwsJob** slaves, DWORD numslaves) = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void ResetCounters() = 0;
	virtual void Release() = 0;
}* LPASYNCWORKERS;

extern "C" ASYNCWORKERS_EXTERN_FUNC LPASYNCWORKERS __cdecl CreateAsyncWorkers();

#endif