#ifndef __ASYNCWORKERS_H
#define __ASYNCWORKERS_H

#include "ISAsyncWorkers.h"


//-----------------------------------------------------------------------------
// Name: AwsJob
// Desc: Contains options and callback for a job
//-----------------------------------------------------------------------------
class AwsJob : public IAwsJob
{
public:
	const AWSJOB_CALLBACK func;
	HANDLE mutex;
	size_t *mycounter, *mastercounter, *slavecounter;

	AwsJob(const AWSJOB_CALLBACK func) : func(func), mycounter(NULL), mastercounter(NULL), slavecounter(NULL), mutex(NULL) {}
	~AwsJob()
	{
		if(mutex)
			CloseHandle(mutex);
	}
	Result SetMutex();
};

//-----------------------------------------------------------------------------
// Name: AsyncWorkers
// Desc: Management class for all workers
//-----------------------------------------------------------------------------
class AsyncWorkers : public IAsyncWorkers
{
private:
	AwsProcState state;
	struct Worker
	{
		HANDLE thread, stoppedevent;
	}* workers;
	DWORD numworkers;
	struct ProcessSpawner
	{
		CREATEAWSPROCESS_CALLBACK creator;
		AwsJob* firstjob;
		LPVOID user;
	} processspawner;
	struct Process
	{
		IAwsProcess* cls;
		AwsJob* currentjob;

		Process(const ProcessSpawner& spawner) : cls(spawner.creator()), currentjob(spawner.firstjob)
		{
			cls->Create(spawner.user);
		}
		~Process()
		{
			cls->Release();
		}
	};
	MiniQueue<Process*, DWORD>* processqueue;
	HANDLE procenable;
	HANDLE queuemutex;
	std::list<AwsJob*> jobs;

	static DWORD __stdcall WorkerThread(LPVOID args);

public:

	AsyncWorkers()
	{
		state = APS_IDLE;
		workers = NULL;
		numworkers = 0;
		processqueue = NULL;
		queuemutex = NULL;
		procenable = NULL;
	}

	void Sync(GLOBALVARDEF_LIST);
	Result Init(const CREATEAWSPROCESS_CALLBACK creator, LPVOID user, const LPAWSJOB firstjob, DWORD& numworkers);
	Result CreateJob(AWSJOB_CALLBACK func, bool ismutex, IAwsJob** job);
	Result CreateCounter(IAwsJob** masters, DWORD nummasters, IAwsJob** slaves, DWORD numslaves);
	void Start();
	void Stop();
	void ResetCounters();
	void Release();
};

#endif