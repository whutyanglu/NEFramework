#include "stdafx.h"
#include "net_http.h"
#include <cassert>
#include "util/string_util.h"
#include "util/url_encode.h"
#include "util/base64.h"
#include "closure.h"
#include "curl/curl.h"
#include "log.h"

namespace net
{

const std::string HTTP_SPLIT = "\r\n\r\n";
const std::string HTTP_ITEM_SPLIT = "\r\n";
const int TIMEOUT = 10;

class DeleterCurl
{
public:
	void operator()(CURL* p) const {
		curl_easy_cleanup(p);
	};
};


static size_t write_callback(char *buffer, size_t size, size_t nitems, void *userp)
{
	HttpResponse *response = static_cast<HttpResponse*>(userp);
	return response->Write(buffer, size * nitems);
}

HttpResponse::HttpResponse()
{
	content_.reserve(512);
}

HttpResponse::~HttpResponse()
{

}

int HttpResponse::GetCode()
{
	return code_;
}

const std::string &HttpResponse::GetBody() const
{
	return body_;
}

const std::string & HttpResponse::GetContent() const
{
	return content_;
}

void HttpResponse::Parse(void * curl)
{
	success_ = false;
	CURLcode res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code_);
	if (res != CURLE_OK) {
		error_msg_ = curl_easy_strerror(res);
		QLOG_ERR(L"{0}") << nbase::UTF8ToUTF16(error_msg_);
		return;
	}

	if (code_ < 200 || code_ > 300) {
		error_msg_ = nbase::IntToString(code_);
		QLOG_ERR(L"{0}") << nbase::UTF8ToUTF16(error_msg_);
		return;
	}

	res = curl_easy_getinfo(curl, CURLINFO_HEADER_SIZE, &head_size_);
	assert(res == CURLE_OK);
	if (res != CURLE_OK) {
		QLOG_ERR(L"{0}") << nbase::UTF8ToUTF16(curl_easy_strerror(res));
		return;
	}
		
	ParseHeader(head_size_);
	ParseBody();

	success_ = true;
	/*int download_size = 0;
	res = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &download_size);
	assert(res == CURLE_OK);
	if (res != CURLE_OK) {
		QLOG_ERR(L"{0}") << nbase::UTF8ToUTF16(curl_easy_strerror(res));
		return;
	}

	int content_size = 0;
	res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_size);
	assert(res == CURLE_OK);
	if (res != CURLE_OK) {
		QLOG_ERR(L"{0}") << nbase::UTF8ToUTF16(curl_easy_strerror(res));
		return;
	}
	*/
}

size_t HttpResponse::Write(const char * data, size_t len)
{
	content_.append(data, len);
	return len;
}

void HttpResponse::ParseHeader(size_t head_size)
{
	// version
	size_t pos_begin = content_.find_first_of(HTTP_ITEM_SPLIT);
	if (pos_begin == std::string::npos) {
		return;
	}

	size_t LEN = HTTP_ITEM_SPLIT.size();
	size_t pos_end;
	size_t pos2;
	do
	{
		pos_begin += LEN;
		pos_end = content_.find_first_of(HTTP_ITEM_SPLIT, pos_begin);
		if (pos_end != std::string::npos) {
			pos2 = content_.find_first_of(": ", pos_begin + LEN);
			if (pos2 != std::string::npos) {
				header_fields_.insert({ content_.substr(pos_begin, pos2 - pos_begin), 
									 content_.substr(pos2 + 2,  pos_end - pos2 - 2)});
			}
		}
		pos_begin = pos_end;

	} while (pos_begin  < head_size - HTTP_SPLIT.size());

	return;
}

void HttpResponse::ParseBody()
{
	body_ = content_.substr(head_size_);

	return;
	// TODO : luyang
	auto iter = header_fields_.find("Content-Length");
	if (iter != header_fields_.end()) {
		if (!nbase::StringToInt(iter->second, &body_size_)) {
			QLOG_ERR(L"HttpResponse::ParseBody StringToInt Failed");
		}

		return;
	}

	iter = header_fields_.find("Transfer-Encoding");
	if (iter != header_fields_.end()) {
		if (iter->second == "chunked") {
			ParseChunked();
			return;
		}
	}

	body_ = content_.substr(head_size_);
}

void HttpResponse::ParseChunked()
{
	int len = 0;
	size_t pos = head_size_;
	do
	{
		// Chunked data size
		std::string chunk_len = content_.substr(pos, 4);
		if (!nbase::StringToInt(chunk_len, &len)) {
			break;
		}
		pos += 4;

		// Chunkd data
		if (len != 0 && pos < content_.size()) {
			body_.append(content_.substr(pos, len));
			pos += len;
		}
		else {
			break;
		}
	} while (pos < content_.size());
}

//////////////////////////////////////////////////////////////////////////////
NetHttp::NetHttp(std::string url)
	: url_(url)
	, actor_(NULL)
{
}

void NetHttp::AsyncPost(response_cb cb, http_fields s, bool base64, bool url_encode)
{
	auto self = shared_from_this();
	auto task = [this, self, cb, s, base64, url_encode]()
	{
		std::unique_ptr<HttpResponse> ptr = Post(s, base64, url_encode);
		cb(std::move(ptr));
	};

	Post2Curl(task);
}

