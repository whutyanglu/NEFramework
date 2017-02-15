#include "stdafx.h"
#include "knowledge_point_node_page.h"
#include "knowledge_point_tile_item_box.h"
#include "knowledge_list_item_box.h"
#include "module/course/course_api_caller.h"

KnowledgePointNodePage::KnowledgePointNodePage(UiPage *back,
	OnGoBack on_goback , OnGoForward on_goforward)
	: UiPage(0, back, on_goback, on_goforward)
{

}

void KnowledgePointNodePage::OnInit()
{
	// 知识点
	box_knowledge_points_tile_style_ = dynamic_cast<ui::Box*>(FindSubContrl(L"box_knowledge_points_tile_style"));
	box_knowledge_points_ = dynamic_cast<ui::HBox*>(FindSubContrl(L"box_knowledge_points"));
	list_knowledge_points_ = dynamic_cast<ui::List*>(FindSubContrl(L"list_knowledge_points"));
	box_empty_knowledge_point_ = dynamic_cast<ui::Box*>(FindSubContrl(L"box_empty_knowledge_point"));

	// 相关练习
	box_supporting_exercises_ = dynamic_cast<ui::HBox*>(FindSubContrl(L"box_supporting_exercises"));
	list_supporting_exercises_ = dynamic_cast<ui::List*>(FindSubContrl(L"list_supporting_exercises"));
	box_empty_exercise_ = dynamic_cast<ui::Box*>(FindSubContrl(L"box_empty_exercise"));

	// 相关试卷
	box_supporting_papers_ = dynamic_cast<ui::HBox*>(FindSubContrl(L"box_supporting_papers"));
	list_supporting_papers_ = dynamic_cast<ui::List*>(FindSubContrl(L"list_supporting_papers"));
	box_empty_papers_ = dynamic_cast<ui::Box*>(FindSubContrl(L"box_empty_papers"));

	using namespace std::placeholders;
	CourseApiCaller::GetInstance()->RegisterOnFetchDirectoryNode(id_, 
		ToWeakCallback(nbase::Bind(&KnowledgePointNodePage::OnFetchDirectoryTreeNode, this, _1, _2, _3, _4, _5, _6)));

}

void KnowledgePointNodePage::OnFetchDirectoryTreeNode(bool succ, std::string message, std::string node_code, 
	const DirNodeVideoVec & knowledges, const DirNodeVideoVec & exercises, const DirNodeVideoVec & papers)
{
	Post2UI([this, succ, message, node_code, knowledges, exercises, papers] {
		if (succ) {
			ShowKnowledgePoints(knowledges);
			ShowSupportingExercises(exercises);
			ShowSupportingPapers(papers);
		}
		else {
			QLOG_ERR(L"FetchDirecotyTreeNode Failed msg : {0}") << nbase::UTF8ToUTF16(message);
		}	
	});
}

void KnowledgePointNodePage::ShowKnowledgePoints(const DirNodeVideoVec & knowledges)
{
	using namespace std::placeholders;
	bool visible = !knowledges.empty();
	box_empty_knowledge_point_->SetVisible(!visible);
	box_knowledge_points_tile_style_->SetVisible(visible && view_style_ == kViewTile);
	list_knowledge_points_->SetVisible(visible && view_style_ == kViewList);

	for (auto & k : knowledges)
	{
		KnowledgeListItemBox *knl_page = new KnowledgeListItemBox(k);
		knl_page->Init(L"/course/knowledge_list_page.xml", pm_, list_knowledge_points_);

		KnowledgePointTileItemBox *tile_page = new KnowledgePointTileItemBox(140, 110,
			std::bind(&KnowledgePointNodePage::OnMouseClickKnowledgePointTileItem, this, _1, _2),
			std::bind(&KnowledgePointNodePage::OnMouseHoverKnowledgePointTileItem, this, _1),
			std::bind(&KnowledgePointNodePage::OnMouseLeaveKnowledgePointTileItem, this, _1));
		tile_page->Init(L"/course/knowledge_point_tile.xml", pm_, box_knowledge_points_);
	}
}

