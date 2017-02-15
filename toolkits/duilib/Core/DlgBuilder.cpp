#include "StdAfx.h"

namespace ui {

	Control* DialogBuilder::Create(STRINGorID xml, std::wstring type,
		PaintManager* pManager, Control* pParent, ControlBuilderCallback pCallback)
	{
		//资源ID为0-65535，两个字节；字符串指针为4个字节
		//字符串以<开头认为是XML字符串，否则认为是XML文件
		if (HIWORD(xml.m_lpstr) != NULL && *(xml.m_lpstr) != _T('<')) {
			std::wstring xmlpath = ResourceManager::GetInstance()->GetXmlPath(xml.m_lpstr);
			if (!xmlpath.empty()) {
				xml = xmlpath.c_str();
			}
		}

		if (HIWORD(xml.m_lpstr) != NULL) {
			if (*(xml.m_lpstr) == _T('<')) {
				if (!xml_.Load(xml.m_lpstr)) return NULL;
			}
			else {
				if (!xml_.LoadFromFile(xml.m_lpstr)) return NULL;
			}
		}
		else {
			HINSTANCE dll_instence = NULL;
			if (instance_)
			{
				dll_instence = instance_;
			}
			else
			{
				dll_instence = PaintManager::GetResourceDll();
			}
			HRSRC hResource = ::FindResource(dll_instence, xml.m_lpstr, type.c_str());
			if (hResource == NULL) return NULL;
			HGLOBAL hGlobal = ::LoadResource(dll_instence, hResource);
			if (hGlobal == NULL) {
				FreeResource(hResource);
				return NULL;
			}

			callback_ = pCallback;
			if (!xml_.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(dll_instence, hResource))) return NULL;
			::FreeResource(hResource);
			type_ = type;
		}

