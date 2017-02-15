#pragma once
//#include "util.h"
#include "base/callback/callback.h"
#include "cef/main_message_loop.h"

//投递任务到UI线程
void Post2UI(const base::Closure& closure);

void Post2UI(const StdClosure& closure);

//投递任务到全局Misc线程
void Post2GlobalMisc(const StdClosure &closure);

//投递任务到DB线程
void Post2Database(const StdClosure &closure);

//投递任务到CURL线程
void Post2Curl(const StdClosure &closure);