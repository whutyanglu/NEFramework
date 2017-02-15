#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#pragma once
#define WM_USER_SET_DPI WM_USER + 200
namespace ui {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class Control;
	class RichEdit;
	class IDropTarget;

	/////////////////////////////////////////////////////////////////////////////////////
	//
	enum UILIB_RESTYPE
	{
		UILIB_FILE=1,		// 来自磁盘文件
		UILIB_ZIP,			// 来自磁盘zip压缩包
		UILIB_RESOURCE,		// 来自资源
		UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
	};
	/////////////////////////////////////////////////////////////////////////////////////
	//


	enum MSGTYPE_UI
	{
		// 内部保留消息
		UIMSG_TRAYICON = WM_USER + 1,
		// 程序自定义消息
		UIMSG_USER = WM_USER + 100,
	};
	/////////////////////////////////////////////////////////////////////////////////////
	//

	// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

	// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_UPDATETEST    0x00000008
#define UIFIND_TOP_FIRST     0x00000010
#define UIFIND_ME_FIRST      0x80000000

	// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040


	// Drag&Drop control
	const TCHAR* const CF_MOVECONTROL = _T("CF_MOVECONTROL");

	typedef struct UILIB_API tagTCFMoveUI
	{
		Control* pControl;
	} TCFMoveUI;

	/////////////////////////////////////////////////////////////////////////////////////
	//
	typedef Control* (*LPCREATECONTROL)(std::wstring pstrType);

	class Window;
	class UILIB_API PaintManager : public DropTarget
	{
	public:
		PaintManager(Window *wnd = NULL);
		~PaintManager();

	public:
		void Init(HWND hWnd, std::wstring pstrName = L"");
		void Cleanup();
		bool IsUpdateNeeded() const;
		void NeedUpdate();
		void Invalidate();
		void Invalidate(RECT& rcItem);

		std::wstring GetName() const;
		HDC GetPaintDC() const;
		HWND GetPaintWindow() const;
		HWND GetTooltipWindow() const;

		POINT GetMousePos() const;
		SIZE GetClientSize() const;
		SIZE GetInitSize();
		void SetInitSize(int cx, int cy);
		RECT& GetSizeBox();
		void SetSizeBox(RECT& rcSizeBox);
		RECT& GetCaptionRect();
		void SetCaptionRect(RECT& rcCaption);
		SIZE GetRoundCorner() const;
		void SetRoundCorner(int cx, int cy);
		SIZE GetMinInfo() const;
		void SetMinInfo(int cx, int cy);
		SIZE GetMaxInfo() const;
		void SetMaxInfo(int cx, int cy);
		bool IsShowUpdateRect() const;
		void SetShowUpdateRect(bool show);

		BYTE GetOpacity() const;
		void SetOpacity(BYTE nOpacity);

		bool IsLayered();
		void SetLayered(bool bLayered);
		RECT& GetLayeredInset();
		void SetLayeredInset(RECT& rcLayeredInset);
		BYTE GetLayeredOpacity();
		void SetLayeredOpacity(BYTE nOpacity);

		ShadowUI* GetShadow();
		// 光标
		bool ShowCaret(bool bShow);
		bool SetCaretPos(RichEdit* obj, int x, int y);
		RichEdit* GetCurrentCaretObject();
		bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight);
		void DrawCaret(HDC hDC, const RECT& rcPaint);
		
		void SetUseGdiplusText(bool bUse);
		bool IsUseGdiplusText() const;
		void SetGdiplusTextRenderingHint(int trh);
		int GetGdiplusTextRenderingHint() const;

		static HINSTANCE GetInstance();
		static std::wstring GetInstancePath();
		static std::wstring GetCurrentPath();
		static HINSTANCE GetResourceDll();
		static std::wstring GetResourcePath();
		static std::wstring GetResourceZip();
		static std::wstring GetResourceZipPwd();
		static bool IsCachedResourceZip();
		static HANDLE GetResourceZipHandle();
		static void SetInstance(HINSTANCE hInst);
		static void SetCurrentPath(std::wstring pStrPath);
		static void SetResourceDll(HINSTANCE hInst);
		static void SetResourcePath(std::wstring pStrPath);
		static void SetResourceZip(LPVOID pVoid, unsigned int len, std::wstring password = L"");
		static void SetResourceZip2(std::wstring pstrZip, bool bCachedResourceZip = false, std::wstring password = L"");
		static void SetResourceType(int nType);
		static int GetResourceType();
		static bool GetHSL(short* H, short* S, short* L);
		static void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
		static void ReloadSkin();
		static PaintManager* GetPaintManager(std::wstring pstrName);
		static CStdPtrArray* GetPaintManagers();
		static bool LoadPlugin(std::wstring pstrModuleName);
		static CStdPtrArray* GetPlugins();

