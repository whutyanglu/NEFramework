#ifndef __UILIST_H__
#define __UILIST_H__

namespace ui {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	typedef int (CALLBACK *PULVCompareFunc)(UINT_PTR, UINT_PTR, UINT_PTR);

	class ListHeader;

#define UILIST_MAX_COLUMNS 32

	typedef struct tagTListInfoUI
	{
		int nColumns;
		RECT rcColumn[UILIST_MAX_COLUMNS];
		int nFont;
		UINT uTextStyle;
		RECT rcTextPadding;
		DWORD dwTextColor;
		DWORD dwBkColor;
		std::wstring sBkImage;
		bool bAlternateBk;
		DWORD dwSelectedTextColor;
		DWORD dwSelectedBkColor;
		std::wstring sSelectedImage;
		DWORD dwHotTextColor;
		DWORD dwHotBkColor;
		std::wstring sHotImage;
		DWORD dwDisabledTextColor;
		DWORD dwDisabledBkColor;
		std::wstring sDisabledImage;
		DWORD dwLineColor;
		bool bShowRowLine;
		bool bShowColumnLine;
		bool bShowHtml;
		bool bMultiExpandable;
	} TListInfo;


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class IListCallback
	{
	public:
		virtual std::wstring GetItemText(Control* pList, int iItem, int iSubItem) = 0;
	};

