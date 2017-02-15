#include "stdafx.h"
#include "main_form.h"
#include "api/net_api.h"
#include "ui/common/cef_box.h"
#include "ui/course/course_box.h"
#include "shared/cef/cef_browser_control.h"
#include "shared/cef/cef_nef_app.h"

MainForm *MainForm::mainform_ = nullptr;

MainForm::MainForm() 
{
	MainForm::mainform_ = this;
}

MainForm::~MainForm()
{
}

MainForm* MainForm::GetMainForm()
{
	return mainform_;
}

void MainForm::InitWindow() 
{
	using namespace ui;
	auto self = this;

	homepage_ = dynamic_cast<shared::CefBrowserControl*>(pm_.FindControl(L"cef_browser_homepage"));
	homepage_->Navagate("www.baidu.com");
	course_box_ = dynamic_cast<ui::VBox*>(pm_.FindControl(L"course_box"));

	// test
	FetchCourseList();
	ui::Button *btn_max = dynamic_cast<ui::Button*>(pm_.FindControl(L"btn_max"));
	btn_max->AttachClick([this, self](const ui::EventArgs &arg) {
		Course course;
		if (!course_list_.empty()) {
			course = course_list_.at(0);
		}

		homepage_->SetVisible(false);
		course_box_->SetVisible(true);
		if (course_page_ == nullptr) {
			course_page_ = new CourseBox(course, nbase::Bind(&MainForm::OnClickVideoRecord, self));
			course_page_->Init(L"\\course\\course.xml", &pm_, course_box_);
		}
		course_page_->SetVisible(true);
		return true; 
	});

	ui::Button *btn_min = dynamic_cast<ui::Button*>(pm_.FindControl(L"btn_min"));
	btn_min->AttachClick([this](const ui::EventArgs &arg) {
		homepage_->SetVisible(true);
		course_box_->SetVisible(false);
		return true;
	});
}

std::wstring MainForm::GetSkinFile()
{
	return L"main_form.xml";
}

std::wstring MainForm::GetSkinFolder()
{
	return L"main";
}

std::wstring MainForm::GetWindowClassName() const
{ 
	return L"MainForm";
}

std::wstring MainForm::GetWindowId()
{
	return L"mainform_wnd_id";
}

UINT MainForm::GetClassStyle() const
{ 
	return CS_DBLCLKS; 
}

void MainForm::OnFinalMessage(HWND hWnd)
{
}

void MainForm::OnClickVideoRecord()
{
	homepage_->SetVisible(false);
	course_box_->SetVisible(false);
	course_page_->SetVisible(false);
}

void MainForm::FetchCourseList()
{
	using namespace std::placeholders;
	std::shared_ptr<NetApi> ptr( new NetApi());
	ptr->FetchCourseList(Teacher::GetInstance()->GetId(), 1, 10, 
		nbase::Bind(&MainForm::OnFetchCourseList, this, _1, _2, _3, _4));
}

void MainForm::OnFetchCourseList(bool succ, std::string message, int total_pages, const std::vector<Course>& course_list)
{
	if (succ) {
		course_list_ = course_list;
		total_pages_ = total_pages;

		if (!course_list.empty()) {
			std::string dir_id = course_list.at(0).dir_id;
		}
		
		QLOG_APP(L"FetchCourseList Success");
	}
	else {
		QLOG_ERR(L"FetchCourseList Failed : {0}") << nbase::UTF8ToUTF16(message);
	}
}

void MainForm::FetchDirectory(std::string dir_id)
{
	std::shared_ptr<NetApi> ptr(new NetApi());
	ptr->FetchCourseDirectory(dir_id);
}


