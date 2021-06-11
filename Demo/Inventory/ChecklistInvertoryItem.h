#pragma once
#include "InventoryItem.h"
#include "../../Framework/Utils.h"
#include "../../LogicComponents/Missions/Mission.h"
#include "../Logic/ChecklistTaskInfo.h"
#include "../LogicController.h"
#include "../Logic/ChecklistItem.h"


class ChecklistInvertoryItem : public InventoryItem
{
public:
	COMPONENT(ChecklistInvertoryItem, InventoryItem);
	COMPONENT_UPDATE(checklist_update);
	COMPONENT_INIT(checklist_init, 2);
	COMPONENT_SHUTDOWN(checklist_shutdown);

	PROP_NAME("checklist_invertory_item");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, mission_info);
	PROP_PARAM(Node, statistics);
	PROP_PARAM(Node, selection_frame);
	PROP_PARAM(Node, good_button);
	PROP_PARAM(Node, bad_button);
	PROP_PARAM(Node, restart_button);
	PROP_PARAM(Node, details_button);
	PROP_PARAM(Node, good_end);
	PROP_PARAM(Node, bad_end);
	PROP_PARAM(Material, good_material);
	PROP_PARAM(Material, bad_material);
	PROP_PARAM(Node, right_glove_pointer);
	PROP_PARAM(Node, left_glove_pointer);
	PROP_PARAM(Node, details_slider_node);
	PROP_PARAM(Node, restart_slider_node);
	PROP_PARAM(Node, good_slider_node);
	PROP_PARAM(Node, bad_slider_node);

	enum ButtonType { Bad, Good, Restart, Details };

	void setMissionInfo(LogicController *logic_controller);
	void setEnabledChecklist(bool state);
	void showAccident(const char *message);
	void showMissionFinised(const char *message);

	template<typename Functor>
	int setOnRestartListener(Functor &&f)
	{
		return on_restart.addListener(std::forward<Functor>(f));
	}

	void removeOnRestartListener(int id)
	{
		on_restart.removeListener(id);
	}

private:
	void checklist_update();
	void checklist_init();
	void checklist_shutdown();

	void pc_update();
	void vr_update();

	ObjectPtr get_hit_object();
	void button_click(ChecklistInvertoryItem::ButtonType type);
	void on_items_state_changed(LogicObject *object, LogicObject::States object_state);
	void show_details();

	void set_slider(ObjectPtr &slider, float scale);

	WorldIntersectionPtr intersection;

	Vector<ChecklistTaskInfo *> tasks_info;
	Vector<ChecklistItem *> checklist_items;
	Vector<int> items_handlers_id;
	int in_progress_index;

	int is_vr_mode;

	enum Masks { GoodButton = 5, BadButton = 6, RestartButton = 7, DetailsButton = 4, ChecklistInteractive = 4 };

	const float button_width = 0.178f;
	const float button_height = 0.058f;
	const float min_button_width = 0.001f;
	const float button_y = -0.001f;

	const float buttons_click_time = 1.0f;
	float buttons_current_time;

	const float restart_click_time = 1.5f;
	float restart_current_time;
	ObjectPtr restart_slider;
	ObjectPtr good_slider;
	ObjectPtr bad_slider;
	ObjectPtr details_slider;

	bool was_clicked;

	ObjectPtr hit_obj;
	int hit_mask;

	Action<void()> on_restart;
};
