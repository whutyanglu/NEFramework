#include "StdAfx.h"

namespace ui {
	IMPLEMENT_DUICONTROL(Control)

		Control::Control()
		:pm_(NULL), 
		parent_(NULL), 
		need_update_(true),
		menu_used_(false),
		visible_(true), 
		internel_visible_(true),
		focused_(false),
		enabled_(true),
		mouse_enabled_(true),
		keybord_enabled_(true),
		float_(false),
		float_align_(0),
		has_setted_pos_(false),
		drag_enabled_(false),
		drop_enabled_(false),
		resource_text_(false),
		short_cut_('\0'),
		tag_(NULL),
		back_color_(0),
		back_color2_(0),
		back_color3_(0),
		fore_color_(0),
		border_color_(0),
		focused_border_color_(0),
		color_hsl_(false),
		border_size_(0),
		border_style_(PS_SOLID),
		tooltip_width_(300),
		cursor_(0),
		instance_(NULL)
	{
		cxy_sz_.cx = cxy_sz_.cy = 0;
		cxy_fired_sz_.cx = cxy_fired_sz_.cy = 0;
		cxy_min_sz_.cx = cxy_min_sz_.cy = 0;
		cxy_max_sz_.cx = cxy_max_sz_.cy = 9999;
		border_round_sz_.cx = border_round_sz_.cy = 0;

		::ZeroMemory(&padding_rect_, sizeof(RECT));
		::ZeroMemory(&rect_, sizeof(RECT));
		::ZeroMemory(&paint_rect_, sizeof(RECT));
		::ZeroMemory(&border_size_,sizeof(RECT));
		float_percent_.left = float_percent_.top = float_percent_.right = float_percent_.bottom = 0.0f;
	}

	Control::~Control()
	{
		RemoveAllCustomAttribute();	
		if( pm_ != NULL ) pm_->ReapObjects(this);
	}

	std::wstring Control::GetName() const
	{
		return name_;
	}

	void Control::SetName(std::wstring pstrName)
	{
		name_ = pstrName;
	}

	LPVOID Control::GetInterface(std::wstring pstrName)
	{
		if( pstrName == DUI_CTR_CONTROL ) return this;
		return NULL;
	}

	std::wstring Control::GetClass() const
	{
		return _T("Control");
	}

	UINT Control::GetControlFlags() const
	{
		return 0;
	}

	bool Control::Activate()
	{
		if( !IsVisible() ) return false;
		if( !IsEnabled() ) return false;
		return true;
	}

	PaintManager* Control::GetManager() const
	{
		return pm_;
	}

	void Control::SetManager(PaintManager* pManager, Control* pParent, bool bInit)
	{
		pm_ = pManager;
		parent_ = pParent;
		if( bInit && parent_ ) Init();
	}

	Control* Control::GetParent() const
	{
		return parent_;
	}

	bool Control::SetTimer(UINT nTimerID, UINT nElapse)
	{
		if(pm_ == NULL) return false;

		return pm_->SetTimer(this, nTimerID, nElapse);
	}

	void Control::KillTimer(UINT nTimerID)
	{
		if(pm_ == NULL) return;

		pm_->KillTimer(this, nTimerID);
	}

	std::wstring Control::GetText() const
	{
		if (!IsResourceText()) return text_;
		return ResourceManager::GetInstance()->GetText(text_);
	}

	std::string Control::GetUTF8Text() const
	{
		std::wstring txt = GetText();
		return nbase::UTF16ToUTF8(txt);
	}

	void Control::SetText(std::wstring pstrText)
	{
		if( text_ == pstrText ) return;

		text_ = pstrText;
		Invalidate();
	}

	void Control::SetUTF8Text(std::string pstrText)
	{
		std::wstring txt = nbase::UTF8ToUTF16(pstrText);
		SetText(txt);
	}

	bool Control::IsResourceText() const
	{
		return resource_text_;
	}

	void Control::SetResourceText(bool bResource)
	{
		if( resource_text_ == bResource ) return;
		resource_text_ = bResource;
		Invalidate();
	}

	bool Control::IsDragEnabled() const
	{
		return drag_enabled_;
	}

	void Control::SetDragEnable(bool bDrag)
	{
		drag_enabled_ = bDrag;
	}

	bool Control::IsDropEnabled() const
	{
		return drop_enabled_;
	}

	void Control::SetDropEnable(bool bDrop)
	{
		drop_enabled_ = bDrop;
	}


	DWORD Control::GetBkColor() const
	{
		return back_color_;
	}

	void Control::SetBkColor(DWORD dwBackColor)
	{
		if( back_color_ == dwBackColor ) return;

		back_color_ = dwBackColor;
		Invalidate();
	}

	DWORD Control::GetBkColor2() const
	{
		return back_color2_;
	}

	void Control::SetBkColor2(DWORD dwBackColor)
	{
		if( back_color2_ == dwBackColor ) return;

		back_color2_ = dwBackColor;
		Invalidate();
	}

	DWORD Control::GetBkColor3() const
	{
		return back_color3_;
	}

	void Control::SetBkColor3(DWORD dwBackColor)
	{
		if( back_color3_ == dwBackColor ) return;

		back_color3_ = dwBackColor;
		Invalidate();
	}

	DWORD Control::GetForeColor() const
	{
		return fore_color_;
	}

	void Control::SetForeColor(DWORD dwForeColor)
	{
		if( fore_color_ == dwForeColor ) return;

		fore_color_ = dwForeColor;
		Invalidate();
	}

