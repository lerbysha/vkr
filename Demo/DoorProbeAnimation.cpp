#include "DoorProbeAnimation.h"


REGISTER_COMPONENT(DoorProbeAnimation);


void DoorProbeAnimation::disable()
{
	if (light)
		light->setIntensity(min_intensity.get());
}

void DoorProbeAnimation::update()
{
	quat rot = door.get()->getRotation();
	float z = rot.getAngle(door.get()->getDirection(AXIS_Z));

	if (min_z_angle.get() < z && z < max_z_angle.get())
	{
		float delta;
		if (use_inverse_angles.get())
			delta = max_z_angle.get() - z;
		else
			delta = z - min_z_angle.get();

		if (delta < 0)
			delta = -delta;

		float intensity = min_intensity.get() + intensity_delta * delta / max_delta_angles;
		light->setIntensity(intensity);
	}
}

void DoorProbeAnimation::on_init()
{
	if (!door)
	{
#if DEBUG
		Log::error("Door Probe Animation: door node is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!probe)
	{
#if DEBUG
		Log::error("Door Probe Animation: probe node is null, please check %s\n", node->getName());
#endif
		return;
	}

	light = checked_ptr_cast<Light>(probe.get());
	if (!light)
	{
#if DEBUG
		Log::error("Door Probe Animation: can't cast probe node to Light, please check %s\n", node->getName());
#endif
		return;
	}

	intensity_delta = max_intensity.get() - min_intensity.get();
	max_delta_angles = Math::abs(max_z_angle.get() - min_z_angle.get());
	light->setIntensity(min_intensity.get());
}