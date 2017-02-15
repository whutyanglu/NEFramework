#include "StdAfx.h"
#include <WinSock2.h>
#pragma comment( lib, "ws2_32.lib" )

DWORD GetLocalIpAddress()   
{   
	WORD wVersionRequested = MAKEWORD(2, 2);   
	WSADATA wsaData;   
	if (WSAStartup(wVersionRequested, &wsaData) != 0)   
		return 0;   
	char local[255] = {0};   
	gethostname(local, sizeof(local));   
	hostent* ph = gethostbyname(local);   
	if (ph == NULL)   
		return 0;   
	in_addr addr;   
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr));   
	DWORD dwIP = MAKEIPADDRESS(addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
	return dwIP;
}

namespace ui
{
	//CDateTimeUI::m_nDTUpdateFlag
#define IP_NONE   0
#define IP_UPDATE 1
#define IP_DELETE 2
#define IP_KEEP   3

	class IPAddressWnd : public Window
	{
	public:
		IPAddressWnd();

		void Init(IPAddress* pOwner);
		RECT CalPos();

		std::wstring GetWindowClassName() const;
		std::wstring GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		IPAddress* owner_;
		HBRUSH m_hBkBrush;
		bool m_bInit;
	};

	IPAddressWnd::IPAddressWnd() : owner_(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
	}

	void IPAddressWnd::Init(IPAddress* pOwner)
	{
		owner_ = pOwner;
		owner_->update_flag_ = IP_NONE;

		if (wnd_ == NULL)
		{
			INITCOMMONCONTROLSEX   CommCtrl;
			CommCtrl.dwSize=sizeof(CommCtrl);
			CommCtrl.dwICC=ICC_INTERNET_CLASSES;//Ö¸¶¨Class
			if(InitCommonControlsEx(&CommCtrl))
			{
				RECT rcPos = CalPos();
				UINT uStyle = WS_CHILD | WS_TABSTOP | WS_GROUP;
				Create(owner_->GetManager()->GetPaintWindow(), L"", uStyle, 0, rcPos);
			}
			SetWindowFont(wnd_, GlobalManager::GetFontInfo(owner_->GetFont(), ::GetDC(wnd_))->handle_, TRUE);
		}

		if (owner_->GetText().empty())
			owner_->ip_ = GetLocalIpAddress();
		::SendMessage(wnd_, IPM_SETADDRESS, 0, owner_->ip_);
		::ShowWindow(wnd_, SW_SHOW);
		::SetFocus(wnd_);

		m_bInit = true;    
	}

	RECT IPAddressWnd::CalPos()
	{
		CDuiRect rcPos = owner_->GetPos();
		return rcPos;
	}

	std::wstring IPAddressWnd::GetWindowClassName() const
	{
		return _T("IPAddressWnd");
	}

	std::wstring IPAddressWnd::GetSuperClassName() const
	{
		return WC_IPADDRESS;
	}

