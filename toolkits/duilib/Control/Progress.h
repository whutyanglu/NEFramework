#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#pragma once

namespace ui
{
	class UILIB_API Progress : public Label
	{
		DECLARE_DUICONTROL(Progress)
	public:
		Progress();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		bool IsShowText();
		void SetShowText(bool bShowText = true);
		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		bool IsStretchForeImage();
		void SetStretchForeImage(bool bStretchForeImage = true);
		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void PaintForeColor(HDC hDC);
		void PaintForeImage(HDC hDC);
		virtual void UpdateText();

	protected:
		bool m_bShowText;
		bool m_bHorizontal;
		bool m_bStretchForeImage;
		int m_nMax;
		int m_nMin;
		int m_nValue;

		std::wstring m_sForeImageModify;
	};

} // namespace DuiLib

#endif // __UIPROGRESS_H__
