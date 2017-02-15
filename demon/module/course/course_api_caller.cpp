#include "stdafx.h"
#include "course_api_caller.h"
#include "api/net_api.h"

void CourseApiCaller::FetchDirectory(std::string dir_id, std::string code, std::string id)
{
	using namespace std::placeholders;
	std::shared_ptr<NetApi> ptr(new NetApi());
	ptr->FetchCourseDirectory(dir_id, code,
		std::bind(&CourseApiCaller::OnFetchDirectory, this, id, _1, _2, _3, _4));
}

void CourseApiCaller::FetchDirectoryNode(std::string tagid, std::string node_code, std::string id)
{
	using namespace std::placeholders;
	std::shared_ptr<NetApi> ptr(new NetApi());
	ptr->FetchDirecotryNodeVideo(tagid, node_code,
		std::bind(&CourseApiCaller::OnFetchDirectoryNode, this, id, _1, _2, _3, _4, _5, _6));
}

void CourseApiCaller::OnFetchDirectory(std::string id, bool succ, std::string message, std::string node_code, const std::vector<DirectoryNode>& dirs)
{
	auto f = on_fetch_directory_.find(id);
	if (f != on_fetch_directory_.end()) {
		f->second(succ, message, node_code, dirs);
	}
}

void CourseApiCaller::OnFetchDirectoryNode(std::string id, bool succ, std::string message, std::string node_code,
	const DirNodeVideoVec& knowledges, const DirNodeVideoVec& exercises, const DirNodeVideoVec& papers)
{
	auto f = on_fetch_directory_node_.find(id);
	if (f != on_fetch_directory_node_.end()) {
		f->second(succ, message, node_code, knowledges, exercises, papers);
	}
}

void CourseApiCaller::RegisterOnFetchDirectory(std::string id, FetchDirectoryCallback cb)
{
	on_fetch_directory_.insert({ id, cb });
}

void CourseApiCaller::RegisterOnFetchDirectoryNode(std::string id, FetchDirectoryNodeVideoCallback cb)
{
	on_fetch_directory_node_.insert({ id, cb });
}
