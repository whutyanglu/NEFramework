#ifndef __UIHOTKEY_H__
#define __UIHOTKEY_H__
#pragma once

namespace ui{
	class HotKey;

	class UILIB_API HotKeyWnd : public Window
	{
	public:
		HotKeyWnd(void);

	public:
		void Init(HotKey * pOwner);
		RECT CalPos();
		std::wstring GetWindowClassName() const;
		void OnFinalMessage(HWND hWnd);
		std::wstring GetSuperClassName() const;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	public:
		void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);
		void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
		DWORD GetHotKey(void) const;
		std::wstring GetHotKeyName();
		void SetRules(WORD wInvalidComb, WORD wModifiers);
		std::wstring GetKeyName(UINT vk, BOOL fExtended);

	protected:
		HotKey * owner_;
		HBRUSH bk_brush_;
		bool is_init_;
	};

	class UILIB_API HotKey : public Label
	{
		DECLARE_DUICONTROL(HotKey)
		friend HotKeyWnd;
	public:
		HotKey();
		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;
		void SetEnabled(bool bEnable = true);
		void SetText(std::wstring pstrText);
		std::wstring GetNormalImage();
		void SetNormalImage(std::wstring pStrImage);
		std::wstring GetHotImage();
		void SetHotImage(std::wstring pStrImage);
		std::wstring GetFocusedImage();
		void SetFocusedImage(std::wstring pStrImage);
		std::wstring GetDisabledImage();
		void SetDisabledImage(std::wstring pStrImage);
		void SetNativeBkColor(DWORD dwBkColor);
		DWORD GetNativeBkColor() const;

		void SetPos(RECT rc);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

	public:
		void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
		DWORD GetHotKey(void) const;
		void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);

	protected:
		HotKeyWnd * wnd_;
		UINT btn_state_;
		std::wstring normal_image_;
		std::wstring hot_image_;
		std::wstring focused_image_;
		std::wstring disabled_image_;
		DWORD bk_color_;

	protected:
		WORD virtual_keycode_;
		WORD modifiers_;
	};
}


#endif