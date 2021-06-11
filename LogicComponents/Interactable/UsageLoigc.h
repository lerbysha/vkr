#pragma once
#include "../LogicObject.h"
#include "../../Framework/Components/VRInteractable.h"


class UsageLogic : public LogicObject
{
public:
	COMPONENT(UsageLogic, LogicObject);

	PROP_NAME("usage_logic");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, interactive_object);

	void disabled() override;

private:
	void on_init() override;
	void on_shutdown() override;
	void on_set_initial_state() override;

	void on_use_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);

	VRInteractable *obj;
	int handler_id;

	float next_use_time;
	float use_interval = 1;
};