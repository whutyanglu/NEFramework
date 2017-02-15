#include "StdAfx.h"
#include "Shadow.h"
#include "math.h"
#include "crtdbg.h"

namespace ui
{

const TCHAR *strWndClassName = _T("PerryShadowWnd");
bool ShadowUI::s_bHasInit = FALSE;

ShadowUI::ShadowUI(void)
: wnd_((HWND)NULL)
, parent_proc_(NULL)
, status_(0)
, darkness_(150)
, sharp_ness_(5)
, size_(0)
, x_offset_(0)
, y_offset_(0)
, color_(RGB(0, 0, 0))
, wnd_size_(0)
, update_(false)
, is_image_mode_(false)
, is_show_shadow_(false)
, is_disable_show_(false)
{
	::ZeroMemory(&show_corner_, sizeof(RECT));
}

ShadowUI::~ShadowUI(void)
{
}

bool ShadowUI::Initialize(HINSTANCE hInstance)
{
	if (s_bHasInit)
		return false;

	// Register window class for shadow window
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= DefWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= strWndClassName;
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);

	s_bHasInit = true;
	return true;
}

void ShadowUI::Create(PaintManager* pPaintManager)
{
	if(!is_show_shadow_)
		return;

	// Already initialized
	_ASSERT(PaintManager::GetInstance() != INVALID_HANDLE_VALUE);
	_ASSERT(pPaintManager != NULL);
	pm_ = pPaintManager;
	HWND hParentWnd = pm_->GetPaintWindow();
	// Add parent window - shadow pair to the map
	_ASSERT(GetShadowMap().find(hParentWnd) == GetShadowMap().end());	// Only one shadow for each window
	GetShadowMap()[hParentWnd] = this;

	// Determine the initial show state of shadow according to parent window's state
	LONG lParentStyle = GetWindowLongPtr(hParentWnd, GWL_STYLE);

	// Create the shadow window
	LONG styleValue = lParentStyle & WS_CAPTION;
	wnd_ = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, strWndClassName, NULL,
		/*WS_VISIBLE | */styleValue | WS_POPUPWINDOW,
		CW_USEDEFAULT, 0, 0, 0, hParentWnd, NULL, PaintManager::GetInstance(), NULL);

	if(!(WS_VISIBLE & lParentStyle))	// Parent invisible
		status_ = SS_ENABLED;
	else if((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle)	// Parent visible but does not need shadow
		status_ = SS_ENABLED | SS_PARENTVISIBLE;
	else	// Show the shadow
	{
		status_ = SS_ENABLED | SS_VISABLE | SS_PARENTVISIBLE;
		::ShowWindow(wnd_, SW_SHOWNOACTIVATE);
		Update(hParentWnd);
	}

	// Replace the original WndProc of parent window to steal messages
	parent_proc_ = GetWindowLongPtr(hParentWnd, GWLP_WNDPROC);

#pragma warning(disable: 4311)	// temporrarily disable the type_cast warning in Win32
	SetWindowLongPtr(hParentWnd, GWLP_WNDPROC, (LONG_PTR)ParentProc);
#pragma warning(default: 4311)

}

std::map<HWND, ShadowUI *>& ShadowUI::GetShadowMap()
{
	static std::map<HWND, ShadowUI *> s_Shadowmap;
	return s_Shadowmap;
}

