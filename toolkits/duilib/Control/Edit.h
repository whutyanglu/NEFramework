#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

namespace ui
{
	class EditWnd;

	class UILIB_API Edit : public Label
	{
		DECLARE_DUICONTROL(Edit)
		friend class EditWnd;
	public:
		Edit();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		void AttachReturn(const EventCallback &cb);
		void AttachTextChange(const EventCallback &cb);

		virtual int GetFixedWidth() override;
		virtual int GetFixedHeight() override;

		void SetEnabled(bool bEnable = true);
		void SetText(std::wstring pstrText);
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		void SetPasswordChar(TCHAR cPasswordChar);
		TCHAR GetPasswordChar() const;
		void SetNumberOnly(bool bNumberOnly);
		bool IsNumberOnly() const;
		int GetWindowStyls() const;

		std::wstring GetNormalImage();
		void SetNormalImage(std::wstring pStrImage);
		std::wstring GetHotImage();
		void SetHotImage(std::wstring pStrImage);
		std::wstring GetFocusedImage();
		void SetFocusedImage(std::wstring pStrImage);
		std::wstring GetDisabledImage();
		void SetDisabledImage(std::wstring pStrImage);
		void SetNativeEditBkColor(DWORD dwBkColor);
		DWORD GetNativeEditBkColor() const;
		void SetNativeEditTextColor(std::wstring pStrColor );
		DWORD GetNativeEditTextColor() const;

		void SetSel(long nStartChar, long nEndChar);
		void SetSelAll();
		void SetReplaceSel(std::wstring lpszReplace);

		void SetTipValue(std::wstring pStrTipValue);
		std::wstring GetTipValue();
		void SetTipValueColor(std::wstring pStrColor);
		DWORD GetTipValueColor();

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		EditWnd* wnd_;

		UINT max_char_;
		bool is_readonly_;
		bool is_password_mode_;
		TCHAR password_char_;
		UINT btn_state_;
		std::wstring normal_image_;
		std::wstring hot_image_;
		std::wstring focused_image_;
		std::wstring disabled_image_;
		std::wstring tip_;
		DWORD tip_color_;
		DWORD bk_color_;
		DWORD text_color_;
		int wnd_style_;
	};
}
#endif // __UIEDIT_H__