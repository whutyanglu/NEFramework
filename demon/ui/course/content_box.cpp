#include "stdafx.h"
#include "content_box.h"
#include "directory_box.h"
#include "directory_tree_view.h"

ContentBox::ContentBox(std::string dir_id)
	: dir_id_(dir_id)
{
}

void ContentBox::OnInit()
{
	ui::VBox *box_directory = dynamic_cast<ui::VBox*>(FindSubContrl(L"box_directory"));
	directory_box_ = std::make_unique<DirectoryBox>(dir_id_);
	directory_box_->Init(L"/course/directory.xml", pm_, box_directory);

	ui::VBox *box_node = dynamic_cast<ui::VBox*>(FindSubContrl(L"box_node"));
	knowledge_point_box_ = std::make_unique<KnowledgePointNodePage>(nullptr);
	knowledge_point_box_->Init(L"/course/knowledge_point.xml", pm_, box_node);


}

