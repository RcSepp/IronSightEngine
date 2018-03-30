#include "AsyncWorkers.h"

//-----------------------------------------------------------------------------
// Name: CreateID3Lib()
// Desc: DLL API for creating an instance of ID3Lib
//-----------------------------------------------------------------------------
ASYNCWORKERS_EXTERN_FUNC LPASYNCWORKERS __cdecl CreateAsyncWorkers()
{
	return new AsyncWorkers();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void AsyncWorkers::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize AsyncWorkers
//-----------------------------------------------------------------------------
Result AsyncWorkers::Init(const CREATEAWSPROCESS_CALLBACK creator, LPVOID user, const LPAWSJOB firstjob, DWORD& numworkers)
{
	// Get number of available processor cores
	SYSTEM_INFO sysinfo;
	BOOL iswow64;
	IsWow64Process(GetCurrentProcess(), &iswow64);
	if(iswow64)
		GetNativeSystemInfo(&sysinfo);
	else
		GetSystemInfo(&sysinfo);

	// Restrict numworkers to a maximum equivalent to the number of available processor cores
	this->numworkers = numworkers = min(numworkers, sysinfo.dwNumberOfProcessors);

	// Create process queue mutex
	queuemutex = CreateMutex(NULL, FALSE, NULL);
	if(!queuemutex)
		return ERR("Error creating mutex");

	// Creat event to enable/disable all workers
	procenable = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!procenable)
		return ERR("Error creating processor enabled event");

	// Create workers
	CHKALLOC(workers = new Worker[this->numworkers]);
	for(DWORD i = 0; i < this->numworkers; i++)
	{
		// Create threads
		workers[i].thread = CreateThread(NULL, 0, WorkerThread, &workers[i], 0, NULL);
		if(!workers[i].thread)
			return ERR("Error creating worker thread");

		// Assign processor cores
		SetThreadIdealProcessor(workers[i].thread, i);

		// Create event to start/stop one specific worker thread
		workers[i].stoppedevent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if(!workers[i].stoppedevent)
			return ERR("Error creating worker stopped event");
	}

	// Create process queue
	processqueue = new MiniQueue<Process*, DWORD>(this->numworkers);

	// Create process spawner
	processspawner.creator = creator;
	processspawner.user = user;
	processspawner.firstjob = (AwsJob*)firstjob;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateJob()
// Desc: Register job callback and set options
//-----------------------------------------------------------------------------
Result AsyncWorkers::CreateJob(AWSJOB_CALLBACK func, bool ismutex, IAwsJob** job)
{
	Result rlt;

	*job = NULL;

	AwsJob* newjob;
	CHKALLOC(newjob = new AwsJob(func));
	if(ismutex)
		CHKRESULT(newjob->SetMutex());

	jobs.push_back(newjob);
	*job = newjob;

	return R_OK;
}

Result AsyncWorkers::CreateCounter(IAwsJob** masters, DWORD nummasters, IAwsJob** slaves, DWORD numslaves)
{
	if(!nummasters || !numslaves)
		return R_OK;
	AwsJob** m = (AwsJob**)masters;
	AwsJob** s = (AwsJob**)slaves;
	Result rlt;

	// Create counters
	size_t* mastercounter = new size_t();
	size_t* slavecounter = new size_t();

	// Assign counters and enable mutex's
	for(DWORD i = 0; i < nummasters; i++)
	{
		m[i]->mycounter = mastercounter;
		m[i]->slavecounter = slavecounter;
		if(!m[i]->mutex)
			CHKRESULT(m[i]->SetMutex());
	}
	for(DWORD i = 0; i < numslaves; i++)
	{
		s[i]->mycounter = slavecounter;
		s[i]->mastercounter = mastercounter;
		if(!s[i]->mutex)
			CHKRESULT(s[i]->SetMutex());
	}

	return R_OK;
}

DWORD __stdcall AsyncWorkers::WorkerThread(LPVOID args)
{
	Worker* worker = (Worker*)args;
	AsyncWorkers* parent = reinterpret_cast<AsyncWorkers*>(&*aws);
	AwsJob* nextjob;
	bool jobisdone;
	Result rlt;

	while(parent->state != APS_CLOSING)
	{
		WaitForSingleObject(parent->procenable, INFINITE);
		switch(parent->state)
		{
		case APS_STOPPED:
			SetEvent(worker->stoppedevent);
			break;

		case APS_IDLE:
			ResetEvent(parent->procenable);
			break;

		case APS_RUNNING:
			// Get processor from queue or create new one
			WaitForSingleObject(parent->queuemutex, INFINITE);
DWORD s = parent->processqueue->size();
Process* q0 = parent->processqueue->queue[0];
Process* q1 = parent->processqueue->queue[1];
Process* q2 = parent->processqueue->queue[2];
Process* q3 = parent->processqueue->queue[3];
Process* q4 = parent->processqueue->queue[4];
			Process* p = parent->processqueue->pop();
			if(!p)
				p = new Process(parent->processspawner);
			ReleaseMutex(parent->queuemutex);

			// Carry out job
			if(p->currentjob->mutex)
				WaitForSingleObject(p->currentjob->mutex, INFINITE); // EDIT: Let worker do something besides waiting
			if((!p->currentjob->mastercounter || *p->currentjob->mastercounter == *p->currentjob->mycounter + 1) &&
			   (!p->currentjob->slavecounter || *p->currentjob->slavecounter == *p->currentjob->mycounter))
			{
				jobisdone = true;
				nextjob = (AwsJob*)p->currentjob->func(p->cls, &parent->state, &jobisdone);
				if(jobisdone && p->currentjob->mycounter)
					(*p->currentjob->mycounter)++;
				if(p->currentjob->mutex)
					ReleaseMutex(p->currentjob->mutex);
				p->currentjob = nextjob;
			}
			else
				if(p->currentjob->mutex)
					ReleaseMutex(p->currentjob->mutex);

			if(p->currentjob == JOB_FINISH)
				// Free finished process
				delete p;
			else
			{
				// Queue process for further processing
				WaitForSingleObject(parent->queuemutex, INFINITE);
				parent->processqueue->push(p);
				ReleaseMutex(parent->queuemutex);
			}
		}
	}

	return 0;
}

void AsyncWorkers::Start()
{
	if(state == APS_RUNNING)
		return;

	state = APS_RUNNING;
	SetEvent(procenable);
}

void AsyncWorkers::Stop()
{
	if(state == APS_STOPPED)
		return;

	for(DWORD i = 0; i < numworkers; i++)
		ResetEvent(workers[i].stoppedevent);
	state = APS_STOPPED;
	SetEvent(procenable);
	for(DWORD i = 0; i < numworkers; i++)
		WaitForSingleObject(workers[i].stoppedevent, INFINITE);
	ResetEvent(procenable);
}

void AsyncWorkers::ResetCounters() //EDIT: Rename
{
	// Reset counters
	for(std::list<AwsJob*>::iterator iter = jobs.begin(); iter != jobs.end(); iter++)
	{
		if((*iter)->mycounter)
			*(*iter)->mycounter = 0;
		if((*iter)->mastercounter)
			*(*iter)->mastercounter = 0;
		if((*iter)->slavecounter)
			*(*iter)->slavecounter = 0;
	}

	// Terminate pending processes
	Process* p;
	while((p = processqueue->pop()) != NULL)
		delete p;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void AsyncWorkers::Release()
{
	// Exit worker threads and remove workers
	if(procenable)
	{
		state = APS_CLOSING;
		SetEvent(procenable);
		for(DWORD i = 0; i < numworkers; i++)
		{
			WaitForSingleObject(workers[i].thread, INFINITE);
			CloseHandle(workers[i].thread);
			CloseHandle(workers[i].stoppedevent);
		}
		CloseHandle(procenable);
	}
	REMOVE_ARRAY(workers);

	// Remove process queue
	REMOVE(processqueue);

	// Remove mutex's
	if(queuemutex)
		CloseHandle(queuemutex);

	// Remove jobs
	for(std::list<AwsJob*>::iterator iter = jobs.begin(); iter != jobs.end(); iter++)
		delete *iter;
	jobs.clear();

	delete this;
}

Result AwsJob::SetMutex()
{
	// Create job mutex
	mutex = CreateMutex(NULL, FALSE, NULL);
	return mutex ? R_OK : ERR("Error job mutex");
}