#include "stdafx.h"

#if !defined NETAPI_TEST

#include "net_api.h"
#include "shared/log.h"
#include "shared/closure.h"
#include "shared/util.h"
#include "net/net_http.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"

namespace
{
const std::string app_key = "msyk";
const std::string response_ok = "10000";
const std::string base_host = "http://appstore.wpxt.tst";
const std::string login_url = base_host + "/teacher_login.htm";
const std::string ws_base_host = "http://192.168.22.251/ws";
const std::string course_list_url = ws_base_host + "/courseShow/courseList";
const std::string course_catelog_url = ws_base_host + "/courseShow/courseCatalogForPC";
//const std::string ws_base_host = "http://squ.tandl.tst/personal/teacher";
//const std::string course_list_url = ws_base_host + "/courseManage/courseConstructList";
//const std::string course_catelog_url = ws_base_host + "/courseShow/courseCatalogForPC";

bool ParseJson(const std::string & content, rapidjson::Document &root, std::string &message)
{
	using namespace rapidjson;

	root.Parse(content.c_str(), content.size());
	if (root.HasParseError()) {
		return false;
	}

	std::string code = root["code"].GetString();
	if (code != response_ok) {
		message = root["message"].GetString();
		return false;
	}

	return true;
}

}

using namespace net;


NetApi::NetApi()
{
	
}

NetApi::~NetApi()
{
	
}

void NetApi::Login(const std::string & user_name, const std::string & password, LoginCallback cb)
{
	auto self = shared_from_this();

	http_fields params;
	params.push_back({"username", user_name});
	params.push_back({"password", nbase::MakeLowerString(QString::GetMd5(password))});

	auto response_cb = ([this, self, cb](std::unique_ptr<HttpResponse> response) {
		bool succ = false;
		std::vector<school> schools;
		std::string name;
		std::string id;
		std::string message;
		rapidjson::Document root;
		
		if (!response->Success()) {
			message = response->GetErrorMsg();
		}
		else {
			const std::string &body = response->GetBody();
			if (!root.Parse(body.c_str(), body.size()).HasParseError()) {
				succ = root["success"].GetInt() == 1;
				if (!succ) {
					message = root["message"].GetString();
					QLOG_ERR(L"login failed  message: {0}") << nbase::UTF8ToUTF16(message);
				}
				else {
					id = root["teacherId"].GetString();
					name = root["realName"].GetString();

					for (auto & sc : root["schools"].GetArray())
					{
						schools.push_back({ sc["id"].GetString(), sc["name"].GetString() });
					}
				}
			}
			else {
				QLOG_ERR(L"login json parsed failed : {0}") << nbase::UTF8ToUTF16(response->GetContent());
				succ = false;
			}
		}

		cb(succ, message, name, id, std::move(schools));
	});

	std::shared_ptr<NetHttp> http = std::make_shared<NetHttp>(login_url);
	http->AsyncGet(response_cb, params);
}

void NetApi::FetchCourseList(std::string teacher_id, int page_index, int size, FetchCourseListCallback cb)
{
	auto self = shared_from_this();

	// 注意顺序和key的计算方式保持一致
	http_fields params;
	params.push_back({"teacherId", teacher_id});
	params.push_back({"pageIndex", nbase::IntToString(page_index) });
	params.push_back({ "size",  nbase::IntToString(size) });
	std::string salt = Salt();
	params.push_back({ "salt", salt });
	params.push_back({ "key", Key(params) });

	auto response_cb = [self, this, cb](std::unique_ptr<HttpResponse> response) {
		const std::string & content = response->GetContent();
		std::string message;
		bool succ = false;
		int total_pages = 0;
		std::vector<Course> courses_vec;
		rapidjson::Document root;
		if (!ParseJson(response->GetBody(), root, message)) {
			QLOG_ERR(L"NetApi::FetchCourseList ParseJson Failed : {0}") << nbase::UTF8ToUTF16(content);
		}
		else {
			succ = true;
			total_pages = root["pages"].GetInt();
			auto & course_list = root["courseList"].GetArray();
			Course course;
			for (auto & c : course_list)
			{
				course.dir_id = c["dirId"].GetString();
				course.id = c["id"].GetInt();
				course.stu_num_ = c["stuNum"].GetInt();
				course.stu_rate_ = c["stuRate"].GetInt();
				course.info = c["courseInfo"].GetString();
				course.name = c["courseName"].GetString();
				course.picture_url = c["pictureUrl"].GetString();
				course.school_name = c["schoolName"].GetString();
				course.teacher_id = c["teacherId"].GetString();
				course.teacher_name = c["teacherName"].GetString();
				courses_vec.push_back(course);
			}
		}

		cb(succ, message, total_pages, courses_vec);
	};
	
	std::shared_ptr<NetHttp> http = std::make_shared<NetHttp>(course_list_url);
	http->AsyncPost(response_cb, params);
}

void NetApi::FetchCourseDirectory(std::string dir_id, std::string node_code, FetchDirectoryCallback cb)
{
	QLOG_APP(L"===========================================================>start FetchCourseDirectory");
	auto self = shared_from_this();

	// 注意顺序和key的计算方式保持一致
	http_fields params;
	params.push_back({ "dirId", dir_id });
	params.push_back({ "code", node_code });
	std::string salt = Salt();
	params.push_back({ "salt",  salt });
	params.push_back({ "key", Key(params) });

	auto response_cb = [self, this, node_code, cb](std::unique_ptr<HttpResponse> response) {
		const std::string & content = response->GetContent();
		std::string message;
		bool succ = false;
		int total_pages = 0;
		std::vector<DirectoryNode> dirs;
		rapidjson::Document root;
		if (!ParseJson(response->GetBody(), root, message)) {
			QLOG_ERR(L"NetApi::FetchCourseDirectory ParseJson Failed : {0}") << nbase::UTF8ToUTF16(content);
		}
		else {
			succ = true;
			auto & tag_list = root["tagList"].GetArray();
			DirectoryNode dir;
			for (auto & d : tag_list)
			{
				dir.id = d["id"].GetString();
				dir.description = d["tagDescription"].GetString();
				dir.tag_id = d["tagId"].GetString();
				dir.name = d["tagName"].GetString();
				dir.is_end = d["isEndTag"].GetBool();
				dir.dir_id = d["dirId"].GetString();
				dir.code_level = d["codeLevel"].GetInt();
				dir.node_code = d["nodeCode"].GetString();
				dirs.push_back(dir);
			}
		}

		if (cb) { 
			cb(succ, message, node_code, dirs);
		}
	};

	std::shared_ptr<NetHttp> http = std::make_shared<NetHttp>(course_catelog_url);
	http->AsyncGet(response_cb, params);
}

void NetApi::FetchDirecotryNodeVideo(std::string tagid, std::string node_code, FetchDirectoryNodeVideo)
{
}

std::string NetApi::Salt()
{
	std::string salt = nbase::Uint64ToString(nbase::Time::Now().ToTimeT());;
	return salt;
}

std::string NetApi::Key(const http_fields & params)
{
	std::string key;
	std::string tmp;
	for (auto & s : params)
	{
		tmp += s.value;
	}

	tmp += app_key;
	key = nbase::MakeLowerString(QString::GetMd5(tmp));

	return key;
}


#endif // NETAPI_TEST