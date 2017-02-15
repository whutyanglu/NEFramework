#ifndef NET_NET_HTTP_H_
#define NET_NET_HTTP_H_

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

namespace net
{
struct http_field
{
	std::string name;
	std::string value;
};

using http_fields = std::vector<http_field>;

class HttpResponse
{
public:
	HttpResponse();
	~HttpResponse();
	int GetCode();
	const std::string & GetBody() const;
	const std::string & GetContent() const;
	void Parse(void *curl);
	size_t Write(const char *data, size_t len);
	bool Success() {
		return success_;
	}
	void SetErrorMsg(const std::string &err) {
		error_msg_ = error_msg_;
	}
	std::string GetErrorMsg() {
		return error_msg_;
	}

private:
	void ParseHeader(size_t head_size);
	void ParseBody();
	void ParseChunked();

private:
	std::map<std::string, std::string> header_fields_;
	std::string content_; // 保留http请求返回内容
	std::string body_;
	int         code_ = 0;
	int		    head_size_ = 0;
	int			body_size_ = 0;
	bool	    success_ = false;
	std::string error_msg_;

};

using response_cb = std::function<void(std::unique_ptr<HttpResponse>)>;

class NetHttp : public std::enable_shared_from_this<NetHttp>
{
public:
	explicit NetHttp(std::string url);
	NetHttp(const NetHttp &other) = delete;
	NetHttp(NetHttp &&rhs) = delete;
	NetHttp& operator=(const NetHttp &other) = delete;
	NetHttp& operator=(NetHttp &&rhs) = delete;
	~NetHttp() = default;

	void AsyncPost(response_cb f, http_fields s = http_fields(), bool base64 = false, bool url_encode = false);
	void AsyncGet(response_cb f, http_fields s = http_fields(), bool base64 = false, bool url_encode = false);
	std::unique_ptr<HttpResponse> Post(http_fields s = http_fields(), bool base64 = false, bool url_encode = false);
	std::unique_ptr<HttpResponse> Get(http_fields s = http_fields(), bool base64 = false, bool url_encode = false);

private:
	std::string ConvertSsmapToString(const http_fields & s, bool base64 = false, bool url_encode = false);

private:
	void *actor_;
	std::string url_;
};

bool NetHttpInit();
void NetHttpRelease();

}// namespace net

#endif // NET_NET_HTTP_H_