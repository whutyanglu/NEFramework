#ifndef NET_NET_HTTP_FILE_H_
#define NET_NET_HTTP_FILE_H_

namespace net
{

using progress_cb = std::function<bool(__int64 dltotal, __int64 dlnow, __int64 ultotal, __int64 ulnow)>;
using finished_cb = std::function<void(bool finished, std::wstring path)>;

class NetHttpFile
{
public:
	void AsyncFetch(std::string url, std::wstring path, finished_cb fcb = nullptr,
		progress_cb pcb = nullptr, bool continue_down = false);
	void AsyncFetchInCurlThread(std::string url, std::wstring path, finished_cb fcb = nullptr,
		progress_cb pcb = nullptr, bool continue_down = false);
	void AsyncUpload(std::string url, std::wstring path, finished_cb fcb = nullptr,progress_cb pcb = nullptr);
	void AsyncUploadInCurlThread(std::string url, std::wstring path, finished_cb fcb = nullptr, progress_cb pcb = nullptr);

private:
	void Fetch(std::string url, std::wstring path, finished_cb fcb, progress_cb pcb, bool continue_down);
	void Upload(std::string url, std::wstring path, finished_cb fcb, progress_cb pcb);
};

} // namespace net

#endif // NET_NET_HTTP_FILE_H_
