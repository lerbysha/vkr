#include "DOFAnimation.h"


REGISTER_COMPONENT(DOFAnimation);


void DOFAnimation::render()
{
	if (current_time > 0)
	{
		float ifps = Game::getIFps();

		float coeff = 0;
		if (add_reverse_play.get())
		{
			coeff = duration.get() - static_cast<float>((Unigine::Math::abs(duration.get() * 0.5 - current_time))) * 2.0f;
			coeff /= duration.get();
		}
		else
			coeff = 1 - current_time / duration.get();

		float focal_distance = lerp(initial_focal_distance.get(), final_focal_distance.get(), coeff);
		float blur = lerp(initial_blur.get(), final_blur.get(), coeff);

		Render::setDOFFocalDistance(focal_distance);
		Render::setDOFBlur(blur);

		current_time -= ifps;
		if (current_time <= 0)
			Render::setFadeColor(vec4::ZERO);
	}
}

void DOFAnimation::on_init()
{
	current_time = 0;
}

void DOFAnimation::on_activate()
{
	current_time = duration.get();
	Render::setDOF(1);
	Render::setDOFFocalDistance(0);
}

void DOFAnimation::on_deactivate()
{
	Render::setDOF(0);
}