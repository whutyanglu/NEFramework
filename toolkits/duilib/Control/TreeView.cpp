#include "StdAfx.h"
#include "TreeView.h"

#pragma warning( disable: 4251 )
namespace ui
{
	IMPLEMENT_DUICONTROL(TreeNode)

	//************************************
	// 函数名称: CTreeNodeUI
	// 返回类型: 
	// 参数信息: CTreeNodeUI * _ParentNode
	// 函数说明: 
	//************************************
	TreeNode::TreeNode( TreeNode* _ParentNode /*= NULL*/ )
	{
		text_color_ = 0x00000000;
		tect_hot_color_ = 0;
		selected_text_color_ = 0;
		selected_text_hot_color_	= 0;

		tree_view_ = NULL;
		level_ = 0;
		visible_ = TRUE;
		is_checkbox_ = FALSE;
		parent_	= NULL;

		hbox_ = new HBox();
		folder_button_ = new CheckBox();
		dotted_line_ = new Label();
		checkbox_ = new CheckBox();
		option_ = new Option();

		this->SetFixedHeight(18);
		this->SetFixedWidth(250);
		folder_button_->SetFixedWidth(GetFixedHeight());
		//dotted_line_->SetFixedWidth(2);
		dotted_line_->SetPadding({ 2,0,0,0});
		checkbox_->SetFixedWidth(GetFixedHeight());
		option_->SetAttribute(_T("align"),_T("left"));
		dotted_line_->SetVisible(FALSE);
		checkbox_->SetVisible(FALSE);
		option_->SetMouseEnabled(FALSE);

		if(_ParentNode) {
			// TODO : luyang
			//if (_tcsicmp(_ParentNode->GetClass().c_str(), _T("TreeNode")) != 0) return;
			dotted_line_->SetVisible(_ParentNode->IsVisible());
			this->SetParentNode(_ParentNode);
		}
		hbox_->SetChildVAlign(DT_VCENTER);
		hbox_->Add(dotted_line_);
		hbox_->Add(folder_button_);
		hbox_->Add(checkbox_);
		hbox_->Add(option_);
		Add(hbox_);
	}
	
	//************************************
	// 函数名称: ~CTreeNodeUI
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	TreeNode::~TreeNode( void )
	{

	}

	//************************************
	// 函数名称: GetClass
	// 返回类型: std::wstring
	// 函数说明: 
	//************************************
	std::wstring TreeNode::GetClass() const
	{
		return _T("TreeNode");
	}

	//************************************
	// 函数名称: GetInterface
	// 返回类型: LPVOID
	// 参数信息: std::wstring pstrName
	// 函数说明: 
	//************************************
	LPVOID TreeNode::GetInterface( std::wstring pstrName )
	{
		if( _tcsicmp(pstrName.c_str(), _T("TreeNode")) == 0 )
			return static_cast<TreeNode*>(this);
		return ListContainerElement::GetInterface(pstrName);
	}

	void TreeNode::SetText(std::wstring txt)
	{
		option_->SetText(txt);
	}

	void TreeNode::AttachDoubleClick(const EventCallback & cb)
	{
		on_event_[kEventMouseDoubleClick] += cb;
	}
	
	//************************************
	// 函数名称: DoEvent
	// 返回类型: void
	// 参数信息: TEventUI & event
	// 函数说明:
	//************************************
	void TreeNode::DoEvent( EventArgs& event )
	{
		event.sender_ = this;

		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( owner_ != NULL ) owner_->DoEvent(event);
			else Box::DoEvent(event);
			return;
		}
		ListContainerElement::DoEvent(event);
		if( event.type_ == kEventMouseDoubleClick ) {
			if( IsEnabled() ) {
				Notify(event);
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseEnter ) {
			if( IsEnabled()) {
				if(selected_ && GetSelItemHotTextColor())
					option_->SetTextColor(GetSelItemHotTextColor());
				else
					option_->SetTextColor(GetItemHotTextColor());
			}
			else 
				option_->SetTextColor(option_->GetDisabledTextColor());

			return;
		}
		if( event.type_ == kEventMouseLeave ) {
			if( IsEnabled()) {
				if(selected_ && GetSelItemTextColor())
					option_->SetTextColor(GetSelItemTextColor());
				else if(!selected_)
					option_->SetTextColor(GetItemTextColor());
			}
			else 
				option_->SetTextColor(option_->GetDisabledTextColor());

			return;
		}
	}

