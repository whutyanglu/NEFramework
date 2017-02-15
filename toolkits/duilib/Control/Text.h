#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace ui
{
	class UILIB_API Text : public Label
	{
		DECLARE_DUICONTROL(Text)
	public:
		Text();
		~Text();

		std::wstring GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(std::wstring pstrName);

		void AttachCustomLinkClick(const EventCallback &cb);

		std::wstring* GetLinkContent(int iIndex);

		void DoEvent(EventArgs& event);
		SIZE EstimateSize(SIZE szAvailable);

		void PaintText(HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int link_id_;
		RECT link_rects_[MAX_LINK];
		std::wstring links_[MAX_LINK];
		int hover_link_;
	};

} // namespace DuiLib

#endif //__UITEXT_H__