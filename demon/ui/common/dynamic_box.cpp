#include "stdafx.h"
#include "dynamic_box.h"
#include "shared/util.h"

DynamicBox::DynamicBox()
{
	id_ = QString::GetGUID();
}

DynamicBox::~DynamicBox()
{
}

void DynamicBox::Init(const std::wstring & xml, ui::PaintManager * pm, ui::Box * parent, ui::ControlBuilderCallback cb)
{
	pm_ = pm;
 	ui::Box *box = dynamic_cast<ui::Box *>(ui::GlobalManager::CreateFromXml(xml, pm, parent, cb));
	if (box) {
		box_ = box;
		OnInit();
	}
}

void DynamicBox::OnInit()
{
}

void DynamicBox::SetVisible(bool visible)
{
	box_->SetVisible(visible);
}

ui::Control * DynamicBox::FindSubContrl(const std::wstring & name)
{
	if (box_) {
		return box_->FindSubControl(name);
	}

	return NULL;
}
