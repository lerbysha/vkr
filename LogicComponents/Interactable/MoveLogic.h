#pragma once
#include "../LogicObject.h"
#include "../../Framework/Components/Objects/ObjMovable.h"


class MoveLogic : public LogicObject
{
public:
	COMPONENT(MoveLogic, LogicObject);

	PROP_NAME("move_logic");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, interactive_object);
	PROP_PARAM(Toggle, use_initial_position);

	void disabled() override;

private:
	void on_init() override;
	void on_set_initial_state() override;
	void on_shutdown() override;

	void on_grab_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);
	void on_hold_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);
	void on_throw_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);

	Mat4 default_transform;

	ObjMovable *obj_movable;
	int grab_id;
	int hold_id;
	int throw_id;
};