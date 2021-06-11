#include "LightProbeAnimation.h"


REGISTER_COMPONENT(LightProbeAnimation);


void LightProbeAnimation::update()
{
	if (time < 1)
	{
		time = saturate(time + (Game::getIFps() / Game::getScale()) / duration.get());

		float intensity = lerp(from_intensity, to_intensity, time);
		for (int i = 0; i < probes.size(); i++)
		{
			LightPtr probe = checked_ptr_cast<Light>(probes[i].get());
			if (probe)
				probe->setIntensity(intensity);
		}
	}
}

void LightProbeAnimation::on_init()
{
	if (!probes.size())
	{
#if DEBUG
		Log::message("Light Probe Animation: probes array is empty, please check %s\n", node->getName());
#endif
		return;
	}

	for (int i = 0; i < probes.size(); i++)
	{
		LightPtr probe = checked_ptr_cast<Light>(probes[i].get());
		if (probe)
			probe->setIntensity(min_intensity.get());
	}

	time = 1;
}

void LightProbeAnimation::on_activate()
{
	from_intensity = min_intensity.get();
	to_intensity = max_intensity.get();

	time = 1 - time;
}

void LightProbeAnimation::on_deactivate()
{
	from_intensity = max_intensity.get();
	to_intensity = min_intensity.get();

	time = 1 - time;
}