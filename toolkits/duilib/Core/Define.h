#pragma once

namespace ui
{
#define MAX_FONT_ID		30000
#define CARET_TIMERID	0x1999

// 鼠标光标定义
#define DUI_ARROW           32512
#define DUI_IBEAM           32513
#define DUI_WAIT            32514
#define DUI_CROSS           32515
#define DUI_UPARROW         32516
#define DUI_SIZE            32640
#define DUI_ICON            32641
#define DUI_SIZENWSE        32642
#define DUI_SIZENESW        32643
#define DUI_SIZEWE          32644
#define DUI_SIZENS          32645
#define DUI_SIZEALL         32646
#define DUI_NO              32648
#define DUI_HAND            32649

// 核心控件
class Control;

// 列表类型
enum ListType
{
	kListList = 0,
	kListCombo,
	kListTree,
};

// CursorType
enum CursorType
{
	kCursorArrow,
	kCursorHand,
	kCursorHandIbeam
};

enum EventType
{
	kEventNone = -1,

	kEventAll,
	kEventFirst,
	kEventWindowInit,

	kEventKeyBegin,
	kEventKeyDown,
	kEventKeyUp,
	kEventChar,
	kEventSyskey,
	kEventKeyEnd,

	kEventMouseBegin,
	kEventMouseMove,
	kEventMouseLeave,
	kEventMouseEnter,
	kEventMouseHover,
	kEventMouseButtonDown,
	kEventMouseButtonUp,
	kEventMouseRightButtonDown,
	kEventMouseRightButtonUp,
	kEventMouseMiddleButtonDown,
	kEventMouseMiddleButtonUp,
	kEventMouseDoubleClick,
	kEventMouseMenu,
	kMouseScrollWheel,
	kEventMouseEnd,

	kEventKillFocus,
	kEventSetFocus,
	kEventWindowSize,
	kEventSetCursor,
	kEventTimer,

	kEventClick,
	kEventSelect,
	kEventUnSelect,
	kEventTextChange,
	kEventCustomLinkClick,
	kEventSelecteChanged,

	kEventScrollChange,

	kEventValueChanged,
	kEventValueChangedMove,
	kEventReturn,
	kEventTab,
	kEventWindowClose,
	kEventDropDown,
	kEventItemExpand,
	kEventItemSelected,
	kEventItemActive,
	kEventItemClick,
	kEventItemLinkClick,
	kEventItemChecked,
	kEventHeaderClick,
	kEventHeaderItemClick,
	
	kEventResize,
	kEventShowActiveX,
	kEventColorChanged,
	kEventTextRollEnd,
	kEventCheckClick,

	kEventTreeItemDoubleClick,

	kEventNotify,	//仅作简单的通知，有复杂数据请通过其他方式实现

	kEventLast
};

class EventArgs
{
public:
	EventType type_		= kEventNone;
	Control* sender_	= nullptr;
	DWORD timestamp_	= 0;
	wchar_t key_		= L'\0';
	WORD key_state_		= 0;
	WPARAM w_param_		= 0;
	LPARAM l_param_		= 0;
	POINT mouse_point_{ 0,0 };
};

/////////////////////////////////////////////////////////////////////////////////////
//

struct TFontInfo
{
	HFONT handle_ = NULL;
	std::wstring name_;
	int size_ = -1;
	bool bold_ = false;
	bool underline_ = false;
	bool italic_ = false;
	TEXTMETRIC tm_;
} ;

struct TImageInfo
{
	HBITMAP hBitmap;
	LPBYTE pBits;
	LPBYTE pSrcBits;
	int nX;
	int nY;
	bool bAlpha;
	bool bUseHSL;
	std::wstring sResType;
	DWORD dwMask;
} ;

struct TDrawInfo
{
	TDrawInfo();
	void Parse(std::wstring pStrImage, std::wstring pStrModify, PaintManager *paintManager);
	void Parse(std::wstring pStrImage, std::wstring pStrModify = L"");
	void Clear();
	bool IsValidDest();

