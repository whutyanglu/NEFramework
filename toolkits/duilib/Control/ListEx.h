#ifndef __UILISTEX_H__
#define __UILISTEX_H__

#pragma once

// TODO : luyang 暂不使用

#include "Box/VBox.h"
#include "Box/HBox.h"

namespace ui {

	class IListComboCallback
	{
	public:
		virtual void GetItemComboTextArray(Control* pCtrl, int iItem, int iSubItem) = 0;
	};

	class Edit;
	class ComboBox;

	class UILIB_API ListEx : public List
	{
		DECLARE_DUICONTROL(ListEx)

	public:
		ListEx();

		std::wstring GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(std::wstring pstrName);

	public: 
		virtual void DoEvent(EventArgs& event);
		bool OnHeaderItemClick(const EventArgs & arg);
		bool OnItemChecked(const EventArgs & arg);

	public:
		void InitListCtrl();

	protected:
		Edit*		m_pEditUI;
		ComboBox*	m_pComboBoxUI;

	public:
		virtual BOOL CheckColumEditable(int nColum);
		virtual Edit* GetEditUI();

		virtual BOOL CheckColumComboBoxable(int nColum);
		virtual ComboBox* GetComboBoxUI();

		virtual BOOL CheckColumCheckBoxable(int nColum);

	public:
		//virtual void Notify(EventArgs& msg);
		BOOL	m_bAddMessageFilter;
		int		m_nRow,m_nColum;
		void	SetEditRowAndColum(int nRow,int nColum) { m_nRow = nRow; m_nColum = nColum; };

	public:
		IListComboCallback* m_pXCallback;
		virtual IListComboCallback* GetTextArrayCallback() const;
		virtual void SetTextArrayCallback(IListComboCallback* pCallback);

	public:
		void OnListItemClicked(int nIndex, int nColum, RECT* lpRCColum, std::wstring lpstrText);
		void OnListItemChecked(int nIndex, int nColum, BOOL bChecked);

	public:
		void SetColumItemColor(int nIndex, int nColum, DWORD iBKColor);
		BOOL GetColumItemColor(int nIndex, int nColum, DWORD& iBKColor);

