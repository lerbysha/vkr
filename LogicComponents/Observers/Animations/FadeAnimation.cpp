#include "FadeAnimation.h"


REGISTER_COMPONENT(FadeAnimation);


void FadeAnimation::render()
{
	if (current_time > 0)
	{
		float ifps = Game::getIFps();
		vec4 fade_color = vec4::ZERO;

		if (random.get())
			fade_color = initial_color.get() + delta_fade_color * itof(rand()) / RAND_MAX;
		else
		{
			float coeff = 0;
			if (add_reverse_play.get())
			{
				coeff = duration.get() - static_cast<float>((Unigine::Math::abs(duration.get() * 0.5 - current_time))) * 2.0f;
				coeff /= duration.get();
			}
			else
				coeff = 1 - current_time / duration.get();

			fade_color = lerp(initial_color.get(), final_color.get(), coeff);
		}

		Render::setFadeColor(fade_color);

		current_time -= ifps;
		if (current_time <= 0)
			Render::setFadeColor(vec4::ZERO);
	}
}

void FadeAnimation::on_init()
{
	current_time = 0;
	delta_fade_color = final_color.get() - initial_color.get();
}

void FadeAnimation::on_activate()
{
	current_time = duration.get();
	Render::setFadeColor(initial_color.get());
}

void FadeAnimation::on_deactivate()
{
	current_time = 0;
	Render::setFadeColor(vec4::ZERO);
}