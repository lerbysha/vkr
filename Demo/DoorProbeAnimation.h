#pragma once
#include "../LogicComponents/Observers/Observer.h"
#include <UnigineLights.h>


class DoorProbeAnimation : public Observer
{
public:
	COMPONENT(DoorProbeAnimation, Observer);

	COMPONENT_UPDATE(update);

	PROP_NAME("door_probe_animation");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, door);
	PROP_PARAM(Node, probe);
	PROP_PARAM(Float, min_z_angle);
	PROP_PARAM(Float, max_z_angle);
	PROP_PARAM(Float, min_intensity);
	PROP_PARAM(Float, max_intensity);
	PROP_PARAM(Toggle, use_inverse_angles);

	void disable() override;

private:
	void update();
	void on_init() override;

	LightPtr light;
	float intensity_delta;
	float max_delta_angles;
};