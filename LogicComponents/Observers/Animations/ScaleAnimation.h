#pragma once
#include "../Observer.h"
#include <UnigineGame.h>


class ScaleAnimation : public Observer
{
public:
	COMPONENT(ScaleAnimation, Observer);
	COMPONENT_UPDATE(update);

	PROP_NAME("scale_animation");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Toggle, hierarchy_based);
	PROP_ARRAY(Node, nodes);
	PROP_PARAM(Float, duration);
	PROP_PARAM(Vec3, initial_scale);
	PROP_PARAM(Vec3, final_scale);

private:
	void update();

	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;

	vec3 from_scale;
	vec3 to_scale;
	float time;
};