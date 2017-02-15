#include "StdAfx.h"

#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

namespace ui {

Window::Window() : wnd_(NULL), old_wnd_proc_(::DefWindowProc), sub_classed_(false)
{
}

HWND Window::GetHWND() const 
{ 
    return wnd_; 
}

UINT Window::GetClassStyle() const
{
    return 0;
}

std::wstring Window::GetSuperClassName() const
{
    return L"";
}

Window::operator HWND() const
{
    return wnd_;
}

HWND Window::CreateDuiWindow( HWND hwndParent, std::wstring pstrWindowName,DWORD dwStyle /*=0*/, DWORD dwExStyle /*=0*/ )
{
	return Create(hwndParent,pstrWindowName,dwStyle,dwExStyle,0,0,0,0,NULL);
}

HWND Window::Create(HWND hwndParent, std::wstring pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
    return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND Window::Create(HWND hwndParent, std::wstring pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
    if( !GetSuperClassName().empty() && !RegisterSuperclass() ) return NULL;
    if( GetSuperClassName().empty() && !RegisterWindowClass() ) return NULL;
	std::wstring window_cls_name = GetWindowClassName();
    wnd_ = ::CreateWindowEx(dwExStyle, window_cls_name.c_str(), pstrName.c_str(),
		dwStyle, x, y, cx, cy, hwndParent, hMenu, PaintManager::GetInstance(), this);
    ASSERT(wnd_!=NULL);
    return wnd_;
}

HWND Window::Subclass(HWND hWnd)
{
    ASSERT(::IsWindow(hWnd));
    ASSERT(wnd_==NULL);
    old_wnd_proc_ = SubclassWindow(hWnd, __WndProc);
    if( old_wnd_proc_ == NULL ) return NULL;
    sub_classed_ = true;
    wnd_ = hWnd;
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
    return wnd_;
}

void Window::Unsubclass()
{
    ASSERT(::IsWindow(wnd_));
    if( !::IsWindow(wnd_) ) return;
    if( !sub_classed_ ) return;
    SubclassWindow(wnd_, old_wnd_proc_);
    old_wnd_proc_ = ::DefWindowProc;
    sub_classed_ = false;
}

void Window::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
    ASSERT(::IsWindow(wnd_));
    if( !::IsWindow(wnd_) ) return;
    ::ShowWindow(wnd_, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}

UINT Window::ShowModal()
{
    ASSERT(::IsWindow(wnd_));
    UINT nRet = 0;
    HWND hWndParent = GetWindowOwner(wnd_);
    ::ShowWindow(wnd_, SW_SHOWNORMAL);
    ::EnableWindow(hWndParent, FALSE);
    MSG msg = { 0 };
    while( ::IsWindow(wnd_) && ::GetMessage(&msg, NULL, 0, 0) ) {
        if( msg.message == WM_CLOSE && msg.hwnd == wnd_ ) {
            nRet = msg.wParam;
            ::EnableWindow(hWndParent, TRUE);
            ::SetFocus(hWndParent);
        }
        if( !PaintManager::TranslateMessage(&msg) ) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        if( msg.message == WM_QUIT ) break;
    }
    ::EnableWindow(hWndParent, TRUE);
    ::SetFocus(hWndParent);
    if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);
    return nRet;
}

void Window::Close(UINT nRet)
{
    ASSERT(::IsWindow(wnd_));
    if( !::IsWindow(wnd_) ) return;
    PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
}

void Window::CenterWindow()
{
    ASSERT(::IsWindow(wnd_));
    ASSERT((GetWindowStyle(wnd_)&WS_CHILD)==0);
    RECT rcDlg = { 0 };
    ::GetWindowRect(wnd_, &rcDlg);
    RECT rcArea = { 0 };
    RECT rcCenter = { 0 };
	HWND hWnd=*this;
    HWND hWndParent = ::GetParent(wnd_);
    HWND hWndCenter = ::GetWindowOwner(wnd_);
	if (hWndCenter!=NULL)
		hWnd=hWndCenter;

	// 处理多显示器模式下屏幕居中
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

    if( hWndCenter == NULL )
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // Find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // The dialog is outside the screen, move it inside
    if( xLeft < rcArea.left ) xLeft = rcArea.left;
    else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
    if( yTop < rcArea.top ) yTop = rcArea.top;
    else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
    ::SetWindowPos(wnd_, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void Window::SetIcon(UINT nRes)
{
	HICON hIcon = (HICON)::LoadImage(PaintManager::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON,
		(::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// 防止高DPI下图标模糊
		LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(wnd_, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);

	hIcon = (HICON)::LoadImage(PaintManager::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON,
		(::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// 防止高DPI下图标模糊
		LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(wnd_, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
}

bool Window::RegisterWindowClass()
{
    WNDCLASS wc = { 0 };
    wc.style = GetClassStyle();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = Window::__WndProc;
    wc.hInstance = PaintManager::GetInstance();
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
	std::wstring wndName = GetWindowClassName();
    wc.lpszClassName = wndName.c_str();
    ATOM ret = ::RegisterClass(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool Window::RegisterSuperclass()
{
    // Get the class information from an existing
    // window so we can subclass it later on...
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
	std::wstring super_cls_name = GetSuperClassName();
    if( !::GetClassInfoEx(NULL, GetSuperClassName().c_str(), &wc) ) {
        if( !::GetClassInfoEx(PaintManager::GetInstance(), super_cls_name.c_str(), &wc) ) {
            ASSERT(!"Unable to locate window class");
            return NULL;
        }
    }
    old_wnd_proc_ = wc.lpfnWndProc;
    wc.lpfnWndProc = Window::__ControlProc;
    wc.hInstance = PaintManager::GetInstance();
	std::wstring clsName = GetWindowClassName();
    wc.lpszClassName = clsName.c_str();
    ATOM ret = ::RegisterClassEx(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK Window::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Window* pThis = NULL;
    if( uMsg == WM_NCCREATE ) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<Window*>(lpcs->lpCreateParams);
        pThis->wnd_ = hWnd;
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
    } 
    else {
        pThis = reinterpret_cast<Window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if( uMsg == WM_NCDESTROY && pThis != NULL ) {
            LRESULT lRes = ::CallWindowProc(pThis->old_wnd_proc_, hWnd, uMsg, wParam, lParam);
            ::SetWindowLongPtr(pThis->wnd_, GWLP_USERDATA, 0L);
            if( pThis->sub_classed_ ) pThis->Unsubclass();
            pThis->wnd_ = NULL;
            pThis->OnFinalMessage(hWnd);
            return lRes;
        }
    }
    if( pThis != NULL ) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } 
    else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK Window::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Window* pThis = NULL;
    if( uMsg == WM_NCCREATE ) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<Window*>(lpcs->lpCreateParams);
        ::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
        pThis->wnd_ = hWnd;
    } 
    else {
        pThis = reinterpret_cast<Window*>(::GetProp(hWnd, _T("WndX")));
        if( uMsg == WM_NCDESTROY && pThis != NULL ) {
            LRESULT lRes = ::CallWindowProc(pThis->old_wnd_proc_, hWnd, uMsg, wParam, lParam);
            if( pThis->sub_classed_ ) pThis->Unsubclass();
            ::SetProp(hWnd, _T("WndX"), NULL);
            pThis->wnd_ = NULL;
            pThis->OnFinalMessage(hWnd);
            return lRes;
        }
    }
    if( pThis != NULL ) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } 
    else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT Window::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(wnd_));
    return ::SendMessage(wnd_, uMsg, wParam, lParam);
} 

LRESULT Window::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(wnd_));
    return ::PostMessage(wnd_, uMsg, wParam, lParam);
}

void Window::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
    ASSERT(::IsWindow(wnd_));
    RECT rc = { 0 };
    if( !::GetClientRect(wnd_, &rc) ) return;
    if( cx != -1 ) rc.right = cx;
    if( cy != -1 ) rc.bottom = cy;
    if( !::AdjustWindowRectEx(&rc, GetWindowStyle(wnd_), (!(GetWindowStyle(wnd_) & WS_CHILD) && (::GetMenu(wnd_) != NULL)), GetWindowExStyle(wnd_)) ) return;
    ::SetWindowPos(wnd_, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::CallWindowProc(old_wnd_proc_, wnd_, uMsg, wParam, lParam);
}

void Window::OnFinalMessage(HWND /*hWnd*/)
{
}

} // namespace DuiLib