	std::wstring Control::GetBkImage()
	{
		return bkimage_;
	}

	void Control::SetBkImage(std::wstring pStrImage)
	{
		if(pm_) pm_->RemoveImage(pStrImage);
		if( bkimage_ == pStrImage ) return;

		bkimage_ = pStrImage;
		Invalidate();
	}
	
	std::wstring Control::GetForeImage() const
	{
		return fore_image_;
	}

	void Control::SetForeImage(std::wstring pStrImage)
	{
		if( fore_image_ == pStrImage ) return;

		fore_image_ = pStrImage;
		Invalidate();
	}

	DWORD Control::GetBorderColor() const
	{
		return border_color_;
	}

	void Control::SetBorderColor(DWORD dwBorderColor)
	{
		if( border_color_ == dwBorderColor ) return;

		border_color_ = dwBorderColor;
		Invalidate();
	}

	DWORD Control::GetFocusBorderColor() const
	{
		return focused_border_color_;
	}

	void Control::SetFocusBorderColor(DWORD dwBorderColor)
	{
		if( focused_border_color_ == dwBorderColor ) return;

		focused_border_color_ = dwBorderColor;
		Invalidate();
	}

	bool Control::IsColorHSL() const
	{
		return color_hsl_;
	}

	void Control::SetColorHSL(bool bColorHSL)
	{
		if( color_hsl_ == bColorHSL ) return;

		color_hsl_ = bColorHSL;
		Invalidate();
	}

