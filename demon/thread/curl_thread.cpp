#include "StdAfx.h"
#include "curl_thread.h"


CurlThread::CurlThread(ThreadId thread_id, const char* name)
	: FrameworkThread(name), thread_id_(thread_id)
{

}

CurlThread::~CurlThread(void)
{

}

void CurlThread::Init()
{
	nbase::ThreadManager::RegisterThread(thread_id_);
}

void CurlThread::Cleanup()
{
	nbase::ThreadManager::UnregisterThread();

	QLOG_APP(L"CurlThread Cleanup");
}