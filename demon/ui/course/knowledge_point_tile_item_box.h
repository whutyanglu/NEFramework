#ifndef MSYK_UI_KNOWLEDGE_POINT_TILE_BOX_H__
#define MSYK_UI_KNOWLEDGE_POINT_TILE_BOX_H__

#include "dynamic_box.h"

class KnowledgePointTileItemBox;

using OnClick = std::function<void(KnowledgePointTileItemBox*, bool)>;
using OnHover = std::function<void(KnowledgePointTileItemBox*)>;
using OnLeave = std::function<void(KnowledgePointTileItemBox*)>;

class KnowledgePointTileItemBox : public DynamicBox
{
public:
	KnowledgePointTileItemBox(int width = 0, int height = 0, OnClick = nullptr, OnHover = nullptr, OnLeave = nullptr);

	virtual void OnInit() override;

	void SetWidth(int width) {width_ = width;}
	void SetHeight(int height) {height_ = height;}

private:
	enum state
	{
		state_normal,
		state_hot,
		state_selected,
	};

private:
	bool is_mouse_enter_ = false;
	bool is_button_down_ = false;
	int width_ = 0;
	int height_ = 0;
	state state_ = state_normal;
	ui::Control *control_selected_ = NULL;
	OnClick on_click_;
	OnHover on_hover_;
	OnLeave on_leave_;
};


#endif // MSYK_UI_KNOWLEDGE_POINT_TILE_BOX_H__