	class IListOwner
	{
	public:
		virtual UINT GetListType() = 0;
		virtual TListInfo* GetListInfo() = 0;
		virtual int GetCurSel() const = 0;
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool send_notify = true) = 0;
		virtual bool SelectMultiItem(int iIndex, bool bTakeFocus = false) = 0;
		virtual bool UnSelectItem(int iIndex, bool bOthers = false) = 0;
		virtual void DoEvent(EventArgs& event) = 0;
	};

	class IList : public IListOwner
	{
	public:
		virtual ListHeader* GetHeader() const = 0;
		virtual Box* GetList() const = 0;
		virtual IListCallback* GetTextCallback() const = 0;
		virtual void SetTextCallback(IListCallback* pCallback) = 0;
		virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
		virtual int GetExpandedItem() const = 0;

		virtual void SetMultiSelect(bool bMultiSel) = 0;
		virtual bool IsMultiSelect() const = 0;
		virtual void SelectAllItems() = 0;
		virtual void UnSelectAllItems() = 0;
		virtual int GetSelectItemCount() const = 0;
		virtual int GetNextSelItem(int nItem) const = 0;
	};

	class IListItem
	{
	public:
		virtual int GetIndex() const = 0;
		virtual void SetIndex(int iIndex) = 0;
		virtual IListOwner* GetOwner() = 0;
		virtual void SetOwner(Control* pOwner) = 0;
		virtual bool IsSelected() const = 0;
		virtual bool Select(bool bSelect = true, bool send_notify = true) = 0;
		virtual bool SelectMulti(bool bSelect = true) = 0;
		virtual bool IsExpanded() const = 0;
		virtual bool Expand(bool bExpand = true) = 0;
		virtual void DrawItemText(HDC hDC, const RECT& rcItem) = 0;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class ListBody;
	class ListHeader;
	class Edit;
	class ComboBox;
	class UILIB_API List : public VBox, public IList
	{
		DECLARE_DUICONTROL(List)

	public:
		List();

		std::wstring GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(std::wstring pstrName);

		void AttachItemSelected(const EventCallback &cb);
		
		void AttachItemLinkClick(const EventCallback &cb);
	
		bool GetScrollSelect();
		void SetScrollSelect(bool bScrollSelect);
		int GetCurSel() const;
		int GetCurSelActivate() const;
		bool SelectItem(int iIndex, bool bTakeFocus = false, bool send_notify = true);
		bool SelectItemActivate(int iIndex);    // 双击选中

		bool SelectMultiItem(int iIndex, bool bTakeFocus = false);
		void SetMultiSelect(bool bMultiSel);
		bool IsMultiSelect() const;
		bool UnSelectItem(int iIndex, bool bOthers = false);
		void SelectAllItems();
		void UnSelectAllItems();
		int GetSelectItemCount() const;
		int GetNextSelItem(int nItem) const;

		ListHeader* GetHeader() const;  
		Box* GetList() const;
		UINT GetListType();
		TListInfo* GetListInfo();

		Control* GetItemAt(int iIndex) const;
		int GetItemIndex(Control* pControl) const;
		bool SetItemIndex(Control* pControl, int iIndex);
		int GetCount() const;
		bool Add(Control* pControl);
		bool AddAt(Control* pControl, int iIndex);
		bool Remove(Control* pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

		bool IsDelayedDestroy() const;
		void SetDelayedDestroy(bool bDelayed);
		int GetChildPadding() const;
		void SetChildPadding(int iPadding);

		void SetItemFont(int index);
		void SetItemTextStyle(UINT uStyle);
		void SetItemTextPadding(RECT rc);
		void SetItemTextColor(DWORD dwTextColor);
		void SetItemBkColor(DWORD dwBkColor);
		void SetItemBkImage(std::wstring pStrImage);
		void SetAlternateBk(bool bAlternateBk);
		void SetSelectedItemTextColor(DWORD dwTextColor);
		void SetSelectedItemBkColor(DWORD dwBkColor);
		void SetSelectedItemImage(std::wstring pStrImage);
		void SetHotItemTextColor(DWORD dwTextColor);
		void SetHotItemBkColor(DWORD dwBkColor);
		void SetHotItemImage(std::wstring pStrImage);
		void SetDisabledItemTextColor(DWORD dwTextColor);
		void SetDisabledItemBkColor(DWORD dwBkColor);
		void SetDisabledItemImage(std::wstring pStrImage);
		void SetItemLineColor(DWORD dwLineColor);
		void SetItemShowRowLine(bool bShowLine = false);
		void SetItemShowColumnLine(bool bShowLine = false);
		bool IsItemShowHtml();
		void SetItemShowHtml(bool bShowHtml = true);
		RECT GetItemTextPadding() const;
		DWORD GetItemTextColor() const;
		DWORD GetItemBkColor() const;
		std::wstring GetItemBkImage() const;
		bool IsAlternateBk() const;
		DWORD GetSelectedItemTextColor() const;
		DWORD GetSelectedItemBkColor() const;
		std::wstring GetSelectedItemImage() const;
		DWORD GetHotItemTextColor() const;
		DWORD GetHotItemBkColor() const;
		std::wstring GetHotItemImage() const;
		DWORD GetDisabledItemTextColor() const;
		DWORD GetDisabledItemBkColor() const;
		std::wstring GetDisabledItemImage() const;
		DWORD GetItemLineColor() const;

		void SetMultiExpanding(bool bMultiExpandable); 
		int GetExpandedItem() const;
		bool ExpandItem(int iIndex, bool bExpand = true);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		IListCallback* GetTextCallback() const;
		void SetTextCallback(IListCallback* pCallback);

		SIZE GetScrollPos() const;
		SIZE GetScrollRange() const;
		void SetScrollPos(SIZE szPos, bool bMsg = true);
		void LineUp();
		void LineDown();
		void PageUp();
		void PageDown();
		void HomeUp();
		void EndDown();
		void LineLeft();
		void LineRight();
		void PageLeft();
		void PageRight();
		void HomeLeft();
		void EndRight();
		void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
		virtual ScrollBar* GetVerticalScrollBar() const;
		virtual ScrollBar* GetHorizontalScrollBar() const;
		BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

		virtual BOOL CheckColumEditable(int nColum) { return FALSE; };
		virtual Edit* GetEditUI() { return NULL; };

		virtual BOOL CheckColumComboBoxable(int nColum) { return FALSE; };
		virtual ComboBox* GetComboBoxUI() { return NULL; };

	protected:
		int GetMinSelItemIndex();
		int GetMaxSelItemIndex();
	protected:
		bool m_bScrollSelect;
		int m_iCurSel;
		bool m_bMultiSel;
		CStdPtrArray m_aSelItems;
		int m_iCurSelActivate;  // 双击的列
		int m_iExpandedItem;
		IListCallback* m_pCallback;
		ListBody* m_pList;
		ListHeader* m_pHeader;
		TListInfo m_ListInfo;

	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ListBody : public VBox
	{
	public:
		ListBody(List* pOwner);


		int GetScrollStepSize() const;
		void SetScrollPos(SIZE szPos, bool bMsg = true);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoEvent(EventArgs& event);
		BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);
	protected:
		static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
		int __cdecl ItemComareFunc(const void *item1, const void *item2);
	protected:
		List* owner_;
		PULVCompareFunc m_pCompareFunc;
		UINT_PTR m_compareData;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ListHeader : public HBox
	{
		DECLARE_DUICONTROL(ListHeader)
	public:
		ListHeader();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void SetScaleHeader(bool bIsScale);
		bool IsScaleHeader() const;

	private:
		bool m_bIsScaleHeader;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ListHeaderItem : public Box
	{
		DECLARE_DUICONTROL(ListHeaderItem)

	public:
		ListHeaderItem();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		void AttachHeaderClick(const EventCallback & cb);
	
		void SetEnabled(bool bEnable = true);

		bool IsDragable() const;
		void SetDragable(bool bDragable);
		DWORD GetSepWidth() const;
		void SetSepWidth(int iWidth);
		DWORD GetTextStyle() const;
		void SetTextStyle(UINT uStyle);
		DWORD GetTextColor() const;
		void SetTextColor(DWORD dwTextColor);
		void SetTextPadding(RECT rc);
		RECT GetTextPadding() const;
		void SetFont(int index);
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);
		std::wstring GetNormalImage() const;
		void SetNormalImage(std::wstring pStrImage);
		std::wstring GetHotImage() const;
		void SetHotImage(std::wstring pStrImage);
		std::wstring GetPushedImage() const;
		void SetPushedImage(std::wstring pStrImage);
		std::wstring GetFocusedImage() const;
		void SetFocusedImage(std::wstring pStrImage);
		std::wstring GetSepImage() const;
		void SetSepImage(std::wstring pStrImage);
		void SetScale(int nScale);
		int GetScale() const;

		void DoEvent(EventArgs& event);
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		RECT GetThumbRect() const;

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		POINT last_mouse_pt_;
		bool dragable_;
		UINT btn_state_;
		int sep_width_;
		DWORD text_color_;
		int font_;
		UINT text_style_;
		bool show_html_;
		RECT text_padding_rect_;
		std::wstring normal_image_;
		std::wstring hot_image_;
		std::wstring pushed_image_;
		std::wstring focused_image_;
		std::wstring sep_image_;
		std::wstring sep_image_modify_;
		int m_nScale;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ListElement : public Control, public IListItem
	{
	public:
		ListElement();

		std::wstring GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(std::wstring pstrName);
		void AttachActive(const EventCallback &cb);

		void SetEnabled(bool bEnable = true);

		int GetIndex() const;
		void SetIndex(int iIndex);

		IListOwner* GetOwner();
		void SetOwner(Control* pOwner);
		void SetVisible(bool bVisible = true);

		bool IsSelected() const;
		bool Select(bool bSelect = true, bool send_notify = true);
		bool SelectMulti(bool bSelect = true);
		bool IsExpanded() const;
		bool Expand(bool bExpand = true);

		void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
		bool Activate();

		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void DrawItemBk(HDC hDC, const RECT& rcItem);

	protected:
		int m_iIndex;
		bool selected_;
		UINT btn_state_;
		IListOwner* owner_;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ListLabelElement : public ListElement
	{
		DECLARE_DUICONTROL(ListLabelElement)
	public:
		ListLabelElement();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		void AttachItemClick(const EventCallback &cb);

		void DoEvent(EventArgs& event);
		SIZE EstimateSize(SIZE szAvailable);
		void DoPaint(HDC hDC, const RECT& rcPaint);

		void DrawItemText(HDC hDC, const RECT& rcItem);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ListTextElement : public ListLabelElement
	{
		DECLARE_DUICONTROL(ListTextElement)
	public:
		ListTextElement();
		~ListTextElement();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		void AttachLinkItemClick(const EventCallback &cb);

		std::wstring GetText(int iIndex) const;
		void SetText(int iIndex, std::wstring pstrText);

		void SetOwner(Control* pOwner);
		std::wstring* GetLinkContent(int iIndex);

		void DoEvent(EventArgs& event);
		SIZE EstimateSize(SIZE szAvailable);

		void DrawItemText(HDC hDC, const RECT& rcItem);

	protected:
		enum { MAX_LINK = 8 };
		int links_;
		RECT link_rects_[MAX_LINK];
		std::wstring links_array_[MAX_LINK];
		int hover_link_;
		IList* owner_;
		CStdPtrArray texts_;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ListContainerElement : public HBox, public IListItem
	{
		DECLARE_DUICONTROL(ListContainerElement)
	public:
		ListContainerElement();

		std::wstring GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(std::wstring pstrName);

		virtual int GetFixedHeight() override;

		void AttachItemActive(const EventCallback &cb);
		void AttachItemClick(const EventCallback &cb);
		void AttachTimer(const EventCallback &cb);
		void AttachMouseMenu(const EventCallback &cb);

		int GetIndex() const;
		void SetIndex(int iIndex);

		IListOwner* GetOwner();
		void SetOwner(Control* pOwner);
		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);

		bool IsSelected() const;
		bool Select(bool bSelect = true, bool send_notify = true);
		bool SelectMulti(bool bSelect = true);
		bool IsExpanded() const;
		bool Expand(bool bExpand = true);

		void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
		bool Activate();

		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void DoPaint(HDC hDC, const RECT& rcPaint);

		virtual void DrawItemText(HDC hDC, const RECT& rcItem);    
		virtual void DrawItemBk(HDC hDC, const RECT& rcItem);

		void SetPos(RECT rc, bool bNeedInvalidate = true);

	protected:
		int m_iIndex;
		bool selected_;
		UINT btn_state_;
		IListOwner* owner_;
	};

} // namespace DuiLib

#endif // __UILIST_H__