LRESULT CALLBACK ShadowUI::ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	_ASSERT(GetShadowMap().find(hwnd) != GetShadowMap().end());	// Shadow must have been attached

	ShadowUI *pThis = GetShadowMap()[hwnd];
	if (pThis->is_disable_show_) {

#pragma warning(disable: 4312)	// temporrarily disable the type_cast warning in Win32
		// Call the default(original) window procedure for other messages or messages processed but not returned
		return ((WNDPROC)pThis->parent_proc_)(hwnd, uMsg, wParam, lParam);
#pragma warning(default: 4312)
	}
	switch(uMsg)
	{
	case WM_ACTIVATEAPP:
	case WM_NCACTIVATE:
		{
			::SetWindowPos(pThis->wnd_, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
			break;
		}
	case WM_WINDOWPOSCHANGED:
		RECT WndRect;
		GetWindowRect(hwnd, &WndRect);
		if (pThis->is_image_mode_) {
			SetWindowPos(pThis->wnd_, hwnd, WndRect.left - pThis->size_, WndRect.top - pThis->size_, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		}
		else {
			SetWindowPos(pThis->wnd_, hwnd, WndRect.left + pThis->x_offset_ - pThis->size_, WndRect.top + pThis->y_offset_ - pThis->size_, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		}
		break;
	case WM_MOVE:
		if(pThis->status_ & SS_VISABLE)
		{
			RECT WndRect;
			GetWindowRect(hwnd, &WndRect);
			if (pThis->is_image_mode_) {
				SetWindowPos(pThis->wnd_, hwnd, WndRect.left - pThis->size_, WndRect.top - pThis->size_, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
			}
			else {
				SetWindowPos(pThis->wnd_, hwnd, WndRect.left + pThis->x_offset_ - pThis->size_, WndRect.top + pThis->y_offset_ - pThis->size_, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
			}
		}
		break;

	case WM_SIZE:
		if(pThis->status_ & SS_ENABLED)
		{
			if(SIZE_MAXIMIZED == wParam || SIZE_MINIMIZED == wParam)
			{
				::ShowWindow(pThis->wnd_, SW_HIDE);
				pThis->status_ &= ~SS_VISABLE;
			}
			else if(pThis->status_ & SS_PARENTVISIBLE)	// Parent maybe resized even if invisible
			{
				// Awful! It seems that if the window size was not decreased
				// the window region would never be updated until WM_PAINT was sent.
				// So do not Update() until next WM_PAINT is received in this case
				if(LOWORD(lParam) > LOWORD(pThis->wnd_size_) || HIWORD(lParam) > HIWORD(pThis->wnd_size_))
					pThis->update_ = true;
				else
					pThis->Update(hwnd);
				if(!(pThis->status_ & SS_VISABLE))
				{
					::ShowWindow(pThis->wnd_, SW_SHOWNOACTIVATE);
					pThis->status_ |= SS_VISABLE;
				}
			}
			pThis->wnd_size_ = lParam;
		}
		break;

	case WM_PAINT:
		{
			if(pThis->update_)
			{
				pThis->Update(hwnd);
				pThis->update_ = false;
			}
			//return hr;
			break;
		}

		// In some cases of sizing, the up-right corner of the parent window region would not be properly updated
		// Update() again when sizing is finished
	case WM_EXITSIZEMOVE:
		if(pThis->status_ & SS_VISABLE)
		{
			pThis->Update(hwnd);
		}
		break;

	case WM_SHOWWINDOW:
		if(pThis->status_ & SS_ENABLED)
		{
			if(!wParam)	// the window is being hidden
			{
				::ShowWindow(pThis->wnd_, SW_HIDE);
				pThis->status_ &= ~(SS_VISABLE | SS_PARENTVISIBLE);
			}
			else if(!(pThis->status_ & SS_PARENTVISIBLE))
			{
				//pThis->Update(hwnd);
				pThis->update_ = true;
				::ShowWindow(pThis->wnd_, SW_SHOWNOACTIVATE);
				pThis->status_ |= SS_VISABLE | SS_PARENTVISIBLE;
			}
		}
		break;

	case WM_DESTROY:
		DestroyWindow(pThis->wnd_);	// Destroy the shadow
		break;
		
	case WM_NCDESTROY:
		GetShadowMap().erase(hwnd);	// Remove this window and shadow from the map
		break;

	}


#pragma warning(disable: 4312)	// temporrarily disable the type_cast warning in Win32
	// Call the default(original) window procedure for other messages or messages processed but not returned
	return ((WNDPROC)pThis->parent_proc_)(hwnd, uMsg, wParam, lParam);
#pragma warning(default: 4312)

}
void GetLastErrorMessage() {          //Formats GetLastError() value.
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf, 0, NULL
	);

	// Display the string.
	//MessageBox(NULL, (const wchar_t*)lpMsgBuf, L"GetLastError", MB_OK | MB_ICONINFORMATION);

	// Free the buffer.
	LocalFree(lpMsgBuf);

}
void ShadowUI::Update(HWND hParent)
{
	if(!is_show_shadow_ || !(status_ & SS_VISABLE)) return;
	RECT WndRect;
	GetWindowRect(hParent, &WndRect);
	int nShadWndWid;
	int nShadWndHei;
	if (is_image_mode_) {
		if(shadow_image_.empty()) return;
		nShadWndWid = WndRect.right - WndRect.left + size_ * 2;
		nShadWndHei = WndRect.bottom - WndRect.top + size_ * 2;
	}
	else {
		if (size_ == 0) return;
		nShadWndWid = WndRect.right - WndRect.left + size_ * 2;
		nShadWndHei = WndRect.bottom - WndRect.top + size_ * 2;
	}
		
	// Create the alpha blending bitmap
	BITMAPINFO bmi;        // bitmap header
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nShadWndWid;
	bmi.bmiHeader.biHeight = nShadWndHei;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = nShadWndWid * nShadWndHei * 4;
	BYTE *pvBits;          // pointer to DIB section
	HBITMAP hbitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pvBits, NULL, 0);
	if (hbitmap == NULL) {
		GetLastErrorMessage();
	}

	HDC hMemDC = CreateCompatibleDC(NULL);
	if (hMemDC == NULL) {
		GetLastErrorMessage();
	}
	HBITMAP hOriBmp = (HBITMAP)SelectObject(hMemDC, hbitmap);
	if (GetLastError()!=0) {
		GetLastErrorMessage();
	}
	if (is_image_mode_)
	{
		RECT rcPaint = {0, 0, nShadWndWid, nShadWndHei};
		const TImageInfo* data = pm_->GetImageEx(shadow_image_, L"", 0);
		if( !data ) return;    
		RECT rcBmpPart = {0};
		rcBmpPart.right = data->nX;
		rcBmpPart.bottom = data->nY;
		RECT corner = show_corner_;
		RenderEngine::DrawImage(hMemDC, data->hBitmap, rcPaint, rcPaint, rcBmpPart, corner, data->bAlpha, 0xFF, true, false, false);
	}
	else
	{
		ZeroMemory(pvBits, bmi.bmiHeader.biSizeImage);
		MakeShadow((UINT32 *)pvBits, hParent, &WndRect);
	}

	POINT ptDst;
	if (is_image_mode_)
	{
		ptDst.x = WndRect.left - size_;
		ptDst.y = WndRect.top - size_;
	}
	else
	{
		ptDst.x = WndRect.left + x_offset_ - size_;
		ptDst.y = WndRect.top + y_offset_ - size_;
	}

	POINT ptSrc = {0, 0};
	SIZE WndSize = {nShadWndWid, nShadWndHei};
	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	MoveWindow(wnd_, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);
	BOOL bRet= ::UpdateLayeredWindow(wnd_, NULL, &ptDst, &WndSize, hMemDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
	_ASSERT(bRet); // something was wrong....
	// Delete used resources
	SelectObject(hMemDC, hOriBmp);
	DeleteObject(hbitmap);
	DeleteDC(hMemDC);
}

void ShadowUI::MakeShadow(UINT32 *pShadBits, HWND hParent, RECT *rcParent)
{
	// The shadow algorithm:
	// Get the region of parent window,
	// Apply morphologic erosion to shrink it into the size (ShadowWndSize - Sharpness)
	// Apply modified (with blur effect) morphologic dilation to make the blurred border
	// The algorithm is optimized by assuming parent window is just "one piece" and without "wholes" on it

	// Get the region of parent window,
	HRGN hParentRgn = CreateRectRgn(0, 0, 0, 0);
	GetWindowRgn(hParent, hParentRgn);

	// Determine the Start and end point of each horizontal scan line
	SIZE szParent = {rcParent->right - rcParent->left, rcParent->bottom - rcParent->top};
	SIZE szShadow = {szParent.cx + 2 * size_, szParent.cy + 2 * size_};
	// Extra 2 lines (set to be empty) in ptAnchors are used in dilation
	int nAnchors = max(szParent.cy, szShadow.cy);	// # of anchor points pares
	int (*ptAnchors)[2] = new int[nAnchors + 2][2];
	int (*ptAnchorsOri)[2] = new int[szParent.cy][2];	// anchor points, will not modify during erosion
	ptAnchors[0][0] = szParent.cx;
	ptAnchors[0][1] = 0;
	ptAnchors[nAnchors + 1][0] = szParent.cx;
	ptAnchors[nAnchors + 1][1] = 0;
	if(size_ > 0)
	{
		// Put the parent window anchors at the center
		for(int i = 0; i < size_; i++)
		{
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchors[i + 1][1] = 0;
			ptAnchors[szShadow.cy - i][0] = szParent.cx;
			ptAnchors[szShadow.cy - i][1] = 0;
		}
		ptAnchors += size_;
	}
	for(int i = 0; i < szParent.cy; i++)
	{
		// find start point
		int j;
		for(j = 0; j < szParent.cx; j++)
		{
			if(PtInRegion(hParentRgn, j, i))
			{
				ptAnchors[i + 1][0] = j + size_;
				ptAnchorsOri[i][0] = j;
				break;
			}
		}

		if(j >= szParent.cx)	// Start point not found
		{
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchorsOri[i][1] = 0;
			ptAnchors[i + 1][0] = szParent.cx;
			ptAnchorsOri[i][1] = 0;
		}
		else
		{
			// find end point
			for(j = szParent.cx - 1; j >= ptAnchors[i + 1][0]; j--)
			{
				if(PtInRegion(hParentRgn, j, i))
				{
					ptAnchors[i + 1][1] = j + 1 + size_;
					ptAnchorsOri[i][1] = j + 1;
					break;
				}
			}
		}
	}

	if(size_ > 0)
		ptAnchors -= size_;	// Restore pos of ptAnchors for erosion
	int (*ptAnchorsTmp)[2] = new int[nAnchors + 2][2];	// Store the result of erosion
	// First and last line should be empty
	ptAnchorsTmp[0][0] = szParent.cx;
	ptAnchorsTmp[0][1] = 0;
	ptAnchorsTmp[nAnchors + 1][0] = szParent.cx;
	ptAnchorsTmp[nAnchors + 1][1] = 0;
	int nEroTimes = 0;
	// morphologic erosion
	for(int i = 0; i < sharp_ness_ - size_; i++)
	{
		nEroTimes++;
		//ptAnchorsTmp[1][0] = szParent.cx;
		//ptAnchorsTmp[1][1] = 0;
		//ptAnchorsTmp[szParent.cy + 1][0] = szParent.cx;
		//ptAnchorsTmp[szParent.cy + 1][1] = 0;
		for(int j = 1; j < nAnchors + 1; j++)
		{
			ptAnchorsTmp[j][0] = max(ptAnchors[j - 1][0], max(ptAnchors[j][0], ptAnchors[j + 1][0])) + 1;
			ptAnchorsTmp[j][1] = min(ptAnchors[j - 1][1], min(ptAnchors[j][1], ptAnchors[j + 1][1])) - 1;
		}
		// Exchange ptAnchors and ptAnchorsTmp;
		int (*ptAnchorsXange)[2] = ptAnchorsTmp;
		ptAnchorsTmp = ptAnchors;
		ptAnchors = ptAnchorsXange;
	}

	// morphologic dilation
	ptAnchors += (size_ < 0 ? -size_ : 0) + 1;	// now coordinates in ptAnchors are same as in shadow window
	// Generate the kernel
	int nKernelSize = size_ > sharp_ness_ ? size_ : sharp_ness_;
	int nCenterSize = size_ > sharp_ness_ ? (size_ - sharp_ness_) : 0;
	UINT32 *pKernel = new UINT32[(2 * nKernelSize + 1) * (2 * nKernelSize + 1)];
	UINT32 *pKernelIter = pKernel;
	for(int i = 0; i <= 2 * nKernelSize; i++)
	{
		for(int j = 0; j <= 2 * nKernelSize; j++)
		{
			double dLength = sqrt((i - nKernelSize) * (i - nKernelSize) + (j - nKernelSize) * (double)(j - nKernelSize));
			if(dLength < nCenterSize)
				*pKernelIter = darkness_ << 24 | PreMultiply(color_, darkness_);
			else if(dLength <= nKernelSize)
			{
				UINT32 nFactor = ((UINT32)((1 - (dLength - nCenterSize) / (sharp_ness_ + 1)) * darkness_));
				*pKernelIter = nFactor << 24 | PreMultiply(color_, nFactor);
			}
			else
				*pKernelIter = 0;
			//TRACE("%d ", *pKernelIter >> 24);
			pKernelIter ++;
		}
		//TRACE("\n");
	}
	// Generate blurred border
	for(int i = nKernelSize; i < szShadow.cy - nKernelSize; i++)
	{
		int j;
		if(ptAnchors[i][0] < ptAnchors[i][1])
		{

			// Start of line
			for(j = ptAnchors[i][0];
				j < min(max(ptAnchors[i - 1][0], ptAnchors[i + 1][0]) + 1, ptAnchors[i][1]);
				j++)
			{
				for(int k = 0; k <= 2 * nKernelSize; k++)
				{
					UINT32 *pPixel = pShadBits +
						(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
					UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
					for(int l = 0; l <= 2 * nKernelSize; l++)
					{
						if(*pPixel < *pKernelPixel)
							*pPixel = *pKernelPixel;
						pPixel++;
						pKernelPixel++;
					}
				}
			}	// for() start of line

			// End of line
			for(j = max(j, min(ptAnchors[i - 1][1], ptAnchors[i + 1][1]) - 1);
				j < ptAnchors[i][1];
				j++)
			{
				for(int k = 0; k <= 2 * nKernelSize; k++)
				{
					UINT32 *pPixel = pShadBits +
						(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
					UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
					for(int l = 0; l <= 2 * nKernelSize; l++)
					{
						if(*pPixel < *pKernelPixel)
							*pPixel = *pKernelPixel;
						pPixel++;
						pKernelPixel++;
					}
				}
			}	// for() end of line

		}
	}	// for() Generate blurred border

	// Erase unwanted parts and complement missing
	UINT32 clCenter = darkness_ << 24 | PreMultiply(color_, darkness_);
	for(int i = min(nKernelSize, max(size_ - y_offset_, 0));
		i < max(szShadow.cy - nKernelSize, min(szParent.cy + size_ - y_offset_, szParent.cy + 2 * size_));
		i++)
	{
		UINT32 *pLine = pShadBits + (szShadow.cy - i - 1) * szShadow.cx;
		if(i - size_ + y_offset_ < 0 || i - size_ + y_offset_ >= szParent.cy)	// Line is not covered by parent window
		{
			for(int j = ptAnchors[i][0]; j < ptAnchors[i][1]; j++)
			{
				*(pLine + j) = clCenter;
			}
		}
		else
		{
			for(int j = ptAnchors[i][0];
				j < min(ptAnchorsOri[i - size_ + y_offset_][0] + size_ - x_offset_, ptAnchors[i][1]);
				j++)
				*(pLine + j) = clCenter;
			for(int j = max(ptAnchorsOri[i - size_ + y_offset_][0] + size_ - x_offset_, 0);
				j < min(ptAnchorsOri[i - size_ + y_offset_][1] + size_ - x_offset_, szShadow.cx);
				j++)
				*(pLine + j) = 0;
			for(int j = max(ptAnchorsOri[i - size_ + y_offset_][1] + size_ - x_offset_, ptAnchors[i][0]);
				j < ptAnchors[i][1];
				j++)
				*(pLine + j) = clCenter;
		}
	}

	// Delete used resources
	delete[] (ptAnchors - (size_ < 0 ? -size_ : 0) - 1);
	delete[] ptAnchorsTmp;
	delete[] ptAnchorsOri;
	delete[] pKernel;
	DeleteObject(hParentRgn);
}

void ShadowUI::ShowShadow(bool bShow)
{
	is_show_shadow_ = bShow;
}

bool ShadowUI::IsShowShadow() const
{
	return is_show_shadow_;
}


void ShadowUI::DisableShadow(bool bDisable) {


	is_disable_show_ = bDisable;
	if (wnd_ != NULL) {

		if (is_disable_show_) {
			::ShowWindow(wnd_, SW_HIDE);
		}
		else {
			// Determine the initial show state of shadow according to parent window's state
			LONG lParentStyle = GetWindowLongPtr(GetParent(wnd_), GWL_STYLE);


			if (!(WS_VISIBLE & lParentStyle))	// Parent invisible
				status_ = SS_ENABLED;
			else if ((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle)	// Parent visible but does not need shadow
				status_ = SS_ENABLED | SS_PARENTVISIBLE;
			else	// Show the shadow
			{
				status_ = SS_ENABLED | SS_VISABLE | SS_PARENTVISIBLE;

			}


			if ((WS_VISIBLE & lParentStyle) && !((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle))// Parent visible && no maxsize or min size
			{
				::ShowWindow(wnd_, SW_SHOWNOACTIVATE);
				Update(GetParent(wnd_));
			}



		}


	}

}
////TODO shadow disnable fix////
bool ShadowUI::IsDisableShadow() const {

	return is_disable_show_;
}

bool ShadowUI::SetSize(int NewSize)
{
	if(NewSize > 35 || NewSize < -35)
		return false;

	size_ = (signed char)NewSize;
	if(wnd_ != NULL && (SS_VISABLE & status_))
		Update(GetParent(wnd_));
	return true;
}

bool ShadowUI::SetSharpness(unsigned int NewSharpness)
{
	if(NewSharpness > 35)
		return false;

	sharp_ness_ = (unsigned char)NewSharpness;
	if(wnd_ != NULL && (SS_VISABLE & status_))
		Update(GetParent(wnd_));
	return true;
}

bool ShadowUI::SetDarkness(unsigned int NewDarkness)
{
	if(NewDarkness > 255)
		return false;

	darkness_ = (unsigned char)NewDarkness;
	if(wnd_ != NULL && (SS_VISABLE & status_))
		Update(GetParent(wnd_));
	return true;
}

bool ShadowUI::SetPosition(int NewXOffset, int NewYOffset)
{
	if(NewXOffset > 35 || NewXOffset < -35 ||
		NewYOffset > 35 || NewYOffset < -35)
		return false;
	
	x_offset_ = (signed char)NewXOffset;
	y_offset_ = (signed char)NewYOffset;
	if(wnd_ != NULL && (SS_VISABLE & status_))
		Update(GetParent(wnd_));
	return true;
}

bool ShadowUI::SetColor(COLORREF NewColor)
{
	color_ = NewColor;
	if(wnd_ != NULL && (SS_VISABLE & status_))
		Update(GetParent(wnd_));
	return true;
}

bool ShadowUI::SetImage(std::wstring szImage)
{
	if (szImage.empty())
		return false;

	is_image_mode_ = true;
	shadow_image_ = szImage;
	if(wnd_ != NULL && (SS_VISABLE & status_))
		Update(GetParent(wnd_));

	return true;
}

bool ShadowUI::SetShadowCorner(RECT rcCorner)
{
	if (rcCorner.left < 0 || rcCorner.top < 0 || rcCorner.right < 0 || rcCorner.bottom < 0) return false;

	show_corner_ = rcCorner;
	if(wnd_ != NULL && (SS_VISABLE & status_)) {
		Update(GetParent(wnd_));
	}

	return true;
}

bool ShadowUI::CopyShadow(ShadowUI* pShadow)
{
	if (is_image_mode_) {
		pShadow->SetImage(shadow_image_);
		pShadow->SetShadowCorner(show_corner_);
		pShadow->SetSize((int)size_);
	}
	else {
		pShadow->SetSize((int)size_);
		pShadow->SetSharpness((unsigned int)sharp_ness_);
		pShadow->SetDarkness((unsigned int)darkness_);
		pShadow->SetColor(color_);
		pShadow->SetPosition((int)x_offset_, (int)y_offset_);
	}

	pShadow->ShowShadow(is_show_shadow_);
	return true;
}
} //namespace DuiLib