	int Control::GetBorderSize() const
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(border_size_);
		return border_size_;
	}

	void Control::SetBorderSize(int nSize)
	{
		if( border_size_ == nSize ) return;

		border_size_ = nSize;
		Invalidate();
	}

	void Control::SetBorderSize( RECT rc )
	{
		border_size_rect_ = rc;
		Invalidate();
	}

	SIZE Control::GetBorderRound() const
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(border_round_sz_);
		return border_round_sz_;
	}

	void Control::SetBorderRound(SIZE cxyRound)
	{
		border_round_sz_ = cxyRound;
		Invalidate();
	}

	bool Control::DrawImage(HDC hDC, std::wstring pStrImage, std::wstring pStrModify)
	{
		return RenderEngine::DrawImageString(hDC, pm_, rect_, paint_rect_, pStrImage, pStrModify, instance_);
	}

	const RECT& Control::GetPos() const
	{
		return rect_;
	}

	RECT Control::GetRelativePos() const
	{
		Control* pParent = GetParent();
		if( pParent != NULL ) {
			RECT rcParentPos = pParent->GetPos();
			CDuiRect rcRelativePos(rect_);
			rcRelativePos.Offset(-rcParentPos.left, -rcParentPos.top);
			return rcRelativePos;
		}
		else {
			return CDuiRect(0, 0, 0, 0);
		}
	}

	RECT Control::GetClientPos() const 
	{
		return rect_;
	}
	void Control::SetPos(RECT rc, bool bNeedInvalidate)
	{
		if( rc.right < rc.left ) rc.right = rc.left;
		if( rc.bottom < rc.top ) rc.bottom = rc.top;

		CDuiRect invalidateRc = rect_;
		if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

		rect_ = rc;
		if( pm_ == NULL ) return;

		if( !has_setted_pos_ ) {
			has_setted_pos_ = true;
			Notify(kEventResize);
			has_setted_pos_ = false;
		}

		need_update_ = false;

		if( bNeedInvalidate && IsVisible() ) {
			invalidateRc.Join(rect_);
			Control* pParent = this;
			RECT rcTemp;
			RECT rcParent;
			while( pParent = pParent->GetParent() ) {
				if( !pParent->IsVisible() ) return;
				rcTemp = invalidateRc;
				rcParent = pParent->GetPos();
				if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) return;
			}
			pm_->Invalidate(invalidateRc);
		}
	}

	void Control::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		cxy_sz_.cx += szOffset.cx;
		cxy_sz_.cy += szOffset.cy;
		NeedParentUpdate();
	}

	int Control::GetWidth() const
	{
		return rect_.right - rect_.left;
	}

	int Control::GetHeight() const
	{
		return rect_.bottom - rect_.top;
	}

	int Control::GetX() const
	{
		return rect_.left;
	}

	int Control::GetY() const
	{
		return rect_.top;
	}

	RECT Control::GetPadding() const
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(padding_rect_);
		return padding_rect_;
	}

	void Control::SetPadding(RECT rcPadding)
	{
		padding_rect_ = rcPadding;
		NeedParentUpdate();
	}

	SIZE Control::GetFixedXY() const
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(cxy_sz_);
		return cxy_sz_;
	}

	void Control::SetFixedXY(SIZE szXY)
	{
		cxy_sz_.cx = szXY.cx;
		cxy_sz_.cy = szXY.cy;
		NeedParentUpdate();
	}

	int Control::GetFixedWidth()
	{
		// auto
		if (cxy_fired_sz_.cx == -1) {
			if (!text_.empty()) {
				cxy_fired_sz_.cx = GlobalManager::GetTextPixelWidth(pm_->GetPaintDC(), NULL, text_);
			}

			if (!bkimage_.empty()) {
				int width = 0;
				const TImageInfo *image_info = pm_->GetImageEx(bkimage_);
				if (image_info == NULL) {
					TDrawInfo draw_info;
					draw_info.Parse(bkimage_);
					if (draw_info.IsValidDest()) {
						width = draw_info.rcDest.right - draw_info.rcDest.left;
					}
					else {
						image_info = pm_->AddImage(draw_info.sImageName, draw_info.sResType, draw_info.dwMask, draw_info.bHSL);
						width = image_info->nX;
					}
				}
				else {
					width = image_info->nX;
				}
				cxy_fired_sz_.cx = width > cxy_fired_sz_.cx ? width : cxy_fired_sz_.cx;
			}
		}

		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_fired_sz_.cx);
		}

		return cxy_fired_sz_.cx;
	}

	void Control::SetFixedWidth(int cx)
	{
		//if( cx < 0 ) return;  TODO : luyang 2017/01/09
		cxy_fired_sz_.cx = cx; 
		NeedParentUpdate();
	}

	int Control::GetFixedHeight()
	{
		// auto
		if (cxy_fired_sz_.cy == -1) {
			cxy_fired_sz_.cy = GlobalManager::GetTextPixelHeight(pm_->GetPaintDC(), NULL);
			if (!bkimage_.empty()) {
				int height = 0;
				const TImageInfo *image_info = pm_->GetImageEx(bkimage_);
				if (image_info == NULL) {
					TDrawInfo draw_info;
					draw_info.Parse(bkimage_);
					if (draw_info.IsValidDest()) {
						height = draw_info.rcDest.bottom - draw_info.rcDest.top;
					}
					else {
						image_info = pm_->AddImage(draw_info.sImageName, draw_info.sResType, draw_info.dwMask, draw_info.bHSL);
						height = image_info->nY;
					}
				}
				else {
					height = image_info->nY;
				}
				cxy_fired_sz_.cy = height > cxy_fired_sz_.cy ? height : cxy_fired_sz_.cy;
			}
		}

		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_fired_sz_.cy);
		}

		return cxy_fired_sz_.cy;
	}

	void Control::SetFixedHeight(int cy)
	{
		// TODO: luyag 2017 / 01 / 09
		//if( cy < 0 ) return; 
		cxy_fired_sz_.cy = cy;
		NeedParentUpdate();
	}

	int Control::GetMinWidth() const
	{
		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_min_sz_.cx);
		}
		return cxy_min_sz_.cx;
	}

	void Control::SetMinWidth(int cx)
	{
		if( cxy_min_sz_.cx == cx ) return;

		if( cx < 0 ) return; 
		cxy_min_sz_.cx = cx;
		NeedParentUpdate();
	}

	int Control::GetMaxWidth() const
	{
		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_max_sz_.cx);
		}
		return cxy_max_sz_.cx;
	}

	void Control::SetMaxWidth(int cx)
	{
		if( cxy_max_sz_.cx == cx ) return;

		if( cx < 0 ) return; 
		cxy_max_sz_.cx = cx;
		NeedParentUpdate();
	}

	int Control::GetMinHeight() const
	{
		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_min_sz_.cy);
		}
		
		return cxy_min_sz_.cy;
	}

	void Control::SetMinHeight(int cy)
	{
		if( cxy_min_sz_.cy == cy ) return;

		if( cy < 0 ) return; 
		cxy_min_sz_.cy = cy;
		NeedParentUpdate();
	}

	int Control::GetMaxHeight() const
	{
		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_max_sz_.cy);
		}

		return cxy_max_sz_.cy;
	}

	void Control::SetMaxHeight(int cy)
	{
		if( cxy_max_sz_.cy == cy ) return;

		if( cy < 0 ) return; 
		cxy_max_sz_.cy = cy;
		NeedParentUpdate();
	}

	TPercentInfo Control::GetFloatPercent() const
	{
		return float_percent_;
	}
	
	void Control::SetFloatPercent(TPercentInfo piFloatPercent)
	{
		float_percent_ = piFloatPercent;
		NeedParentUpdate();
	}

	void Control::SetFloatAlign(UINT uAlign)
	{
		float_align_ = uAlign;
		NeedParentUpdate();
	}

	UINT Control::GetFloatAlign() const
	{
		return float_align_;
	}

	std::wstring Control::GetToolTip() const
	{
		if (!IsResourceText()) return tooltip_;
		return ResourceManager::GetInstance()->GetText(tooltip_);
	}

	void Control::SetToolTip(std::wstring pstrText)
	{
		std::wstring strTemp(pstrText);
		nbase::StringReplaceAll(_T("<n>"), _T("\r\n"), strTemp);
		tooltip_ = strTemp;
	}

	void Control::SetToolTipWidth( int nWidth )
	{
		tooltip_width_ = nWidth;
	}

	int Control::GetToolTipWidth( void )
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(tooltip_width_);
		return tooltip_width_;
	}
	
	WORD Control::GetCursor()
	{
		return cursor_;
	}

	void Control::SetCursor(WORD wCursor)
	{
		cursor_ = wCursor;
		Invalidate();
	}

	TCHAR Control::GetShortcut() const
	{
		return short_cut_;
	}

	void Control::SetShortcut(TCHAR ch)
	{
		short_cut_ = ch;
	}

	bool Control::IsContextMenuUsed() const
	{
		return menu_used_;
	}

	void Control::SetContextMenuUsed(bool bMenuUsed)
	{
		menu_used_ = bMenuUsed;
	}

	std::wstring Control::GetUserData()
	{
		return user_data_;
	}

	void Control::SetUserData(std::wstring pstrText)
	{
		user_data_ = pstrText;
	}

	UINT_PTR Control::GetTag() const
	{
		return tag_;
	}

	void Control::SetTag(UINT_PTR pTag)
	{
		tag_ = pTag;
	}

	bool Control::IsVisible() const
	{

		return visible_ && internel_visible_;
	}

	void Control::SetVisible(bool bVisible)
	{
		if( visible_ == bVisible ) return;

		bool v = IsVisible();
		visible_ = bVisible;
		if( focused_ ) focused_ = false;
		if (!bVisible && pm_ && pm_->GetFocus() == this) {
			pm_->SetFocus(NULL) ;
		}
		if( IsVisible() != v ) {
			NeedParentUpdate();
		}
	}

	void Control::SetInternVisible(bool bVisible)
	{
		internel_visible_ = bVisible;
		if (!bVisible && pm_ && pm_->GetFocus() == this) {
			pm_->SetFocus(NULL) ;
		}
	}

	bool Control::IsEnabled() const
	{
		return enabled_;
	}

	void Control::SetEnabled(bool bEnabled)
	{
		if( enabled_ == bEnabled ) return;

		enabled_ = bEnabled;
		Invalidate();
	}

	bool Control::IsMouseEnabled() const
	{
		return mouse_enabled_;
	}

	void Control::SetMouseEnabled(bool bEnabled)
	{
		mouse_enabled_ = bEnabled;
	}

	bool Control::IsKeyboardEnabled() const
	{
		return keybord_enabled_ ;
	}
	void Control::SetKeyboardEnabled(bool bEnabled)
	{
		keybord_enabled_ = bEnabled ; 
	}

	bool Control::IsFocused() const
	{
		return focused_;
	}

	void Control::SetFocus()
	{
		if( pm_ != NULL ) pm_->SetFocus(this);
	}

	bool Control::IsFloat() const
	{
		return float_;
	}

	void Control::SetFloat(bool bFloat)
	{
		if( float_ == bFloat ) return;

		float_ = bFloat;
		NeedParentUpdate();
	}

	Control* Control::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
		if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
		if( (uFlags & UIFIND_HITTEST) != 0 && (!mouse_enabled_ || !::PtInRect(&rect_, * static_cast<LPPOINT>(pData))) ) return NULL;
		return Proc(this, pData);
	}

	void Control::Invalidate()
	{
		if( !IsVisible() ) return;

		RECT invalidateRc = rect_;

		Control* pParent = this;
		RECT rcTemp;
		RECT rcParent;
		while( pParent = pParent->GetParent() )
		{
			rcTemp = invalidateRc;
			rcParent = pParent->GetPos();
			if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
			{
				return;
			}
		}

		if( pm_ != NULL ) pm_->Invalidate(invalidateRc);
	}

	bool Control::IsUpdateNeeded() const
	{
		return need_update_;
	}

	void Control::NeedUpdate()
	{
		if( !IsVisible() ) return;
		need_update_ = true;
		Invalidate();

		if( pm_ != NULL ) pm_->NeedUpdate();
	}

	void Control::NeedParentUpdate()
	{
		if( GetParent() ) {
			GetParent()->NeedUpdate();
			GetParent()->Invalidate();
		}
		else {
			NeedUpdate();
		}

		if( pm_ != NULL ) pm_->NeedUpdate();
	}

	DWORD Control::GetAdjustColor(DWORD dwColor)
	{
		if( !color_hsl_ ) return dwColor;
		short H, S, L;
		PaintManager::GetHSL(&H, &S, &L);
		return RenderEngine::AdjustColor(dwColor, H, S, L);
	}

	void Control::Init()
	{
		DoInit();

		auto it = on_xml_event_.find(kEventFirst);
		EventArgs arg;
		arg.sender_ = this;
		arg.type_ = kEventFirst;
		if (it != on_xml_event_.end() && it->second.empty()) { 
			it->second(arg); 
		}
	}

	void Control::DoInit()
	{

	}

	void Control::Event(EventArgs& arg)
	{
		DoEvent(arg);
		auto it = on_event_.find(kEventAll);
		if (it != on_event_.end() && !it->second.empty()) {
			it->second(arg);
		}
		
		it = on_event_.find(arg.type_);
		if (it != on_event_.end() && !it->second.empty()) {
			it->second(arg);
		}
	}

	void Control::DoEvent(EventArgs& event)
	{
		if( event.type_ == kEventSetCursor ) {
			if( GetCursor() ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(GetCursor())));
			}
			else {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			}
			return;
		}

		if( event.type_ == kEventSetFocus ) 
		{
			focused_ = true;
			Invalidate();
			return;
		}
		if( event.type_ == kEventKillFocus ) {
			focused_ = false;
			Invalidate();
			return;
		}
		if( event.type_ == kEventTimer ){
			Notify(kEventTimer, event.w_param_, event.l_param_);
			return;
		}
		if( event.type_ == kEventMouseMenu ){
			if( IsContextMenuUsed() ) {
				Notify(kEventMouseMenu, event.w_param_, event.l_param_);
				return;
			}
		}

		if( parent_ != NULL ) parent_->DoEvent(event);
	}

	void Control::Notify(EventType t, WPARAM wParam, LPARAM lParam, wchar_t key, WORD keyState, POINT pt)
	{
		auto it = on_event_.find(t);
		if (it != on_event_.end() && !it->second.empty()) {
			EventArgs arg;
			arg.type_ = t;
			arg.sender_ = this;
			arg.key_ = key;
			arg.key_state_ = keyState;
			arg.w_param_ = wParam;
			arg.l_param_ = lParam;
			arg.timestamp_ = ::GetTickCount();
			arg.mouse_point_ = pm_->GetLastPoint();
			it->second(arg);
		}
	}

	void Control::Notify(const EventArgs & arg)
	{
		auto it = on_event_.find(arg.type_);
		if (it != on_event_.end() && !it->second.empty()) {
			it->second(arg);
		}
	}

	void Control::DetachEvent(EventType type)
	{
		auto it = on_event_.find(type);
		on_event_.erase(it);
	}

	void Control::AddCustomAttribute(std::wstring pstrName, std::wstring pstrAttr)
	{
		if( pstrName.empty() || pstrAttr.empty()) return;
		std::wstring* pCostomAttr = new std::wstring(pstrAttr);
		if (pCostomAttr != NULL) {
			if (custom_attributes_map_.Find(pstrName) == NULL)
				custom_attributes_map_.Set(pstrName, (LPVOID)pCostomAttr);
			else
				delete pCostomAttr;
		}
	}

	std::wstring Control::GetCustomAttribute(std::wstring pstrName) const
	{
		if( pstrName.empty() ) return NULL;
		std::wstring* pCostomAttr = static_cast<std::wstring*>(custom_attributes_map_.Find(pstrName));
		if( pCostomAttr ) return pCostomAttr->c_str();
		return NULL;
	}

	bool Control::RemoveCustomAttribute(std::wstring pstrName)
	{
		if( pstrName.empty()) return NULL;
		std::wstring* pCostomAttr = static_cast<std::wstring*>(custom_attributes_map_.Find(pstrName));
		if( !pCostomAttr ) return false;

		delete pCostomAttr;
		return custom_attributes_map_.Remove(pstrName);
	}

	void Control::RemoveAllCustomAttribute()
	{
		std::wstring* pCostomAttr;
		for( int i = 0; i< custom_attributes_map_.GetSize(); i++ ) {
			std::wstring key = custom_attributes_map_.GetAt(i);
			if(!key.empty()) {
				pCostomAttr = static_cast<std::wstring*>(custom_attributes_map_.Find(key));
				delete pCostomAttr;
			}
		}
		custom_attributes_map_.Resize();
	}

	void Control::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if (_tcsicmp(pstrName, L"Class") == 0) {
			ASSERT(!set_unclass_attributes_);
			std::wstring cls = GlobalManager::GetClass(strValue);
			if (!cls.empty()) {
				ApplyAttributeList(cls);
			}
		}
		else {
			set_unclass_attributes_ = true;

			if (_tcsicmp(pstrName, L"pos") == 0) {
				RECT rcPos = { 0 };
				LPTSTR pstr = NULL;
				rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
				rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
				rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
				rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				SIZE szXY = { rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom };
				SetFixedXY(szXY);
				SetFixedWidth(rcPos.right - rcPos.left);
				SetFixedHeight(rcPos.bottom - rcPos.top);
			}
			else if (_tcsicmp(pstrName, L"float") == 0) {
				std::wstring nValue = strValue;
				// ∂ØÃ¨º∆À„œ‡∂‘±»¿˝
				if (nValue.find(L',') != std::string::npos) {
					SetFloat(_tcsicmp(pstrValue, L"true") == 0);
				}
				else {
					TPercentInfo piFloatPercent = { 0 };
					LPTSTR pstr = NULL;
					piFloatPercent.left = _tcstod(pstrValue, &pstr);  ASSERT(pstr);
					piFloatPercent.top = _tcstod(pstr + 1, &pstr);    ASSERT(pstr);
					piFloatPercent.right = _tcstod(pstr + 1, &pstr);  ASSERT(pstr);
					piFloatPercent.bottom = _tcstod(pstr + 1, &pstr); ASSERT(pstr);
					SetFloatPercent(piFloatPercent);
					SetFloat(true);
				}
			}
			else if (_tcsicmp(pstrName, L"floatalign") == 0) {
				UINT uAlign = GetFloatAlign();

				// Ω‚ŒˆŒƒ◊÷ Ù–‘
				while (*pstrValue != _T('\0')) {
					std::wstring sValue;
					while (*pstrValue == _T(',') || *pstrValue == _T(' ')) pstrValue = ::CharNext(pstrValue);

					while (*pstrValue != _T('\0') && *pstrValue != _T(',') && *pstrValue != _T(' ')) {
						LPTSTR pstrTemp = ::CharNext(pstrValue);
						while (pstrValue < pstrTemp) {
							sValue += *pstrValue++;
						}
					}
					sValue = nbase::MakeLowerString(sValue);
					if (sValue.compare(_T("null")) == 0) {
						uAlign = 0;
					}
					if (sValue.compare(_T("left")) == 0) {
						uAlign &= ~(DT_CENTER | DT_RIGHT);
						uAlign |= DT_LEFT;
					}
					else if (sValue.compare(_T("center")) == 0) {
						uAlign &= ~(DT_LEFT | DT_RIGHT);
						uAlign |= DT_CENTER;
					}
					else if (sValue.compare(_T("right")) == 0) {
						uAlign &= ~(DT_LEFT | DT_CENTER);
						uAlign |= DT_RIGHT;
					}
					else if (sValue.compare(_T("top")) == 0) {
						uAlign &= ~(DT_BOTTOM | DT_VCENTER);
						uAlign |= DT_TOP;
					}
					else if (sValue.compare(_T("vcenter")) == 0) {
						uAlign &= ~(DT_TOP | DT_BOTTOM);
						uAlign |= DT_VCENTER;
					}
					else if (sValue.compare(_T("bottom")) == 0) {
						uAlign &= ~(DT_TOP | DT_VCENTER);
						uAlign |= DT_BOTTOM;
					}
				}
				SetFloatAlign(uAlign);
			}
			else if (_tcsicmp(pstrName, _T("padding")) == 0) {
				RECT rcPadding = { 0 };
				LPTSTR pstr = NULL;
				rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
				rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
				rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
				rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				SetPadding(rcPadding);
			}
			else if (_tcsicmp(pstrName, _T("bkcolor")) == 0 || _tcsicmp(pstrName, _T("bkcolor1")) == 0) {
				SetBkColor(GlobalManager::GetColor(strValue));
			}
			else if (_tcsicmp(pstrName, _T("bkcolor2")) == 0) {
				SetBkColor2(GlobalManager::GetColor(strValue));
			}
			else if (_tcsicmp(pstrName, _T("bkcolor3")) == 0) {
				SetBkColor3(GlobalManager::GetColor(strValue));
			}
			else if (_tcsicmp(pstrName, _T("forecolor")) == 0) {
				SetForeColor(GlobalManager::GetColor(strValue));
			}
			else if (_tcsicmp(pstrName, _T("bordercolor")) == 0) {
				SetBorderColor(GlobalManager::GetColor(strValue));
			}
			else if (_tcsicmp(pstrName, _T("focusbordercolor")) == 0) {
				SetFocusBorderColor(GlobalManager::GetColor(strValue));
			}
			else if (_tcsicmp(pstrName, _T("colorhsl")) == 0) SetColorHSL(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("bordersize")) == 0) {
				std::wstring nValue = pstrValue;
				if (nValue.find(',') == std::string::npos) {
					SetBorderSize(_ttoi(pstrValue));
					RECT rcPadding = { 0 };
					SetBorderSize(rcPadding);
				}
				else {
					RECT rcPadding = { 0 };
					LPTSTR pstr = NULL;
					rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
					rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
					SetBorderSize(rcPadding);
				}
			}
			else if (_tcsicmp(pstrName, _T("leftbordersize")) == 0) SetLeftBorderSize(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("topbordersize")) == 0) SetTopBorderSize(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("rightbordersize")) == 0) SetRightBorderSize(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("bottombordersize")) == 0) SetBottomBorderSize(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("borderstyle")) == 0) SetBorderStyle(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("borderround")) == 0) {
				SIZE cxyRound = { 0 };
				LPTSTR pstr = NULL;
				cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
				cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
				SetBorderRound(cxyRound);
			}
			else if (_tcsicmp(pstrName, _T("bkimage")) == 0) SetBkImage(pstrValue);
			else if (_tcsicmp(pstrName, _T("foreimage")) == 0) SetForeImage(pstrValue);
			else if (_tcsicmp(pstrName, _T("width")) == 0) {
				if (_tcsicmp(pstrValue, L"auto") == 0) {
					SetFixedWidth(-1);
				}
				else if (_tcsicmp(pstrValue, L"stretch") == 0) {
					SetFixedWidth(0);
				}
				else {
					SetFixedWidth(_ttoi(pstrValue));
				}
			} 
			else if (_tcsicmp(pstrName, _T("height")) == 0) {
				if (_tcsicmp(pstrValue, L"auto") == 0) {
					SetFixedHeight(-1);
				}
				else if (_tcsicmp(pstrValue, L"stretch") == 0) {
					SetFixedHeight(0);
				}
				else {
					SetFixedHeight(_ttoi(pstrValue));
				}
			}
			else if (_tcsicmp(pstrName, _T("minwidth")) == 0) SetMinWidth(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("minheight")) == 0) SetMinHeight(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("maxwidth")) == 0) SetMaxWidth(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("maxheight")) == 0) SetMaxHeight(_ttoi(pstrValue));
			else if (_tcsicmp(pstrName, _T("name")) == 0) SetName(pstrValue);
			else if (_tcsicmp(pstrName, _T("drag")) == 0) SetDragEnable(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("drop")) == 0) SetDropEnable(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("resourcetext")) == 0) SetResourceText(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("text")) == 0) SetText(pstrValue);
			else if (_tcsicmp(pstrName, _T("tooltip")) == 0) SetToolTip(pstrValue);
			else if (_tcsicmp(pstrName, _T("userdata")) == 0) SetUserData(pstrValue);
			else if (_tcsicmp(pstrName, _T("enabled")) == 0) SetEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("mouse")) == 0) SetMouseEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("keyboard")) == 0) SetKeyboardEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("visible")) == 0) SetVisible(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("float")) == 0) SetFloat(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("shortcut")) == 0) SetShortcut(pstrValue[0]);
			else if (_tcsicmp(pstrName, _T("menu")) == 0) SetContextMenuUsed(_tcsicmp(pstrValue, _T("true")) == 0);
			else if (_tcsicmp(pstrName, _T("cursor")) == 0 && pstrValue) {
				if (_tcsicmp(pstrValue, _T("arrow")) == 0)			SetCursor(DUI_ARROW);
				else if (_tcsicmp(pstrValue, _T("ibeam")) == 0)	SetCursor(DUI_IBEAM);
				else if (_tcsicmp(pstrValue, _T("wait")) == 0)		SetCursor(DUI_WAIT);
				else if (_tcsicmp(pstrValue, _T("cross")) == 0)	SetCursor(DUI_CROSS);
				else if (_tcsicmp(pstrValue, _T("uparrow")) == 0)	SetCursor(DUI_UPARROW);
				else if (_tcsicmp(pstrValue, _T("size")) == 0)		SetCursor(DUI_SIZE);
				else if (_tcsicmp(pstrValue, _T("icon")) == 0)		SetCursor(DUI_ICON);
				else if (_tcsicmp(pstrValue, _T("sizenwse")) == 0)	SetCursor(DUI_SIZENWSE);
				else if (_tcsicmp(pstrValue, _T("sizenesw")) == 0)	SetCursor(DUI_SIZENESW);
				else if (_tcsicmp(pstrValue, _T("sizewe")) == 0)	SetCursor(DUI_SIZEWE);
				else if (_tcsicmp(pstrValue, _T("sizens")) == 0)	SetCursor(DUI_SIZENS);
				else if (_tcsicmp(pstrValue, _T("sizeall")) == 0)	SetCursor(DUI_SIZEALL);
				else if (_tcsicmp(pstrValue, _T("no")) == 0)		SetCursor(DUI_NO);
				else if (_tcsicmp(pstrValue, _T("hand")) == 0)		SetCursor(DUI_HAND);
			}
			else if (_tcsicmp(pstrName, _T("innerstyle")) == 0) {
				std::wstring sXmlData = pstrValue;
				nbase::StringReplaceAll(_T("&quot;"), _T("\""), sXmlData);
				auto pstrList = sXmlData.c_str();
				std::wstring sItem;
				std::wstring sValue;
				while (*pstrList != _T('\0')) {
					sItem.clear();
					sValue.clear();
					while (*pstrList != _T('\0') && *pstrList != _T('=')) {
						LPTSTR pstrTemp = ::CharNext(pstrList);
						while (pstrList < pstrTemp) {
							sItem += *pstrList++;
						}
					}
					ASSERT(*pstrList == _T('='));
					if (*pstrList++ != _T('=')) return;
					ASSERT(*pstrList == _T('\"'));
					if (*pstrList++ != _T('\"')) return;
					while (*pstrList != _T('\0') && *pstrList != _T('\"')) {
						LPTSTR pstrTemp = ::CharNext(pstrList);
						while (pstrList < pstrTemp) {
							sValue += *pstrList++;
						}
					}
					ASSERT(*pstrList == _T('\"'));
					if (*pstrList++ != _T('\"')) return;
					SetAttribute(sItem, sValue);
					if (*pstrList++ != _T(' ') && *pstrList++ != _T(',')) return;
				}
			}
		}
		
	}

	Control* Control::ApplyAttributeList(std::wstring strValue)
	{
		auto pstrList = strValue.c_str();

		std::wstring sItem;
		std::wstring sValue;
		while( *pstrList != _T('\0') ) {
			sItem.clear();
			sValue.clear();
			while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
				LPTSTR pstrTemp = ::CharNext(pstrList);
				while( pstrList < pstrTemp) {
					sItem += *pstrList++;
				}
			}
			ASSERT( *pstrList == _T('=') );
			if( *pstrList++ != _T('=') ) return this;
			ASSERT( *pstrList == _T('\"') );
			if( *pstrList++ != _T('\"') ) return this;
			while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
				LPTSTR pstrTemp = ::CharNext(pstrList);
				while( pstrList < pstrTemp) {
					sValue += *pstrList++;
				}
			}
			ASSERT( *pstrList == _T('\"') );
			if( *pstrList++ != _T('\"') ) return this;
			SetAttribute(sItem, sValue);
			if( *pstrList++ != _T(' ') && *pstrList++ != _T(',') ) return this;
		}
		return this;
	}

	SIZE Control::EstimateSize(SIZE szAvailable)
	{
		if(pm_ != NULL)
			return pm_->GetDPIObj()->Scale(cxy_fired_sz_);
		return cxy_fired_sz_;
	}

	void Control::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&paint_rect_, &rcPaint, &rect_) ) return;

		// ªÊ÷∆—≠–Ú£∫±≥æ∞—’…´->±≥æ∞Õº->◊¥Ã¨Õº->Œƒ±æ->±ﬂøÚ
		SIZE cxyBorderRound;
		RECT rcBorderSize;
		if (pm_) {
			cxyBorderRound = GetManager()->GetDPIObj()->Scale(border_round_sz_);
			rcBorderSize = GetManager()->GetDPIObj()->Scale(border_size_rect_);
		}
		else {
			cxyBorderRound = border_round_sz_;
			rcBorderSize = border_size_rect_;
		}

		if( cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0 ) {
			RenderClip roundClip;
			RenderClip::GenerateRoundClip(hDC, paint_rect_,  rect_, cxyBorderRound.cx, cxyBorderRound.cy, roundClip);
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintStatusImage(hDC);
			PaintForeColor(hDC);
			PaintForeImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
		}
		else {
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintStatusImage(hDC);
			PaintForeColor(hDC);
			PaintForeImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
		}
	}

	void Control::PaintBkColor(HDC hDC)
	{
		if( back_color_ != 0 ) {
			if( back_color2_ != 0 ) {
				if( back_color3_ != 0 ) {
					RECT rc = rect_;
					rc.bottom = (rc.bottom + rc.top) / 2;
					RenderEngine::DrawGradient(hDC, rc, GetAdjustColor(back_color_), GetAdjustColor(back_color2_), true, 8);
					rc.top = rc.bottom;
					rc.bottom = rect_.bottom;
					RenderEngine::DrawGradient(hDC, rc, GetAdjustColor(back_color2_), GetAdjustColor(back_color3_), true, 8);
				}
				else {
					RenderEngine::DrawGradient(hDC, rect_, GetAdjustColor(back_color_), GetAdjustColor(back_color2_), true, 16);
				}
			}
			else if( back_color_ >= 0xFF000000 ) RenderEngine::DrawColor(hDC, paint_rect_, GetAdjustColor(back_color_));
			else RenderEngine::DrawColor(hDC, rect_, GetAdjustColor(back_color_));
		}
	}

	void Control::PaintBkImage(HDC hDC)
	{
		if( bkimage_.empty() ) return;
		if( !DrawImage(hDC, bkimage_) ) {}
	}

	void Control::PaintStatusImage(HDC hDC)
	{
		return;
	}

	void Control::PaintForeColor(HDC hDC)
	{
		RenderEngine::DrawColor(hDC, rect_, GetAdjustColor(fore_color_));
	}
	
	void Control::PaintForeImage(HDC hDC)
	{
		if( fore_image_.empty() ) return;
		DrawImage(hDC, fore_image_);
	}

	void Control::PaintText(HDC hDC)
	{
		return;
	}

	void Control::PaintBorder(HDC hDC)
	{
		int nBorderSize;
		SIZE cxyBorderRound;
		RECT rcBorderSize;
		if (pm_) {
			nBorderSize = GetManager()->GetDPIObj()->Scale(border_size_);
			cxyBorderRound = GetManager()->GetDPIObj()->Scale(border_round_sz_);
			rcBorderSize = GetManager()->GetDPIObj()->Scale(border_size_rect_);
		}
		else {
			nBorderSize = border_size_;
			cxyBorderRound = border_round_sz_;
			rcBorderSize = border_size_rect_;

		}
		
		if(border_color_ != 0 || focused_border_color_ != 0) {
			//ª≠‘≤Ω«±ﬂøÚ
			if(nBorderSize > 0 && ( cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0 )) {
				if (IsFocused() && focused_border_color_ != 0)
					RenderEngine::DrawRoundRect(hDC, rect_, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy, GetAdjustColor(focused_border_color_), border_style_);
				else
					RenderEngine::DrawRoundRect(hDC, rect_, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy, GetAdjustColor(border_color_), border_style_);
			}
			else {
				if (IsFocused() && focused_border_color_ != 0 && border_size_ > 0) {
					RenderEngine::DrawRect(hDC, rect_, nBorderSize, GetAdjustColor(focused_border_color_), border_style_);
				}
				else if(rcBorderSize.left > 0 || rcBorderSize.top > 0 || rcBorderSize.right > 0 || rcBorderSize.bottom > 0) {
					RECT rcBorder;

					if(rcBorderSize.left > 0){
						rcBorder		= rect_;
						rcBorder.right	= rcBorder.left;
						RenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.left,GetAdjustColor(border_color_),border_style_);
					}
					if(rcBorderSize.top > 0){
						rcBorder		= rect_;
						rcBorder.bottom	= rcBorder.top;
						RenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.top,GetAdjustColor(border_color_),border_style_);
					}
					if(rcBorderSize.right > 0){
						rcBorder		= rect_;
						rcBorder.right -= 1;
						rcBorder.left	= rcBorder.right;
						RenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.right,GetAdjustColor(border_color_),border_style_);
					}
					if(rcBorderSize.bottom > 0){
						rcBorder		= rect_;
						rcBorder.bottom -= 1;
						rcBorder.top	= rcBorder.bottom;
						RenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.bottom,GetAdjustColor(border_color_),border_style_);
					}
				}
				else if(nBorderSize > 0) {
					RenderEngine::DrawRect(hDC, rect_, nBorderSize, GetAdjustColor(border_color_), border_style_);
				}
			}
		}
	}

	void Control::DoPostPaint(HDC hDC, const RECT& rcPaint)
	{
		return;
	}

	int Control::GetLeftBorderSize() const
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(border_size_rect_.left);
		return border_size_rect_.left;
	}

	void Control::SetLeftBorderSize( int nSize )
	{
		border_size_rect_.left = nSize;
		Invalidate();
	}

	int Control::GetTopBorderSize() const
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(border_size_rect_.top);
		return border_size_rect_.top;
	}

	void Control::SetTopBorderSize( int nSize )
	{
		border_size_rect_.top = nSize;
		Invalidate();
	}

	int Control::GetRightBorderSize() const
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(border_size_rect_.right);
		return border_size_rect_.right;
	}

	void Control::SetRightBorderSize( int nSize )
	{
		border_size_rect_.right = nSize;
		Invalidate();
	}

	int Control::GetBottomBorderSize() const
	{
		if(pm_ != NULL) return pm_->GetDPIObj()->Scale(border_size_rect_.bottom);
		return border_size_rect_.bottom;
	}

	void Control::SetBottomBorderSize( int nSize )
	{
		border_size_rect_.bottom = nSize;
		Invalidate();
	}

	int Control::GetBorderStyle() const
	{
		return border_style_;
	}

	void Control::SetBorderStyle( int nStyle )
	{
		border_style_ = nStyle;
		Invalidate();
	}

} // namespace DuiLib
