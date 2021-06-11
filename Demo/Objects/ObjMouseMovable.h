#pragma once
#include "../../Framework/Components/Objects/ObjMovable.h"
#include "../../Framework/Utils.h"


class ObjMouseMovable : public ObjMovable
{
public:
	COMPONENT(ObjMouseMovable, ObjMovable);
	COMPONENT_INIT(movable_init);

	PROP_NAME("mouse_movable");
	PROP_AUTOSAVE(0);

	void grabIt(VRPlayer *player, int hand_num) override;
	void holdIt(VRPlayer *player, int hand_num) override;

private:
	void movable_init();

	Math::quat handy_rot_quat;
	Math::Mat4 local_transform;
	float handy_pos_factor = 20;
	float grab_x;
	float grab_y;
	Vec3 offset;
};
