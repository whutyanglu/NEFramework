#ifndef __UITRAICON_H__
#define __UITRAICON_H__

#pragma once
#include <ShellAPI.h>

namespace ui
{
	class UILIB_API TrayIcon
	{
	public:
		TrayIcon(void);
		~TrayIcon(void);

	public:
		void CreateTrayIcon( HWND _RecvHwnd, UINT _IconIDResource, std::wstring _ToolTipText = L"", UINT _Message = NULL);
		void DeleteTrayIcon();
		bool SetTooltipText(std::wstring _ToolTipText);
		bool SetTooltipText(UINT _IDResource);
		std::wstring GetTooltipText() const;

		bool SetIcon(HICON _Hicon);
		bool SetIcon(std::wstring _IconFile);
		bool SetIcon(UINT _IDResource);
		HICON GetIcon() const;
		void SetHideIcon();
		void SetShowIcon();
		void RemoveIcon();
		bool Enabled(){return m_bEnabled;};
		bool IsVisible(){return !m_bVisible;};

	private:
		bool m_bEnabled;
		bool m_bVisible;
		HWND m_hWnd;
		UINT m_uMessage;
		HICON m_hIcon;
		NOTIFYICONDATA	m_trayData;
	};
}
#endif // 

