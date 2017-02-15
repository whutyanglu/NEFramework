#include "stdafx.h"
#include "closure.h"
#include "shared/threads.h"
#include "base/thread/thread_manager.h"
#include "cef/main_message_loop.h"

class TaskWrapper : public CefTask
{
public:
	TaskWrapper(const StdClosure & closure)
		:closure_(closure)
	{}

	TaskWrapper(StdClosure && closure)
		:closure_(std::move(closure))
	{}

	virtual void Execute() override
	{
		if (closure_) {
			closure_();
		}
	}

	IMPLEMENT_REFCOUNTING(TaskWrapper)

private:
	StdClosure closure_;
};

void Post2UI(const base::Closure& closure)
{
	MAIN_POST_CLOSURE(closure);
}

void Post2UI(const StdClosure & closure)
{
	CefRefPtr<TaskWrapper> task(new TaskWrapper(closure));
	MAIN_POST_TASK(task);
}

void Post2GlobalMisc(const StdClosure &closure)
{
	nbase::ThreadManager::PostTask(kThreadGlobalMisc, closure);
}

void Post2Database(const StdClosure &closure)
{
	nbase::ThreadManager::PostTask(kThreadDatabase, closure);
}

void Post2Curl(const StdClosure & closure)
{
	nbase::ThreadManager::PostTask(kThreadCurl, closure);
}
