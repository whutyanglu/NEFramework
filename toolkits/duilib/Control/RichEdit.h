#ifndef __UIRICHEDIT_H__
#define __UIRICHEDIT_H__

#pragma once
#include <Imm.h>
#pragma comment(lib,"imm32.lib")

namespace ui {

	class TxtWinHost;

	class UILIB_API RichEdit : public Box, public IMessageFilterUI
	{
	DECLARE_DUICONTROL(RichEdit)
	public:
		RichEdit();
		~RichEdit();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		void AttachReturn(const EventCallback &cb);
		void AttachTextChange(const EventCallback &cb);

		bool IsMultiLine();
		void SetMultiLine(bool bMultiLine);
		bool IsWantTab();
		void SetWantTab(bool bWantTab = true);
		bool IsWantReturn();
		void SetWantReturn(bool bWantReturn = true);
		bool IsWantCtrlReturn();
		void SetWantCtrlReturn(bool bWantCtrlReturn = true);
		bool IsRich();
		void SetRich(bool bRich = true);
		bool IsReadOnly();
		void SetReadOnly(bool bReadOnly = true);
		bool GetWordWrap();
		void SetWordWrap(bool bWordWrap = true);
		int GetFont();
		void SetFont(int index);
		void SetFont(std::wstring pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
		void SetEnabled(bool bEnabled);
		LONG GetWinStyle();
		void SetWinStyle(LONG lStyle);
		DWORD GetTextColor();
		void SetTextColor(DWORD dwTextColor);
		int GetLimitText();
		void SetLimitText(int iChars);
		long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
		std::wstring GetText() const;
		void SetText(std::wstring pstrText);
		bool GetModify() const;
		void SetModify(bool bModified = true) const;
		void GetSel(CHARRANGE &cr) const;
		void GetSel(long& nStartChar, long& nEndChar) const;
		int SetSel(CHARRANGE &cr);
		int SetSel(long nStartChar, long nEndChar);
		void ReplaceSel(std::wstring lpszNewText, bool bCanUndo);
		void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
		std::wstring GetSelText() const;
		int SetSelAll();
		int SetSelNone();
		WORD GetSelectionType() const;
		bool GetZoom(int& nNum, int& nDen) const;
		bool SetZoom(int nNum, int nDen);
		bool SetZoomOff();
		bool GetAutoURLDetect() const;
		bool SetAutoURLDetect(bool bAutoDetect = true);
		DWORD GetEventMask() const;
		DWORD SetEventMask(DWORD dwEventMask);
		std::wstring GetTextRange(long nStartChar, long nEndChar) const;
		void HideSelection(bool bHide = true, bool bChangeStyle = false);
		void ScrollCaret();
		int InsertText(long nInsertAfterChar, std::wstring lpstrText, bool bCanUndo = false);
		int AppendText(std::wstring lpstrText, bool bCanUndo = false);
		DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
		bool SetDefaultCharFormat(CHARFORMAT2 &cf);
		DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
		bool SetSelectionCharFormat(CHARFORMAT2 &cf);
		bool SetWordCharFormat(CHARFORMAT2 &cf);
		DWORD GetParaFormat(PARAFORMAT2 &pf) const;
		bool SetParaFormat(PARAFORMAT2 &pf);
		bool CanUndo();
		bool CanRedo();
		bool CanPaste();
		bool Redo();
		bool Undo();
		void Clear();
		void Copy();
		void Cut();
		void Paste();
		int GetLineCount() const;
		std::wstring GetLine(int nIndex, int nMaxLength) const;
		int LineIndex(int nLine = -1) const;
		int LineLength(int nLine = -1) const;
		bool LineScroll(int nLines, int nChars = 0);
		long LineFromChar(long nIndex) const;
		CDuiPoint PosFromChar(UINT nChar) const;
		int CharFromPos(CDuiPoint pt) const;
		void EmptyUndoBuffer();
		UINT SetUndoLimit(UINT nLimit);
		long StreamIn(int nFormat, EDITSTREAM &es);
		long StreamOut(int nFormat, EDITSTREAM &es);
		void SetAccumulateDBCMode(bool bDBCMode);
		bool IsAccumulateDBCMode();

		void DoInit();
		// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
		// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
		bool SetDropAcceptFile(bool bAccept);
		virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
		IDropTarget* GetTxDropTarget();
		virtual bool OnTxViewChanged(BOOL bUpdate);
		virtual void OnTxNotify(DWORD iNotify, void *pv);

		void SetScrollPos(SIZE szPos, bool bMsg = true);
		void LineUp();
		void LineDown();
		void PageUp();
		void PageDown();
		void HomeUp();
		void EndDown();
		void LineLeft();
		void LineRight();
		void PageLeft();
		void PageRight();
		void HomeLeft();
		void EndRight();

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		void DoEvent(EventArgs& event);
		void DoPaint(HDC hDC, const RECT& rcPaint);

		std::wstring GetNormalImage();
		void SetNormalImage(std::wstring pStrImage);
		std::wstring GetHotImage();
		void SetHotImage(std::wstring pStrImage);
		std::wstring GetFocusedImage();
		void SetFocusedImage(std::wstring pStrImage);
		std::wstring GetDisabledImage();
		void SetDisabledImage(std::wstring pStrImage);
		void PaintStatusImage(HDC hDC);
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);

		void SetTipValue(std::wstring pStrTipValue);
		std::wstring GetTipValue();
		void SetTipValueColor(std::wstring pStrColor);
		DWORD GetTipValueColor();
		void SetTipValueAlign(UINT uAlign);
		UINT GetTipValueAlign();

		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	protected:
		TxtWinHost* win_host_;
		bool is_vscrollbar_fixing_;
		bool want_tab_;
		bool want_return_;
		bool want_ctrl_return_;
		bool rich_;
		bool read_only_;
		bool word_wrap_;
		DWORD text_color_;
		int font_;
		int text_limit_;
		LONG twh_style_;
		bool draw_caret_;
		bool inited_;
		bool  accumulate_dbc_ ; // TRUE - need to cumulate ytes from 2 WM_CHAR msgs
		// we are in this mode when we receive VK_PROCESSKEY
		UINT lead_byte_; // use when we are in _fAccumulateDBC mode

		UINT btn_state_;
		std::wstring normal_image_;
		std::wstring hot_image_;
		std::wstring focused_image_;
		std::wstring disabled_image_;
		RECT text_padding_rect_;
		std::wstring tip_;
		DWORD tip_color_;
		UINT tip_align_;
	};

} // namespace DuiLib

#endif // __UIRICHEDIT_H__
