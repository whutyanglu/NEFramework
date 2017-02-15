#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once

namespace ui {

typedef Control* (CALLBACK* FINDCONTROLPROC)(Control*, LPVOID);

class UILIB_API Control : public virtual nbase::SupportWeakCallback
{
	DECLARE_DUICONTROL(Control)
public:
	Control();
	virtual ~Control();

public:
	virtual std::wstring GetName() const;
	virtual void SetName(std::wstring pstrName);
	virtual std::wstring GetClass() const;
	virtual LPVOID GetInterface(std::wstring pstrName);
	virtual UINT GetControlFlags() const;

	virtual bool Activate();
	virtual PaintManager* GetManager() const;
	virtual void SetManager(PaintManager* pManager, Control* pParent, bool bInit = true);
	virtual Control* GetParent() const;
	void setInstance(HINSTANCE instance = NULL) { instance_ = instance; };

	// 定时器
	bool SetTimer(UINT nTimerID, UINT nElapse);
	void KillTimer(UINT nTimerID);

	// 文本相关
	virtual std::wstring GetText() const;
	virtual std::string  GetUTF8Text() const;
	virtual void SetText(std::wstring pstrText);
	virtual void SetUTF8Text(std::string pstrText);

	virtual bool IsResourceText() const;
	virtual void SetResourceText(bool bResource);

	virtual bool IsDragEnabled() const;
	virtual void SetDragEnable(bool bDrag);

	virtual bool IsDropEnabled() const;
	virtual void SetDropEnable(bool bDrop);

	// 图形相关
	DWORD GetBkColor() const;
	void SetBkColor(DWORD dwBackColor);
	DWORD GetBkColor2() const;
	void SetBkColor2(DWORD dwBackColor);
	DWORD GetBkColor3() const;
	void SetBkColor3(DWORD dwBackColor);
	DWORD GetForeColor() const;
	void SetForeColor(DWORD dwForeColor);
	std::wstring GetBkImage();
	void SetBkImage(std::wstring pStrImage);
	std::wstring GetForeImage() const;
	void SetForeImage(std::wstring pStrImage);

	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
	bool IsColorHSL() const;
	void SetColorHSL(bool bColorHSL);
	SIZE GetBorderRound() const;
	void SetBorderRound(SIZE cxyRound);
	bool DrawImage(HDC hDC, std::wstring pStrImage, std::wstring pStrModify = L"");

	
	//边框相关
	int GetBorderSize() const;
	void SetBorderSize(int nSize);
	DWORD GetBorderColor() const;
	void SetBorderColor(DWORD dwBorderColor);
	void SetBorderSize(RECT rc);
	int GetLeftBorderSize() const;
	void SetLeftBorderSize(int nSize);
	int GetTopBorderSize() const;
	void SetTopBorderSize(int nSize);
	int GetRightBorderSize() const;
	void SetRightBorderSize(int nSize);
	int GetBottomBorderSize() const;
	void SetBottomBorderSize(int nSize);
	int GetBorderStyle() const;
	void SetBorderStyle(int nStyle);

