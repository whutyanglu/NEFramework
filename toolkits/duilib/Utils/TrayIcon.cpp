#include "StdAfx.h"
#include "TrayIcon.h"

namespace ui
{
	TrayIcon::TrayIcon(void)
	{
		memset(&m_trayData, 0, sizeof(m_trayData));
		m_bEnabled = false;
		m_bVisible = false;
		m_hWnd = NULL;
		m_uMessage = UIMSG_TRAYICON;
	}

	TrayIcon::~TrayIcon(void)
	{
		DeleteTrayIcon();
	}

	void TrayIcon::CreateTrayIcon( HWND _RecvHwnd, UINT _IconIDResource, std::wstring _ToolTipText, UINT _Message)
	{
		if(!_RecvHwnd || _IconIDResource <= 0 ){
			return;
		}
		if(_Message != 0) m_uMessage = _Message;
		m_hIcon = LoadIcon(PaintManager::GetInstance(), MAKEINTRESOURCE(_IconIDResource));
		m_trayData.cbSize = sizeof(NOTIFYICONDATA);
		m_trayData.hWnd	 = _RecvHwnd;
		m_trayData.uID	 = _IconIDResource;
		m_trayData.hIcon = m_hIcon;
		m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_trayData.uCallbackMessage = m_uMessage;
		if(!_ToolTipText.empty()) _tcscpy(m_trayData.szTip, _ToolTipText.c_str());
		Shell_NotifyIcon(NIM_ADD, &m_trayData);
		m_bEnabled = true;
	}

	void TrayIcon::DeleteTrayIcon()
	{
		Shell_NotifyIcon(NIM_DELETE, &m_trayData);
		m_bEnabled = false;
		m_bVisible = false;
		m_hWnd = NULL;
		m_uMessage = UIMSG_TRAYICON;
	}

	bool TrayIcon::SetTooltipText( std::wstring _ToolTipText )
	{
		if(!_ToolTipText.empty()) _tcscpy(m_trayData.szTip,_ToolTipText.c_str());
		if (!m_bEnabled) return FALSE;
		m_trayData.uFlags = NIF_TIP;
		return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;
	}

	bool TrayIcon::SetTooltipText( UINT _IDResource )
	{
		TCHAR mbuf[256] = {0};
		LoadString(PaintManager::GetInstance(), _IDResource,mbuf, 256);
		return SetTooltipText(mbuf);
	}

	std::wstring TrayIcon::GetTooltipText() const
	{
		return m_trayData.szTip;
	}

	bool TrayIcon::SetIcon( HICON _Hicon )
	{
		if(_Hicon) m_hIcon = _Hicon;
		m_trayData.uFlags = NIF_ICON;
		m_trayData.hIcon = _Hicon;
		
		if (!m_bEnabled) return FALSE;
		return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;

		return false;
	}

	bool TrayIcon::SetIcon( std::wstring _IconFile )
	{
		HICON hIcon = LoadIcon(PaintManager::GetInstance(),_IconFile.c_str());
		return SetIcon(hIcon);
	}

	bool TrayIcon::SetIcon( UINT _IDResource )
	{
		HICON hIcon = LoadIcon(PaintManager::GetInstance(), MAKEINTRESOURCE(_IDResource));
		return SetIcon(hIcon);
	}

	HICON TrayIcon::GetIcon() const
	{
		HICON hIcon = NULL;
		hIcon = m_trayData.hIcon;
		return hIcon;
	}

	void TrayIcon::SetHideIcon()
	{
		if (IsVisible()) {
			SetIcon((HICON)NULL);
			m_bVisible = TRUE;
		}
	}

	void TrayIcon::SetShowIcon()
	{
		if (!IsVisible()) {
			SetIcon(m_hIcon);
			m_bVisible = FALSE;
		}
	}

	void TrayIcon::RemoveIcon()
	{
		m_trayData.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, &m_trayData);
		m_bEnabled = FALSE;
	}
}
