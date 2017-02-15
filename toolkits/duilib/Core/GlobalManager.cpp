#include "StdAfx.h"
#include "base/memory/singleton.h"
#include "GlobalManager.h"

namespace ui
{

DWORD GlobalManager::default_disabled_color_ = 0xFFA7A6AA;
DWORD GlobalManager::default_font_color_ = 0xFF000000;
DWORD GlobalManager::default_link_font_color_ = 0xFF0000FF;
DWORD GlobalManager::default_link_hover_font_color_ = 0xFFD3215F;
DWORD GlobalManager::default_selected_bk_color_ = 0xFFBAE4FF;

TFontInfo GlobalManager::default_font_info_;
std::vector<TFontInfo> GlobalManager::fonts_(0);
std::unordered_map<std::wstring, std::wstring> GlobalManager::classes_;
std::unordered_map<std::wstring, DWORD> GlobalManager::colors_;
std::unordered_map<std::wstring, std::shared_ptr<WindowImplBase>> GlobalManager::forms_;

bool GlobalManager::Start(std::wstring theme_dir)
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfCharSet = DEFAULT_CHARSET;
	HFONT hDefaultFont = ::CreateFontIndirect(&lf);
	default_font_info_.handle_ = hDefaultFont;
	default_font_info_.name_ = lf.lfFaceName;
	default_font_info_.size_ = -lf.lfHeight;
	default_font_info_.bold_ = (lf.lfWeight >= FW_BOLD);
	default_font_info_.underline_ = (lf.lfUnderline == TRUE);
	default_font_info_.italic_ = (lf.lfItalic == TRUE);
	::ZeroMemory(&default_font_info_.tm_, sizeof(default_font_info_.tm_));

	// Load
	Markup xml;
	if (!xml.LoadFromFile(L"global.xml")) {
		return false;
	} 

	// Parse
	MarkupNode node = xml.GetRoot();
	if (!node.IsValid()) {
		return false;
	}

	LPCTSTR name = NULL;
	for (node = node.GetChild(); node.IsValid(); node = node.GetSibling())
	{
		name = node.GetName();
		if (_tcsicmp(name, L"Font") == 0) {
			ParseFont(node);
		}
		else if (_tcsicmp(name, L"Color") == 0) {
			ParseColor(node);
		}
		else if (_tcsicmp(name, L"Class") == 0) {
			ParseClass(node);
		}
	}

	return true;
}

void GlobalManager::Shutdown()
{
	RemoveAllClasses();
	RemoveAllFonts();
	RemoveAllColors();
	RemoveAllForms();
}

DWORD GlobalManager::GetDefaultDisabledColor()
{
	return default_disabled_color_;
}

void GlobalManager::SetDefaultDisabledColor(DWORD dwColor)
{
	default_disabled_color_ = dwColor;
}

DWORD GlobalManager::GetDefaultFontColor()
{
	return default_font_color_;
}

void GlobalManager::SetDefaultFontColor(DWORD dwColor)
{
	default_font_color_ = dwColor;
}

DWORD GlobalManager::GetDefaultLinkFontColor()
{
	return default_link_font_color_;
}

void GlobalManager::SetDefaultLinkFontColor(DWORD dwColor)
{
	default_link_font_color_ = dwColor;
}

DWORD GlobalManager::GetDefaultLinkHoverFontColor()
{
	return default_link_hover_font_color_;
}

void GlobalManager::SetDefaultLinkHoverFontColor(DWORD dwColor)
{
	default_link_hover_font_color_ = dwColor;
}

DWORD GlobalManager::GetDefaultSelectedBkColor()
{
	return default_selected_bk_color_;
}

void GlobalManager::SetDefaultSelectedBkColor(DWORD dwColor)
{
	default_selected_bk_color_ = dwColor;
}

TFontInfo * GlobalManager::GetDefaultFontInfo(HDC dc)
{
	if (default_font_info_.tm_.tmHeight == 0 && dc != NULL) {
		HFONT old_font = (HFONT)::SelectObject(dc, default_font_info_.handle_);
		::GetTextMetrics(dc, &default_font_info_.tm_);
		::SelectObject(dc, old_font);
	}
	return &default_font_info_;
}


void GlobalManager::AddColor(std::wstring name, std::wstring value)
{
	colors_.insert({ name, ConvertColor(value) });
}

DWORD GlobalManager::ConvertColor(std::wstring color)
{
	ASSERT(!color.empty());
	if (color.empty()) {
		return -1;
	}

	if (color.at(0) == L'#') {
		color.erase(color.begin());
	}

	LPTSTR pstr = NULL;
	return _tcstoul(color.c_str(), &pstr, 16);
}

DWORD GlobalManager::GetColor(std::wstring name)
{
	auto it = colors_.find(name);
	ASSERT(it != colors_.end());
	if (it == colors_.end()) {
		return -1;
	}

	return it->second;
}

