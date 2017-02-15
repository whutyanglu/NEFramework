#include "stdafx.h"
#include "course_box.h"
#include "course_navigation_box.h"
#include "content_box.h"
#include "cef_page.h"
#include "knowledge_point_page.h"

const std::string basic_info_url = "www.baidu.com";
const std::string supporting_exercises_url = "www.sina.cn";

CourseBox::CourseBox(Course course, OnClickVideoRecord cb)
	: course_(course)
	, on_click_video_record_(cb)
{
}

void CourseBox::OnInit()
{
	ui::Button * btn_record_video = dynamic_cast<ui::Button *>(FindSubContrl(L"btn_record_video"));
	btn_record_video->AttachClick([this](const ui::EventArgs &arg) {
		if (on_click_video_record_) {
			on_click_video_record_();
		}
		return true;
	});


	// ×ó²àµ¼º½À¸
	ui::VBox *box = dynamic_cast<ui::VBox*>(FindSubContrl(L"course_navigation_bar"));
	navigation_box_ = new CousreNavigationBox(std::bind(&CourseBox::OnItemSelected, this, std::placeholders::_1));
	navigation_box_->Init(L"/course/course_navigation.xml", pm_, box);

	root_box_ = dynamic_cast<ui::VBox*>(FindSubContrl(L"root_box"));
	GenerateBasicInfoPage();

	ui::Button *btn_goback = dynamic_cast<ui::Button*>(FindSubContrl(L"btn_goback"));
	ui::Button *btn_forward = dynamic_cast<ui::Button*>(FindSubContrl(L"btn_forward"));
	btn_goback->AttachClick([this](const ui::EventArgs &arg) {
		if (!current_page_->GoBack()) {
			
		}
		return true;
	});
	btn_forward->AttachClick([this](const ui::EventArgs &arg) {
		if (!current_page_->GoForward()) {

		}
		return true;
	});
}

void CourseBox::OnItemSelected(NavigationIndex index)
{
	NavigationIndex navigation_index = (NavigationIndex)current_page_->GetIndex();;
	if (navigation_index == index) {
		current_page_->Refresh();
	}
	else {
		switch (index)
		{
		case kNavigationNon:
			break;
		case kNavigationBasicInfo:
			GenerateBasicInfoPage();
			break;
		case kNavigationKnowledgePoint:
			GenerateKnowledgePage();
			break;
		case kNavigationSupportingExercises:
			GenerateSupportingExercisePage();
			break;
		case kNavigationExerciseExplain:
			break;
		case kNavigationSupportingPapers:
			break;
		case kNavigationRelatingInfo:
			break;
		default:
			break;
		}

	}
}

void CourseBox::OnGoBack(UiPage * page)
{
	OnShowPage(page);
}

void CourseBox::OnGoNext(UiPage * page)
{
	OnShowPage(page);
}

void CourseBox::SetVisible(bool visible)
{
	if (current_page_ != nullptr) {
		current_page_->SetVisible(visible);
	}
}

void CourseBox::OnShowPage(UiPage * page)
{
	current_page_ = page;
	NavigationIndex index = (NavigationIndex)page->GetIndex();
	navigation_box_->SelectedItem(index);

	auto box = page->Box();
	if (box) {
		root_box_->RemoveAll();
		root_box_->Add(box);
	}
}

void CourseBox::GenerateBasicInfoPage()
{
	GeneratePage<CefPage>(kNavigationBasicInfo, current_page_, pm_, root_box_, basic_info_url,
		std::bind(&CourseBox::OnGoBack, this, std::placeholders::_1),
		std::bind(&CourseBox::OnGoNext, this, std::placeholders::_1));
}

void CourseBox::GenerateKnowledgePage()
{
	GeneratePage<KnowledgePointPage>(course_.dir_id, kNavigationKnowledgePoint, current_page_,
		std::bind(&CourseBox::OnGoBack, this, std::placeholders::_1),
		std::bind(&CourseBox::OnGoNext, this, std::placeholders::_1));

	current_page_->Init(L"/course/knowledge_point_page.xml", pm_, root_box_);
}

void CourseBox::GenerateSupportingExercisePage()
{
	GeneratePage<CefPage>(kNavigationSupportingExercises, current_page_, pm_, root_box_, supporting_exercises_url,
		std::bind(&CourseBox::OnGoBack, this, std::placeholders::_1),
		std::bind(&CourseBox::OnGoNext, this, std::placeholders::_1));
}

void CourseBox::GenerateExercisesExplainPage()
{
}

void CourseBox::GenerateSupportingPapersPage()
{
}

void CourseBox::GenerateRelatedInfoPage()
{
}

