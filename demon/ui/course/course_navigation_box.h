#ifndef MSYK_UI_COURSE_NAVIGATION_BOX_H__
#define MSYK_UI_COURSE_NAVIGATION_BOX_H__

#include "dynamic_box.h"

enum NavigationIndex
{
	kNavigationNon = -1,
	kNavigationBasicInfo,			// 基本信息
	kNavigationKnowledgePoint,      // 知识点讲解
	kNavigationSupportingExercises, // 配套练习
	kNavigationExerciseExplain,		// 习题讲解
	kNavigationSupportingPapers,	// 配套试卷
	kNavigationRelatingInfo,		// 相关资料
};

using OnNavigationItemSelected = std::function<void(NavigationIndex)>;

class CousreNavigationBox : public DynamicBox
{
public:
	CousreNavigationBox(OnNavigationItemSelected cb = nullptr);
	virtual void OnInit();

	void SelectedItem(NavigationIndex index);

private:
	OnNavigationItemSelected on_item_selected_ = nullptr;
	ui::List* list_ = nullptr;
};


#endif // MSYK_UI_COURSE_NAVIGATION_BOX_H__