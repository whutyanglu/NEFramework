#ifndef MSYK_UI_KNOWLEDGE_LIST_ITEM_BOX_H__
#define MSYK_UI_KNOWLEDGE_LIST_ITEM_BOX_H__

#include "dynamic_box.h"
#include "user/directory.h"

class KnowledgeListItemBox : public DynamicBox
{
public:
	KnowledgeListItemBox(const DirectoryNodeVideo &node, bool show_checkbox = true);
	virtual void OnInit() override;

private:
	bool show_checkbox_ = true;
	DirectoryNodeVideo node_;
};


#endif // MSYK_UI_KNOWLEDGE_LIST_ITEM_BOX_H__