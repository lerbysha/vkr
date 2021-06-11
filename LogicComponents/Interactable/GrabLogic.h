#pragma once
#include "../LogicObject.h"
#include "../../Framework/Components/VRInteractable.h"


class GrabLogic : public LogicObject
{
public:
	COMPONENT(GrabLogic, LogicObject);

	PROP_NAME("grab_logic");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, interactive_object);

	void disabled() override;

private:
	void on_init() override;
	void on_shutdown() override;
	void on_set_initial_state() override;

	void on_grab_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);
	void on_throw_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);

	VRInteractable *obj;
	int grab_handler_id;
	int throw_handler_id;

	//float next_use_time;
	//float use_interval = 1;
};