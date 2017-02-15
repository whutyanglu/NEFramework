#ifdef UILIB_STATIC
#define UILIB_API 
#else
#if defined(UILIB_EXPORTS)
#	if defined(_MSC_VER)
#		define UILIB_API __declspec(dllexport)
#	else
#		define UILIB_API 
#	endif
#else
#	if defined(_MSC_VER)
#		define UILIB_API __declspec(dllimport)
#	else
#		define UILIB_API 
#	endif
#endif
#endif
#define UILIB_COMDAT __declspec(selectany)

#pragma warning(disable:4251)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <comdef.h>
#include <gdiplus.h>

#include <map>
#include <unordered_map>
#include <string>

#include "Utils/Utils.h"
#include "Utils/unzip.h"
#include "Utils/VersionHelpers.h"
#include "Core/Markup.h"
#include "Utils/observer_impl_base.h"
#include "Utils/Shadow.h"
#include "Utils/Delegate.h"
#include "Utils/DragDropImpl.h"
#include "Utils/TrayIcon.h"
#include "Utils/DPI.h"

#include "Core/Define.h"
#include "Core/ResourceManager.h"
#include "Core/PaintManager.h"
#include "Core/Base.h"
#include "Core/ControlFactory.h"
#include "Core/Control.h"
#include "Core/Box.h"
#include "Core/DlgBuilder.h"
#include "Core/Render.h"
#include "Core/GlobalManager.h"

#include "Utils/WinImplBase.h"

#include "Box/VBox.h"
#include "Box/HBox.h"
#include "Box/TileBox.h"
#include "Box/TabBox.h"
#include "Box/ChildBox.h"

#include "Control/List.h"
#include "Control/Combo.h"
#include "Control/ScrollBar.h"
#include "Control/TreeView.h"

#include "Control/Label.h"
#include "Control/Text.h"
#include "Control/Edit.h"
#include "Control/GifAnim.h"
#include "Control/GifAnimEx.h"

#include "Control/Animation.h"
#include "Box/AnimationTabBox.h"
#include "Control/Button.h"
#include "Control/Option.h"

#include "Control/Progress.h"
#include "Control/Slider.h"

#include "Control/ComboBox.h"
#include "Control/RichEdit.h"
#include "Control/DateTime.h"
#include "Control/IPAddress.h"
#include "Control/IPAddressEx.h"

#include "Control/ActiveX.h"
#include "Control/Flash.h"

#include "Control/Menu.h"
#include "Control/GroupBox.h"
#include "Control/RollText.h"
#include "Control/ColorPalette.h"
#include "Control/ListEx.h"
#include "Control/HotKey.h"
#include "Control/FadeButton.h"
#include "Control/Ring.h"


#pragma comment( lib, "comctl32.lib" )
#pragma comment( lib, "GdiPlus.lib" )
#pragma comment( lib, "Imm32.lib" )


using wsws_umap = std::unordered_map<std::wstring, std::wstring>;