		return Create(pCallback, pManager, pParent);
	}

	Control* DialogBuilder::Create(ControlBuilderCallback pCallback, PaintManager* pManager, Control* pParent)
	{
		callback_ = pCallback;
		MarkupNode root = xml_.GetRoot();
		if( !root.IsValid() ) return NULL;

		if( pManager ) {
			LPCTSTR pstrClass = NULL;
			int nAttributes = 0;
			LPCTSTR pstrName = NULL;
			LPCTSTR pstrValue = NULL;
			LPTSTR pstr = NULL;
			for( MarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
				pstrClass = node.GetName();
				if( _tcsicmp(pstrClass, _T("Image")) == 0 ) {
					nAttributes = node.GetAttributeCount();
					LPCTSTR pImageName = NULL;
					LPCTSTR pImageResType = NULL;
					bool shared = false;
					DWORD mask = 0;
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);
						if( _tcsicmp(pstrName, _T("name")) == 0 ) {
							pImageName = pstrValue;
						}
						else if( _tcsicmp(pstrName, _T("restype")) == 0 ) {
							pImageResType = pstrValue;
						}
						else if( _tcsicmp(pstrName, _T("mask")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							mask = _tcstoul(pstrValue, &pstr, 16);
						}
						else if( _tcsicmp(pstrName, _T("shared")) == 0 ) {
							shared = (_tcsicmp(pstrValue, _T("true")) == 0);
						}
					}
					if( pImageName ) pManager->AddImage(pImageName, pImageResType, mask, false, shared);
				}
				else if( _tcsicmp(pstrClass, _T("Default")) == 0 ) {
					nAttributes = node.GetAttributeCount();
					LPCTSTR pControlName = NULL;
					LPCTSTR pControlValue = NULL;
					bool shared = false;
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);
						if( _tcsicmp(pstrName, _T("name")) == 0 ) {
							pControlName = pstrValue;
						}
						else if( _tcsicmp(pstrName, _T("value")) == 0 ) {
							pControlValue = pstrValue;
						}
						else if( _tcsicmp(pstrName, _T("shared")) == 0 ) {
							shared = (_tcsicmp(pstrValue, _T("true")) == 0);
						}
					}
					if( pControlName ) {
						pManager->AddDefaultAttributeList(pControlName, pControlValue, shared);
					}
				}
			}

			pstrClass = root.GetName();
			if( _tcsicmp(pstrClass, _T("Window")) == 0 ) {
				if( pManager->GetPaintWindow() ) {
					int nAttributes = root.GetAttributeCount();
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = root.GetAttributeName(i);
						pstrValue = root.GetAttributeValue(i);
						if( _tcsicmp(pstrName, _T("size")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetInitSize(pManager->GetDPIObj()->Scale(cx), pManager->GetDPIObj()->Scale(cy));
						} 
						else if( _tcsicmp(pstrName, _T("sizebox")) == 0 ) {
							RECT rcSizeBox = { 0 };
							LPTSTR pstr = NULL;
							rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
							rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
							rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
							pManager->SetSizeBox(rcSizeBox);
						}
						else if( _tcsicmp(pstrName, _T("caption")) == 0 ) {
							RECT rcCaption = { 0 };
							LPTSTR pstr = NULL;
							rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
							rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
							rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
							pManager->SetCaptionRect(rcCaption);
						}
						else if( _tcsicmp(pstrName, _T("roundcorner")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetRoundCorner(cx, cy);
						} 
						else if( _tcsicmp(pstrName, _T("mininfo")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetMinInfo(cx, cy);
						}
						else if( _tcsicmp(pstrName, _T("maxinfo")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetMaxInfo(cx, cy);
						}
						else if( _tcsicmp(pstrName, _T("showdirty")) == 0 ) {
							pManager->SetShowUpdateRect(_tcsicmp(pstrValue, _T("true")) == 0);
						} 
						else if( _tcsicmp(pstrName, _T("opacity")) == 0 || _tcsicmp(pstrName, _T("alpha")) == 0 ) {
							pManager->SetOpacity(_ttoi(pstrValue));
						} 
						else if( _tcscmp(pstrName, _T("layeredopacity")) == 0 ) {
							pManager->SetLayeredOpacity(_ttoi(pstrValue));
						} 
						else if( _tcscmp(pstrName, _T("layered")) == 0 || _tcscmp(pstrName, _T("bktrans")) == 0) {
							pManager->SetLayered(_tcsicmp(pstrValue, _T("true")) == 0);
						}	
						else if( _tcsicmp(pstrName, _T("shadowsize")) == 0 ) {
							pManager->GetShadow()->SetSize(_ttoi(pstrValue));
						}
						else if( _tcsicmp(pstrName, _T("shadowsharpness")) == 0 ) {
							pManager->GetShadow()->SetSharpness(_ttoi(pstrValue));
						}
						else if( _tcsicmp(pstrName, _T("shadowdarkness")) == 0 ) {
							pManager->GetShadow()->SetDarkness(_ttoi(pstrValue));
						}
						else if( _tcsicmp(pstrName, _T("shadowposition")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->GetShadow()->SetPosition(cx, cy);
						}
						else if( _tcsicmp(pstrName, _T("shadowcolor")) == 0 ) {
							pManager->GetShadow()->SetColor(GlobalManager::GetColor(pstrValue));
						}
						else if( _tcsicmp(pstrName, _T("shadowcorner")) == 0 ) {
							RECT rcCorner = { 0 };
							LPTSTR pstr = NULL;
							rcCorner.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
							rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
							rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
							pManager->GetShadow()->SetShadowCorner(rcCorner);
						}
						else if( _tcsicmp(pstrName, _T("shadowimage")) == 0 ) {
							pManager->GetShadow()->SetImage(pstrValue);
						}
						else if( _tcsicmp(pstrName, _T("showshadow")) == 0 ) {
							pManager->GetShadow()->ShowShadow(_tcsicmp(pstrValue, _T("true")) == 0);
						} 
						else if( _tcsicmp(pstrName, _T("gdiplustext")) == 0 ) {
							pManager->SetUseGdiplusText(_tcsicmp(pstrValue, _T("true")) == 0);
						} 
						else if( _tcsicmp(pstrName, _T("textrenderinghint")) == 0 ) {
							pManager->SetGdiplusTextRenderingHint(_ttoi(pstrValue));
						} 
					}
				}
			}
		}
		return _Parse(&root, pParent, pManager);
	}

	Markup* DialogBuilder::GetMarkup()
	{
		return &xml_;
	}

	std::wstring DialogBuilder::GetLastErrorMessage() const
	{
		return xml_.GetLastErrorMessage();
	}

	std::wstring DialogBuilder::GetLastErrorLocation() const
	{
		return xml_.GetLastErrorLocation();
	}

	Control* DialogBuilder::_Parse(MarkupNode* pRoot, Control* pParent, PaintManager* pManager)
	{
		IBox* pContainer = NULL;
		Control* pReturn = NULL;
		for( MarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() ) {
			LPCTSTR pstrClass = node.GetName();
			if( _tcsicmp(pstrClass, _T("Image")) == 0 || _tcsicmp(pstrClass, _T("Font")) == 0 \
				|| _tcsicmp(pstrClass, _T("Default")) == 0 || _tcsicmp(pstrClass, _T("Style")) == 0 ) continue;

			Control* pControl = NULL;
			if( _tcsicmp(pstrClass, _T("Include")) == 0 ) {
				if( !node.HasAttributes() ) continue;
				int count = 1;
				LPTSTR pstr = NULL;
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				if ( node.GetAttributeValue(_T("count"), szValue, cchLen) )
					count = _tcstol(szValue, &pstr, 10);
				cchLen = lengthof(szValue) - 1;
				if ( !node.GetAttributeValue(_T("source"), szValue, cchLen) ) continue;
				for ( int i = 0; i < count; i++ ) {
					DialogBuilder builder;
					if( !type_.empty() ) { // 使用资源dll，从资源中读取
						WORD id = (WORD)_tcstol(szValue, &pstr, 10); 
						pControl = builder.Create((UINT)id, type_, pManager, pParent, callback_);
					}
					else {
						pControl = builder.Create(szValue, L"", pManager, pParent, callback_);
					}
				}
				continue;
			}
			else {
				pControl = dynamic_cast<Control*>(CControlFactory::GetInstance()->CreateControl(pstrClass));

				// 检查插件
				if( pControl == NULL ) {
					CStdPtrArray* pPlugins = PaintManager::GetPlugins();
					LPCREATECONTROL lpCreateControl = NULL;
					for( int i = 0; i < pPlugins->GetSize(); ++i ) {
						lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);
						if( lpCreateControl != NULL ) {
							pControl = lpCreateControl(pstrClass);
							if( pControl != NULL ) break;
						}
					}
				}
				// 回掉创建
				if( pControl == NULL && callback_) {
					pControl = callback_(pstrClass);
				}
			}

			if( pControl == NULL ) {
				continue;
			}

			// Add children
			if( node.HasChildren() ) {
				_Parse(&node, pControl, pManager);
			}

			// Attach to parent
			// 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
			TreeView* pTreeView = NULL;
			if( pParent != NULL && pControl != NULL ) {
				TreeNode* pParentTreeNode = static_cast<TreeNode*>(pParent->GetInterface(_T("TreeNode")));
				TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(_T("TreeNode")));
				pTreeView = static_cast<TreeView*>(pParent->GetInterface(_T("TreeView")));

				// TreeNode子节点
				if(pTreeNode != NULL) {
					if(pParentTreeNode) {
						pTreeView = pParentTreeNode->GetTreeView();

						// 计算树层数
						int level = 0;
						MarkupNode pNode = node.GetParent();
						for (; _tcsicmp(pNode.GetName(), L"TreeNode") == 0 || _tcsicmp(pNode.GetName(), L"DirectoryTreeNode") == 0;
							pNode = pNode.GetParent())
						{
							++level;
						}
						pTreeNode->SetTreeLevel(level);

						if(!pParentTreeNode->Add(pTreeNode)) {
							delete pTreeNode;
							pTreeNode = NULL;
							continue;
						}
					}
					else {
						if(pTreeView != NULL) {
							if(!pTreeView->Add(pTreeNode)) {
								delete pTreeNode;
								pTreeNode = NULL;
								continue;
							}
						}
					}
				}
				// TreeNode子控件
				else if(pParentTreeNode != NULL) {
					pParentTreeNode->GetTreeNodeHoriznotal()->Add(pControl);
				}
				// 普通控件
				else {
					if( pContainer == NULL ) pContainer = static_cast<IBox*>(pParent->GetInterface(_T("IBox")));
					ASSERT(pContainer);
					if( pContainer == NULL ) return NULL;
					if( !pContainer->Add(pControl) ) {
						delete pControl;
						continue;
					}
				}
			}
			if( pControl == NULL ) continue;

			// Init default attributes
			if( pManager ) {
				if(pTreeView != NULL) {
					pControl->SetManager(pManager, pTreeView, true);
				}
				else {
					// TODO : luyang 
					/*pControl->SetManager(pManager, NULL, false);*/
					pControl->SetManager(pManager, pParent, false);
				}
				std::wstring pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
				if( !pDefaultAttributes.empty() ) {
					pControl->ApplyAttributeList(pDefaultAttributes);
				}
			}
			
			// Process attributes
			if (node.HasAttributes()) {
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				for (int i = 0; i < nAttributes; i++) {
					pControl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
				}
			}

			// TODO : luyang
			/*if( pManager ) {
				if(pTreeView == NULL) {
					pControl->SetManager(NULL, NULL, false);
				}
			}*/
			// Return first item
			if( pReturn == NULL ) pReturn = pControl;
		}
		return pReturn;
	}

} // namespace DuiLib
