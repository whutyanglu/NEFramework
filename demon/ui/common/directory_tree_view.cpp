#include "stdafx.h"
#include "directory_tree_view.h"

IMPLEMENT_DUICONTROL(DirectoryTreeNode)

DirectoryTreeNode::DirectoryTreeNode(DirectoryNode node, DirectoryTreeNode *parent_node)
	: directory_node_(node)
	, TreeNode(parent_node)
{
	using namespace std::placeholders;

	btn_add_ = new Button;
	btn_add_->ApplyAttributeList(L"class=\"btn_list_add\"");
	btn_add_->AttachClick(nbase::Bind(&DirectoryTreeNode::OnClickAdd, this, _1));

	btn_edit_ = new Button;
	btn_edit_->ApplyAttributeList(L"class=\"btn_list_edit\"");
	btn_edit_->AttachClick(nbase::Bind(&DirectoryTreeNode::OnClickRename, this, _1));
	
	btn_delete_ = new Button;
	btn_delete_->ApplyAttributeList(L"class=\"btn_list_del\"");
	btn_delete_->AttachClick(nbase::Bind(&DirectoryTreeNode::OnClickDelete, this, _1));

	hbox_controller_ = new HBox();
	hbox_controller_->Add(btn_add_);
	hbox_controller_->Add(btn_edit_);
	hbox_controller_->Add(btn_delete_);
	hbox_controller_->ApplyAttributeList(L"width=\"auto\" childpadding=\"10\" visible=\"false\" inset=\"0,5,10,0\"");

	edt_name_ = new Edit();
	edt_name_->ApplyAttributeList(L"width=\"100\" height=\"24\" visible=\"false\"");
	edt_name_->AttachReturn(nbase::Bind(&DirectoryTreeNode::OnEditReturn, this, _1));
	edt_name_->AttachKillFocus(nbase::Bind(&DirectoryTreeNode::OnEditKillFocus, this, _1));
	edt_name_->AttachSetFocus(nbase::Bind(&DirectoryTreeNode::OnEditSetFocus, this, _1));
	edt_name_->AttachReturn(nbase::Bind(&DirectoryTreeNode::OnEditReturn, this, _1));
	edt_name_->AttachKillFocus(nbase::Bind(&DirectoryTreeNode::OnEditKillFocus, this, _1));
	edt_name_->AttachSetFocus(nbase::Bind(&DirectoryTreeNode::OnEditSetFocus, this, _1));

	hbox_->Add(edt_name_);
	hbox_->Add(new Control());
	hbox_->Add(hbox_controller_);

	AttachMouseEnter([this](const ui::EventArgs &arg) {
		hbox_controller_->SetVisible(true);
		return true; 
	});
	AttachMouseLeave([this](const ui::EventArgs &arg) {
		hbox_controller_->SetVisible(false);
		return true;
	});
}

DirectoryTreeNode::~DirectoryTreeNode()
{
}

std::wstring DirectoryTreeNode::GetClass() const
{
	return L"DirectoryTreeNode";
}

LPVOID DirectoryTreeNode::GetInterface(std::wstring pstrName)
{
	if (_tcsicmp(pstrName.c_str(), _T("DirectoryTreeNode")) == 0)
		return static_cast<DirectoryTreeNode*>(this);
	return __super::GetInterface(pstrName);
}

DirectoryTreeNode* DirectoryTreeNode::Add(const std::string &node_code, const DirectoryNode &dir_node,
	OnDirNodeExpand on_expand, OnDirNodeDelete on_delete, OnDirNodeEdit on_edit)
{
	DirectoryTreeNode* node = NULL;
	DirectoryTreeNode* tmp_node = NULL;
	DirectoryTreeNode* parent_node = NULL;

	if (node_code.empty()) {
		parent_node = this;
	}
	else {
		parent_node = GetNode(this, node_code);
	}
	
	if (parent_node != NULL) {
		node = new DirectoryTreeNode(dir_node, parent_node);
		node->ApplyAttributeList(L"class=\"treenode_common\"");
		node->SetTreeLevel(dir_node.code_level);
		node->option_->SetUTF8Text(dir_node.name);
		parent_node->Add(node);

		int indentation = GetTreeView()->GetItemIndentation();
		int max_indentation = GetTreeView()->GetMaxItemIndentation();
		node->SetIndentation(indentation, max_indentation);

		node->on_expand_ = on_expand;
		node->on_delete_ = on_delete;
		node->on_edit_ = on_edit;

		node->folder_button_->AttachSelectedChanged(nbase::Bind(&DirectoryTreeNode::OnExpand, node, std::placeholders::_1));
		node->AttachDoubleClick(nbase::Bind(&DirectoryTreeNode::OnExpand, node, std::placeholders::_1));
	}

	return node;
}

bool DirectoryTreeNode::Add(Control * pControl)
{
	if (_tcsicmp(pControl->GetClass().c_str(), _T("DirectoryTreeNode")) == 0)
		return AddChildNode((TreeNode*)pControl);

	return ListContainerElement::Add(pControl);
}

