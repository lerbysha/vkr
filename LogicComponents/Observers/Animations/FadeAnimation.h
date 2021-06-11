#pragma once
#include "../Observer.h"
#include <UnigineRender.h>
#include <UnigineGame.h>


class FadeAnimation : public Observer
{
public:
	COMPONENT(FadeAnimation, Observer);
	COMPONENT_POST_UPDATE(render);

	PROP_NAME("fade_animation");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Float, duration);
	PROP_PARAM(Vec4, initial_color);
	PROP_PARAM(Vec4, final_color);
	PROP_PARAM(Toggle, random);
	PROP_PARAM(Toggle, add_reverse_play);

private:
	void render();

	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;

	Render *render_ptr;
	float current_time;
	vec4 delta_fade_color;
};