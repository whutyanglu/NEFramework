#include "stdafx.h"
#include "knowledge_point_page.h"
#include "api/net_api.h"
#include "module/course/course_api_caller.h"


KnowledgePointPage::KnowledgePointPage(std::string dir_id, int index, UiPage * back,
	OnGoBack on_goback, OnGoForward on_goforward)
	: UiPage(index, back, on_goback, on_goforward)
	, dir_id_(dir_id)
{

	
}

KnowledgePointPage::~KnowledgePointPage()
{

}

void KnowledgePointPage::OnInit()
{
	// direcoty box
	ui::VBox *box_directory = dynamic_cast<ui::VBox*>(FindSubContrl(L"box_directory"));
	directory_box_ = std::make_unique<DirectoryBox>(dir_id_);
	directory_box_->Init(L"/course/directory.xml", pm_, box_directory);
	directory_box_->GetTreeView()->RegisterOnDirNodeSelected([this](DirectoryTreeMode mode, DirectoryTreeNode* node) {
		if (mode == kDirTreeModeEdit) {
			if (node != NULL) {
				GenerateKnowledgeNode(node->GetDirectoryNode().node_code, node->GetDirectoryNode().tag_id);
			}
		}	
	});

	box_node_container_ = dynamic_cast<ui::Box*>(FindSubContrl(L"box_knowledgepoint_node"));

	ui::Option *opt_list_style = dynamic_cast<ui::Option*>(FindSubContrl(L"opt_list_style"));
	opt_list_style->AttachSelectedChanged([this](const ui::EventArgs &arg) {
		current_page_->SetViewStyle(arg.w_param_ == 0 ?
			KnowledgePointNodePage::kViewList : KnowledgePointNodePage::kViewTile);
		return true;
	});
}

UiPage * KnowledgePointPage::BeforeGoBack()
{
	if (current_page_ != nullptr) {
		auto ptr = current_page_->GetBack();
		if (ptr != NULL) {
			OnShowPage(ptr, true);
			return this;
		}
	}
	
	return __super::BeforeGoBack();
}

UiPage * KnowledgePointPage::BeforeGoForward()
{
	if (current_page_ != nullptr) {
		auto ptr = current_page_->GetForward();
		if (ptr != NULL) {
			OnShowPage(ptr, true);
			return this;
		}
	}

	return __super::BeforeGoForward();
}

void KnowledgePointPage::GenerateKnowledgeNode(const std::string& node_code, const std::string& tagid)
{
	auto ptr = std::make_unique<KnowledgePointNodePage>(current_page_);
	ptr->Init(L"/course/knowledge_point_node.xml", pm_, box_node_container_);
	ptr->FetchNodeInfo(node_code, tagid);

	current_page_ = ptr.get();
	pages_.push_back(std::move(ptr));
	
	OnShowPage(current_page_);
}

void KnowledgePointPage::OnShowPage(UiPage * page, bool send_notify)
{
	box_node_container_->RemoveAll();
	box_node_container_->Add(page->Box());
	current_page_ = dynamic_cast<KnowledgePointNodePage*>(page);

	if (send_notify) {
		std::string node_code = current_page_->GetNodeCode();
		directory_box_->SelectNode(node_code);
	}
}