bool DirectoryTreeNode::AddAt(Control * pControl, int iIndex)
{
	if (NULL == static_cast<DirectoryTreeNode*>(pControl->GetInterface(_T("DirectoryTreeNode"))))
		return false;

	DirectoryTreeNode* pIndexNode = static_cast<DirectoryTreeNode*>(nodes_.GetAt(iIndex));
	if (!pIndexNode) {
		if (!nodes_.Add(pControl))
			return false;
	}
	else if (pIndexNode && !nodes_.InsertAt(iIndex, pControl))
		return false;

	if (!pIndexNode && tree_view_ && tree_view_->GetItemAt(GetTreeIndex() + 1))
		pIndexNode = static_cast<DirectoryTreeNode*>(tree_view_->GetItemAt(GetTreeIndex() + 1)->GetInterface(_T("DirectoryTreeNode")));

	pControl = CalLocation((TreeNode*)pControl);

	if (tree_view_ && pIndexNode)
		return tree_view_->AddAt((TreeNode*)pControl, pIndexNode);
	else
		return tree_view_->Add((TreeNode*)pControl);

	return true;
}

bool DirectoryTreeNode::AddChildNode(TreeNode * pControl)
{
	if (!pControl)
		return false;
	
	if (_tcsicmp(pControl->GetClass().c_str(), _T("DirectoryTreeNode")) != 0)
		return false;

	pControl = CalLocation(pControl);

	bool nRet = true;
	DirectoryTreeNode* pNode = static_cast<DirectoryTreeNode*>(nodes_.GetAt(nodes_.GetSize() - 1));
	if (tree_view_) {
		if (!pNode || !pNode->GetLastNode())
			nRet = tree_view_->AddAt(pControl, GetTreeIndex() + 1) >= 0;
		else nRet = tree_view_->AddAt(pControl, pNode->GetLastNode()->GetTreeIndex() + 1) >= 0;
	}

	if (nRet) {
		nodes_.Add(pControl);
	}
	return nRet;
}

void DirectoryTreeNode::DoEvent(EventArgs & event)
{
	__super::DoEvent(event);

	if (!IsEnabled() || state_ == state_edit) {
		return;
	}

	if (event.type_ == kEventMouseEnter || event.type_ == kEventMouseLeave) {
		Notify(event);
	}
}

void DirectoryTreeNode::SelectNode(const std::string & node_code, bool send_notify)
{
	DirectoryTreeNode *node = GetNode(this, node_code);
	if (node) {
		TreeView *tree_view = GetTreeView();
		tree_view->ExpandItemReverse(node, true);
		node->Select(true, send_notify);
	}
}

void DirectoryTreeNode::RegisterCallback(OnDirNodeExpand on_expand, OnDirNodeDelete on_delete, OnDirNodeEdit on_edit)
{
	on_expand_ = on_expand;
	on_delete_ = on_delete;	
	on_edit_ = on_edit;

	folder_button_->AttachClick(nbase::Bind(&DirectoryTreeNode::OnExpand, this, std::placeholders::_1));
	AttachDoubleClick(nbase::Bind(&DirectoryTreeNode::OnExpand, this, std::placeholders::_1));
}

DirectoryTreeNode * DirectoryTreeNode::GetNode(DirectoryTreeNode * node, const std::string & node_code, OnDirNodeTrace cb_trace)
{
	DirectoryTreeNode *tmp_node = nullptr;
	if (!node) {
		return tmp_node;
	}
	else {
		if (cb_trace) {
			cb_trace(node);
		}
	}

	if (node && node->directory_node_.node_code == node_code) {
		return node;
	}

	if (node->IsHasChild()) {
		for (int i = 0; i < node->nodes_.GetSize(); ++i)
		{
			tmp_node = static_cast<DirectoryTreeNode*>(node->nodes_.GetAt(i));
			tmp_node = GetNode(tmp_node, node_code);
			if (tmp_node) {
				return tmp_node;
			}
		}
	}

	return tmp_node;
}

bool DirectoryTreeNode::OnClickAdd(const ui::EventArgs & arg)
{
	std::wstring name;
	/*if (on_add_) {
		on_add_(this);
	}

	if (name.empty()) {
		name = L"新建节点";
	}

	BookTreeNode *node = new BookTreeNode(DirectoryNode(), this);
	node->ApplyAttributeList(L"class=\"treenode_common\"");
	node->SetTreeLevel(level_ + 1);
	node->option_->SetText(name);
	Add(node);

	int indentation = GetTreeView()->GetItemIndentation();
	int max_indentation = GetTreeView()->GetMaxItemIndentation();
	node->SetIndentation(indentation, max_indentation);
*/
	return true;
}

bool DirectoryTreeNode::OnClickRename(const ui::EventArgs & arg)
{
	RECT rect = option_->GetRelativePos();
	option_->SetVisible(false);
	edt_name_->SetVisible(true);
	edt_name_->SetPos(rect);
	edt_name_->SetText(option_->GetText());
	edt_name_->SetFocus();

	state_ = state_edit;
	hbox_controller_->SetVisible(false);

	return true;
}

