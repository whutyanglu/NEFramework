#ifndef MSYK_UI_CONTENT_BOX_H__
#define MSYK_UI_CONTENT_BOX_H__

#include "../common/dynamic_box.h"
#include "../common/directory_tree_view.h"
#include "directory_box.h"
#include "knowledge_point_node_page.h"
#include "user/directory.h"


class ContentBox : public DynamicBox
{
public:
	ContentBox(std::string dir_id);
	virtual void OnInit();

private:
	std::unique_ptr<DirectoryBox> directory_box_ = nullptr;
	std::unique_ptr<KnowledgePointNodePage> knowledge_point_box_ = nullptr;

	std::string dir_id_;
};

#endif // MSYK_UI_CONTENT_BOX_H__