#ifndef _DUILIB_TITLEBOX_H__
#define _DUILIB_TITLEBOX_H__

#pragma once

namespace ui
{
	class UILIB_API TileBox : public Box
	{
		DECLARE_DUICONTROL(TileBox)
	public:
		TileBox();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		void SetPos(RECT rc, bool bNeedInvalidate = true);

		SIZE GetItemSize() const;
		void SetItemSize(SIZE szItem);
		int GetColumns() const;
		void SetColumns(int nCols);

		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

	protected:
		SIZE item_size_;
		int columns_;
	};
}
#endif // _DUILIB_TITLEBOX_H__
