#ifndef MSYK_KNOWLEDGE_POINT_BOX_H__
#define MSYK_KNOWLEDGE_POINT_BOX_H__

#include "ui_page.h"
#include "directory_box.h"
#include "knowledge_list_item_box.h"
#include "knowledge_point_tile_item_box.h"

class KnowledgePointNodePage : public UiPage
{
public:
	enum ViewStyle
	{
		kViewList,
		kViewTile,
	};

	KnowledgePointNodePage(UiPage *back, OnGoBack on_goback = nullptr, OnGoForward on_goforward = nullptr);

	virtual void OnInit();
	void OnFetchDirectoryTreeNode(bool succ, std::string message, std::string node_code, 
		const DirNodeVideoVec &knowledges, const DirNodeVideoVec &exercises, const DirNodeVideoVec & papers);
	void ShowKnowledgePoints(const DirNodeVideoVec &knowledges);
	void ShowSupportingExercises(const DirNodeVideoVec &knowledges);
	void ShowSupportingPapers(const DirNodeVideoVec &knowledges);

	void OnMouseHoverKnowledgePointTileItem(KnowledgePointTileItemBox* page);
	void OnMouseLeaveKnowledgePointTileItem(KnowledgePointTileItemBox* page);
	void OnMouseClickKnowledgePointTileItem(KnowledgePointTileItemBox* page, bool selected);
	ViewStyle GetViewStyle() {
		return view_style_;
	}
	void SetViewStyle(ViewStyle s);
	void FetchNodeInfo(std::string node_code, std::string tagid);
	std::string GetNodeCode() {
		return node_code_;
	}
	
private:
	std::string node_code_;
	std::string tagid_;
	// 知识点
	ui::List *list_knowledge_points_ = nullptr;
	ui::HBox *box_knowledge_points_ = nullptr;
	ui::Box  *box_knowledge_points_tile_style_ = nullptr;
	ui::Box *box_empty_knowledge_point_ = nullptr;

	// 相关练习
	ui::HBox *box_supporting_exercises_ = nullptr;
	ui::List *list_supporting_exercises_ = nullptr;
	ui::Box *box_empty_exercise_ = nullptr;

	// 相关试卷
	ui::HBox *box_supporting_papers_ = nullptr;
	ui::List *list_supporting_papers_ = nullptr;
	ui::Box *box_empty_papers_ = nullptr;

	ViewStyle view_style_ = kViewList;
	 
};


#endif // MSYK_KNOWLEDGE_POINT_BOX_H__