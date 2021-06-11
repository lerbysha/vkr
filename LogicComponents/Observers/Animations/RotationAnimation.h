#pragma once
#include "../Observer.h"
#include <UnigineGame.h>


class RotationAnimation : public Observer
{
public:
	COMPONENT(RotationAnimation, Observer);
	COMPONENT_UPDATE(update);

	PROP_NAME("rotation_animation");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Toggle, hierarchy_based);
	PROP_ARRAY(Node, nodes);
	PROP_PARAM(Toggle, loop);
	PROP_PARAM(Float, velocity);
	PROP_PARAM(Toggle, axis_x);
	PROP_PARAM(Toggle, axis_y);
	PROP_PARAM(Toggle, axis_z);
	PROP_PARAM(Float, duration);
	PROP_PARAM(Vec3, initial_rotation);
	PROP_PARAM(Vec3, final_rotation);

private:
	void update();

	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;

	vec3 from_rotation;
	vec3 to_rotation;
	float time;

	vec3 delta_rotation;
	int function_state;
};