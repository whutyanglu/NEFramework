#ifndef NEFRAMEWORK_DUILIB_GLOBAL_MANAGER_H__
#define NEFRAMEWORK_DUILIB_GLOBAL_MANAGER_H__

#include "duilib/Utils/WinImplBase.h"
#include "duilib/Core/Define.h"

namespace ui
{
class UILIB_API GlobalManager
{
public:
	static bool Start(std::wstring theme_dir);
	static void Shutdown();

	/// Defaults
	static DWORD GetDefaultDisabledColor();
	static void SetDefaultDisabledColor(DWORD dwColor);
	static DWORD GetDefaultFontColor();
	static void SetDefaultFontColor(DWORD dwColor);
	static DWORD GetDefaultLinkFontColor();
	static void SetDefaultLinkFontColor(DWORD dwColor);
	static DWORD GetDefaultLinkHoverFontColor();
	static void SetDefaultLinkHoverFontColor(DWORD dwColor);
	static DWORD GetDefaultSelectedBkColor();
	static void SetDefaultSelectedBkColor(DWORD dwColor);
	static TFontInfo* GetDefaultFontInfo(HDC dc);
	
	/// Colors
	static void  AddColor(std::wstring name, std::wstring value);
	static DWORD ConvertColor(std::wstring color);
	static DWORD GetColor(std::wstring name);
	static void  RemoveAllColors();
	static void  RemoveAllForms();

	/// Fonts
	static HFONT AddFont(TFontInfo & tf);
	static HFONT AddFont(std::wstring name, int size, bool bold, bool underline, bool italic);
	static HFONT GetFont(int i);
	static HFONT GetFont(std::wstring name, int size, bool bold, bool underline, bool italic);
	static TFontInfo* GetFontInfo(size_t i, HDC dc);
	static TFontInfo* GetFontInfo(HFONT h, HDC dc);
	static int  GetFontIndex(HFONT h);
	static int  GetFontIndex(std::wstring name, int size, bool bold, bool underline, bool italic);
	static void RemoveFont(int i);
	static void RemoveFont(HFONT h);
	static void RemoveAllFonts();

	/// Classes
	static void RemoveAllClasses();
	static void AddClass(std::wstring name, std::wstring value);
	static std::wstring GetClass(std::wstring name);

	/// Msics
	static long GetTextPixelWidth(HDC dc, HFONT f, std::wstring txt);
	static long GetTextPixelHeight(HDC dc, HFONT f);

	static long GetTextPixelWidth(HDC dc, int font_id, std::wstring txt);
	static long GetTextPixelHeight(HDC dc, int font_id);

	/// Windows
	template<typename T, typename... Args>
	static std::shared_ptr<WindowImplBase> SingletonShow(Args && ... args)
	{
		std::wstring window_id = T::GetWindowId();
		std::shared_ptr<WindowImplBase> ptr = GetFormFromWindowId(window_id);
		if (!ptr) {
			ptr = std::shared_ptr<T>(new T(args...));
			ptr->Create(NULL, ptr->GetWindowClassName(), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME, RECT{ 0,0,0,0 });
			forms_.insert({ window_id, ptr });
		}
	
		ptr->ShowWindow();
		ptr->CenterWindow();

		return ptr;
	}

	template<typename T, typename... Args>
	static std::shared_ptr<WindowImplBase> CreateForm(Args && ... args)
	{
		std::wstring window_id = T::GetWindowId();
		std::shared_ptr<WindowImplBase> ptr = std::shared_ptr<T>(new T(args...));
		ptr->Create(NULL, ptr->GetWindowClassName(), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME, RECT{ 0,0,0,0 });
		ptr->ShowWindow();
		ptr->CenterWindow();

		return ptr;
	}

	static void RemoveForm(std::wstring wnd_id);

	static std::shared_ptr<WindowImplBase> GetFormFromWindowId(std::wstring wnd_id);

	/// 
	static Control* CreateFromXml(const std::wstring &xml, PaintManager *pm, Control *parent = NULL, ControlBuilderCallback cb = nullptr);

	
private:
	static void ParseFont(MarkupNode node);
	static void ParseColor(MarkupNode node);
	static void ParseClass(MarkupNode node);


private:
	static DWORD default_disabled_color_;
	static DWORD default_font_color_;
	static DWORD default_link_font_color_;
	static DWORD default_link_hover_font_color_;
	static DWORD default_selected_bk_color_;
	static TFontInfo default_font_info_;

private:
	static std::vector<TFontInfo> fonts_;
	static std::unordered_map<std::wstring, std::wstring> classes_;
	static std::unordered_map<std::wstring, DWORD> colors_;
	static std::unordered_map<std::wstring, std::shared_ptr<WindowImplBase>> forms_;
};

} // namespace DuiLib


#endif // ~NEFRAMEWORK_DUILIB_GLOBAL_MANAGER_H__