void GlobalManager::RemoveAllColors()
{
	colors_.clear();
}

void GlobalManager::RemoveAllForms()
{
	forms_.clear();
}

HFONT GlobalManager::AddFont(TFontInfo & tf)
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

	if (!tf.name_.empty() && tf.name_ != L"system") {
		_tcsncpy(lf.lfFaceName, tf.name_.c_str(), LF_FACESIZE);
	}

	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -tf.size_;
	if (tf.bold_) lf.lfWeight = FW_BOLD;
	if (tf.underline_) lf.lfUnderline = TRUE;
	if (tf.italic_) lf.lfItalic = TRUE;

	HFONT hFont = ::CreateFontIndirect(&lf);
	if (hFont == NULL) {
		return NULL;
	}
	tf.handle_ = hFont;

	fonts_.push_back(tf);
	return hFont;
}

HFONT GlobalManager::AddFont(std::wstring name, int size, bool bold, bool underline, bool italic)
{
	TFontInfo tf;
	::ZeroMemory(&tf, sizeof(tf));
	tf.name_ = name;
	tf.size_ = size;
	tf.bold_ = bold;
	tf.underline_ = underline;
	tf.italic_ = italic;

	return AddFont(tf);
}


HFONT GlobalManager::GetFont(int i)
{
	if (i < 0 || i >= fonts_.size()) {
		return default_font_info_.handle_;
	}

	return fonts_.at(i).handle_;
}

HFONT GlobalManager::GetFont(std::wstring name, int size, bool bold, bool underline, bool italic)
{
	for (auto & f : fonts_)
	{
		if (f.name_ == name && f.size_ == size && f.bold_ == bold && f.underline_ == underline && f.italic_ == italic) {
			return f.handle_;
		}
	}

	return NULL;
}

TFontInfo* GlobalManager::GetFontInfo(size_t i, HDC dc)
{
	if (i < 0 || i >= fonts_.size()) {
		return GetDefaultFontInfo(dc);
	}

	TFontInfo *p = &(fonts_.at(i));
	if (dc && p->tm_.tmHeight == 0) {
		if (dc) {
			HFONT hOldFont = (HFONT) ::SelectObject(dc, p->handle_);
			::GetTextMetrics(dc, &p->tm_);
			::SelectObject(dc, hOldFont);
		}
	}

	return p;
}

TFontInfo * GlobalManager::GetFontInfo(HFONT h, HDC dc)
{
	size_t i = 0;
	for (; i < fonts_.size(); i++)
	{
		if (fonts_.at(i).handle_ == h) {
			return GetFontInfo(i, dc);
		}

	}

	return GetDefaultFontInfo(dc);
}

int GlobalManager::GetFontIndex(HFONT h)
{
	for (int i = 0; i < (int)fonts_.size(); ++i)
	{
		if (fonts_.at(i).handle_ == h) {
			return i;
		}
	}

	return -1;
}

int GlobalManager::GetFontIndex(std::wstring name, int size, bool bold, bool underline, bool italic)
{
	for (int i = 0; i < (int)fonts_.size(); ++i)
	{
		auto & f = fonts_.at(i);
		if (f.name_ == name && f.size_ == size && f.bold_ == bold && f.underline_ == underline && f.italic_ == italic) {
			return i;
		}
	}

	return -1;
}

void GlobalManager::RemoveFont(int i)
{
	ASSERT(i >= 0 && i < fonts_.size());
	if (i < 0 || i >= fonts_.size()) {
		return;
	}

	auto it = fonts_.begin();
	std::advance(it, i);
	::DeleteObject(it->handle_);

	fonts_.erase(it);
}

void GlobalManager::RemoveFont(HFONT h)
{
	for (auto it = fonts_.begin(); it != fonts_.end(); ++it)
	{
		if (it->handle_ == h) {
			::DeleteObject(it->handle_);
			fonts_.erase(it);
			break;
		}
	}
}

void GlobalManager::RemoveAllFonts()
{
	for(const auto &f: fonts_)
	{
		if(f.handle_ != NULL){
			::DeleteObject(f.handle_);
		}
	}
	fonts_.clear();
	fonts_.resize(0);
}

void GlobalManager::RemoveAllClasses()
{
	classes_.clear();
}

void GlobalManager::AddClass(std::wstring name, std::wstring value)
{
	nbase::StringReplaceAll(L"&quot;", L"\"", value);
	classes_.insert({ name, value });
}

std::wstring GlobalManager::GetClass(std::wstring name)
{
	auto it = classes_.find(name);
	ASSERT(it != classes_.end());
	if (it == classes_.end()) {
		return L"";
	}

	return it->second;
}

