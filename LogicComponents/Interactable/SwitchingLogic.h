#pragma once
#include "../LogicObject.h"
#include "../../Framework/Components/Objects/ObjSwitch.h"


class SwitchingLogic : public LogicObject
{
public:
	COMPONENT(SwitchingLogic, LogicObject);

	PROP_NAME("switching_logic");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, switch_node);
	PROP_PARAM(Toggle, activated);

	void disabled() override;

private:
	void on_init() override;
	void on_shutdown() override;
	void on_set_initial_state() override;

	void on_use_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);

	ObjSwitch *obj_switch;
	int handler_id;
};