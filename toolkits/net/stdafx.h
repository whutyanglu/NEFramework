// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <string>
#include <memory>

#include "base/callback/callback.h"
#include "base/util/string_util.h"
#include "base/util/unicode.h"
#include "base/util/string_number_conversions.h"
#include "base/memory/deleter.h"
#include "base/memory/singleton.h"
#include "base/win32/platform_string_util.h"
#include "base/win32/path_util.h"
#include "base/win32/win_util.h"
#include "base/thread/thread_manager.h"
#include "base/macros.h"
#include "base/base_types.h"
#include "base/file/file_util.h"
#include "base/time/time.h"
#include "base/framework/task.h"
