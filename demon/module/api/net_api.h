#ifndef MSYKMODULE_API_NET_API_H__
#define MSYKMODULE_API_NET_API_H__

#include "user/teacher.h"
#include "user/course.h"
#include "user/directory.h"
#include "net/net_http.h"

using namespace net;

// 参数列表： succ, message, name, id, schools
using LoginCallback = std::function<void(bool, std::string, std::string, std::string, const std::vector<school> &)>;
using FetchCourseListCallback = std::function<void(bool, std::string, int, const std::vector<Course> &)>;
using FetchDirectoryCallback = std::function<void(bool, std::string, std::string, const std::vector<DirectoryNode> &)>;
using FetchDirectoryNodeVideoCallback = std::function<void(bool, std::string, std::string,
	const DirNodeVideoVec&, const DirNodeVideoVec&, const DirNodeVideoVec&)>;

class NetApi : public std::enable_shared_from_this<NetApi>
{
public:
	NetApi();
	virtual ~NetApi();

	void Login(const std::string & user_name, const std::string & password, LoginCallback cb);
	void FetchCourseList(std::string teacher_id, int page_index = 1, int size = 10, FetchCourseListCallback cb = nullptr);
	void FetchCourseDirectory(std::string dir_id, std::string code = "", FetchDirectoryCallback cb = nullptr);
	void FetchDirecotryNodeVideo(std::string tagid, std::string node_code, FetchDirectoryNodeVideoCallback);

private:
	std::string Salt();
	std::string Key(const http_fields & params);
};

#ifdef NETAPI_TEST
void netapi_test_start();
#endif


#endif //~MSYKMODULE_API_NET_API_H__