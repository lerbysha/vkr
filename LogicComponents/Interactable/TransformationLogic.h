#pragma once
#include "../LogicObject.h"
#include "../../Framework/Components/VRInteractable.h"
#include "../../Framework/SoundManager.h"
#include "../../Framework/Components/Players/VRPlayerVR.h"
#include "../../Framework/Components/Objects/ObjMovable.h"
#include "../../Framework/Components/Objects/ObjSelectable.h"


class TransformationLogic : public LogicObject
{
public:
	COMPONENT(TransformationLogic, LogicObject);
	COMPONENT_UPDATE(update);

	PROP_NAME("transformation_logic");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, object);
	PROP_PARAM(Toggle, use_position);
	PROP_PARAM(Vec3, min_position);
	PROP_PARAM(Vec3, max_position);
	PROP_PARAM(Toggle, use_rotation);
	PROP_PARAM(Vec3, min_rotation);
	PROP_PARAM(Vec3, max_rotation);
	PROP_PARAM(Vec3, initial_position);
	PROP_PARAM(Vec3, initial_rotation);
	PROP_PARAM(Vec3, final_position);
	PROP_PARAM(Vec3, final_rotation);
	PROP_PARAM(Toggle, use_vr_interpolation);
	PROP_PARAM(Float, pc_interpolation_time);
	PROP_PARAM(File, initial_sound);
	PROP_PARAM(File, final_sound);

private:
	void on_init() override;
	void on_set_initial_state() override;
	void on_shutdown() override;

	void on_grab_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);
	void on_throw_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);

	ObjMovable *movable;

	int is_vr_mode;
	VRPlayer *vr_player;
	int hand_num;
	int grab_handler_id;
	int throw_handler_id;

	Vec3 to_pos;
	Vec3 from_pos;
	quat to_rot;
	quat from_rot;
	float current_time;

	void update();
	void update_state();

	bool is_hold;
	bool interpolation_completed;
	const float speed_reaction = 5;
	VRInteractable *interactable;
};