		bool IsForceUseSharedRes() const;
		void SetForceUseSharedRes(bool bForce);

		void DeletePtr(void* ptr);

		const TImageInfo* GetImage(std::wstring bitmap);
		const TImageInfo* GetImageEx(std::wstring bitmap, std::wstring type = L"", DWORD mask = 0, bool bUseHSL = false, HINSTANCE instance = NULL);
		const TImageInfo* AddImage(std::wstring bitmap, std::wstring type = L"", DWORD mask = 0, bool bUseHSL = false, bool bShared = false, HINSTANCE instance = NULL);
		const TImageInfo* AddImage(std::wstring bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared = false);
		void RemoveImage(std::wstring bitmap, bool bShared = false);
		void RemoveAllImages(bool bShared = false);
		static void ReloadSharedImages();
		void ReloadImages();

		const TDrawInfo* GetDrawInfo(std::wstring pStrImage, std::wstring pStrModify);
		void RemoveDrawInfo(std::wstring pStrImage, std::wstring pStrModify);
		void RemoveAllDrawInfos();

		void AddDefaultAttributeList(std::wstring pStrControlName, std::wstring pStrControlAttrList, bool bShared = false);
		std::wstring GetDefaultAttributeList(std::wstring pStrControlName) const;
		bool RemoveDefaultAttributeList(std::wstring pStrControlName, bool bShared = false);
		void RemoveAllDefaultAttributeList(bool bShared = false);

		void AddWindowCustomAttribute(std::wstring pstrName, std::wstring pstrAttr);
		std::wstring GetWindowCustomAttribute(std::wstring pstrName) const;
		bool RemoveWindowCustomAttribute(std::wstring pstrName);
		void RemoveAllWindowCustomAttribute();

		const TImageInfo* GetImageString(std::wstring pStrImage, std::wstring pStrModify = L"");

		// 初始化拖拽
		bool InitDragDrop();
		virtual bool OnDrop(FORMATETC* pFmtEtc, STGMEDIUM& medium,DWORD *pdwEffect);

		bool AttachDialog(Control* pControl);
		bool InitControls(Control* pControl, Control* pParent = NULL);
		void ReapObjects(Control* pControl);

		bool AddOptionGroup(std::wstring pStrGroupName, Control* pControl);
		CStdPtrArray* GetOptionGroup(std::wstring pStrGroupName);
		void RemoveOptionGroup(std::wstring pStrGroupName, Control* pControl);
		void RemoveAllOptionGroups();

		Control* GetFocus() const;
		void SetFocus(Control* pControl);
		void SetFocusNeeded(Control* pControl);

		bool SetNextTabControl(bool bForward = true);

		bool SetTimer(Control* pControl, UINT nTimerID, UINT uElapse);
		bool KillTimer(Control* pControl, UINT nTimerID);
		void KillTimer(Control* pControl);
		void RemoveAllTimers();

		void SetCapture();
		void ReleaseCapture();
		bool IsCaptured();

		bool IsPainting();
		void SetPainting(bool bIsPainting);

		bool AddPreMessageFilter(IMessageFilterUI* pFilter);
		bool RemovePreMessageFilter(IMessageFilterUI* pFilter);

		bool AddMessageFilter(IMessageFilterUI* pFilter);
		bool RemoveMessageFilter(IMessageFilterUI* pFilter);

		int GetPostPaintCount() const;
		bool IsPostPaint(Control* pControl);
		bool AddPostPaint(Control* pControl);
		bool RemovePostPaint(Control* pControl);
		bool SetPostPaintIndex(Control* pControl, int iIndex);

		int GetPaintChildWndCount() const;
		bool AddPaintChildWnd(HWND hChildWnd);
		bool RemovePaintChildWnd(HWND hChildWnd);

		void AddDelayedCleanup(Control* pControl);

		bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
		bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
		bool TranslateAccelerator(LPMSG pMsg);

		Control* GetRoot() const;
		Control* FindControl(POINT pt) const;
		Control* FindControl(std::wstring pstrName) const;
		Control* FindSubControlByPoint(Control* pParent, POINT pt) const;
		Control* FindSubControlByName(Control* pParent, std::wstring pstrName) const;
		Control* FindSubControlByClass(Control* pParent, std::wstring pstrClass, int iIndex = 0);
		CStdPtrArray* FindSubControlsByClass(Control* pParent, std::wstring pstrClass);

		static void MessageLoop();
		static bool TranslateMessage(const LPMSG pMsg);
		static void Term();

