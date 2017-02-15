#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace ui {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class ComboWnd;

	class UILIB_API Combo : public Box, public IListOwner
	{
		DECLARE_DUICONTROL(Combo)
		friend class ComboWnd;

	public:
		Combo();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		void AttachDropDown(const EventCallback &cb);
		void AttachItemSelected(const EventCallback &cb);

		void DoInit();
		UINT GetControlFlags() const;

		std::wstring GetText() const;
		void SetEnabled(bool bEnable = true);

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
		int GetFont() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);

		std::wstring GetDropBoxAttributeList();
		void SetDropBoxAttributeList(std::wstring pstrList);
		SIZE GetDropBoxSize() const;
		void SetDropBoxSize(SIZE szDropBox);

		UINT GetListType();
		TListInfo* GetListInfo();
		int GetCurSel() const;  
		bool SelectItem(int iIndex, bool bTakeFocus = false, bool send_notify = true);
		bool SelectMultiItem(int iIndex, bool bTakeFocus = false);
		bool UnSelectItem(int iIndex, bool bOthers = false);
		bool SetItemIndex(Control* pControl, int iIndex);

		bool Add(Control* pControl);
		bool AddAt(Control* pControl, int iIndex);
		bool Remove(Control* pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();

		bool Activate();

		std::wstring GetNormalImage() const;
		void SetNormalImage(std::wstring pStrImage);
		std::wstring GetHotImage() const;
		void SetHotImage(std::wstring pStrImage);
		std::wstring GetPushedImage() const;
		void SetPushedImage(std::wstring pStrImage);
		std::wstring GetFocusedImage() const;
		void SetFocusedImage(std::wstring pStrImage);
		std::wstring GetDisabledImage() const;
		void SetDisabledImage(std::wstring pStrImage);

		bool GetScrollSelect();
		void SetScrollSelect(bool bScrollSelect);
		
		void SetItemFont(int index);
		void SetItemTextStyle(UINT uStyle);
		RECT GetItemTextPadding() const;
		void SetItemTextPadding(RECT rc);
		DWORD GetItemTextColor() const;
		void SetItemTextColor(DWORD dwTextColor);
		DWORD GetItemBkColor() const;
		void SetItemBkColor(DWORD dwBkColor);
		std::wstring GetItemBkImage() const;
		void SetItemBkImage(std::wstring pStrImage);
		bool IsAlternateBk() const;
		void SetAlternateBk(bool bAlternateBk);
		DWORD GetSelectedItemTextColor() const;
		void SetSelectedItemTextColor(DWORD dwTextColor);
		DWORD GetSelectedItemBkColor() const;
		void SetSelectedItemBkColor(DWORD dwBkColor);
		std::wstring GetSelectedItemImage() const;
		void SetSelectedItemImage(std::wstring pStrImage);
		DWORD GetHotItemTextColor() const;
		void SetHotItemTextColor(DWORD dwTextColor);
		DWORD GetHotItemBkColor() const;
		void SetHotItemBkColor(DWORD dwBkColor);
		std::wstring GetHotItemImage() const;
		void SetHotItemImage(std::wstring pStrImage);
		DWORD GetDisabledItemTextColor() const;
		void SetDisabledItemTextColor(DWORD dwTextColor);
		DWORD GetDisabledItemBkColor() const;
		void SetDisabledItemBkColor(DWORD dwBkColor);
		std::wstring GetDisabledItemImage() const;
		void SetDisabledItemImage(std::wstring pStrImage);
		DWORD GetItemLineColor() const;
		void SetItemLineColor(DWORD dwLineColor);
		bool IsItemShowHtml();
		void SetItemShowHtml(bool bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void DoPaint(HDC hDC, const RECT& rcPaint);
		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		ComboWnd* combo_wnd_ = NULL;

		int cur_sel_;
		DWORD	text_color_;
		DWORD	disabled_text_color_;
		int		font_;
		UINT	text_style_;
		RECT	text_padding_rc_;
		bool	is_show_html_;
		std::wstring drop_box_attributes_;
		SIZE drop_box_size_;
		UINT btn_state_;

		std::wstring normal_image_;
		std::wstring hot_image_;
		std::wstring pushed_image_;
		std::wstring focused_image_;
		std::wstring disabled_image_;

		bool is_scroll_select_;
		TListInfo list_info_;
	};

} // namespace DuiLib

#endif // __UICOMBO_H__
