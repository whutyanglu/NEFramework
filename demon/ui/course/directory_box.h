#ifndef MSYK_UI_DIRECTORY_BOX_H__
#define MSYK_UI_DIRECTORY_BOX_H__

#include <map>
#include "dynamic_box.h"
#include "directory_tree_view.h"

class  DirectoryTreeView;
class  DirectoryTreeNode;
struct DirectoryNode;

using OnInitialNodeFetched = std::function<void(const std::string&, const std::string&)>;

class DirectoryBox : public DynamicBox
{
public:
	DirectoryBox(std::string dir_id);
	virtual void OnInit();

public:
	void SetEmptyDirectory();
	void OnFetchDirectory(bool succ, std::string message, std::string node_code, const std::vector<DirectoryNode> &directory);
	void OnTreeNodeExpand(bool expanded, DirectoryTreeNode* node);
	void SelectNode(const std::string &node_code);

	DirectoryTreeView *GetTreeView() {
		return tree_directory_;
	}

private:
	DirectoryTreeView *tree_directory_ = nullptr;
	DirectoryTreeNode *root_node_ = nullptr;
	ui::Control  *box_empty_directory_ = nullptr;
	ui::Button   *btn_refer_directory = nullptr;
	ui::Button   *btn_create_directory = nullptr;

	OnDirNodeExpand  on_node_expand_ = nullptr;
	OnDirNodeSelected on_node_selected = nullptr;

private:
	std::string dir_id_;
};


#endif // MSYK_UI_DIRECTORY_BOX_H__