bool DirectoryTreeNode::OnClickDelete(const ui::EventArgs & arg)
{
	if (parent_) {
		parent_->Remove(this);
	}
	else {
		GetTreeView()->RemoveAll();
	}
	
	if (on_delete_) {
		on_delete_();
	}

	return true;
}

bool DirectoryTreeNode::OnEditReturn(const ui::EventArgs & arg)
{
	option_->SetFocus();
	return true;
}

bool DirectoryTreeNode::OnEditKillFocus(const ui::EventArgs & arg)
{
	SaveName();
	return true;
}

bool DirectoryTreeNode::OnEditSetFocus(const ui::EventArgs & arg)
{
	return true;
}

bool DirectoryTreeNode::OnMouseEnter(const ui::EventArgs & arg)
{
	hbox_->SetVisible(true);
	return true;
}

bool DirectoryTreeNode::OnMouseLeave(const ui::EventArgs & arg)
{
	hbox_->SetVisible(false);
	return true;
}

void DirectoryTreeNode::SaveName()
{
	edt_name_->SetVisible(false);
	std::wstring name = edt_name_->GetText();
	if (!name.empty()) {
		option_->SetText(name);
		option_->SetVisible(true);
	}
	state_ = state_view;
}

bool DirectoryTreeNode::OnExpand(const ui::EventArgs & arg)
{
	if (on_expand_) {
		on_expand_(folder_button_->GetCheck(), this);
	}
	return true;
}

IMPLEMENT_DUICONTROL(DirectoryTreeView)
DirectoryTreeView::DirectoryTreeView()
{
	AttachItemSelected(nbase::Bind(&DirectoryTreeView::OnItemSelected, this, std::placeholders::_1));
}

DirectoryTreeView::~DirectoryTreeView()
{
}

std::wstring DirectoryTreeView::GetClass() const
{
	return L"DirectoryTreeView";
}

LPVOID DirectoryTreeView::GetInterface(std::wstring pstrName)
{
	if (_tcsicmp(pstrName.c_str(), _T("DirectoryTreeView")) == 0)
		return static_cast<DirectoryTreeView*>(this);
	return __super::GetInterface(pstrName);
}

bool DirectoryTreeView::Add(TreeNode * pControl)
{
	if(!pControl) return false;
	if (_tcsicmp(pControl->GetClass().c_str(), _T("DirectoryTreeNode")) != 0) return false;

	pControl->AttachDoubleClick(std::bind(&TreeView::OnDBClickItem, this, std::placeholders::_1));
	pControl->GetFolderButton()->AttachSelectedChanged(std::bind(&TreeView::OnFolderChanged, this, std::placeholders::_1));
	pControl->GetCheckBox()->AttachSelectedChanged(std::bind(&TreeView::OnCheckBoxChanged, this, std::placeholders::_1));

	pControl->SetVisibleFolderBtn(folder_btn_visible_);
	pControl->SetVisibleCheckBtn(check_btn_visible_);
	if (item_min_width_ > 0)
		pControl->SetMinWidth(item_min_width_);

	List::Add(pControl);

	if (pControl->GetCountChild() > 0) {
		int nCount = pControl->GetCountChild();
		for (int nIndex = 0; nIndex < nCount; nIndex++) {
			TreeNode* pNode = pControl->GetChildNode(nIndex);
			if (pNode) Add(pNode);
		}
	}

	pControl->SetTreeView(this);
	return true;
}

long DirectoryTreeView::AddAt(TreeNode * pControl, int iIndex)
{
	if (!pControl) return -1;
	if (_tcsicmp(pControl->GetClass().c_str(), _T("DirectoryTreeNode")) != 0) return -1;
	pControl->AttachDoubleClick(std::bind(&TreeView::OnDBClickItem, this, std::placeholders::_1));
	pControl->GetFolderButton()->AttachSelectedChanged(std::bind(&TreeView::OnFolderChanged, this, std::placeholders::_1));
	pControl->GetCheckBox()->AttachSelectedChanged(std::bind(&TreeView::OnCheckBoxChanged, this, std::placeholders::_1));

	pControl->SetVisibleFolderBtn(folder_btn_visible_);
	pControl->SetVisibleCheckBtn(check_btn_visible_);
	
	if (item_min_width_ > 0) {
		pControl->SetMinWidth(item_min_width_);
	}
	List::AddAt(pControl, iIndex);
	if (pControl->GetCountChild() > 0) {
		int nCount = pControl->GetCountChild();
		for (int nIndex = 0; nIndex < nCount; nIndex++) {
			TreeNode* pNode = pControl->GetChildNode(nIndex);
			if (pNode)
				return AddAt(pNode, iIndex + 1);
		}
	}
	else {
		return iIndex + 1;
	}

	return -1;
}


bool DirectoryTreeView::OnItemSelected(const ui::EventArgs & arg)
{
	int index = arg.w_param_;
	assert(index >= 0 && index < GetCount());
	DirectoryTreeNode *node = dynamic_cast<DirectoryTreeNode*>(GetItemAt(index));
	if (on_item_selected_ && node) {
		on_item_selected_(mode_, node);
	}

	return true;
}

