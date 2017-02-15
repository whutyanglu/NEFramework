#include "stdafx.h"
#include "net_http_file.h"
#include <thread>
#include "net_http.h"
#include "util/string_util.h"
#include "closure.h"
#include "log.h"
#include "curl/curl.h"

namespace net
{
__int64 original_size = 0;

static size_t write_callback_download(char *buffer, size_t size, size_t nitems, void *userp)
{
	FILE *f = (FILE*)userp;
	return fwrite(buffer,  size,  nitems, f);
}

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	FILE *f = (FILE *)userp;
	size_t retcode = fread(ptr, size, nmemb, f);
	return retcode;
}

static size_t write_callback_upload(char *buffer, size_t size, size_t nitems, void *userp)
{
	HttpResponse *response = static_cast<HttpResponse*>(userp);
	return response->Write(buffer, size * nitems);
}

static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	progress_cb *pcb = static_cast<progress_cb*>(clientp);
	if (dltotal > 0) {
		bool b = (*pcb)(dltotal + original_size, dlnow + original_size, ultotal, ulnow);
		return b ? 1 : 0;
	}

	return 0;
}

void NetHttpFile::AsyncFetch(std::string url, std::wstring path, finished_cb fcb, progress_cb pcb, bool resume_from_breakpoint)
{
	auto thd_cb = [this, url, path, fcb, pcb, resume_from_breakpoint]()
	{
		Fetch(url, path, fcb, pcb, resume_from_breakpoint);
	};
	
	std::thread thd(thd_cb);
	thd.detach();
}

void NetHttpFile::AsyncFetchInCurlThread(std::string url, std::wstring path, finished_cb fcb, progress_cb pcb, bool resume_from_breakpoint)
{
	auto thd_cb = [this, url, path, fcb, pcb, resume_from_breakpoint]()
	{
		Fetch(url, path, fcb, pcb, resume_from_breakpoint);
	};
	
	Post2Curl(thd_cb);
}

void NetHttpFile::AsyncUpload(std::string url, std::wstring path, finished_cb fcb, progress_cb pcb)
{
	auto thd_cb = [this, url, path, fcb, pcb]()
	{
		Upload(url, path, fcb, pcb);
	};

	std::thread thd(thd_cb);
	thd.detach();
}

void NetHttpFile::AsyncUploadInCurlThread(std::string url, std::wstring path, finished_cb fcb, progress_cb pcb)
{
	auto thd_cb = [this, url, path, fcb, pcb]()
	{
		Upload(url, path, fcb, pcb);
	};

	Post2Curl(thd_cb);
}

void NetHttpFile::Fetch(std::string url, std::wstring path, finished_cb fcb, progress_cb pcb, bool resume_from_breakpoint)
{
	original_size = 0;
	CURL *curl = curl_easy_init();
	assert(curl != NULL);
	if (curl == NULL) {
		QLOG_ERR(L"curl_easy_init failed");
		return;
	}

	FILE *f = _wfopen(path.c_str(), resume_from_breakpoint ? L"ab" : L"wb");
	assert(f != NULL);
	if (f == NULL) {
		return;
	}

	CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	assert(res == CURLE_OK);
	res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
	assert(res == CURLE_OK);

	res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_download);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);

	if (pcb) {
		res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		assert(res == CURLE_OK);
		res = curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
		assert(res == CURLE_OK);
		res = curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &pcb);
		assert(res == CURLE_OK);
	}
	
	if (resume_from_breakpoint) {
		curl_off_t fsize = nbase::GetFileSize(path);
		if (fsize > 0) {
			std::string tmp = nbase::StringPrintf("%lld-", fsize);
			res = curl_easy_setopt(curl, CURLOPT_RANGE, tmp.c_str());
			assert(res == CURLE_OK);
			original_size = fsize;
		}
	}

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(f);

	fcb(res == CURLE_OK, path);
}

void NetHttpFile::Upload(std::string url, std::wstring path, finished_cb fcb, progress_cb pcb)
{
	CURL *curl = curl_easy_init();
	assert(curl != NULL);
	if (curl == NULL) {
		QLOG_ERR(L"curl_easy_init failed");
		return;
	}

	std::unique_ptr<HttpResponse> ptr(new HttpResponse);

	CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	assert(res == CURLE_OK);
	res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
	assert(res == CURLE_OK);
	
 	curl_off_t upload_size = nbase::GetFileSize(path);
	FILE *f = _wfopen(path.c_str(), L"rb");
	if (f == NULL) {
		return;
	}
	res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(curl, CURLOPT_READDATA, f);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, upload_size);

	res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_upload);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, ptr.get());

	if (pcb) {
		res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		assert(res == CURLE_OK);
		res = curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
		assert(res == CURLE_OK);
		res = curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &pcb);
		assert(res == CURLE_OK);
	}

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(f);

	fcb(res == CURLE_OK, path);
}

}//namespace net