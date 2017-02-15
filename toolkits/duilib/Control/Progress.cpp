#include "StdAfx.h"
#include "Progress.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(Progress)

	Progress::Progress() : m_bShowText(false), m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0), m_bStretchForeImage(true)
	{
		text_style_ = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
	}

	std::wstring Progress::GetClass() const
	{
		return _T("Progress");
	}

	LPVOID Progress::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_PROGRESS) == 0 ) return static_cast<Progress*>(this);
		return Label::GetInterface(pstrName);
	}
	
	bool Progress::IsShowText()
	{
		return m_bShowText;
	}

	void Progress::SetShowText(bool bShowText)
	{
		if( m_bShowText == bShowText ) return;
		m_bShowText = bShowText;
		if(!m_bShowText) SetText(_T(""));
	}

	bool Progress::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void Progress::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int Progress::GetMinValue() const
	{
		return m_nMin;
	}

	void Progress::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int Progress::GetMaxValue() const
	{
		return m_nMax;
	}

	void Progress::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int Progress::GetValue() const
	{
		return m_nValue;
	}

	void Progress::SetValue(int nValue)
	{
		if(nValue == m_nValue || nValue<m_nMin || nValue > m_nMax) {
			return;
		}
		m_nValue = nValue;
		Invalidate();
		UpdateText();
	}

	void Progress::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("isstretchfore"))==0) SetStretchForeImage(_tcsicmp(pstrValue, _T("true")) == 0? true : false);
		else Label::SetAttribute(strName, strValue);
	}

	void Progress::PaintForeColor(HDC hDC)
	{
		if(fore_color_ == 0) return;

		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = rect_;
		if( m_bHorizontal ) {
			rc.right = rect_.left + (m_nValue - m_nMin) * (rect_.right - rect_.left) / (m_nMax - m_nMin);
		}
		else {
			rc.bottom = rect_.top + (rect_.bottom - rect_.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
		
		}
		
		RenderEngine::DrawColor(hDC, rc, GetAdjustColor(fore_color_));
	}

	void Progress::PaintForeImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMin) * (rect_.right - rect_.left) / (m_nMax - m_nMin);
			rc.bottom = rect_.bottom - rect_.top;
		}
		else {
			rc.top = (rect_.bottom - rect_.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
			rc.right = rect_.right - rect_.left;
			rc.bottom = rect_.bottom - rect_.top;
		}

		if( !fore_image_.empty() ) {
			m_sForeImageModify.clear();
			int sw = MulDiv(rc.right - rc.left, 100, GetManager()->GetDPIObj()->GetScale());
			int sh = MulDiv(rc.bottom - rc.top, 100, GetManager()->GetDPIObj()->GetScale());
			rc.left = MulDiv(rc.left, 100, GetManager()->GetDPIObj()->GetScale());
			rc.top = MulDiv(rc.top, 100, GetManager()->GetDPIObj()->GetScale());
			rc.right = rc.left + sw;
			rc.bottom = rc.top + sh;
			if (m_bStretchForeImage) {
				m_sForeImageModify = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), rc.left, rc.top, rc.right, rc.bottom);
			}
			else {
				m_sForeImageModify = nbase::StringPrintf(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), rc.left, rc.top, rc.right, rc.bottom, rc.left, rc.top, rc.right, rc.bottom);
			}

			if( !DrawImage(hDC, fore_image_, m_sForeImageModify) ) {}
			else return;
		}
	}

	bool Progress::IsStretchForeImage()
	{
		return m_bStretchForeImage;
	}

	void Progress::SetStretchForeImage( bool bStretchForeImage /*= true*/ )
	{
		if (m_bStretchForeImage==bStretchForeImage)		return;
		m_bStretchForeImage=bStretchForeImage;
		Invalidate();
	}

	void Progress::UpdateText()
	{
		if(m_bShowText) {
			std::wstring sText;
			sText = nbase::StringPrintf(_T("%.0f%%"), (m_nValue - m_nMin) * 100.0f / (m_nMax - m_nMin));
			SetText(sText);
		}
	}
}
