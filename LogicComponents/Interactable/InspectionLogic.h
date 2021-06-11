#pragma once
#include "../LogicObject.h"
#include "../../Framework/Components/Objects/ObjInspectable.h"


class InspectionLogic : public LogicObject
{
public:
	COMPONENT(InspectionLogic, LogicObject);

	PROP_NAME("inspection_logic");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, inspectable_object);

	void disabled() override;

private:
	void on_init() override;
	void on_shutdown() override;
	void on_set_initial_state() override;

	void on_inspect_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);

	ObjInspectable *obj;
	int handler_id;
};