#ifndef MSYK_BOOK_TREE_VIEW_H__
#define MSYK_BOOK_TREE_VIEW_H__

#include "user/directory.h"

using namespace ui;

class DirectoryTreeNode;

enum  DirectoryTreeMode
{
	kDirTreeModeEdit,
	kDirTreeModeScan,
	kDirTreeModeVideoRecord,
};

using OnDirNodeExpand = std::function<void(bool, DirectoryTreeNode*)>;
using OnDirNodeDelete = std::function<void()>;
using OnDirNodeEdit = std::function<void(std::string)>;
using OnDirNodeSelected = std::function<void(DirectoryTreeMode, DirectoryTreeNode*)>;
using OnDirNodeTrace = std::function<void(DirectoryTreeNode*)>;

class DirectoryTreeNode : public TreeNode
{
public:
	enum state
	{
		state_view,
		state_edit,
	};
	DECLARE_DUICONTROL(DirectoryTreeNode)
	DirectoryTreeNode(DirectoryNode node = DirectoryNode(),  DirectoryTreeNode *parent_node = nullptr);
	virtual ~DirectoryTreeNode();

	virtual std::wstring GetClass() const override;
	virtual LPVOID GetInterface(std::wstring pstrName) override;

	// 通过根节点加载
	DirectoryTreeNode* Add(const std::string &node_code, const DirectoryNode &node,
		OnDirNodeExpand on_expand = nullptr, OnDirNodeDelete on_delete = nullptr, OnDirNodeEdit on_edit = nullptr);
	virtual bool  Add(Control* pControl) override;
	virtual bool  AddAt(Control* pControl, int iIndex) override;
	virtual bool  AddChildNode(TreeNode* pControl) override;
	virtual void  DoEvent(EventArgs& event) override;

	void SelectNode(const std::string &node_code, bool send_notify = false);
	const DirectoryNode&  GetDirectoryNode() {
		return directory_node_;
	}
	
	void RegisterCallback(OnDirNodeExpand on_expand = nullptr, OnDirNodeDelete on_delete = nullptr, OnDirNodeEdit on_edit = nullptr);

private:
	DirectoryTreeNode* GetNode(DirectoryTreeNode *node, const std::string &node_code, OnDirNodeTrace cb_trace = nullptr);
	
private:
	bool OnClickAdd(const ui::EventArgs &arg);
	bool OnClickRename(const ui::EventArgs &arg);
	bool OnClickDelete(const ui::EventArgs &arg);
	bool OnEditReturn(const ui::EventArgs &arg);
	bool OnEditKillFocus(const ui::EventArgs &arg);
	bool OnEditSetFocus(const ui::EventArgs &arg);
	bool OnMouseEnter(const ui::EventArgs &arg);
	bool OnMouseLeave(const ui::EventArgs &arg);

	void SaveName();

	bool OnExpand(const ui::EventArgs &arg);

private:
	OnDirNodeExpand on_expand_ = nullptr;
	OnDirNodeDelete on_delete_ = nullptr;
	OnDirNodeEdit   on_edit_   = nullptr;
	state        state_		= state_view;

private:
	HBox	*hbox_controller_ = nullptr;
	Button	*btn_add_    = nullptr;
	Button  *btn_edit_ = nullptr;
	Button  *btn_delete_ = nullptr;
	Edit    *edt_name_	 = nullptr;

private:
	DirectoryNode directory_node_;
};

class DirectoryTreeView : public ui::TreeView
{
public:
	DECLARE_DUICONTROL(DirectoryTreeView)
	DirectoryTreeView();
	virtual ~DirectoryTreeView();

	virtual std::wstring GetClass() const;
	virtual LPVOID	GetInterface(std::wstring pstrName);

	virtual bool Add(TreeNode *pControl) override;
	virtual long AddAt(TreeNode* pControl, int iIndex) override;
	void RegisterOnDirNodeSelected(OnDirNodeSelected cb) {
		on_item_selected_ = cb;
	}

protected:
	bool OnItemSelected(const ui::EventArgs &arg);

private:
	DirectoryTreeMode mode_ = kDirTreeModeEdit;
	OnDirNodeSelected on_item_selected_ = nullptr;
};


#endif //~MSYK_BOOK_TREE_VIEW_H__