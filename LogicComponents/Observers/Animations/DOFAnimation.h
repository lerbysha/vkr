#pragma once
#include "../Observer.h"
#include <UnigineRender.h>
#include <UnigineGame.h>


class DOFAnimation : public Observer
{
public:
	COMPONENT(DOFAnimation, Observer);
	COMPONENT_POST_UPDATE(render);

	PROP_NAME("dof_animation");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Float, initial_focal_distance);
	PROP_PARAM(Float, final_focal_distance);
	PROP_PARAM(Float, initial_blur);
	PROP_PARAM(Float, final_blur);
	PROP_PARAM(Float, duration);
	PROP_PARAM(Toggle, add_reverse_play);

private:
	void render();

	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;

	float current_time;
	vec4 delta_fade_color;
};