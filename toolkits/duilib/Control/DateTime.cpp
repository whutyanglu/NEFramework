#include "StdAfx.h"
#include "DateTime.h"

namespace ui
{
	//CDateTimeUI::m_nDTUpdateFlag
#define DT_NONE   0
#define DT_UPDATE 1
#define DT_DELETE 2
#define DT_KEEP   3

	class DateTimeWnd : public Window
	{
	public:
		DateTimeWnd();

		void Init(DateTime* pOwner);
		RECT CalPos();

		std::wstring GetWindowClassName() const;
		std::wstring GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		DateTime* owner_;
		HBRUSH m_hBkBrush;
		bool m_bInit;
		bool m_bDropOpen;
		SYSTEMTIME m_oldSysTime;
	};

	DateTimeWnd::DateTimeWnd() : owner_(NULL), m_hBkBrush(NULL), m_bInit(false), m_bDropOpen(false)
	{
	}

	void DateTimeWnd::Init(DateTime* pOwner)
	{
		owner_ = pOwner;
		owner_->update_flag_ = DT_NONE;

		if (wnd_ == NULL)
		{
			RECT rcPos = CalPos();
			UINT uStyle = WS_CHILD;
			Create(owner_->GetManager()->GetPaintWindow(), L"", uStyle, 0, rcPos);
			SetWindowFont(wnd_, GlobalManager::GetFontInfo(owner_->GetFont(),  ::GetDC(wnd_))->handle_, TRUE);
		}

		if (owner_->GetText().empty()) {
			::GetLocalTime(&owner_->sys_time_);
		}
		memcpy(&m_oldSysTime, &owner_->sys_time_, sizeof(SYSTEMTIME));
		::SendMessage(wnd_, DTM_SETSYSTEMTIME, 0, (LPARAM)&owner_->sys_time_);
		::ShowWindow(wnd_, SW_SHOWNOACTIVATE);
		::SetFocus(wnd_);

		m_bInit = true;    
	}

	RECT DateTimeWnd::CalPos()
	{
		CDuiRect rcPos = owner_->GetPos();

		Control* pParent = owner_;
		RECT rcParent;
		while( pParent = pParent->GetParent() ) {
			if( !pParent->IsVisible() ) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
			rcParent = pParent->GetClientPos();
			if( !::IntersectRect(&rcPos, &rcPos, &rcParent) ) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
		}

		return rcPos;
	}

	std::wstring DateTimeWnd::GetWindowClassName() const
	{
		return _T("DateTimeWnd");
	}

	std::wstring DateTimeWnd::GetSuperClassName() const
	{
		return DATETIMEPICK_CLASS;
	}

	void DateTimeWnd::OnFinalMessage(HWND hWnd)
	{
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		if( owner_->GetManager()->IsLayered() ) {
			owner_->GetManager()->RemovePaintChildWnd(hWnd);
		} 
		owner_->wnd_ = NULL;
		delete this;
	}

