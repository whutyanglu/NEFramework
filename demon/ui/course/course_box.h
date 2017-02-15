#ifndef MSYK_UI_COURSE_BOX_H__
#define MSYK_UI_COURSE_BOX_H__

#include <list>
#include "dynamic_box.h"
#include "ui_page.h"
#include "cef_box.h"
#include "course_navigation_box.h"
#include "user/course.h"

class CousreNavigationBox;
class CefBox;

using OnClickVideoRecord = std::function<void()>;

class CourseBox : public DynamicBox
{
public:
	CourseBox(Course course, OnClickVideoRecord cb = nullptr);

	virtual void OnInit();
	void OnItemSelected(NavigationIndex index);
	void OnGoBack(UiPage *page);
	void OnGoNext(UiPage *page);
	virtual void SetVisible(bool visible = true);

private:
	void OnShowPage(UiPage *page);
	void GenerateBasicInfoPage();
	void GenerateKnowledgePage();
	void GenerateSupportingExercisePage();
	void GenerateExercisesExplainPage();
	void GenerateSupportingPapersPage();
	void GenerateRelatedInfoPage();

	template<typename T, typename ... Args>
	void GeneratePage(Args && ... args)
	{
		std::unique_ptr<UiPage> ptr = std::make_unique<T>(args...);
		current_page_ = ptr.get();
		pages_.push_back(std::move(ptr));

		navigation_box_->SelectedItem((NavigationIndex)current_page_->GetIndex());

		OnShowPage(current_page_);
	}

private:
	// 页面相关
	CousreNavigationBox *navigation_box_ = nullptr; // 左侧导航栏
 	ui::VBox *root_box_ = nullptr;				    // 右侧信息
	UiPage *current_page_ = nullptr;
	std::list<std::unique_ptr<UiPage>> pages_;		// 历史pages
	NavigationIndex	 current_item_index_ = kNavigationNon;
	OnClickVideoRecord on_click_video_record_ = nullptr;

	// 课程数据
	Course course_;
};


#endif // MSYK_UI_COURSE_BOX_H__
