#ifndef __UIROTATE_H__
#define __UIROTATE_H__

#pragma once

namespace ui
{
	class Ring : public Label
	{
		enum
		{ 
			RING_TIMERID = 100,
		};
		DECLARE_DUICONTROL(Ring)
	public:
		Ring();
		~Ring();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void SetBkImage(std::wstring pStrImage);	
		virtual void DoEvent(EventArgs& event);
		virtual void PaintBkImage(HDC hDC);	

	private:
		void InitImage();
		void DeleteImage();

	public:
		float m_fCurAngle;
		Gdiplus::Image* m_pBkimage;
	};
}

#endif // __UIROTATE_H__