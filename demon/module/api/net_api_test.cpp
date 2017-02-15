#include "stdafx.h"
#include "net_api.h"
#include <thread>
#include "shared/log.h"
#include "shared/closure.h"
#include "shared/util.h"

#include <map>

#ifdef NETAPI_TEST

namespace {
const int time_roll = 300;
#if NETAPI_TEST_SUCCESS 
bool success = true;
std::string message = u8"success³É¹¦";
#else
bool success = false;
std::string message = u8"failedÊ§°Ü";
#endif
DirectoryTree g_node_tree;
// ===================== Ä¿Â¼½áµãÏÂÊÓÆµÐÅÏ¢=================
DirNodeVideoVec g_knowledges{ { 1, u8"¹þ¹þ²âÊÔ", "2017-02-09 17:30", 
L".\\cousre\\icon_edit_vedio.png" },
{ 1, u8"Èý½Çº¯Êý", "2017-02-09 17:31", 
L".\\cousre\\icon_edit_pdf_mini.png" } };
DirNodeVideoVec g_exercises{ { 1, u8"¹þ¹þ²âÊÔ", "2017-02-09 17:30", L".\\cousre\\icon_edit_vedio.png" },
{ 1, u8"Èý½Çº¯Êý", "2017-02-09 17:31", L".\\cousre\\icon_edit_pdf_mini.png" } };
DirNodeVideoVec g_papers;

DirNodeVideoVec g_knowledges1;
DirNodeVideoVec g_exercises1;
DirNodeVideoVec g_papers1;
bool			g_exchange = true;

}

void netapi_test_start()
{
	// ==================== Ä¿Â¼Ê÷ ====================
	const std::string node_0001 = "0001";
	const std::string node_0002 = "0002";
	const std::string node_0003 = "0003";

	const std::string node_0001001 = "00010001";
	const std::string node_0001002 = "00010002";
	const std::string node_0001003 = "00010003";

	const std::string node_0002001 = "00020001";
	const std::string node_0002002 = "00020002";
	const std::string node_0002003 = "00020003";

	const std::string node_00010010001 = "000100010001";
	const std::string node_00010010002 = "000100010002";
	const std::string node_00010010003 = "000100010003";

	g_node_tree.Add("", new DirectoryNode(node_0001, 1));
	g_node_tree.Add("", new DirectoryNode(node_0002, 1));
	g_node_tree.Add("", new DirectoryNode(node_0003, 1));

	g_node_tree.Add(node_0001, new DirectoryNode(node_0001001, 2));
	g_node_tree.Add(node_0001, new DirectoryNode(node_0001002, 2));
	g_node_tree.Add(node_0001, new DirectoryNode(node_0001003, 2));
	g_node_tree.Add(node_0002, new DirectoryNode(node_0002001, 2));
	g_node_tree.Add(node_0002, new DirectoryNode(node_0002002, 2));
	g_node_tree.Add(node_0002, new DirectoryNode(node_0002003, 2));

	g_node_tree.Add(node_0001001, new DirectoryNode(node_00010010001, 3));
	g_node_tree.Add(node_0001001, new DirectoryNode(node_00010010002, 3));
	g_node_tree.Add(node_0001001, new DirectoryNode(node_00010010003, 3));

	//
	std::wstring app_apth = QPath::GetAppPath() + L"themes\\default";

	g_knowledges = { { 1, u8"¹þ¹þ²âÊÔ", "2017-02-09 17:30",
		app_apth + L"\\course\\icon_edit_vedio_mini.png" },
		{ 1, u8"Èý½Çº¯Êý", "2017-02-09 17:31",
		app_apth + L"\\course\\icon_edit_pdf_mini.png" } };
	g_exercises = { { 1, u8"¹þ¹þ²âÊÔ", "2017-02-09 17:30", app_apth + L"\\course\\icon_edit_vedio_mini.png" },
	{ 1, u8"Èý½Çº¯Êý", "2017-02-09 17:31", app_apth + L"\\course\\icon_edit_pdf_mini.png" } };
	
	g_knowledges1 = { { 1, u8"111¹þ¹þ²âÊÔ", "2017-02-09 17:30",
		app_apth + L"\\course\\icon_edit_vedio_mini.png" },
		{ 1, u8"111Èý½Çº¯Êý", "2017-02-09 17:31",
		app_apth + L"\\course\\icon_edit_pdf_mini.png" } };
	g_exercises1 = { { 1, u8"111¹þ¹þ²âÊÔ", "2017-02-09 17:30", app_apth + L"\\course\\icon_edit_vedio_mini.png" },
	{ 1, u8"111Èý½Çº¯Êý", "2017-02-09 17:31", app_apth + L"\\course\\icon_edit_pdf_mini.png" } };

}

NetApi::NetApi()
{

}

NetApi::~NetApi()
{

}

void NetApi::Login(const std::string & user_name, const std::string & password, LoginCallback cb)
{
	QLOG_APP(L"===========================================================>start Login");
	auto self = shared_from_this();

	auto task = [cb, self]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(time_roll));
		std::string name = u8"ÑîÂ¶";
		std::string id = u8"123";
		std::vector<school> schools;
		cb(success, message, name, id, std::move(schools));
	};

	Post2Curl(task);
}

void NetApi::FetchCourseList(std::string teacher_id, int page_index, int size, FetchCourseListCallback cb)
{
	QLOG_APP(L"===========================================================>start FetchCourseList");
	auto self = shared_from_this();

	auto task = [cb, self]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(time_roll));
		std::vector<Course> courses_vec;
		Course course;
		for (int i = 0; i < 1; ++i)
		{
			course.dir_id = u8"course.dir_id";
			course.id = 1;
			course.stu_num = 1;
			course.stu_rate = 2;
			course.info = u8"course.info";
			course.name = u8"²âÊÔname";
			course.picture_url = u8"Í¼Æ¬µØÖ·";
			course.school_name = u8"Ñ§Ð£Ãû³Æ";
			course.teacher_id = u8"course.teacher_id";
			course.teacher_name = u8"ÀÏÊ¦Ãû³Æ";
			courses_vec.push_back(course);
		}
	};

	Post2Curl(task);
}

void NetApi::FetchCourseDirectory(std::string dir_id, std::string node_code, FetchDirectoryCallback cb)
{
	QLOG_APP(L"===========================================================>start FetchCourseDirectory");
	auto self = shared_from_this();

	auto task = [cb, node_code, self]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(time_roll));
		std::vector<DirectoryNode> dirs = g_node_tree.GetChildNodes(node_code);
		cb(success, message, node_code, dirs);
	};

	Post2Curl(task);
}

void NetApi::FetchDirecotryNodeVideo(std::string tagid, std::string node_code, FetchDirectoryNodeVideoCallback cb)
{
	QLOG_APP(L"===========================================================>start FetchCourseDirectory");
	auto self = shared_from_this();

	auto task = [cb, tagid, node_code, self]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(time_roll));
		if (g_exchange) {
			cb(success, message, node_code, g_knowledges, g_exercises, g_papers);
		}
		else {
			cb(success, message, node_code, g_knowledges1, g_exercises1, g_papers1);
		}
		g_exchange = !g_exchange;
	};

	Post2Curl(task);
}

std::string NetApi::Salt()
{
	return "";
}

std::string NetApi::Key(const http_fields & params)
{
	return "";
}

#endif //~NETAPI_TEST