	LRESULT DateTimeWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE){
			memcpy(&owner_->sys_time_, &m_oldSysTime, sizeof(SYSTEMTIME));
			owner_->update_flag_ = DT_UPDATE;
			owner_->UpdateText();
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else if(uMsg == OCM_NOTIFY){
			NMHDR* pHeader=(NMHDR*)lParam;
			if(pHeader != NULL && pHeader->hwndFrom == wnd_) {
				if(pHeader->code == DTN_DATETIMECHANGE) {
					LPNMDATETIMECHANGE lpChage=(LPNMDATETIMECHANGE)lParam;
					::SendMessage(wnd_, DTM_GETSYSTEMTIME, 0, (LPARAM)&owner_->sys_time_);
					owner_->update_flag_ = DT_UPDATE;
					owner_->UpdateText();
				}
				else if(pHeader->code == DTN_DROPDOWN) {
					m_bDropOpen = true;

				}
				else if(pHeader->code == DTN_CLOSEUP) {
					::SendMessage(wnd_, DTM_GETSYSTEMTIME, 0, (LPARAM)&owner_->sys_time_);
					owner_->update_flag_ = DT_UPDATE;
					owner_->UpdateText();
					PostMessage(WM_CLOSE);
					m_bDropOpen = false;
				}
			}
			bHandled = FALSE;
		}
		else if(uMsg == WM_KILLFOCUS){
			if(!m_bDropOpen) {
				PostMessage(WM_CLOSE);
			}
			bHandled = FALSE;
		}
		else if( uMsg == WM_PAINT) {
			if (owner_->GetManager()->IsLayered()) {
				owner_->GetManager()->AddPaintChildWnd(wnd_);
			}
			bHandled = FALSE;
		}
		else bHandled = FALSE;
		if( !bHandled ) return Window::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}
	//////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_DUICONTROL(DateTime)

	DateTime::DateTime()
	{
		::GetLocalTime(&sys_time_);
		is_readonly_ = false;
		wnd_ = NULL;
		update_flag_=DT_UPDATE;
		UpdateText();
		update_flag_ = DT_NONE;
	}

	std::wstring DateTime::GetClass() const
	{
		return _T("DateTime");
	}

	LPVOID DateTime::GetInterface(std::wstring pstrName)
	{
		if( _tcscmp(pstrName.c_str(), DUI_CTR_DATETIME) == 0 ) return static_cast<DateTime*>(this);
		return Label::GetInterface(pstrName);
	}

	SYSTEMTIME& DateTime::GetTime()
	{
		return sys_time_;
	}

	void DateTime::SetTime(SYSTEMTIME* pst)
	{
		sys_time_ = *pst;
		Invalidate();
		update_flag_ = DT_UPDATE;
		UpdateText();
		update_flag_ = DT_NONE;
	}

	void DateTime::SetReadOnly(bool bReadOnly)
	{
		is_readonly_ = bReadOnly;
		Invalidate();
	}

	bool DateTime::IsReadOnly() const
	{
		return is_readonly_;
	}

	void DateTime::UpdateText()
	{
		if (update_flag_ == DT_DELETE) {
			SetText(_T(""));
		}
		else if (update_flag_ == DT_UPDATE) {
			std::wstring sText;
			sText = nbase::StringPrintf(_T("%4d-%02d-%02d"), sys_time_.wYear, sys_time_.wMonth, sys_time_.wDay, sys_time_.wHour, sys_time_.wMinute);
			SetText(sText);
		}
	}

	void DateTime::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Label::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetCursor && IsEnabled() )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if( event.type_ == kEventWindowSize )
		{
			if( wnd_ != NULL ) pm_->SetFocusNeeded(this);
		}
		if( event.type_ == kMouseScrollWheel )
		{
			if( wnd_ != NULL ) return;
		}
		if( event.type_ == kEventSetFocus && IsEnabled() ) 
		{
			if( wnd_ ) return;
			wnd_ = new DateTimeWnd();
			ASSERT(wnd_);
			wnd_->Init(this);
			wnd_->ShowWindow();
		}
		if( event.type_ == kEventKillFocus && IsEnabled() ) 
		{
			Invalidate();
		}
		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick || event.type_ == kEventMouseRightButtonDown) 
		{
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && wnd_ == NULL )
				{
					wnd_ = new DateTimeWnd();
					ASSERT(wnd_);
				}
				if( wnd_ != NULL )
				{
					wnd_->Init(this);
					wnd_->ShowWindow();
				}
			}
			return;
		}
		if( event.type_ == kEventMouseMove ) 
		{
			return;
		}
		if( event.type_ == kEventMouseButtonUp ) 
		{
			return;
		}
		if( event.type_ == kEventMouseMenu )
		{
			return;
		}
		if( event.type_ == kEventMouseEnter )
		{
			return;
		}
		if( event.type_ == kEventMouseLeave )
		{
			return;
		}

		Label::DoEvent(event);
	}
}
