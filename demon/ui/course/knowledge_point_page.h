#ifndef MSYK_KNOWLEDGE_POINT_PAGE_H__
#define MSYK_KNOWLEDGE_POINT_PAGE_H__

#include "ui_page.h"
#include "knowledge_point_node_page.h"
#include "directory_box.h"

class KnowledgePointPage : public UiPage
{
public:
	KnowledgePointPage(std::string dir_id, int index, UiPage *back, 
		OnGoBack on_goback = nullptr, OnGoForward on_gonext = nullptr);
	~KnowledgePointPage();

	virtual void OnInit();
	virtual UiPage* BeforeGoBack() override;
	virtual UiPage* BeforeGoForward() override;

	void GenerateKnowledgeNode(const std::string& node_code, const std::string& tagid);

private:
	void OnShowPage(UiPage *page, bool send_notify = false);

private:
	std::string dir_id_;
	std::unique_ptr<DirectoryBox> directory_box_ = nullptr;
	ui::Box *box_node_container_ = nullptr;

	KnowledgePointNodePage *current_page_ = nullptr;
	std::list<std::unique_ptr<UiPage>> pages_;		// ÀúÊ·pages
};

#endif // MSYK_KNOWLEDGE_POINT_PAGE_H__