		CDPI* GetDPIObj();
		void ResetDPIAssets();
		void RebuildFont(TFontInfo* pFontInfo);
		void SetDPI(int iDPI);
		static void SetAllDPI(int iDPI);

		bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		void UsedVirtualWnd(bool bUsed);

		POINT GetLastPoint() { return last_mouse_pt_; }

		Window *GetWindow();

	private:
		CStdPtrArray* GetFoundControls();
		static Control* CALLBACK __FindControlFromNameHash(Control* pThis, LPVOID pData);
		static Control* CALLBACK __FindControlFromCount(Control* pThis, LPVOID pData);
		static Control* CALLBACK __FindControlFromPoint(Control* pThis, LPVOID pData);
		static Control* CALLBACK __FindControlFromTab(Control* pThis, LPVOID pData);
		static Control* CALLBACK __FindControlFromShortcut(Control* pThis, LPVOID pData);
		static Control* CALLBACK __FindControlFromName(Control* pThis, LPVOID pData);
		static Control* CALLBACK __FindControlFromClass(Control* pThis, LPVOID pData);
		static Control* CALLBACK __FindControlsFromClass(Control* pThis, LPVOID pData);
		static Control* CALLBACK __FindControlsFromUpdate(Control* pThis, LPVOID pData);

		static void AdjustSharedImagesHSL();
		void AdjustImagesHSL();

	private:
		std::wstring name_;
		//
		Window *host_wnd_ = nullptr;
		HWND paint_wnd_;	//所附加的窗体的句柄
		HDC paint_dc_;
		HDC dc_off_screen_;
		HDC dc_background_;
		HBITMAP bmp_off_screen_;
		BYTE* off_screen_bits_;
		HBITMAP bmp_background_;
		COLORREF* bits_background_;

		CDPI* DPI_;

		bool show_update_rect_;
		// 是否开启Gdiplus
		bool use_gdiplus_text_;
		int trh_;
		ULONG_PTR gdi_plus_token_;
		Gdiplus::GdiplusStartupInput *gduplus_startup_input_;

		// 提示信息
		HWND tooltip_wnd_;
		TOOLINFO tool_tip_;
		
		// RichEdit光标
		RECT caret_rect_;
		bool caret_active_;
		bool caret_showing_;
		RichEdit* current_caret_object_;

		// 窗口阴影
		ShadowUI shadow_;
		
		//
		Control* root_;
		Control* focus_;
		Control* event_hover_;
		Control* event_click_;
		Control* event_key_;
		//
		POINT last_mouse_pt_;
		SIZE min_wnd_sz_;
		SIZE max_wnd_sz_;
		SIZE init_wnd_sz_;
		RECT size_box_sz_;
		SIZE round_corner_sz_;
		RECT caption_rect_;
		UINT timer_id_;
		bool is_first_layout_;
		bool need_update_;
		bool need_focus_;
		bool off_screen_paint_;
		
		BYTE opacity_;
		bool layered_;
		RECT layered_inset_rect_;
		bool layered_changed_;
		RECT layered_update_rect_;
		//TDrawInfo m_diLayered;

		bool mouse_tracking_;
		bool mouse_captured_;
		bool is_painting_;
		bool used_virtual_wnd_;

		//
		CStdPtrArray naotifiers_;
		CStdPtrArray timers_;
		CStdPtrArray translate_accelerator_;
		CStdPtrArray pre_message_filters_;
		CStdPtrArray message_filters_;
		CStdPtrArray post_paint_controls_;
		CStdPtrArray child_wnds_;
		CStdPtrArray delayed_cleanup_;
		CStdPtrArray async_notify_;
		CStdPtrArray found_controls_;
		CStdStringPtrMap name_hash_;
		CStdStringPtrMap custom_wnd_attr_hash_;
		CStdStringPtrMap option_group_;
		
		bool use_forced_shared_res_;
		TResInfo res_info_;

		// 拖拽
		bool drag_mode_;
		HBITMAP drag_bitmap_;

		//
		static HINSTANCE instance_;
		static HINSTANCE resource_instance_;
		static std::wstring resource_path_;
		static std::wstring resource_zip_;
		static std::wstring resource_zip_wnd_;  //Garfield 20160325 带密码zip包解密
		static HANDLE resource_zip_handle_;
		static bool resource_zip_cached_;
		static int res_type_;
		static TResInfo shared_res_info_;
		static bool use_hsl_;
		static short H_;
		static short S_;
		static short L_;
		static CStdPtrArray pre_messages_;
		static CStdPtrArray plugins_;
	};

} // namespace DuiLib

#endif // __UIMANAGER_H__