	private:
		void HideEditAndComboCtrl();
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API ListContainerHeaderItem : public HBox
	{
		DECLARE_DUICONTROL(ListContainerHeaderItem)

	public:
		ListContainerHeaderItem();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(BOOL bEnable = TRUE);

		BOOL IsDragable() const;
		void SetDragable(BOOL bDragable);
		DWORD GetSepWidth() const;
		void SetSepWidth(int iWidth);
		DWORD GetTextStyle() const;
		void SetTextStyle(UINT uStyle);
		DWORD GetTextColor() const;
		void SetTextColor(DWORD dwTextColor);
		void SetTextPadding(RECT rc);
		RECT GetTextPadding() const;
		void SetFont(int index);
		BOOL IsShowHtml();
		void SetShowHtml(BOOL bShowHtml = TRUE);
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

		void DoEvent(EventArgs& event);
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		RECT GetThumbRect() const;

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		POINT last_mouse_pt_;
		BOOL dragable_;
		UINT btn_state_;
		int sep_width_;
		DWORD text_color_;
		int font_;
		UINT text_style_;
		BOOL show_html_;
		RECT text_padding_rect_;
		std::wstring normal_image_;
		std::wstring hot_image_;
		std::wstring pushed_image_;
		std::wstring focused_image_;
		std::wstring sep_image_;
		std::wstring sep_image_modify_;

		//支持编辑
		BOOL editable_;

		//支持组合框
		BOOL comboable_;

		//支持复选框
		BOOL checkboxable_;

	public:
		BOOL GetColumeEditable();
		void SetColumeEditable(BOOL bEnable);

		BOOL GetColumeComboable();
		void SetColumeComboable(BOOL bEnable);

		BOOL GetColumeCheckable();
		void SetColumeCheckable(BOOL bEnable);

	public:
		void SetCheck(BOOL bCheck);
		BOOL GetCheck();

	private:
		UINT	checkbox_state_;
		BOOL	checked_;

		std::wstring checkbox_normal_image_;
		std::wstring checkbox_hot_image_;
		std::wstring checkbox_pushed_image_;
		std::wstring checkbox_focused_image_;
		std::wstring checkbox_disabled_image_;

		std::wstring checkbox_selected_image_;
		std::wstring checkbox_fore_image_;

		SIZE cxy_checkbox_;

	public:
		BOOL DrawCheckBoxImage(HDC hDC, std::wstring pStrImage, std::wstring strModify = L"");
		std::wstring GetCheckBoxNormalImage();
		void SetCheckBoxNormalImage(std::wstring pStrImage);
		std::wstring GetCheckBoxHotImage();
		void SetCheckBoxHotImage(std::wstring pStrImage);
		std::wstring GetCheckBoxPushedImage();
		void SetCheckBoxPushedImage(std::wstring pStrImage);
		std::wstring GetCheckBoxFocusedImage();
		void SetCheckBoxFocusedImage(std::wstring pStrImage);
		std::wstring GetCheckBoxDisabledImage();
		void SetCheckBoxDisabledImage(std::wstring pStrImage);

		std::wstring GetCheckBoxSelectedImage();
		void SetCheckBoxSelectedImage(std::wstring pStrImage);
		std::wstring GetCheckBoxForeImage();
		void SetCheckBoxForeImage(std::wstring pStrImage);

		void GetCheckBoxRect(RECT &rc);	

		int GetCheckBoxWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		void SetCheckBoxWidth(int cx);      // 预设的参考值
		int GetCheckBoxHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		void SetCheckBoxHeight(int cy);     // 预设的参考值


	public:
		Box* owner_;
		void SetOwner(Box* pOwner);
		Box* GetOwner();
	};
	 
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ListTextExtElement : public ListLabelElement
	{
		DECLARE_DUICONTROL(ListTextExtElement)

	public:
		ListTextExtElement();
		~ListTextExtElement();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		std::wstring GetText(int iIndex) const;
		void SetText(int iIndex, std::wstring pstrText);

		void SetOwner(Control* pOwner);
		std::wstring* GetLinkContent(int iIndex);

		void DoEvent(EventArgs& event);
		SIZE EstimateSize(SIZE szAvailable);

		void DrawItemText(HDC hDC, const RECT& rcItem);

	protected:
		enum { MAX_LINK = 8 };
		int link_id_;
		RECT link_rects_[MAX_LINK];
		std::wstring links_[MAX_LINK];
		int hover_link_;
		List* owner_;
		CStdPtrArray texts_;

	private:
		UINT	checkbox_state_;
		BOOL	checked_;

		std::wstring checkbox_normal_image_;
		std::wstring checkbox_hot_image_;
		std::wstring checkbox_pushed_image_;
		std::wstring checkbox_focused_image_;
		std::wstring checkbox_disabled_image_;

		std::wstring checkbox_selected_image_;
		std::wstring checkbox_fore_image_;

		SIZE cxy_checkbox_;

	public:
		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		virtual void PaintStatusImage(HDC hDC);
		BOOL DrawCheckBoxImage(HDC hDC, std::wstring pStrImage, std::wstring pStrModify, RECT& rcCheckBox);
		std::wstring GetCheckBoxNormalImage();
		void SetCheckBoxNormalImage(std::wstring pStrImage);
		std::wstring GetCheckBoxHotImage();
		void SetCheckBoxHotImage(std::wstring pStrImage);
		std::wstring GetCheckBoxPushedImage();
		void SetCheckBoxPushedImage(std::wstring pStrImage);
		std::wstring GetCheckBoxFocusedImage();
		void SetCheckBoxFocusedImage(std::wstring pStrImage);
		std::wstring GetCheckBoxDisabledImage();
		void SetCheckBoxDisabledImage(std::wstring pStrImage);

		std::wstring GetCheckBoxSelectedImage();
		void SetCheckBoxSelectedImage(std::wstring pStrImage);
		std::wstring GetCheckBoxForeImage();
		void SetCheckBoxForeImage(std::wstring pStrImage);

		void GetCheckBoxRect(int nIndex, RECT &rc);	
		void GetColumRect(int nColum, RECT &rc);

		int GetCheckBoxWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		void SetCheckBoxWidth(int cx);      // 预设的参考值
		int GetCheckBoxHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		void SetCheckBoxHeight(int cy);     // 预设的参考值

		void SetCheck(BOOL bCheck);
		BOOL GetCheck() const;

	public:
		int HitTestColum(POINT ptMouse);
		BOOL CheckColumEditable(int nColum);

	private:
		typedef struct tagColumColorNode
		{
			BOOL  bEnable;
			DWORD iTextColor;
			DWORD iBKColor;
		}COLUMCOLORNODE;

		COLUMCOLORNODE ColumCorlorArray[UILIST_MAX_COLUMNS];

	public:
		void SetColumItemColor(int nColum, DWORD iBKColor);
		BOOL GetColumItemColor(int nColum, DWORD& iBKColor);

	};
} // namespace DuiLib

#endif // __UILISTEX_H__