	//************************************
	// 函数名称: Invalidate
	// 返回类型: void
	// 函数说明: 
	//************************************
	void TreeNode::Invalidate()
	{
		if( !IsVisible() )
			return;

		if( GetParent() ) {
			Box* pParentContainer = static_cast<Box*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = rect_;
				if( !::IntersectRect(&invalidateRc, &rect_, &rc) ) 
					return;

				Control* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() ) {
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
						return;
				}

				if( pm_ != NULL ) pm_->Invalidate(invalidateRc);
			}
			else {
				Box::Invalidate();
			}
		}
		else {
			Box::Invalidate();
		}
	}
	
	//************************************
	// 函数名称: Select
	// 返回类型: bool
	// 参数信息: bool bSelect
	// 函数说明: 
	//************************************
	bool TreeNode::Select( bool bSelect /*= true*/ , bool send_notify)
	{
		bool nRet = ListContainerElement::Select(bSelect, send_notify);
		if(selected_)
			option_->SetTextColor(GetSelItemTextColor());
		else 
			option_->SetTextColor(GetItemTextColor());

		return nRet;
	}

	bool TreeNode::SelectMulti(bool bSelect)
	{
		bool nRet = ListContainerElement::SelectMulti(bSelect);
		if(selected_)
			option_->SetTextColor(GetSelItemTextColor());
		else 
			option_->SetTextColor(GetItemTextColor());

		return nRet;
	}
	//************************************
	// 函数名称: Add
	// 返回类型: bool
	// 参数信息: CControlUI * _pTreeNodeUI
	// 函数说明: 通过节点对象添加节点
	//************************************
	bool TreeNode::Add( Control* _pTreeNodeUI )
	{
		if (_tcsicmp(_pTreeNodeUI->GetClass().c_str(), _T("TreeNode")) == 0)
			return AddChildNode((TreeNode*)_pTreeNodeUI);
		
		return ListContainerElement::Add(_pTreeNodeUI);
	}

	//************************************
	// 函数名称: AddAt
	// 返回类型: bool
	// 参数信息: CControlUI * pControl
	// 参数信息: int iIndex				该参数仅针对当前节点下的兄弟索引，并非列表视图索引
	// 函数说明: 
	//************************************
	bool TreeNode::AddAt( Control* pControl, int iIndex )
	{
		if(NULL == static_cast<TreeNode*>(pControl->GetInterface(_T("TreeNode"))))
			return FALSE;

		TreeNode* pIndexNode = static_cast<TreeNode*>(nodes_.GetAt(iIndex));
		if(!pIndexNode){
			if(!nodes_.Add(pControl))
				return FALSE;
		}
		else if(pIndexNode && !nodes_.InsertAt(iIndex,pControl))
			return FALSE;

		if(!pIndexNode && tree_view_ && tree_view_->GetItemAt(GetTreeIndex()+1))
			pIndexNode = static_cast<TreeNode*>(tree_view_->GetItemAt(GetTreeIndex()+1)->GetInterface(_T("TreeNode")));

		pControl = CalLocation((TreeNode*)pControl);

		if(tree_view_ && pIndexNode)
			return tree_view_->AddAt((TreeNode*)pControl,pIndexNode);
		else 
			return tree_view_->Add((TreeNode*)pControl);

		return TRUE;
	}

	//************************************
	// 函数名称: Remove
	// 返回类型: bool
	// 参数信息: CControlUI * pControl
	// 函数说明: 
	//************************************
	bool TreeNode::Remove( Control* pControl )
	{
		return RemoveAt((TreeNode*)pControl);
	}

	//************************************
	// 函数名称: SetVisibleTag
	// 返回类型: void
	// 参数信息: bool _IsVisible
	// 函数说明: 
	//************************************
	void TreeNode::SetVisibleTag( bool _IsVisible )
	{
		visible_ = _IsVisible;
	}

	//************************************
	// 函数名称: GetVisibleTag
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool TreeNode::GetVisibleTag()
	{
		return visible_;
	}

	//************************************
	// 函数名称: SetItemText
	// 返回类型: void
	// 参数信息: std::wstring pstrValue
	// 函数说明: 
	//************************************
	void TreeNode::SetItemText( std::wstring pstrValue )
	{
		option_->SetText(pstrValue);
	}

	//************************************
	// 函数名称: GetItemText
	// 返回类型: DuiLib::std::wstring
	// 函数说明: 
	//************************************
	std::wstring TreeNode::GetItemText()
	{
		return option_->GetText();
	}

	//************************************
	// 函数名称: CheckBoxSelected
	// 返回类型: void
	// 参数信息: bool _Selected
	// 函数说明: 
	//************************************
	void TreeNode::CheckBoxSelected( bool _Selected )
	{
		checkbox_->Selected(_Selected);
	}

	//************************************
	// 函数名称: IsCheckBoxSelected
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool TreeNode::IsCheckBoxSelected() const
	{
		return checkbox_->IsSelected();
	}

	//************************************
	// 函数名称: IsHasChild
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool TreeNode::IsHasChild() const
	{
		return !nodes_.IsEmpty();
	}
	
	//************************************
	// 函数名称: AddChildNode
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * _pTreeNodeUI
	// 函数说明: 
	//************************************
	bool TreeNode::AddChildNode( TreeNode* _pTreeNodeUI )
	{
		if (!_pTreeNodeUI)
			return FALSE;

		if (_tcsicmp(_pTreeNodeUI->GetClass().c_str(), _T("TreeNode")) != 0)
			return FALSE;

		_pTreeNodeUI = CalLocation(_pTreeNodeUI);

		bool nRet = TRUE;

		if(tree_view_){
			TreeNode* pNode = static_cast<TreeNode*>(nodes_.GetAt(nodes_.GetSize()-1));
			if(!pNode || !pNode->GetLastNode())
				nRet = tree_view_->AddAt(_pTreeNodeUI,GetTreeIndex()+1) >= 0;
			else nRet = tree_view_->AddAt(_pTreeNodeUI,pNode->GetLastNode()->GetTreeIndex()+1) >= 0;
		}

		if(nRet)
			nodes_.Add(_pTreeNodeUI);

		return nRet;
	}

	//************************************
	// 函数名称: RemoveAt
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * _pTreeNodeUI
	// 函数说明: 
	//************************************
	bool TreeNode::RemoveAt( TreeNode* _pTreeNodeUI )
	{
		int nIndex = nodes_.Find(_pTreeNodeUI);
		TreeNode* pNode = static_cast<TreeNode*>(nodes_.GetAt(nIndex));
		if(pNode && pNode == _pTreeNodeUI)
		{
			while(pNode->IsHasChild())
				pNode->RemoveAt(static_cast<TreeNode*>(pNode->nodes_.GetAt(0)));

			nodes_.Remove(nIndex);

			if(tree_view_)
				tree_view_->Remove(_pTreeNodeUI);

			return TRUE;
		}
		return FALSE;
	}

	//************************************
	// 函数名称: SetParentNode
	// 返回类型: void
	// 参数信息: CTreeNodeUI * _pParentTreeNode
	// 函数说明: 
	//************************************
	void TreeNode::SetParentNode( TreeNode* _pParentTreeNode )
	{
		parent_ = _pParentTreeNode;
	}

	//************************************
	// 函数名称: GetParentNode
	// 返回类型: CTreeNodeUI*
	// 函数说明: 
	//************************************
	TreeNode* TreeNode::GetParentNode()
	{
		return parent_;
	}

	//************************************
	// 函数名称: GetCountChild
	// 返回类型: long
	// 函数说明: 
	//************************************
	long TreeNode::GetCountChild()
	{
		return nodes_.GetSize();
	}

	//************************************
	// 函数名称: SetTreeView
	// 返回类型: void
	// 参数信息: CTreeViewUI * _CTreeViewUI
	// 函数说明: 
	//************************************
	void TreeNode::SetTreeView( TreeView* _CTreeViewUI )
	{
		tree_view_ = _CTreeViewUI;
	}

	//************************************
	// 函数名称: GetTreeView
	// 返回类型: CTreeViewUI*
	// 函数说明: 
	//************************************
	TreeView* TreeNode::GetTreeView()
	{
		return tree_view_;
	}

	//************************************
	// 函数名称: SetAttribute
	// 返回类型: void
	// 参数信息: std::wstring pstrName
	// 参数信息: std::wstring pstrValue
	// 函数说明: 
	//************************************
	void TreeNode::SetAttribute( std::wstring strName, std::wstring strValue )
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		std::wstring cls;
		if(_tcsicmp(pstrName, _T("text")) == 0 )
			option_->SetText(pstrValue);
		else if (_tcsicmp(pstrName, _T("horizattr")) == 0) {
			cls = GlobalManager::GetClass(pstrValue);
			hbox_->ApplyAttributeList(cls);
		}
		else if (_tcsicmp(pstrName, _T("dotlineattr")) == 0) {
			cls = GlobalManager::GetClass(pstrValue);
			dotted_line_->ApplyAttributeList(cls);
		}
		else if (_tcsicmp(pstrName, _T("folderattr")) == 0) {
			cls = GlobalManager::GetClass(pstrValue);
			folder_button_->ApplyAttributeList(cls);
		}
		else if (_tcsicmp(pstrName, _T("checkboxattr")) == 0) {
			cls = GlobalManager::GetClass(pstrValue);
			checkbox_->ApplyAttributeList(cls);
		}
		else if (_tcsicmp(pstrName, _T("itemattr")) == 0) {
			cls = GlobalManager::GetClass(pstrValue);
			option_->ApplyAttributeList(cls);
		}
		else if(_tcsicmp(pstrName, _T("itemtextcolor")) == 0 ){
			SetItemTextColor(GlobalManager::GetColor(strValue));
		}
		else if(_tcsicmp(pstrName, _T("itemhottextcolor")) == 0 ){
			SetItemHotTextColor(GlobalManager::GetColor(strValue));
		}
		else if(_tcsicmp(pstrName, _T("selitemtextcolor")) == 0 ){
			SetSelItemTextColor(GlobalManager::GetColor(strValue));
		}
		else if(_tcsicmp(pstrName, _T("selitemhottextcolor")) == 0 ){
			SetSelItemHotTextColor(GlobalManager::GetColor(strValue));
		}
		else ListContainerElement::SetAttribute(strName, strValue);
	}

	//************************************
	// 函数名称: GetTreeNodes
	// 返回类型: DuiLib::CStdPtrArray
	// 函数说明: 
	//************************************
	CStdPtrArray TreeNode::GetTreeNodes()
	{
		return nodes_;
	}

	//************************************
	// 函数名称: GetChildNode
	// 返回类型: CTreeNodeUI*
	// 参数信息: int _nIndex
	// 函数说明: 
	//************************************
	TreeNode* TreeNode::GetChildNode( int _nIndex )
	{
		return static_cast<TreeNode*>(nodes_.GetAt(_nIndex));
	}

	//************************************
	// 函数名称: SetVisibleFolderBtn
	// 返回类型: void
	// 参数信息: bool _IsVisibled
	// 函数说明: 
	//************************************
	void TreeNode::SetVisibleFolderBtn( bool _IsVisibled )
	{
		folder_button_->SetVisible(_IsVisibled);
	}

	//************************************
	// 函数名称: GetVisibleFolderBtn
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool TreeNode::GetVisibleFolderBtn()
	{
		return folder_button_->IsVisible();
	}

	//************************************
	// 函数名称: SetVisibleCheckBtn
	// 返回类型: void
	// 参数信息: bool _IsVisibled
	// 函数说明: 
	//************************************
	void TreeNode::SetVisibleCheckBtn( bool _IsVisibled )
	{
		checkbox_->SetVisible(_IsVisibled);
	}

	//************************************
	// 函数名称: GetVisibleCheckBtn
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool TreeNode::GetVisibleCheckBtn()
	{
		return checkbox_->IsVisible();
	}
	
	//************************************
	// 函数名称: GetNodeIndex
	// 返回类型: int
	// 函数说明: 取得全局树视图的索引
	//************************************
	int TreeNode::GetTreeIndex()
	{
		if(!tree_view_)
			return -1;

		for(int nIndex = 0;nIndex < tree_view_->GetCount();nIndex++){
			if(this == tree_view_->GetItemAt(nIndex))
				return nIndex;
		}

		return -1;
	}
	
	//************************************
	// 函数名称: GetNodeIndex
	// 返回类型: int
	// 函数说明: 取得相对于兄弟节点的当前索引
	//************************************
	int TreeNode::GetNodeIndex()
	{
		if(!GetParentNode() && !tree_view_)
			return -1;

		if(!GetParentNode() && tree_view_)
			return GetTreeIndex();

		return GetParentNode()->GetTreeNodes().Find(this);
	}

	void TreeNode::SetIndentation(int indentation, int max_indentation)
	{
		int fixed_width = dotted_line_->GetFixedWidth();
		if (fixed_width <= 0) {
			int width = level_ > 0 ? (fixed_width + indentation * (level_)) : fixed_width;
			width = width > (max_indentation + fixed_width) ? (max_indentation == -1 ? width : max_indentation) : width;
			dotted_line_->SetFixedWidth(width);
		}
	}

	//************************************
	// 函数名称: GetLastNode
	// 返回类型: CTreeNodeUI*
	// 函数说明:
	//************************************
	TreeNode* TreeNode::GetLastNode( )
	{
		if(!IsHasChild()) return this;

		TreeNode* nRetNode = NULL;
		for(int nIndex = 0;nIndex < GetTreeNodes().GetSize();nIndex++){
			TreeNode* pNode = static_cast<TreeNode*>(GetTreeNodes().GetAt(nIndex));
			if(!pNode) continue;
			if(pNode->IsHasChild())
				nRetNode = pNode->GetLastNode();
			else 
				nRetNode = pNode;
		}
		
		return nRetNode;
	}
	
	//************************************
	// 函数名称: CalLocation
	// 返回类型: CTreeNodeUI*
	// 参数信息: CTreeNodeUI * _pTreeNodeUI
	// 函数说明: 缩进计算
	//************************************
	TreeNode* TreeNode::CalLocation( TreeNode* _pTreeNodeUI )
	{
		_pTreeNodeUI->GetDottedLine()->SetVisible(TRUE);
		_pTreeNodeUI->SetParentNode(this);
		_pTreeNodeUI->GetItemButton()->SetGroup(option_->GetGroup());
		_pTreeNodeUI->SetTreeView(tree_view_);

	/*	int indentation = GetTreeView()->GetItemIndentation();
		int max_indentaiton = GetTreeView()->GetMaxItemIndentation();
		int width = dotted_line_->GetFixedWidth() + indentation * (_pTreeNodeUI->GetTreeLevel() - 1);
		width = width > max_indentaiton ? (max_indentaiton == -1 ? width : max_indentaiton) : width;
		_pTreeNodeUI->GetDottedLine()->SetFixedWidth(width);*/

		//_pTreeNodeUI->SetIndentation();

		//SetIndentation(GetTreeView()->GetItemIndentation(), GetTreeView()->GetMaxItemIndentation());

		return _pTreeNodeUI;
	}

	//************************************
	// 函数名称: SetTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwTextColor
	// 函数说明: 
	//************************************
	void TreeNode::SetItemTextColor( DWORD _dwItemTextColor )
	{
		text_color_	= _dwItemTextColor;
		option_->SetTextColor(text_color_);
	}

	//************************************
	// 函数名称: GetTextColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	DWORD TreeNode::GetItemTextColor() const
	{
		return text_color_;
	}

	//************************************
	// 函数名称: SetTextHotColor
	// 返回类型: void
	// 参数信息: DWORD _dwTextHotColor
	// 函数说明: 
	//************************************
	void TreeNode::SetItemHotTextColor( DWORD _dwItemHotTextColor )
	{
		tect_hot_color_ = _dwItemHotTextColor;
		Invalidate();
	}

	//************************************
	// 函数名称: GetTextHotColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	DWORD TreeNode::GetItemHotTextColor() const
	{
		return tect_hot_color_;
	}

	//************************************
	// 函数名称: SetSelItemTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwSelItemTextColor
	// 函数说明: 
	//************************************
	void TreeNode::SetSelItemTextColor( DWORD _dwSelItemTextColor )
	{
		selected_text_color_ = _dwSelItemTextColor;
		Invalidate();
	}

	//************************************
	// 函数名称: GetSelItemTextColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	DWORD TreeNode::GetSelItemTextColor() const
	{
		return selected_text_color_;
	}

	//************************************
	// 函数名称: SetSelHotItemTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwSelHotItemTextColor
	// 函数说明: 
	//************************************
	void TreeNode::SetSelItemHotTextColor( DWORD _dwSelHotItemTextColor )
	{
		selected_text_hot_color_ = _dwSelHotItemTextColor;
		Invalidate();
	}

	//************************************
	// 函数名称: GetSelHotItemTextColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	DWORD TreeNode::GetSelItemHotTextColor() const
	{
		return selected_text_hot_color_;
	}

	/*****************************************************************************/
	/*****************************************************************************/
	/*****************************************************************************/
	IMPLEMENT_DUICONTROL(TreeView)
	
	//************************************
	// 函数名称: CTreeViewUI
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	TreeView::TreeView( void ) : folder_btn_visible_(TRUE),check_btn_visible_(FALSE),item_min_width_(0)
	{
		this->GetHeader()->SetVisible(FALSE);
	}
	
	//************************************
	// 函数名称: ~CTreeViewUI
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	TreeView::~TreeView( void )
	{
		
	}

	//************************************
	// 函数名称: GetClass
	// 返回类型: std::wstring
	// 函数说明: 
	//************************************
	std::wstring TreeView::GetClass() const
	{
		return _T("TreeView");
	}


	UINT TreeView::GetListType()
	{
		return kListTree;
	}

	//************************************
	// 函数名称: GetInterface
	// 返回类型: LPVOID
	// 参数信息: std::wstring pstrName
	// 函数说明: 
	//************************************
	LPVOID TreeView::GetInterface( std::wstring pstrName )
	{
		if( _tcsicmp(pstrName.c_str(), _T("TreeView")) == 0 ) return static_cast<TreeView*>(this);
		return List::GetInterface(pstrName);
	}

	void TreeView::DoInit()
	{
		for (int i = 0; i < GetCount(); ++i)
		{
			TreeNode *pNode = static_cast<TreeNode*>(GetItemAt(i)->GetInterface(L"TreeNode"));
			if (pNode) {
				pNode->SetIndentation(item_indentation_, max_item_indentation_);
			}
		}
	}

	//************************************
	// 函数名称: Add
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * pControl
	// 函数说明: 
	//************************************
	bool TreeView::Add( TreeNode* pControl )
	{
		if (!pControl) return false;
		if (_tcsicmp(pControl->GetClass().c_str(), _T("TreeNode")) != 0) return false;

		pControl->AttachDoubleClick(std::bind(&TreeView::OnDBClickItem, this, std::placeholders::_1));
		pControl->GetFolderButton()->AttachSelectedChanged(std::bind(&TreeView::OnFolderChanged, this, std::placeholders::_1));
		pControl->GetCheckBox()->AttachSelectedChanged(std::bind(&TreeView::OnCheckBoxChanged, this, std::placeholders::_1));

		pControl->SetVisibleFolderBtn(folder_btn_visible_);
		pControl->SetVisibleCheckBtn(check_btn_visible_);
		if(item_min_width_ > 0)
			pControl->SetMinWidth(item_min_width_);
		//pControl->SetIndentation(item_indentation_, max_item_indentation_);

		List::Add(pControl);

		if(pControl->GetCountChild() > 0) {
			int nCount = pControl->GetCountChild();
			for(int nIndex = 0;nIndex < nCount;nIndex++) {
				TreeNode* pNode = pControl->GetChildNode(nIndex);
				if(pNode) Add(pNode);
			}
		}

		pControl->SetTreeView(this);
		return true;
	}

	//************************************
	// 函数名称: AddAt
	// 返回类型: long
	// 参数信息: CTreeNodeUI * pControl
	// 参数信息: int iIndex
	// 函数说明: 该方法不会将待插入的节点进行缩位处理，若打算插入的节点为非根节点，请使用AddAt(CTreeNodeUI* pControl,CTreeNodeUI* _IndexNode) 方法
	//************************************
	long TreeView::AddAt( TreeNode* pControl, int iIndex )
	{
		if (!pControl) return -1;
		if (_tcsicmp(pControl->GetClass().c_str(), _T("TreeNode")) != 0) return -1;
		pControl->AttachDoubleClick(std::bind(&TreeView::OnDBClickItem, this, std::placeholders::_1));
		pControl->GetFolderButton()->AttachSelectedChanged(std::bind(&TreeView::OnFolderChanged, this, std::placeholders::_1));
		pControl->GetCheckBox()->AttachSelectedChanged(std::bind(&TreeView::OnCheckBoxChanged, this, std::placeholders::_1));

		pControl->SetVisibleFolderBtn(folder_btn_visible_);
		pControl->SetVisibleCheckBtn(check_btn_visible_);

		if(item_min_width_ > 0) {
			pControl->SetMinWidth(item_min_width_);
		}
		List::AddAt(pControl, iIndex);
		if(pControl->GetCountChild() > 0) {
			int nCount = pControl->GetCountChild();
			for(int nIndex = 0; nIndex < nCount; nIndex++) {
				TreeNode* pNode = pControl->GetChildNode(nIndex);
				if(pNode)
					return AddAt(pNode,iIndex+1);
			}
		}
		else {
			return iIndex + 1;
		}

		return -1;
	}

	//************************************
	// 函数名称: AddAt
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * pControl
	// 参数信息: CTreeNodeUI * _IndexNode
	// 函数说明:
	//************************************
	bool TreeView::AddAt( TreeNode* pControl, TreeNode* _IndexNode )
	{
		if(!_IndexNode && !pControl)
			return FALSE;

		int nItemIndex = -1;
		for(int nIndex = 0;nIndex < GetCount();nIndex++) {
			if(_IndexNode == GetItemAt(nIndex)) {
				nItemIndex = nIndex;
				break;
			}
		}

		if(nItemIndex == -1)
			return FALSE;

		return AddAt(pControl,nItemIndex) >= 0;
	}

	//************************************
	// 函数名称: Remove
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * pControl
	// 函数说明: pControl 对象以及下的所有节点将被一并移除
	//************************************
	bool TreeView::Remove( TreeNode* pControl )
	{
		if(pControl->GetCountChild() > 0) {
			int nCount = pControl->GetCountChild();
			for(int nIndex = 0;nIndex < nCount;nIndex++) {
				TreeNode* pNode = pControl->GetChildNode(nIndex);
				if(pNode){
					pControl->Remove(pNode);
				}
			}
		}
		List::Remove(pControl);
		return TRUE;
	}

	//************************************
	// 函数名称: RemoveAt
	// 返回类型: bool
	// 参数信息: int iIndex
	// 函数说明: iIndex 索引以及下的所有节点将被一并移除
	//************************************
	bool TreeView::RemoveAt( int iIndex )
	{
		TreeNode* pItem = (TreeNode*)GetItemAt(iIndex);
		if(pItem->GetCountChild())
			Remove(pItem);
		return TRUE;
	}

	void TreeView::RemoveAll()
	{
		List::RemoveAll();
	}

	//************************************
	// 函数名称: OnCheckBoxChanged
	// 返回类型: bool
	// 参数信息: void * param
	// 函数说明: 
	//************************************
	bool TreeView::OnCheckBoxChanged(const EventArgs& arg)
	{
		CheckBox* pCheckBox = (CheckBox*)arg.sender_;
		TreeNode* pItem = (TreeNode*)pCheckBox->GetParent()->GetParent();
		SetItemCheckBox(pCheckBox->GetCheck(), pItem);
		return true;
	}
	
	//************************************
	// 函数名称: OnFolderChanged
	// 返回类型: bool
	// 参数信息: void * param
	// 函数说明: 
	//************************************
	bool TreeView::OnFolderChanged(const EventArgs& arg)
	{
		CheckBox* pFolder = (CheckBox*)arg.sender_;
		TreeNode* pItem = (TreeNode*)pFolder->GetParent()->GetParent();
		bool is_checked = pFolder->GetCheck();
		pItem->SetVisibleTag(is_checked);
		SetItemExpand(is_checked,pItem);
		return true;
	}
	
	//************************************
	// 函数名称: OnDBClickItem
	// 返回类型: bool
	// 参数信息: void * param
	// 函数说明:
	//************************************
	bool TreeView::OnDBClickItem( const EventArgs &arg )
	{
		TreeNode* pItem		= static_cast<TreeNode*>(arg.sender_);
		CheckBox* pFolder	= pItem->GetFolderButton();
		bool is_checked = pFolder->GetCheck();
		pFolder->Selected(!is_checked, false);
		pItem->SetVisibleTag(!is_checked);
		SetItemExpand(!is_checked, pItem);

		return true;
	}

	//************************************
	// 函数名称: SetItemCheckBox
	// 返回类型: bool
	// 参数信息: bool _Selected
	// 参数信息: CTreeNodeUI * _TreeNode
	// 函数说明: 
	//************************************
	bool TreeView::SetItemCheckBox( bool _Selected,TreeNode* _TreeNode /*= NULL*/)
	{
		if(_TreeNode) {
			if(_TreeNode->GetCountChild() > 0) {
				int nCount = _TreeNode->GetCountChild();
				for(int nIndex = 0;nIndex < nCount;nIndex++) {
					TreeNode* pItem = _TreeNode->GetChildNode(nIndex);
					pItem->GetCheckBox()->Selected(_Selected);
					if(pItem->GetCountChild())
						SetItemCheckBox(_Selected,pItem);
				}
			}
			return TRUE;
		}
		else {
			int nIndex = 0;
			int nCount = GetCount();
			while(nIndex < nCount) {
				TreeNode* pItem = (TreeNode*)GetItemAt(nIndex);
				pItem->GetCheckBox()->Selected(_Selected);
				if(pItem->GetCountChild())
					SetItemCheckBox(_Selected,pItem);

				nIndex++;
			}
			return TRUE;
		}
		return FALSE;
	}

	//************************************
	// 函数名称: SetItemExpand
	// 返回类型: void
	// 参数信息: bool _Expanded
	// 参数信息: CTreeNodeUI * _TreeNode
	// 函数说明: 
	//************************************
	void TreeView::SetItemExpand( bool _Expanded,TreeNode* _TreeNode /*= NULL*/, bool force)
	{
		if(_TreeNode) {
			if(_TreeNode->GetCountChild() > 0) {
				int nCount = _TreeNode->GetCountChild();
				for(int nIndex = 0;nIndex < nCount;nIndex++) {
					TreeNode* pItem = _TreeNode->GetChildNode(nIndex);
					pItem->SetVisible(_Expanded);
					if((pItem->GetCountChild() && pItem->GetFolderButton()->IsSelected()) || force) {
						SetItemExpand(_Expanded, pItem, force);
					}
				}
			}
		}
		else {
			int nIndex = 0;
			int nCount = GetCount();
			while(nIndex < nCount) {
				TreeNode* pItem = (TreeNode*)GetItemAt(nIndex);
				pItem->SetVisible(_Expanded);
				if(pItem->GetCountChild() && pItem->GetFolderButton()->IsSelected()) {
					SetItemExpand(_Expanded,pItem);
				}
				nIndex++;
			}
		}
	}

	void TreeView::ExpandItemReverse(TreeNode * node, bool expand)
	{
		TreeNode *parent = node->GetParentNode();
		while (parent && parent->GetTreeLevel() != 0)
		{
			CheckBox* pFolder = parent->GetFolderButton();
			pFolder->Selected(true, false);
			parent->SetVisibleTag(true);
			parent->SetVisible(true);
			
			if (parent->IsHasChild()) {
				int count = parent->GetCountChild();
				for (int i = 0; i < count; ++i)
				{
					TreeNode* pItem = (TreeNode*)parent->GetChildNode(i);
					CheckBox* pFolder = pItem->GetFolderButton();
					bool is_checked = pFolder->GetCheck();
					pFolder->Selected(is_checked, false);
					pItem->SetVisible(true);
					pItem->SetVisibleTag(is_checked);
					
				}
			}
			
			parent = parent->GetParentNode();
		}
	}



	//************************************
	// 函数名称: SetVisibleFolderBtn
	// 返回类型: void
	// 参数信息: bool _IsVisibled
	// 函数说明: 
	//************************************
	void TreeView::SetVisibleFolderBtn( bool _IsVisibled )
	{
		folder_btn_visible_ = _IsVisibled;
		int nCount = this->GetCount();
		for(int nIndex = 0; nIndex < nCount; nIndex++) {
			TreeNode* pItem = static_cast<TreeNode*>(this->GetItemAt(nIndex));
			pItem->GetFolderButton()->SetVisible(folder_btn_visible_);
		}
	}

	//************************************
	// 函数名称: GetVisibleFolderBtn
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool TreeView::GetVisibleFolderBtn()
	{
		return folder_btn_visible_;
	}

	//************************************
	// 函数名称: SetVisibleCheckBtn
	// 返回类型: void
	// 参数信息: bool _IsVisibled
	// 函数说明: 
	//************************************
	void TreeView::SetVisibleCheckBtn( bool _IsVisibled )
	{
		check_btn_visible_ = _IsVisibled;
		int nCount = this->GetCount();
		for(int nIndex = 0; nIndex < nCount; nIndex++) {
			TreeNode* pItem = static_cast<TreeNode*>(this->GetItemAt(nIndex));
			pItem->GetCheckBox()->SetVisible(check_btn_visible_);
		}
	}

	//************************************
	// 函数名称: GetVisibleCheckBtn
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool TreeView::GetVisibleCheckBtn()
	{
		return check_btn_visible_;
	}

	//************************************
	// 函数名称: SetItemMinWidth
	// 返回类型: void
	// 参数信息: UINT _ItemMinWidth
	// 函数说明: 
	//************************************
	void TreeView::SetItemMinWidth( UINT _ItemMinWidth )
	{
		item_min_width_ = _ItemMinWidth;

		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetMinWidth(GetItemMinWidth());
			}
		}
		Invalidate();
	}

	//************************************
	// 函数名称: GetItemMinWidth
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT TreeView::GetItemMinWidth()
	{
		return item_min_width_;
	}
	
	//************************************
	// 函数名称: SetItemTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwItemTextColor
	// 函数说明: 
	//************************************
	void TreeView::SetItemTextColor( DWORD _dwItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetItemTextColor(_dwItemTextColor);
			}
		}
	}

	//************************************
	// 函数名称: SetItemHotTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwItemHotTextColor
	// 函数说明: 
	//************************************
	void TreeView::SetItemHotTextColor( DWORD _dwItemHotTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetItemHotTextColor(_dwItemHotTextColor);
			}
		}
	}

	//************************************
	// 函数名称: SetSelItemTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwSelItemTextColor
	// 函数说明: 
	//************************************
	void TreeView::SetSelItemTextColor( DWORD _dwSelItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetSelItemTextColor(_dwSelItemTextColor);
			}
		}
	}
		
	//************************************
	// 函数名称: SetSelItemHotTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwSelHotItemTextColor
	// 函数说明: 
	//************************************
	void TreeView::SetSelItemHotTextColor( DWORD _dwSelHotItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetSelItemHotTextColor(_dwSelHotItemTextColor);
			}
		}
	}

	//************************************
	// 函数名称: SetAttribute
	// 返回类型: void
	// 参数信息: std::wstring pstrName
	// 参数信息: std::wstring pstrValue
	// 函数说明: 
	//************************************
	void TreeView::SetAttribute( std::wstring strName, std::wstring strValue )
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if(_tcsicmp(pstrName,_T("visiblefolderbtn")) == 0)
			SetVisibleFolderBtn(_tcsicmp(pstrValue,_T("TRUE")) == 0);
		else if(_tcsicmp(pstrName,_T("visiblecheckbtn")) == 0)
			SetVisibleCheckBtn(_tcsicmp(pstrValue,_T("TRUE")) == 0);
		else if(_tcsicmp(pstrName,_T("itemminwidth")) == 0)
			SetItemMinWidth(_ttoi(pstrValue));
		else if(_tcsicmp(pstrName, _T("itemtextcolor")) == 0 ){
			SetItemTextColor(GlobalManager::GetColor(strValue));
		}
		else if(_tcsicmp(pstrName, _T("itemhottextcolor")) == 0 ){
			SetItemHotTextColor(GlobalManager::GetColor(strValue));
		}
		else if(_tcsicmp(pstrName, _T("selitemtextcolor")) == 0 ){
			SetSelItemTextColor(GlobalManager::GetColor(strValue));
		}
		else if(_tcsicmp(pstrName, _T("selitemhottextcolor")) == 0 ){
			SetSelItemHotTextColor(GlobalManager::GetColor(strValue));
		}
		else if (_tcsicmp(pstrName, L"itemindentation") == 0) {
			item_indentation_ = _ttoi(pstrValue);
		}
		else if (_tcsicmp(pstrName, L"maxitemindetation") == 0) {
			max_item_indentation_ = _ttoi(pstrValue);
		}

		else List::SetAttribute(strName, strValue);
	}

}