void KnowledgePointNodePage::ShowSupportingExercises(const DirNodeVideoVec & exercises)
{
	using namespace std::placeholders;
	bool visible = !exercises.empty();
	box_empty_exercise_->SetVisible(!visible);
	list_supporting_exercises_->SetVisible(visible && view_style_ == kViewList);
	box_supporting_exercises_->SetVisible(visible && view_style_ == kViewTile);

	for (auto & k : exercises)
	{
		KnowledgeListItemBox *knl_page = new KnowledgeListItemBox(k, false);
		knl_page->Init(L"/course/knowledge_list_page.xml", pm_, list_supporting_exercises_);

		KnowledgePointTileItemBox *tile_page = new KnowledgePointTileItemBox(140, 110,
			std::bind(&KnowledgePointNodePage::OnMouseClickKnowledgePointTileItem, this, _1, _2),
			std::bind(&KnowledgePointNodePage::OnMouseHoverKnowledgePointTileItem, this, _1),
			std::bind(&KnowledgePointNodePage::OnMouseLeaveKnowledgePointTileItem, this, _1));
		tile_page->Init(L"/course/knowledge_point_tile.xml", pm_, box_supporting_exercises_);
	}
}

void KnowledgePointNodePage::ShowSupportingPapers(const DirNodeVideoVec & papers)
{
	using namespace std::placeholders;
	bool visible = !papers.empty();
	box_empty_papers_->SetVisible(!visible);
	list_supporting_papers_->SetVisible(visible && view_style_ == kViewList);
	box_supporting_papers_->SetVisible(visible && view_style_ == kViewTile);

	for (auto & k : papers)
	{
		KnowledgeListItemBox *knl_page = new KnowledgeListItemBox(k, false);
		knl_page->Init(L"/course/knowledge_list_page.xml", pm_, list_supporting_papers_);

		KnowledgePointTileItemBox *tile_page = new KnowledgePointTileItemBox(140, 110,
			std::bind(&KnowledgePointNodePage::OnMouseClickKnowledgePointTileItem, this, _1, _2),
			std::bind(&KnowledgePointNodePage::OnMouseHoverKnowledgePointTileItem, this, _1),
			std::bind(&KnowledgePointNodePage::OnMouseLeaveKnowledgePointTileItem, this, _1));
		tile_page->Init(L"/course/knowledge_point_tile.xml", pm_, box_supporting_papers_);
	}
}

void KnowledgePointNodePage::OnMouseHoverKnowledgePointTileItem(KnowledgePointTileItemBox * page)
{
	page->Box()->SetBkImage(L"/course/bg_fkst_hover.png");
	ui::Control * control_selected = page->FindSubContrl(L"control_selected");
	control_selected->SetBkImage(L"/course/icon_checkbox02.png");
	control_selected->SetVisible(true);
}

void KnowledgePointNodePage::OnMouseLeaveKnowledgePointTileItem(KnowledgePointTileItemBox * page)
{
	page->Box()->SetBkImage(L"");
	ui::Control * control_selected = page->FindSubContrl(L"control_selected");
	control_selected->SetVisible(false);
	return;
}

void KnowledgePointNodePage::OnMouseClickKnowledgePointTileItem(KnowledgePointTileItemBox * page, bool selected)
{
	ui::Control * control_selected = page->FindSubContrl(L"control_selected");
	if (selected) {
		page->Box()->SetBkImage(L"/course/bg_fkst_sel.png");
		control_selected->SetBkImage(L"/course/icon_checkbox02_sel.png");
		control_selected->SetVisible(true);
	}
	else {
		page->Box()->SetBkImage(L"");
		control_selected->SetBkImage(L"/course/icon_checkbox02.png");
		control_selected->SetVisible(false);
	}
	return;
}

void KnowledgePointNodePage::SetViewStyle(ViewStyle s)
{
	if (view_style_ == s) {
		return;
	}

	view_style_ = s;
	bool visible = list_knowledge_points_->GetCount() > 0;
	list_knowledge_points_->SetVisible(visible && view_style_ == kViewList);
	box_knowledge_points_tile_style_->SetVisible(visible && view_style_ == kViewTile);

	visible = list_supporting_exercises_->GetCount() > 0;
	list_supporting_exercises_->SetVisible(visible && view_style_ == kViewList);
	box_supporting_exercises_->SetVisible(visible && view_style_ == kViewTile);

	visible = list_supporting_papers_->GetCount() > 0;
	list_supporting_papers_->SetVisible(visible && view_style_ == kViewList);
	box_supporting_papers_->SetVisible(visible && view_style_ == kViewTile);
}

void KnowledgePointNodePage::FetchNodeInfo(std::string node_code, std::string tagid)
{
	node_code_ = node_code;
	tagid_ = tagid;

	CourseApiCaller::GetInstance()->FetchDirectoryNode(tagid_, node_code_, id_);
}
