#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#pragma once

namespace ui
{
	class UILIB_API ScrollBar : public Control
	{
		DECLARE_DUICONTROL(ScrollBar)
	public:
		ScrollBar();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		void AttachScrollChange(const EventCallback &cb);

		virtual void DoInit();

		Box* GetOwner() const;
		void SetOwner(Box* pOwner);

		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);
		void SetFocus();

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetScrollRange() const;
		void SetScrollRange(int nRange);
		int GetScrollPos() const;
		void SetScrollPos(int nPos);
		int GetLineSize() const;
		void SetLineSize(int nSize);

		bool GetShowButton1();
		void SetShowButton1(bool bShow);
		std::wstring GetButton1NormalImage();
		void SetButton1NormalImage(std::wstring pStrImage);
		std::wstring GetButton1HotImage();
		void SetButton1HotImage(std::wstring pStrImage);
		std::wstring GetButton1PushedImage();
		void SetButton1PushedImage(std::wstring pStrImage);
		std::wstring GetButton1DisabledImage();
		void SetButton1DisabledImage(std::wstring pStrImage);

		bool GetShowButton2();
		void SetShowButton2(bool bShow);
		std::wstring GetButton2NormalImage();
		void SetButton2NormalImage(std::wstring pStrImage);
		std::wstring GetButton2HotImage();
		void SetButton2HotImage(std::wstring pStrImage);
		std::wstring GetButton2PushedImage();
		void SetButton2PushedImage(std::wstring pStrImage);
		std::wstring GetButton2DisabledImage();
		void SetButton2DisabledImage(std::wstring pStrImage);

		std::wstring GetThumbNormalImage();
		void SetThumbNormalImage(std::wstring pStrImage);
		std::wstring GetThumbHotImage();
		void SetThumbHotImage(std::wstring pStrImage);
		std::wstring GetThumbPushedImage();
		void SetThumbPushedImage(std::wstring pStrImage);
		std::wstring GetThumbDisabledImage();
		void SetThumbDisabledImage(std::wstring pStrImage);

		std::wstring GetRailNormalImage();
		void SetRailNormalImage(std::wstring pStrImage);
		std::wstring GetRailHotImage();
		void SetRailHotImage(std::wstring pStrImage);
		std::wstring GetRailPushedImage();
		void SetRailPushedImage(std::wstring pStrImage);
		std::wstring GetRailDisabledImage();
		void SetRailDisabledImage(std::wstring pStrImage);

		std::wstring GetBkNormalImage();
		void SetBkNormalImage(std::wstring pStrImage);
		std::wstring GetBkHotImage();
		void SetBkHotImage(std::wstring pStrImage);
		std::wstring GetBkPushedImage();
		void SetBkPushedImage(std::wstring pStrImage);
		std::wstring GetBkDisabledImage();
		void SetBkDisabledImage(std::wstring pStrImage);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void DoPaint(HDC hDC, const RECT& rcPaint);

		void PaintBk(HDC hDC);
		void PaintButton1(HDC hDC);
		void PaintButton2(HDC hDC);
		void PaintThumb(HDC hDC);
		void PaintRail(HDC hDC);

	protected:

		enum
		{ 
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool m_bHorizontal;
		int m_nRange;
		int m_nScrollPos;
		int m_nLineSize;
		Box* owner_;
		POINT last_mouse_pt_;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int m_nScrollRepeatDelay;

		std::wstring m_sBkNormalImage;
		std::wstring m_sBkHotImage;
		std::wstring m_sBkPushedImage;
		std::wstring m_sBkDisabledImage;

		bool m_bShowButton1;
		RECT m_rcButton1;
		UINT m_uButton1State;
		std::wstring m_sButton1NormalImage;
		std::wstring m_sButton1HotImage;
		std::wstring m_sButton1PushedImage;
		std::wstring m_sButton1DisabledImage;

		bool m_bShowButton2;
		RECT m_rcButton2;
		UINT m_uButton2State;
		std::wstring m_sButton2NormalImage;
		std::wstring m_sButton2HotImage;
		std::wstring m_sButton2PushedImage;
		std::wstring m_sButton2DisabledImage;

		RECT m_rcThumb;
		UINT m_uThumbState;
		std::wstring m_sThumbNormalImage;
		std::wstring thumb_hot_image_;
		std::wstring thumb_pushed_image_;
		std::wstring m_sThumbDisabledImage;

		std::wstring m_sRailNormalImage;
		std::wstring m_sRailHotImage;
		std::wstring m_sRailPushedImage;
		std::wstring m_sRailDisabledImage;

		std::wstring image_modify_;
	};
}

#endif // __UISCROLLBAR_H__