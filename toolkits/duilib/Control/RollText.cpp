#include "stdafx.h"
#include "RollText.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(RollText)

	RollText::RollText(void)
	{
		m_nScrollPos = 0;
		m_nText_W_H = 0;
		m_nStep = 5;
		m_bUseRoll = FALSE;
		m_nRollDirection = ROLLTEXT_LEFT;
	}

	RollText::~RollText(void)
	{
		pm_->KillTimer(this, ROLLTEXT_ROLL_END);
		pm_->KillTimer(this, ROLLTEXT_TIMERID);
	}

	std::wstring RollText::GetClass() const
	{
		return _T("RollText");
	}

	LPVOID RollText::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("RollText")) == 0 ) return static_cast<RollText*>(this);
		return Label::GetInterface(pstrName);
	}

	void RollText::AttachRollEnd(const EventCallback & cb)
	{
		on_event_[kEventTextRollEnd] += cb;
	}

	void RollText::BeginRoll(int nDirect, LONG lTimeSpan, LONG lMaxTimeLimited)
	{
		m_nRollDirection = nDirect;
		if (m_bUseRoll)
		{
			EndRoll();
		}
		m_nText_W_H = 0;
	
		pm_->KillTimer(this, ROLLTEXT_TIMERID);
		pm_->SetTimer(this, ROLLTEXT_TIMERID, lTimeSpan);
	
		pm_->KillTimer(this, ROLLTEXT_ROLL_END);
		pm_->SetTimer(this, ROLLTEXT_ROLL_END, lMaxTimeLimited*1000);

		m_bUseRoll = TRUE;
	}

	void RollText::EndRoll()
	{
		if (!m_bUseRoll) return;

		pm_->KillTimer(this, ROLLTEXT_ROLL_END);
		pm_->KillTimer(this, ROLLTEXT_TIMERID);
		
		m_bUseRoll = FALSE;
	}

	void RollText::SetPos(RECT rc)
	{
		Label::SetPos(rc);
		m_nText_W_H = 0;			//布局变化重新计算
	}

	void RollText::SetText( std::wstring pstrText )
	{
		Label::SetText(pstrText);
		m_nText_W_H = 0;			//文本变化重新计算
	}

	void RollText::DoEvent(EventArgs& event)
	{
		if (event.type_ == kEventTimer && event.w_param_ == ROLLTEXT_ROLL_END){
			pm_->KillTimer(this, ROLLTEXT_ROLL_END);
			Notify(kEventTextRollEnd);
		}
		else if( event.type_ == kEventTimer && event.w_param_ == ROLLTEXT_TIMERID ) {
			Invalidate();
			return;
		}
		Label::DoEvent(event);
	}

	void RollText::PaintText(HDC hDC)
	{
		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();
		if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();
		DWORD dwTextColor = IsEnabled() ? text_color_ : disabled_text_color_;
		std::wstring sText = GetText();
		if( sText.empty() ) return;
		RECT rcTextPadding = GetTextPadding();
		CDuiRect  rcClient;
		rcClient = rect_;
		rcClient.left += rcTextPadding.left;
		rcClient.right -= rcTextPadding.right;
		rcClient.top += rcTextPadding.top;
		rcClient.bottom -= rcTextPadding.bottom;

		if(m_nText_W_H > 0)
		{
			int nScrollRange = 0;

			if (m_nRollDirection == ROLLTEXT_LEFT || m_nRollDirection == ROLLTEXT_RIGHT) {	//左面移动
				nScrollRange = m_nText_W_H + rcClient.GetWidth();

				rcClient.Offset((m_nRollDirection == ROLLTEXT_LEFT?rcClient.GetWidth():-rcClient.GetWidth()), 0);
				rcClient.Offset((m_nRollDirection == ROLLTEXT_LEFT?-m_nScrollPos:m_nScrollPos), 0);
				rcClient.right += (m_nText_W_H - rcClient.GetWidth());
			} else {																		//上下移动
				nScrollRange = m_nText_W_H + rcClient.GetHeight();

				rcClient.Offset(0, (m_nRollDirection == ROLLTEXT_UP?rcClient.GetHeight():-rcClient.GetHeight()));
				rcClient.Offset(0, (m_nRollDirection == ROLLTEXT_UP?-m_nScrollPos:m_nScrollPos));
				rcClient.bottom += (m_nText_W_H - rcClient.GetHeight());
			}

			m_nScrollPos += m_nStep;
			if (m_nScrollPos > nScrollRange)
			{
				m_nScrollPos = 0;
			}
		}

		RECT rc = rcClient;

		UINT uTextStyle = DT_WORDBREAK | DT_EDITCONTROL;

		if(m_nText_W_H == 0)
		{
			uTextStyle |= DT_CALCRECT;				//第一次计算文本宽度或高度
			if (m_nRollDirection == ROLLTEXT_LEFT || m_nRollDirection == ROLLTEXT_RIGHT) {	//左面移动
				rc.right += 10000;
			} else {																		//上下移动
				rc.bottom += 10000;
			}
		}

		if( show_html_ ) {
			int nLinks = 0;
			RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, dwTextColor, NULL, NULL, nLinks, uTextStyle);
		} else {
			RenderEngine::DrawText(hDC, pm_, rc, sText, dwTextColor, font_, uTextStyle);
		}

		if(m_nText_W_H == 0)
		{
			m_nText_W_H = (m_nRollDirection == ROLLTEXT_LEFT || m_nRollDirection == ROLLTEXT_RIGHT)?(rc.right - rc.left):(rc.bottom - rc.top);		//计算文本宽度或高度
		}
	}
}