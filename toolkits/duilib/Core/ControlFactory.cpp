#include "StdAfx.h"
#include "ControlFactory.h"

namespace ui 
{
	CControlFactory::CControlFactory()
	{
		INNER_REGISTER_DUICONTROL(Control);
		INNER_REGISTER_DUICONTROL(Box);
		INNER_REGISTER_DUICONTROL(Button);
		INNER_REGISTER_DUICONTROL(Combo);
		INNER_REGISTER_DUICONTROL(ComboBox);
		INNER_REGISTER_DUICONTROL(DateTime);
		INNER_REGISTER_DUICONTROL(Edit);
		INNER_REGISTER_DUICONTROL(ActiveX);
		INNER_REGISTER_DUICONTROL(Flash);
		INNER_REGISTER_DUICONTROL(GifAnim);
#ifdef USE_XIMAGE_EFFECT
		INNER_REGISTER_DUICONTROL(GifAnimEx);
#endif
		INNER_REGISTER_DUICONTROL(GroupBox);
		INNER_REGISTER_DUICONTROL(IPAddress);
		INNER_REGISTER_DUICONTROL(IPAddressEx);
		INNER_REGISTER_DUICONTROL(Label);
		INNER_REGISTER_DUICONTROL(List);
		INNER_REGISTER_DUICONTROL(ListHeader);
		INNER_REGISTER_DUICONTROL(ListHeaderItem);
		INNER_REGISTER_DUICONTROL(ListLabelElement);
		INNER_REGISTER_DUICONTROL(ListTextElement);
		INNER_REGISTER_DUICONTROL(ListContainerElement);
		INNER_REGISTER_DUICONTROL(Menu);
		INNER_REGISTER_DUICONTROL(MenuElement);
		INNER_REGISTER_DUICONTROL(Option);
		INNER_REGISTER_DUICONTROL(CheckBox);
		INNER_REGISTER_DUICONTROL(Progress);
		INNER_REGISTER_DUICONTROL(RichEdit);
		INNER_REGISTER_DUICONTROL(ScrollBar);
		INNER_REGISTER_DUICONTROL(Slider);
		INNER_REGISTER_DUICONTROL(Text);
		INNER_REGISTER_DUICONTROL(TreeNode);
		INNER_REGISTER_DUICONTROL(TreeView);
		INNER_REGISTER_DUICONTROL(AnimationTabBox);
		INNER_REGISTER_DUICONTROL(ChildBox);
		INNER_REGISTER_DUICONTROL(HBox);
		INNER_REGISTER_DUICONTROL(TabBox);
		INNER_REGISTER_DUICONTROL(TileBox);
		INNER_REGISTER_DUICONTROL(VBox);
		INNER_REGISTER_DUICONTROL(RollText);
		INNER_REGISTER_DUICONTROL(ColorPalette);
		INNER_REGISTER_DUICONTROL(ListEx);
		INNER_REGISTER_DUICONTROL(ListContainerHeaderItem);
		INNER_REGISTER_DUICONTROL(ListTextExtElement);
		INNER_REGISTER_DUICONTROL(HotKey);
		INNER_REGISTER_DUICONTROL(FadeButton);
		INNER_REGISTER_DUICONTROL(Ring);
	}

	CControlFactory::~CControlFactory()
	{
	}

	Control* CControlFactory::CreateControl(std::wstring strClassName)
	{
		MAP_DUI_CTRATECLASS::iterator iter = m_mapControl.find(strClassName);
		if ( iter == m_mapControl.end()) {
			return NULL;
		}
		else {
			return (Control*) (iter->second());
		}
	}

	void CControlFactory::RegistControl(std::wstring strClassName, CreateClass pFunc)
	{
		m_mapControl.insert(MAP_DUI_CTRATECLASS::value_type(strClassName, pFunc));
	}

	CControlFactory* CControlFactory::GetInstance()  
	{
		static CControlFactory* pInstance = new CControlFactory;
		return pInstance;
	}

	void CControlFactory::Release()
	{
		delete this;
	}
}