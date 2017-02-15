#include "StdAfx.h"
#include "Flash.h"
#include <atlcomcli.h>

#define DISPID_FLASHEVENT_FLASHCALL	 ( 0x00C5 )
#define DISPID_FLASHEVENT_FSCOMMAND	 ( 0x0096 )
#define DISPID_FLASHEVENT_ONPROGRESS	( 0x07A6 )

namespace ui
{
	IMPLEMENT_DUICONTROL(Flash)

	Flash::Flash(void)
		: ref_(0)
		, cookie_(0)
		, m_pFlash(NULL)
		, event_handler_(NULL)
	{
		std::wstring strFlashCLSID=_T("{D27CDB6E-AE6D-11CF-96B8-444553540000}");
		OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
		::MultiByteToWideChar(::GetACP(), 0, strFlashCLSID, -1, szCLSID, lengthof(szCLSID) - 1);
#else
		_tcsncpy(szCLSID, strFlashCLSID.c_str(), lengthof(szCLSID) - 1);
#endif
		::CLSIDFromString(szCLSID, &clsid_);
	}

	Flash::~Flash(void)
	{
		if (event_handler_)
		{
			event_handler_->Release();
			event_handler_=NULL;
		}
		ReleaseControl();
	}

	std::wstring Flash::GetClass() const
	{
		return DUI_CTR_FLASH;
	}

	LPVOID Flash::GetInterface(std::wstring pstrName )
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_FLASH) == 0 ) return static_cast<Flash*>(this);
		return ActiveX::GetInterface(pstrName);
	}

	HRESULT STDMETHODCALLTYPE Flash::GetTypeInfoCount( __RPC__out UINT *pctinfo )
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Flash::GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo )
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Flash::GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId )
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Flash::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
	{

		return S_OK;
		switch(dispIdMember)
		{
		case DISPID_FLASHEVENT_FLASHCALL:
			{
				if (pDispParams->cArgs != 1 || pDispParams->rgvarg[0].vt != VT_BSTR) 
					return E_INVALIDARG;
				return this->FlashCall(pDispParams->rgvarg[0].bstrVal);
			}
		case DISPID_FLASHEVENT_FSCOMMAND:
			{
				if( pDispParams && pDispParams->cArgs == 2 )
				{
					if( pDispParams->rgvarg[0].vt == VT_BSTR &&
						pDispParams->rgvarg[1].vt == VT_BSTR )
					{
						return FSCommand(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
					}
					else
					{
						return DISP_E_TYPEMISMATCH;
					}
				}
				else
				{
					return DISP_E_BADPARAMCOUNT;
				}
			}
		case DISPID_FLASHEVENT_ONPROGRESS:
			{
				return OnProgress(*pDispParams->rgvarg[0].plVal);
			}
		case DISPID_READYSTATECHANGE:
			{
				return this->OnReadyStateChange(pDispParams->rgvarg[0].lVal);
			}
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Flash::QueryInterface( REFIID riid, void **ppvObject )
	{
		*ppvObject = NULL;

		if( riid == IID_IUnknown)
			*ppvObject = static_cast</*IOleWindow**/LPUNKNOWN>(this);
		else if( riid == IID_IDispatch)
			*ppvObject = static_cast<IDispatch*>(this);
		else if( riid ==  __uuidof(_IShockwaveFlashEvents))
			*ppvObject = static_cast<_IShockwaveFlashEvents*>(this);

		if( *ppvObject != NULL )
			AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}

	ULONG STDMETHODCALLTYPE Flash::AddRef( void )
	{
		::InterlockedIncrement(&ref_); 
		return ref_;
	}

	ULONG STDMETHODCALLTYPE Flash::Release( void )
	{
		::InterlockedDecrement(&ref_);
		return ref_;
	}

	HRESULT Flash::OnReadyStateChange (long newState)
	{
		if (event_handler_)
		{
			return event_handler_->OnReadyStateChange(newState);
		}
		return S_OK;
	}

	HRESULT Flash::OnProgress(long percentDone )
	{
		if (event_handler_)
		{
			return event_handler_->OnProgress(percentDone);
		}
		return S_OK;
	}

	HRESULT Flash::FSCommand (_bstr_t command, _bstr_t args)
	{
		if (event_handler_)
		{
			return event_handler_->FSCommand((LPCTSTR)command, (LPCTSTR)args);
		}
		return S_OK;
	}

	HRESULT Flash::FlashCall( _bstr_t request )
	{
		if (event_handler_)
		{
			return event_handler_->FlashCall((LPCTSTR)request);
		}
		return S_OK;
	}

	void Flash::ReleaseControl()
	{
		//GetManager()->RemoveTranslateAccelerator(this);
		RegisterEventHandler(FALSE);
		if (m_pFlash)
		{
			m_pFlash->Release();
			m_pFlash=NULL;
		}
	}

	bool Flash::DoCreateControl()
	{
		if (!ActiveX::DoCreateControl())
			return false;
		//GetManager()->AddTranslateAccelerator(this);
		GetControl(__uuidof(IShockwaveFlash),(LPVOID*)&m_pFlash);
		RegisterEventHandler(TRUE);
		return true;
	}

	void Flash::SetFlashEventHandler( CFlashEventHandler* pHandler )
	{
		if (event_handler_!=NULL)
		{
			event_handler_->Release();
		}
		if (pHandler==NULL)
		{
			event_handler_=pHandler;
			return;
		}
		event_handler_=pHandler;
		event_handler_->AddRef();
	}

	LRESULT Flash::TranslateAccelerator( MSG *pMsg )
	{
		if(pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST)
			return S_FALSE;

		if( m_pFlash == NULL )
			return E_NOTIMPL;

		// 当前Web窗口不是焦点,不处理加速键
		BOOL bIsChild = FALSE;
		HWND hTempWnd = NULL;
		HWND hWndFocus = ::GetFocus();

		hTempWnd = hWndFocus;
		while(hTempWnd != NULL)
		{
			if(hTempWnd == hwnd_host_)
			{
				bIsChild = TRUE;
				break;
			}
			hTempWnd = ::GetParent(hTempWnd);
		}
		if(!bIsChild)
			return S_FALSE;

		CComPtr<IOleInPlaceActiveObject> pObj;
		if (FAILED(m_pFlash->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID *)&pObj)))
			return S_FALSE;

		HRESULT hResult = pObj->TranslateAccelerator(pMsg);
		return hResult;
	}

	HRESULT Flash::RegisterEventHandler( BOOL inAdvise )
	{
		if (m_pFlash==NULL)
			return S_FALSE;

		HRESULT hr=S_FALSE;
		CComPtr<IConnectionPointContainer>  pCPC;
		CComPtr<IConnectionPoint> pCP;
		
		hr=m_pFlash->QueryInterface(IID_IConnectionPointContainer,(void **)&pCPC);
		if (FAILED(hr))
			return hr;
		hr=pCPC->FindConnectionPoint(__uuidof(_IShockwaveFlashEvents),&pCP);
		if (FAILED(hr))
			return hr;

		if (inAdvise)
		{
			hr = pCP->Advise((IDispatch*)this, &cookie_);
		}
		else
		{
			hr = pCP->Unadvise(cookie_);
		}
		return hr; 
	}

};