	std::wstring sDrawString;
	std::wstring sDrawModify;
	std::wstring sImageName;
	std::wstring sResType;
	RECT rcDest;
	RECT rcSource;
	RECT rcCorner;
	DWORD dwMask;
	BYTE uFade;
	bool bHole;
	bool bTiledX;
	bool bTiledY;
	bool bHSL;
};

struct UILIB_API TPercentInfo
{
	double left;
	double top;
	double right;
	double bottom;
} ;

struct UILIB_API TResInfo
{
	DWORD m_dwDefaultDisabledColor;
	DWORD m_dwDefaultFontColor;
	DWORD m_dwDefaultLinkFontColor;
	DWORD m_dwDefaultLinkHoverFontColor;
	DWORD m_dwDefaultSelectedBkColor;
	TFontInfo m_DefaultFontInfo;
	CStdStringPtrMap m_CustomFonts;
	CStdStringPtrMap m_ImageHash;
	CStdStringPtrMap m_AttrHash;
	CStdStringPtrMap m_StyleHash;
	CStdStringPtrMap m_DrawInfoHash;
};


// Listener interface
class INotify
{
public:
	virtual void Notify(EventArgs& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class ITranslateAccelerator
{
public:
	virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};

//定义所有消息类型
//////////////////////////////////////////////////////////////////////////

#define DUI_MSGTYPE_MENU                   (_T("menu"))
#define DUI_MSGTYPE_LINK                   (_T("link"))

#define DUI_MSGTYPE_TIMER                  (_T("timer"))
#define DUI_MSGTYPE_CLICK                  (_T("click"))
#define DUI_MSGTYPE_DBCLICK                (_T("dbclick"))

#define DUI_MSGTYPE_RETURN                 (_T("return"))
#define DUI_MSGTYPE_SCROLL                 (_T("scroll"))

#define DUI_MSGTYPE_DROPDOWN               (_T("dropdown"))
#define DUI_MSGTYPE_SETFOCUS               (_T("setfocus"))

#define DUI_MSGTYPE_KILLFOCUS              (_T("killfocus"))
#define DUI_MSGTYPE_ITEMCLICK 		   	   (_T("itemclick"))
#define DUI_MSGTYPE_ITEMRCLICK 			   (_T("itemrclick"))
#define DUI_MSGTYPE_TABSELECT              (_T("tabselect"))

#define DUI_MSGTYPE_ITEMSELECT 		   	   (_T("itemselect"))
#define DUI_MSGTYPE_ITEMEXPAND             (_T("itemexpand"))
#define DUI_MSGTYPE_WINDOWINIT             (_T("windowinit"))
#define DUI_MSGTYPE_WINDOWSIZE             (_T("windowsize"))
#define DUI_MSGTYPE_BUTTONDOWN 		   	   (_T("buttondown"))
#define DUI_MSGTYPE_MOUSEENTER			   (_T("mouseenter"))
#define DUI_MSGTYPE_MOUSELEAVE			   (_T("mouseleave"))

#define DUI_MSGTYPE_TEXTCHANGED            (_T("textchanged"))
#define DUI_MSGTYPE_HEADERCLICK            (_T("headerclick"))
#define DUI_MSGTYPE_ITEMDBCLICK            (_T("itemdbclick"))
#define DUI_MSGTYPE_SHOWACTIVEX            (_T("showactivex"))

#define DUI_MSGTYPE_ITEMCOLLAPSE           (_T("itemcollapse"))
#define DUI_MSGTYPE_ITEMACTIVATE           (_T("itemactivate"))
#define DUI_MSGTYPE_VALUECHANGED           (_T("valuechanged"))
#define DUI_MSGTYPE_VALUECHANGED_MOVE      (_T("movevaluechanged"))

#define DUI_MSGTYPE_SELECTCHANGED 		   (_T("selectchanged"))
#define DUI_MSGTYPE_UNSELECTED	 		   (_T("unselected"))

#define DUI_MSGTYPE_TREEITEMDBCLICK 		(_T("treeitemdbclick"))
#define DUI_MSGTYPE_CHECKCLICK				(_T("checkclick"))
#define DUI_MSGTYPE_TEXTROLLEND 			(_T("textrollend"))
#define DUI_MSGTYPE_COLORCHANGED		    (_T("colorchanged"))

#define DUI_MSGTYPE_LISTITEMSELECT 		   	(_T("listitemselect"))
#define DUI_MSGTYPE_LISTITEMCHECKED 		(_T("listitemchecked"))
#define DUI_MSGTYPE_COMBOITEMSELECT 		(_T("comboitemselect"))
#define DUI_MSGTYPE_LISTHEADERCLICK			(_T("listheaderclick"))
#define DUI_MSGTYPE_LISTHEADITEMCHECKED		(_T("listheaditemchecked"))
#define DUI_MSGTYPE_LISTPAGECHANGED			(_T("listpagechanged"))


/////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
///

#define  DUI_CTR_EDIT                            (_T("Edit"))
#define  DUI_CTR_LIST                            (_T("List"))
#define  DUI_CTR_TEXT                            (_T("Text"))

#define  DUI_CTR_COMBO                           (_T("Combo"))
#define  DUI_CTR_LABEL                           (_T("Label"))
#define  DUI_CTR_FLASH							 (_T("Flash"))

#define  DUI_CTR_BUTTON                          (_T("Button"))
#define  DUI_CTR_OPTION                          (_T("Option"))
#define  DUI_CTR_SLIDER                          (_T("Slider"))

#define  DUI_CTR_CONTROL                         (_T("Control"))
#define  DUI_CTR_ACTIVEX                         (_T("ActiveX"))
#define  DUI_CTR_GIFANIM                         (_T("GifAnim"))

#define  DUI_CTR_LISTITEM                        (_T("ListItem"))
#define  DUI_CTR_PROGRESS                        (_T("Progress"))
#define  DUI_CTR_RICHEDIT                        (_T("RichEdit"))
#define  DUI_CTR_CHECKBOX                        (_T("CheckBox"))
#define  DUI_CTR_COMBOBOX                        (_T("ComboBox"))
#define  DUI_CTR_DATETIME                        (_T("DateTime"))
#define  DUI_CTR_TREEVIEW                        (_T("TreeView"))
#define  DUI_CTR_TREENODE                        (_T("TreeNode"))

#define  DUI_CTR_BOX							 (_T("Box"))
#define  DUI_CTR_TABBOX							 (_T("TabBox"))
#define  DUI_CTR_SCROLLBAR                       (_T("ScrollBar"))
#define  DUI_CTR_IPADDRESS                       (_T("IPAddress"))

#define  DUI_CTR_LISTHEADER                      (_T("ListHeader"))
#define  DUI_CTR_LISTFOOTER                      (_T("ListFooter"))
#define  DUI_CTR_TILEBox                         (_T("TileBox"))
#define  DUI_CTR_WEBBROWSER                      (_T("WebBrowser"))

#define  DUI_CTR_CHILDBOX                        (_T("ChildBox"))
#define  DUI_CTR_LISTELEMENT                     (_T("ListElement"))

#define  DUI_CTR_VBOX							 (_T("VBox"))
#define  DUI_CTR_LISTHEADERITEM                  (_T("ListHeaderItem"))

#define  DUI_CTR_LISTTEXTELEMENT                 (_T("ListTextElement"))

#define  DUI_CTR_HBOX							 (_T("HBox"))
#define  DUI_CTR_LISTLABELELEMENT                (_T("ListLabelElement"))

#define  DUI_CTR_ANIMATIONTABLAYOUT				 (_T("AnimationTabBox"))

#define  DUI_CTR_LISTCONTAINERELEMENT            (_T("ListContainerElement"))

#define  DUI_CTR_TEXTSCROLL						 (_T("TextScroll"))

#define DUI_CTR_COLORPALETTE					 (_T("ColorPalette"))
	///
	//////////////END控件名称宏定义//////////////////////////////////////////////////

}// namespace uilib

