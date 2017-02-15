#include "stdafx.h"
#include "util.h"
#include "shared/log.h"
#include "shellapi.h"
#include "base/encrypt/encrypt_impl.h"
#include "openssl/md5.h"

//
std::string QString::GetGUID()
{
	std::string guid;
	nbase::CreateGUID(guid);
	return guid;
}

std::string QString::GetMd5(const std::string& input)
{
	std::string src = input;
	nbase::Openssl_Encrypt_Hash<nbase::ENC_MD5> md5Encrypt;
	md5Encrypt.Encrypt(src);

	std::string dst;
	for (unsigned char i : src)
	{
		dst.append(nbase::StringPrintf("%02X", i));
	}

	return dst; 
}

std::string QString::GetFileMd5(const std::wstring &path)
{
	unsigned char md5[16] = { 0 };
	FILE *f = _wfopen(path.c_str(), L"rb");
	if (f == NULL) {
		return "";
	}

	std::string dst;
	MD5_CTX ctx;
	MD5_Init(&ctx);

	const int BUF_LEN = 4096;
	unsigned char buff[BUF_LEN];
	int readed_bytes = 0;
	do {
		readed_bytes = fread(buff, 1, BUF_LEN, f);
		MD5_Update(&ctx, buff, readed_bytes);
	} while (!feof(f));

	MD5_Final(md5, &ctx);
	for (int i = 0; i < 16; ++i)
	{
		dst.append(nbase::StringPrintf("%02X", md5[i]));
	}

	fclose(f);
	return dst;
}

void QString::NIMFreeBuf(void *data)
{
	// TODO
	//return  //nim::Global::FreeBuf(data);
}

//
std::wstring QPath::GetAppPath()
{
	return nbase::win32::GetCurrentModuleDirectory();
}

std::wstring QPath::GetUserAppDataDir(const std::string& app_account)
{
	return L""; //todo
	//return nbase::UTF8ToUTF16(nim::Tool::GetUserAppdataDir(app_account));
}

std::wstring QPath::GetLocalAppDataDir()
{
	return L""; // TODO 
	//return nbase::UTF8ToUTF16(nim::Tool::GetLocalAppdataDir());
}

std::wstring QPath::GetNimAppDataDir()
{
	std::wstring dir = QPath::GetLocalAppDataDir();
#ifdef _DEBUG
	dir.append(L"Netease\\NIM_Debug\\");
#else
	dir.append(L"Netease\\NIM\\");
#endif

	return dir;
}

//
std::map<std::wstring,std::wstring> QCommand::key_value_;

void QCommand::ParseCommand( const std::wstring &cmd )
{
	/*std::list<std::wstring> arrays = ui::StringHelper::Split(cmd, L"/");
	for(std::list<std::wstring>::const_iterator i = arrays.begin(); i != arrays.end(); i++)
	{
		std::list<std::wstring> object = ui::StringHelper::Split(*i, L" ");
		assert(object.size() == 2);
		key_value_[ *object.begin() ] = *object.rbegin();
	}*/
}

void QCommand::ParseCommand2(const std::wstring &cmd)
{
	//std::list<std::wstring> arrays = ui::StringHelper::Split(cmd, L" ");
	//for (std::list<std::wstring>::const_iterator i = arrays.begin(); i != arrays.end(); i++)
	//{
	//	std::list<std::wstring> object = ui::StringHelper::Split(*i, L"=");
	//	assert(object.size() == 2);
	//	key_value_[*object.begin()] = *object.rbegin();
	//}
}

std::wstring QCommand::Get( const std::wstring &key )
{
	std::map<std::wstring,std::wstring>::const_iterator i = key_value_.find(key);
	if(i == key_value_.end())
		return L"";
	else
		return i->second;
}

void QCommand::Set( const std::wstring &key, const std::wstring &value )
{
	key_value_[key] = value;
}

void QCommand::Erase(const std::wstring &key)
{
	key_value_.erase(key);
}

bool QCommand::AppStartWidthCommand( const std::wstring &app, const std::wstring &cmd )
{
	HINSTANCE hInst = ::ShellExecuteW(NULL, L"open", app.c_str(), cmd.c_str(), NULL, SW_SHOWNORMAL);
	return (int)hInst > 32;
}

bool QCommand::RestartApp(const std::wstring &cmd)
{
	wchar_t app[1024] = { 0 };
	GetModuleFileName(NULL, app, 1024);
	HINSTANCE hInst = ::ShellExecuteW(NULL, L"open", app, cmd.c_str(), NULL, SW_SHOWNORMAL);
	return (int)hInst > 32;
}