long GlobalManager::GetTextPixelWidth(HDC dc, HFONT f, std::wstring txt)
{
	SIZE sz = { 0 };
	if (dc != NULL && f != NULL) {
		HFONT old_font = (HFONT)::SelectObject(dc, f);
		::GetTextExtentPoint32(dc, txt.c_str(), txt.length(), &sz);
		::SelectObject(dc, old_font);
	}
	else {
		::GetTextExtentPoint32(dc, txt.c_str(), txt.length(), &sz);
	}
	
	return sz.cx;
}

long GlobalManager::GetTextPixelHeight(HDC dc, HFONT f)
{
	TEXTMETRIC tm = { 0 };
	if (dc && f) {
		HFONT hOldFont = (HFONT) ::SelectObject(dc, f);
		::GetTextMetrics(dc, &tm);
		::SelectObject(dc, hOldFont);
		return tm.tmHeight;
	}
	else {
		::GetTextMetrics(dc, &tm);
	}

	return tm.tmHeight;
}

long GlobalManager::GetTextPixelWidth(HDC dc, int font_id, std::wstring txt)
{
	HFONT f = GetFont(font_id);
	if (f != NULL) {
		return GetTextPixelWidth(dc, f, txt);
	}
	return 0;
}

long GlobalManager::GetTextPixelHeight(HDC dc, int font_id)
{
	HFONT f = GetFont(font_id);
	if (f != NULL) {
		return GetTextPixelHeight(dc, f);
	}
	return 0;
}

void GlobalManager::RemoveForm(std::wstring wnd_id)
{
	auto it = forms_.find(wnd_id);
	if (it != forms_.end()) {
		forms_.erase(it);
	}
}

void GlobalManager::ParseFont(MarkupNode node)
{
	TFontInfo tfont;
	::ZeroMemory(&tfont, sizeof(tfont));

	LPCTSTR attr_name = NULL;
	LPCTSTR atrr_value = NULL;
	LPTSTR pstr = NULL;

	for (int i = 0; i < node.GetAttributeCount(); ++i)
	{
		attr_name = node.GetAttributeName(i);
		atrr_value = node.GetAttributeValue(i);
		if (_tcsicmp(attr_name, _T("name")) == 0) {
			tfont.name_ = atrr_value;
		}
		else if (_tcsicmp(attr_name, _T("size")) == 0) {
			tfont.size_ = _tcstol(atrr_value, &pstr, 10);
		}
		else if (_tcsicmp(attr_name, _T("bold")) == 0) {
			tfont.bold_ = (_tcsicmp(atrr_value, _T("true")) == 0);
		}
		else if (_tcsicmp(attr_name, _T("underline")) == 0) {
			tfont.underline_ = (_tcsicmp(atrr_value, _T("true")) == 0);
		}
		else if (_tcsicmp(attr_name, _T("italic")) == 0) {
			tfont.italic_ = (_tcsicmp(atrr_value, _T("true")) == 0);
		}
	}

	AddFont(tfont);
}

void GlobalManager::ParseColor(MarkupNode node)
{
	LPCTSTR attr_name = NULL;
	LPCTSTR atrr_value = NULL;
	
	std::wstring name;
	std::wstring value;
	for (int i = 0; i < node.GetAttributeCount(); ++i)
	{
		attr_name = node.GetAttributeName(i);
		atrr_value = node.GetAttributeValue(i);
		if (_tcsicmp(attr_name, L"name") == 0) {
			name = atrr_value;
		}
		else if (_tcsicmp(attr_name, L"value") == 0) {
			value = atrr_value;
		}
	}

	if (!name.empty() && !value.empty()) {
		AddColor(name, value);
	}
}

void GlobalManager::ParseClass(MarkupNode node)
{
	LPCTSTR attr_name = NULL;
	LPCTSTR atrr_value = NULL;

	std::wstring name;
	std::wstring value;
	for (int i = 0; i < node.GetAttributeCount(); ++i)
	{
		attr_name = node.GetAttributeName(i);
		atrr_value = node.GetAttributeValue(i);
		if (_tcsicmp(attr_name, L"name") == 0) {
			name = atrr_value;
		}
		else if (_tcsicmp(attr_name, L"value") == 0) {
			value = atrr_value;
		}
	}

	if (!name.empty() && !value.empty()) {
		AddClass(name, value);
	}
}

std::shared_ptr<WindowImplBase> GlobalManager::GetFormFromWindowId(std::wstring wnd_id)
{
	auto iter = forms_.find(wnd_id);
	if (iter != forms_.end()) { 
		if (iter->second->GetHWND() != NULL && ::IsWindow(iter->second->GetHWND())) {
			return iter->second;
		}
	}

	return nullptr;
}

Control* GlobalManager::CreateFromXml(const std::wstring &xml, PaintManager *pm, Control *parent, ControlBuilderCallback cb)
{
	DialogBuilder builder;
	return builder.Create(xml.c_str(), L"", pm, parent, cb);
}

} // namespace ui