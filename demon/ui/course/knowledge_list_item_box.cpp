#include "stdafx.h"
#include "knowledge_list_item_box.h"

KnowledgeListItemBox::KnowledgeListItemBox(const DirectoryNodeVideo &node, bool show_checkbox)
	: node_(node)
	, show_checkbox_(show_checkbox)
{
}

void KnowledgeListItemBox::OnInit()
{
	box_->SetFixedHeight(45);
	FindSubContrl(L"checkbox_select")->SetVisible(show_checkbox_);

	// TODO : 根据url 下载图片显示
	FindSubContrl(L"control_icon")->SetBkImage(node_.picture_path);
	FindSubContrl(L"label_name")->SetUTF8Text(node_.name);
	FindSubContrl(L"label_date")->SetUTF8Text(node_.upload_time);
}
