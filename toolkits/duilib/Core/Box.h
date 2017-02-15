#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#pragma once

#include "../Core/DlgBuilder.h"

namespace ui {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class IBox
	{
	public:
		virtual Control* GetItemAt(int iIndex) const = 0;
		virtual int GetItemIndex(Control* pControl) const  = 0;
		virtual bool SetItemIndex(Control* pControl, int iIndex)  = 0;
		virtual int GetCount() const = 0;	
		virtual bool Add(Control* pControl) = 0;
		virtual bool AddAt(Control* pControl, int iIndex)  = 0;
		virtual bool Remove(Control* pControl) = 0;
		virtual bool RemoveAt(int iIndex)  = 0;
		virtual void RemoveAll() = 0;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//
	class ScrollBar;

	class UILIB_API Box : public Control, public IBox
	{
		DECLARE_DUICONTROL(Box)

	public:
		Box();
		virtual ~Box();

	public:
		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
	
		Control* GetItemAt(int iIndex) const;
		int GetItemIndex(Control* pControl) const;
		bool SetItemIndex(Control* pControl, int iIndex);
		int GetCount() const;
		bool Add(Control* pControl);
		bool AddAt(Control* pControl, int iIndex);
		bool Remove(Control* pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();

		virtual int GetFixedWidth();
		virtual int GetFixedHeight();

		void DoEvent(EventArgs& event);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		void SetEnabled(bool bEnabled);
		void SetMouseEnabled(bool bEnable = true);

		virtual RECT GetInset() const;
		virtual void SetInset(RECT rcInset); // 设置内边距，相当于设置客户区
		virtual int GetChildPadding() const;
		virtual void SetChildPadding(int iPadding);
		virtual UINT GetChildAlign() const;
		virtual void SetChildAlign(UINT iAlign);
		virtual UINT GetChildVAlign() const;
		virtual void SetChildVAlign(UINT iVAlign);
		virtual bool IsAutoDestroy() const;
		virtual void SetAutoDestroy(bool bAuto);
		virtual bool IsDelayedDestroy() const;
		virtual void SetDelayedDestroy(bool bDelayed);
		virtual bool IsMouseChildEnabled() const;
		virtual void SetMouseChildEnabled(bool bEnable = true);

		virtual int FindSelectable(int iIndex, bool bForward = true) const;

		RECT GetClientPos() const;
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		void DoPaint(HDC hDC, const RECT& rcPaint);

		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void SetManager(PaintManager* pManager, Control* pParent, bool bInit = true);
		Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

		bool SetSubControlText(std::wstring pstrSubControlName,std::wstring pstrText);
		bool SetSubControlFixedHeight(std::wstring pstrSubControlName,int cy);
		bool SetSubControlFixedWdith(std::wstring pstrSubControlName,int cx);
		bool SetSubControlUserData(std::wstring pstrSubControlName,std::wstring pstrText);

		std::wstring GetSubControlText(std::wstring pstrSubControlName);
		int GetSubControlFixedHeight(std::wstring pstrSubControlName);
		int GetSubControlFixedWdith(std::wstring pstrSubControlName);
		const std::wstring GetSubControlUserData(std::wstring pstrSubControlName);
		Control* FindSubControl(std::wstring pstrSubControlName);

		virtual SIZE GetScrollPos() const;
		virtual SIZE GetScrollRange() const;
		virtual void SetScrollPos(SIZE szPos, bool bMsg = true);
		virtual void SetScrollStepSize(int nSize);
		virtual int GetScrollStepSize() const;
		virtual void LineUp();
		virtual void LineDown();
		virtual void PageUp();
		virtual void PageDown();
		virtual void HomeUp();
		virtual void EndDown();
		virtual void LineLeft();
		virtual void LineRight();
		virtual void PageLeft();
		virtual void PageRight();
		virtual void HomeLeft();
		virtual void EndRight();
		virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
		virtual ScrollBar* GetVerticalScrollBar() const;
		virtual ScrollBar* GetHorizontalScrollBar() const;

	protected:
		virtual void SetFloatPos(int iIndex);
		virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

	protected:
		CStdPtrArray items_;
		RECT rc_inset_;
		int child_padding_;
		UINT child_align_;
		UINT child_v_align_;
		bool auto_destroy_;
		bool delayed_destroy_;
		bool child_mouse_enabled_;
		int	 scroll_steop_size_;

		ScrollBar* vertical_scrollbar_ = NULL;
		ScrollBar* horizontal_scrollbar_ = NULL;
		std::wstring	vscrollbar_class_;
		std::wstring	hscorllbar_class_;
	};

} // namespace DuiLib

#endif // __UICONTAINER_H__