	void IPAddressWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		// Clear reference and die
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		owner_->wnd_ = NULL;
		delete this;
	}

	LRESULT IPAddressWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if( uMsg == WM_KILLFOCUS )
		{
			bHandled = TRUE;
			return 0;
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
		else if (uMsg == WM_KEYUP && (wParam == VK_DELETE || wParam == VK_BACK))
		{
			LRESULT lRes = ::DefWindowProc(wnd_, uMsg, wParam, lParam);
			owner_->update_flag_ = IP_DELETE;
			owner_->UpdateText();
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else if (uMsg == WM_KEYUP && wParam == VK_ESCAPE)
		{
			LRESULT lRes = ::DefWindowProc(wnd_, uMsg, wParam, lParam);
			owner_->update_flag_ = IP_KEEP;
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else if( uMsg == OCM_COMMAND ) {
			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_KILLFOCUS )
			{
				lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
			}
		}
		else bHandled = FALSE;
		if( !bHandled ) return Window::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT IPAddressWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HWND hWndFocus = GetFocus();
		while (hWndFocus)
		{
			if (GetFocus() == wnd_)
			{
				bHandled = TRUE;
				return 0;
			}
			hWndFocus = GetParent(hWndFocus);
		}

		LRESULT lRes = ::DefWindowProc(wnd_, uMsg, wParam, lParam);
		if (owner_->update_flag_ == IP_NONE)
		{
			::SendMessage(wnd_, IPM_GETADDRESS, 0, (LPARAM)&owner_->ip_);
			owner_->update_flag_ = IP_UPDATE;
			owner_->UpdateText();
		}
		::ShowWindow(wnd_, SW_HIDE);
		return lRes;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_DUICONTROL(IPAddress)

		IPAddress::IPAddress()
	{
		ip_ = GetLocalIpAddress();
		read_only_ = false;
		wnd_ = NULL;
		update_flag_=IP_UPDATE;
		UpdateText();
		update_flag_ = IP_NONE;
	}

	std::wstring IPAddress::GetClass() const
	{
		return DUI_CTR_IPADDRESS;
	}

	LPVOID IPAddress::GetInterface(std::wstring pstrName)
	{
		if( _tcscmp(pstrName.c_str(), DUI_CTR_IPADDRESS) == 0 ) return static_cast<IPAddress*>(this);
		return Label::GetInterface(pstrName);
	}

	DWORD IPAddress::GetIP()
	{
		return ip_;
	}

	void IPAddress::SetIP(DWORD dwIP)
	{
		ip_ = dwIP;
		UpdateText();
	}

	void IPAddress::SetReadOnly(bool bReadOnly)
	{
		read_only_ = bReadOnly;
		Invalidate();
	}

	bool IPAddress::IsReadOnly() const
	{
		return read_only_;
	}

	void IPAddress::UpdateText()
	{
		if (update_flag_ == IP_DELETE)
			SetText(_T(""));
		else if (update_flag_ == IP_UPDATE)
		{
			TCHAR szIP[MAX_PATH] = {0};
			in_addr addr;
			addr.S_un.S_addr = ip_;
			_stprintf(szIP, _T("%d.%d.%d.%d"), addr.S_un.S_un_b.s_b4, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b1);
			SetText(szIP);
		}
	}

	void IPAddress::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Label::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetCursor && IsEnabled() ){
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}

		if( event.type_ == kEventWindowSize ){
			if( wnd_ != NULL ) pm_->SetFocusNeeded(this);
		}

		if( event.type_ == kMouseScrollWheel ){
			if( wnd_ != NULL ) return;
		}

		if( event.type_ == kEventSetFocus && IsEnabled() ) {
			if( wnd_ ) 
			{
				return;
			}
			wnd_ = new IPAddressWnd();
			ASSERT(wnd_);
			wnd_->Init(this);
			wnd_->ShowWindow();
		}

		if( event.type_ == kEventKillFocus && IsEnabled() ) {
			Invalidate();
		}

		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick || event.type_ == kEventMouseRightButtonDown) {
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && wnd_ == NULL ){
					wnd_ = new IPAddressWnd();
					ASSERT(wnd_);
				}
				if( wnd_ != NULL ){
					wnd_->Init(this);
					wnd_->ShowWindow();
				}
			}
			return;
		}

		if( event.type_ == kEventMouseMove ) {
			return;
		}
		if( event.type_ == kEventMouseButtonUp ) {
			return;
		}
		if( event.type_ == kEventMouseMenu ){
			return;
		}
		if( event.type_ == kEventMouseEnter ){
			return;
		}
		if( event.type_ == kEventMouseLeave ){
			return;
		}

		Label::DoEvent(event);
	}

	void IPAddress::SetAttribute(std::wstring pstrName, std::wstring pstrValue )
	{
		Label::SetAttribute(pstrName, pstrValue);
	}
}
