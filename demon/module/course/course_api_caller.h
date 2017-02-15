#ifndef MSYK_MODULE_COURSE_COURSE_API_CALLER_H__
#define MSYK_MODULE_COURSE_COURSE_API_CALLER_H__

#include "user/directory.h"
#include "api/net_api.h"

class CourseApiCaller
{
public:
	CourseApiCaller() {}
	SINGLETON_DEFINE(CourseApiCaller);

	void FetchDirectory(std::string dir_id, std::string code = "", std::string id = "");
	void FetchDirectoryNode(std::string tagid, std::string node_code, std::string id = "");
	void OnFetchDirectory(std::string id, bool, std::string, std::string node_code, const std::vector<DirectoryNode> & dirs);
	void OnFetchDirectoryNode(std::string id, bool, std::string, std::string, 
		const DirNodeVideoVec&, const DirNodeVideoVec&, const DirNodeVideoVec&);
	void RegisterOnFetchDirectory(std::string id, FetchDirectoryCallback cb);
	void RegisterOnFetchDirectoryNode(std::string id, FetchDirectoryNodeVideoCallback cb);

private:
	std::map<std::string, FetchDirectoryCallback> on_fetch_directory_;
	std::map<std::string, FetchDirectoryNodeVideoCallback> on_fetch_directory_node_;
};


#endif // ~MSYK_MODULE_COURSE_COURSE_API_CALLBACK_H__