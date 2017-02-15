#ifndef __DUILIB_ACTIVEX_H__
#define __DUILIB_ACTIVEX_H__

#pragma once

struct IOleObject;


namespace ui {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class ActiveXCtrl;

	template< class T >
	class SafeRelease
	{
	public:
		SafeRelease(T* p) : p_(p) { };
		~SafeRelease() { if( p_ != NULL ) p_->Release(); };
		T* Detach() { T* t = p_; p_ = NULL; return t; };
		T* p_;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API ActiveX : public Control, public IMessageFilterUI
	{
		DECLARE_DUICONTROL(ActiveX)

		friend class ActiveXCtrl;
	public:
		ActiveX();
		virtual ~ActiveX();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		HWND GetHostWindow() const;

		virtual bool IsDelayCreate() const;
		virtual void SetDelayCreate(bool bDelayCreate = true);
		virtual bool IsMFC() const;
		virtual void SetMFC(bool bMFC = false);

		bool CreateControl(const CLSID clsid);
		bool CreateControl(std::wstring pstrCLSID);
		HRESULT GetControl(const IID iid, LPVOID* ppRet);
		CLSID GetClisd() const;
		std::wstring GetModuleName() const;
		void SetModuleName(std::wstring pstrText);

		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		void DoPaint(HDC hDC, const RECT& rcPaint);

		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	protected:
		virtual void ReleaseControl();
		virtual bool DoCreateControl();

	protected:
		CLSID clsid_;
		std::wstring module_name_;
		bool is_created_;
		bool is_delay_created_;
		bool is_mfc_;
		IOleObject* unkown_ptr_;
		ActiveXCtrl* control_;
		HWND hwnd_host_;
	};

} // namespace ui

#endif // __DUILIB_ACTIVEX_H__