void NetHttp::AsyncGet(response_cb cb, http_fields s, bool base64, bool url_encode)
{
	auto self = shared_from_this();
	auto task = [this, self, cb, s, base64, url_encode]()
	{
		std::unique_ptr<HttpResponse> ptr = Get(s, base64, url_encode);
		cb(std::move(ptr));
	};
	
	Post2Curl(task);
}

std::unique_ptr<HttpResponse> NetHttp::Post(http_fields s, bool base64, bool url_encode)
{
	actor_ = curl_easy_init();
	assert(actor_ != NULL);

	std::unique_ptr<CURL, DeleterCurl> curl_scope(actor_, DeleterCurl());

	CURLcode res = CURLE_OK;
	res = curl_easy_setopt(actor_, CURLOPT_VERBOSE, 0);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(actor_, CURLOPT_URL, url_.c_str());
	assert(res == CURLE_OK);
	res = curl_easy_setopt(actor_, CURLOPT_POST, 1);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(actor_, CURLOPT_HEADER, 1);
	assert(res == CURLE_OK);

	std::string postfields;
	if (!s.empty()) {
		postfields = ConvertSsmapToString(s, base64, url_encode);
		res = curl_easy_setopt(actor_, CURLOPT_POSTFIELDS, postfields.c_str());
		assert(res == CURLE_OK);
		res = curl_easy_setopt(actor_, CURLOPT_POSTFIELDSIZE, postfields.size());
		assert(res == CURLE_OK);
		QLOG_APP(L"http post : url = {0}, fields = {1}") << url_ << postfields;
	}

	res = curl_easy_setopt(actor_, CURLOPT_TIMEOUT, TIMEOUT);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(actor_, CURLOPT_WRITEFUNCTION, write_callback);
	assert(res == CURLE_OK);
	std::unique_ptr<HttpResponse> response_uptr = std::make_unique<HttpResponse>();
	res = curl_easy_setopt(actor_, CURLOPT_WRITEDATA, response_uptr.get());
	assert(res == CURLE_OK);

	res = curl_easy_perform(actor_);
	if (res == CURLE_OK) {	
		response_uptr->Parse(actor_);
		QLOG_APP(L"http post response : {0}") << nbase::UTF8ToUTF16(response_uptr->GetContent());
	}
	else {
		response_uptr->SetErrorMsg(curl_easy_strerror(res));
		QLOG_ERR(L"http post failed : {0}") << curl_easy_strerror(res);
	}

	return response_uptr;
}

std::unique_ptr<HttpResponse> NetHttp::Get(http_fields s, bool base64, bool url_encode)
{
	if (!s.empty()) {
		url_ += "?";
		url_ += ConvertSsmapToString(s, base64, url_encode);
	}

	actor_ = curl_easy_init();
	assert(actor_ != NULL);
	if (actor_ == NULL) {
		return nullptr;
	}

	QLOG_APP(L"http get : url = {0}") << url_;

	std::unique_ptr<CURL, DeleterCurl> curl_scope(actor_, DeleterCurl());

	CURLcode res = CURLE_OK;
	res = curl_easy_setopt(actor_, CURLOPT_VERBOSE, 0);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(actor_, CURLOPT_URL, url_.c_str());
	assert(res == CURLE_OK);
	res = curl_easy_setopt(actor_, CURLOPT_HEADER, 1);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(actor_, CURLOPT_TIMEOUT, TIMEOUT);
	assert(res == CURLE_OK);
	res = curl_easy_setopt(actor_, CURLOPT_WRITEFUNCTION, write_callback);
	assert(res == CURLE_OK);

	std::unique_ptr<HttpResponse> ptr(new HttpResponse);
	res = curl_easy_setopt(actor_, CURLOPT_WRITEDATA, ptr.get());
	assert(res == CURLE_OK);

	res = curl_easy_perform(actor_);
	if (res == CURLE_OK) {
		ptr->Parse(actor_);
		QLOG_APP(L"http get response : {0}") << nbase::UTF8ToUTF16(ptr->GetContent());
	}
	else {
		ptr->SetErrorMsg(curl_easy_strerror(res));
		QLOG_ERR(L"http get failed : {0}") << curl_easy_strerror(res);
	}

	return ptr;
}

std::string NetHttp::ConvertSsmapToString(const http_fields & s, bool base64, bool url_encode)
{
	std::string params;
	std::string tmp;
	for (auto it = s.begin(); it != s.end(); ++it)
	{
		params += it->name;
		params += "=";
		tmp = it->value;
		if (base64) {
			nbase::Base64Encode(it->value, &tmp);
		}
		if (url_encode) {
			tmp = nbase::UrlEncode(tmp);
		}
		params += tmp;
		params += "&";
	}

	if (!params.empty()) {
		params.erase(--params.end());
	}
	
	return params;
}

bool NetHttpInit()
{
	CURLcode res = curl_global_init(CURL_GLOBAL_WIN32);
	return res == CURLE_OK;
}

void NetHttpRelease()
{
	curl_global_cleanup();
}

} // namespace net
