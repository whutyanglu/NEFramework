#ifndef UITreeView_h__
#define UITreeView_h__

#include <vector>
using namespace std;

#pragma once

namespace ui
{
	class TreeView;
	class CheckBox;
	class Label;
	class Option;

	class UILIB_API TreeNode : public ListContainerElement
	{
		DECLARE_DUICONTROL(TreeNode)
	public:
		TreeNode(TreeNode* _ParentNode = NULL);
		~TreeNode(void);

	public:
		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		virtual void SetText(std::wstring txt) override;
		virtual bool AddChildNode(TreeNode* _pTreeNodeUI);
		void AttachDoubleClick(const EventCallback &cb);

		void DoEvent(EventArgs& event);
		void Invalidate();
		bool Select(bool bSelect = true, bool send_notify = true);
		bool SelectMulti(bool bSelect = true);

		bool Add(Control* _pTreeNodeUI);
		bool AddAt(Control* pControl, int iIndex);

		bool Remove(Control* pControl);
		void SetVisibleTag(bool _IsVisible);
		bool GetVisibleTag();
		void SetItemText(std::wstring pstrValue);
		std::wstring GetItemText();
		void CheckBoxSelected(bool _Selected);
		bool IsCheckBoxSelected() const;
		bool IsHasChild() const;
		long GetTreeLevel() const {
			return level_;
		}
		void SetTreeLevel(long lv) {
			level_ = lv;
		}
	
		bool RemoveAt(TreeNode* _pTreeNodeUI);
		void SetParentNode(TreeNode* _pParentTreeNode);
		TreeNode* GetParentNode();
		long GetCountChild();
		void SetTreeView(TreeView* _CTreeViewUI);
		TreeView* GetTreeView();
		TreeNode* GetChildNode(int _nIndex);
		void SetVisibleFolderBtn(bool _IsVisibled);
		bool GetVisibleFolderBtn();
		void SetVisibleCheckBtn(bool _IsVisibled);
		bool GetVisibleCheckBtn();
		void SetItemTextColor(DWORD _dwItemTextColor);
		DWORD GetItemTextColor() const;
		void SetItemHotTextColor(DWORD _dwItemHotTextColor);
		DWORD GetItemHotTextColor() const;
		void SetSelItemTextColor(DWORD _dwSelItemTextColor);
		DWORD GetSelItemTextColor() const;
		void SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor);
		DWORD GetSelItemHotTextColor() const;
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		CStdPtrArray GetTreeNodes();
		int			 GetTreeIndex();
		int			 GetNodeIndex();

		void SetIndentation(int indentation, int max_indentation);

	public:
		HBox*	GetTreeNodeHoriznotal() const {return hbox_;};
		CheckBox*			GetFolderButton() const {return folder_button_;};
		Label*				GetDottedLine() const {return dotted_line_;};
		CheckBox*			GetCheckBox() const {return checkbox_;};
		Option*				GetItemButton() const {return option_;};

	protected:
		TreeNode* GetLastNode();
		TreeNode* CalLocation(TreeNode* _pTreeNodeUI);

	protected:
		long	level_ = 0;
		bool	visible_;
		bool	is_checkbox_;
		DWORD	text_color_;
		DWORD	tect_hot_color_;
		DWORD	selected_text_color_;
		DWORD	selected_text_hot_color_;

		TreeView*			tree_view_;
		HBox*				hbox_;
		CheckBox*			folder_button_;
		Label*				dotted_line_;
		CheckBox*			checkbox_;
		Option*				option_;
		TreeNode*			parent_;
		CStdPtrArray		nodes_;

	};

	class UILIB_API TreeView : public List
	{
		DECLARE_DUICONTROL(TreeView)
	public:
		TreeView(void);
		virtual ~TreeView(void);

	public:
		virtual std::wstring GetClass() const;
		virtual LPVOID	GetInterface(std::wstring pstrName);

		virtual void DoInit();

		virtual UINT GetListType();
		virtual bool Add(TreeNode* pControl );
		virtual long AddAt(TreeNode* pControl, int iIndex );
		virtual bool AddAt(TreeNode* pControl,TreeNode* _IndexNode);
		virtual bool Remove(TreeNode* pControl);
		virtual bool RemoveAt(int iIndex);
		virtual void RemoveAll();
		virtual bool OnCheckBoxChanged(const EventArgs& arg);
		virtual bool OnFolderChanged(const EventArgs& arg);
		virtual bool OnDBClickItem(const EventArgs& arg);
		virtual bool SetItemCheckBox(bool _Selected,TreeNode* _TreeNode = NULL);
		virtual void SetItemExpand(bool _Expanded, TreeNode* _TreeNode = NULL, bool force = false);
		void ExpandItemReverse(TreeNode *node, bool expand);
		virtual void SetVisibleFolderBtn(bool _IsVisibled);
		virtual bool GetVisibleFolderBtn();
		virtual void SetVisibleCheckBtn(bool _IsVisibled);
		virtual bool GetVisibleCheckBtn();
		virtual void SetItemMinWidth(UINT _ItemMinWidth);
		virtual UINT GetItemMinWidth();
		virtual void SetItemTextColor(DWORD _dwItemTextColor);
		virtual void SetItemHotTextColor(DWORD _dwItemHotTextColor);
		virtual void SetSelItemTextColor(DWORD _dwSelItemTextColor);
		virtual void SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor);
		virtual void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		int GetItemIndentation() {
			return item_indentation_;
		}

		int GetMaxItemIndentation() {
			return max_item_indentation_;
		}

	protected:
		UINT item_min_width_;
		int  item_indentation_ = 16;
		int  max_item_indentation_ = -1;
		bool folder_btn_visible_;
		bool check_btn_visible_;
	};
}


#endif // UITreeView_h__
