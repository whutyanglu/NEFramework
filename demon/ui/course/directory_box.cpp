#include "stdafx.h"
#include "directory_box.h"
#include "directory_tree_view.h"
#include "user/directory.h"
#include "module/course/course_api_caller.h"
#include "base/callback/callback.h"

DirectoryBox::DirectoryBox(std::string dir_id)
	: dir_id_(dir_id)
{
	
}

void DirectoryBox::OnInit()
{
	using namespace std::placeholders;
	tree_directory_ = dynamic_cast<DirectoryTreeView*>(FindSubContrl(L"tree_directory"));
	root_node_ = dynamic_cast<DirectoryTreeNode*>(FindSubContrl(L"directory_root_node"));
	box_empty_directory_ = FindSubContrl(L"box_empty_directory");

	CourseApiCaller::GetInstance()->RegisterOnFetchDirectory(id_,
		ToWeakCallback(nbase::Bind(&DirectoryBox::OnFetchDirectory, this, _1, _2, _3, _4)));
	CourseApiCaller::GetInstance()->FetchDirectory(dir_id_, "", id_);
}

void DirectoryBox::SetEmptyDirectory()
{
	tree_directory_->SetVisible(false);
	box_empty_directory_->SetVisible(true);
}

void DirectoryBox::OnFetchDirectory(bool succ, std::string message, std::string node_code, 
	const std::vector<DirectoryNode>& directory)
{
	using namespace std::placeholders;

	for (auto & d : directory)
	{
		DirectoryTreeNode * node = root_node_->Add(node_code, d, 
			nbase::Bind(&DirectoryBox::OnTreeNodeExpand, this, _1, _2));
	}

	if (node_code.empty() && !directory.empty()) {
		tree_directory_->SelectItem(1, true); 
	}
}

void DirectoryBox::OnTreeNodeExpand(bool is_expaned, DirectoryTreeNode * node)
{
	if (is_expaned && node && !node->GetDirectoryNode().is_end && !node->IsHasChild()) {
		CourseApiCaller::GetInstance()->FetchDirectory(dir_id_, node->GetDirectoryNode().node_code, id_);
	}
}

void DirectoryBox::SelectNode(const std::string & node_code)
{
	root_node_->SelectNode(node_code);
}