	// 位置相关
	virtual RECT GetRelativePos() const; // 相对(父控件)位置
	virtual RECT GetClientPos() const; // 客户区域（除去scrollbar和inset）
	virtual const RECT& GetPos() const;
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
	virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
	virtual int GetWidth() const;
	virtual int GetHeight() const;
	virtual int GetX() const;
	virtual int GetY() const;
	virtual RECT GetPadding() const;
	virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
	virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
	virtual int GetFixedWidth();       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void SetFixedWidth(int cx);      // 预设的参考值
	virtual int GetFixedHeight();      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void SetFixedHeight(int cy);     // 预设的参考值
	virtual int GetMinWidth() const;
	virtual void SetMinWidth(int cx);
	virtual int GetMaxWidth() const;
	virtual void SetMaxWidth(int cx);
	virtual int GetMinHeight() const;
	virtual void SetMinHeight(int cy);
	virtual int GetMaxHeight() const;
	virtual void SetMaxHeight(int cy);
	virtual TPercentInfo GetFloatPercent() const;
	virtual void SetFloatPercent(TPercentInfo piFloatPercent);
	virtual void SetFloatAlign(UINT uAlign);
	virtual UINT GetFloatAlign() const;
	// 鼠标提示
	virtual std::wstring GetToolTip() const;
	virtual void SetToolTip(std::wstring pstrText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	 GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

	// 光标
	virtual WORD GetCursor();
	virtual void SetCursor(WORD wCursor);

	// 快捷键
	virtual TCHAR GetShortcut() const;
	virtual void SetShortcut(TCHAR ch);

	// 菜单
	virtual bool IsContextMenuUsed() const;
	virtual void SetContextMenuUsed(bool bMenuUsed);

	// 用户属性
	virtual std::wstring GetUserData(); // 辅助函数，供用户使用
	virtual void SetUserData(std::wstring pstrText); // 辅助函数，供用户使用
	virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
	virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用

	// 一些重要的属性
	virtual bool IsVisible() const;
	virtual void SetVisible(bool bVisible = true);
	virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
	virtual bool IsEnabled() const;
	virtual void SetEnabled(bool bEnable = true);
	virtual bool IsMouseEnabled() const;
	virtual void SetMouseEnabled(bool bEnable = true);
	virtual bool IsKeyboardEnabled() const;
	virtual void SetKeyboardEnabled(bool bEnable = true);
	virtual bool IsFocused() const;
	virtual void SetFocus();
	virtual bool IsFloat() const;
	virtual void SetFloat(bool bFloat = true);

	virtual Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	void Invalidate();
	bool IsUpdateNeeded() const;
	void NeedUpdate();
	void NeedParentUpdate();
	DWORD GetAdjustColor(DWORD dwColor);

	virtual void Init();
	virtual void DoInit();

	virtual void Event(EventArgs& evt);
	virtual void DoEvent(EventArgs& evt);
	void Notify(EventType t, WPARAM wParam = 0, LPARAM lParam = 0, wchar_t key = L'\0',
		WORD keyState = 0, POINT pt = {0,0});
	void Notify(const EventArgs & arg);

	// 自定义(未处理的)属性
	void AddCustomAttribute(std::wstring pstrName, std::wstring pstrAttr);
	std::wstring GetCustomAttribute(std::wstring pstrName) const;
	bool RemoveCustomAttribute(std::wstring pstrName);
	void RemoveAllCustomAttribute();

	virtual void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
	Control* ApplyAttributeList(std::wstring pstrList);

	virtual SIZE EstimateSize(SIZE szAvailable);

	virtual void DoPaint(HDC hDC, const RECT& rcPaint);
	virtual void PaintBkColor(HDC hDC);
	virtual void PaintBkImage(HDC hDC);
	virtual void PaintStatusImage(HDC hDC);
	virtual void PaintForeColor(HDC hDC);
	virtual void PaintForeImage(HDC hDC);
	virtual void PaintText(HDC hDC);
	virtual void PaintBorder(HDC hDC);
	virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

	void AttachAllEvents(const EventCallback& callback)
	{
		on_event_[kEventAll] += callback;
	}

	void AttachMouseEnter(const EventCallback& callback)
	{
		on_event_[kEventMouseEnter] += callback;
	}

	void AttachMouseLeave(const EventCallback& callback)
	{
		on_event_[kEventMouseLeave] += callback;
	}

	void AttachMouseHover(const EventCallback& callback)
	{
		on_event_[kEventMouseHover] += callback;
	}

	void AttachButtonDown(const EventCallback& callback)
	{
		on_event_[kEventMouseButtonDown] += callback;
	}

	void AttachButtonUp(const EventCallback& callback)
	{
		on_event_[kEventMouseButtonUp] += callback;
	}

	void AttachSetFocus(const EventCallback& callback)
	{
		on_event_[kEventSetFocus] += callback;
	}

	void AttachKillFocus(const EventCallback& callback)
	{
		on_event_[kEventKillFocus] += callback;
	}

	void AttachMenu(const EventCallback& callback)
	{
		on_event_[kEventMouseMenu] += callback;
	}

	void AttachResize(const EventCallback& callback)
	{
		on_event_[kEventResize] += callback;
	}

	void DetachEvent(EventType type);

protected:
	friend class DlgBuilder;

	void AttachXmlEvent(EventType eventType, const EventCallback& callback)
	{
		on_xml_event_[eventType] += callback;
	}

public:
	EventMap    on_event_;
	EventMap	on_xml_event_;

protected:
	PaintManager* pm_;
	Control* parent_;
	std::wstring virtual_wnd_;
	std::wstring name_;
	bool need_update_;
	bool menu_used_;
	RECT rect_;
	RECT padding_rect_;
	SIZE cxy_sz_;
	SIZE cxy_fired_sz_;
	SIZE cxy_min_sz_;
	SIZE cxy_max_sz_;
	bool visible_;
	bool internel_visible_;
	bool enabled_;
	bool mouse_enabled_;
	bool keybord_enabled_ ;
	bool focused_;
	bool float_;
	TPercentInfo float_percent_;
	UINT float_align_;
	bool has_setted_pos_; // 防止SetPos循环调用

	bool drag_enabled_;
	bool drop_enabled_;

	bool resource_text_;
	std::wstring text_;
	std::wstring tooltip_;
	TCHAR short_cut_;
	std::wstring user_data_;
	UINT_PTR tag_;

	DWORD back_color_;
	DWORD back_color2_;
	DWORD back_color3_;
	DWORD fore_color_;
	std::wstring bkimage_;
	std::wstring fore_image_;
	DWORD border_color_;
	DWORD focused_border_color_;
	bool color_hsl_;
	int border_size_;
	int border_style_;
	int tooltip_width_;
	WORD cursor_;
	SIZE border_round_sz_;
	RECT paint_rect_;
	RECT border_size_rect_;
    HINSTANCE instance_;

	CStdStringPtrMap custom_attributes_map_;
	bool set_unclass_attributes_ = false;
};

} // namespace DuiLib

#endif // __UICONTROL_H__
