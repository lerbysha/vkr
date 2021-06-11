#pragma once
#include "../Observer.h"
#include <UnigineGame.h>
#include <UnigineLights.h>


class LightProbeAnimation : public Observer
{
public:
	COMPONENT(LightProbeAnimation, Observer);
	COMPONENT_UPDATE(update);

	PROP_NAME("light_probe_animation");
	PROP_AUTOSAVE(0);

	PROP_ARRAY(Node, probes);
	PROP_PARAM(Float, min_intensity);
	PROP_PARAM(Float, max_intensity);
	PROP_PARAM(Float, duration);

private:
	void update();
	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;

	float time;
	float from_intensity;
	float to_intensity;
};