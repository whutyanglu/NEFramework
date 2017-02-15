#pragma once

#include "user/course.h"

namespace shared
{
class CefBrowserControl;
class NDFControl;
}

class DynamicBox;
class MainForm;

class MainForm : public ui::WindowImplBase
{
public:
	MainForm();
	~MainForm();

public:
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetSkinFolder() override;

	virtual std::wstring GetWindowClassName() const;
	static  std::wstring GetWindowId();
	UINT GetClassStyle() const;
	void InitWindow();
	void OnFinalMessage(HWND hWnd);

	void OnClickVideoRecord();

	static MainForm* GetMainForm();
	shared::NDFControl *GetNdfView() const;

private:
	void FetchCourseList();
	void OnFetchCourseList(bool succ, std::string message, int total_pages, const std::vector<Course> &course_list);
	void FetchDirectory(std::string dir_id);

private:
	std::vector<Course> course_list_;
	int	total_pages_ = 0;
	ui::VBox *course_box_ = NULL;
	shared::CefBrowserControl*  homepage_ = nullptr;
	DynamicBox   *course_page_ = nullptr;
	static MainForm	*mainform_;
};
