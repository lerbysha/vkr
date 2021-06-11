#include "CameraCinematic.h"
#include <UnigineUserInterface.h>
#include "Framework/Components/VRPlayerSpawner.h"

REGISTER_COMPONENT(CameraCinematic)

using namespace Unigine;
using namespace Math;

bool CameraCinematic::is_cinematic_mode = false;
float CameraCinematic::source_distance_scale = 1.0f;
float CameraCinematic::last_fov = 0.0f;

void CameraCinematic::init()
{
	player = checked_ptr_cast<Player>(node);
	if (player)
	{
		source_distance_scale = Render::getDistanceScale();
		last_fov = source_fov;
	}
}

void CameraCinematic::update()
{
	if (!player || !is_cinematic_mode)
		return;

	float current_fov = clamp(player->getFov(), min_fov.get(), max_fov.get());
	if (!Math::compare(last_fov, current_fov))
	{
		float k = source_fov.get() / current_fov;
		Render::setDistanceScale(source_distance_scale * k);

		last_fov = current_fov;
	}
}

void CameraCinematic::setCinematicMode(int enabled)
{
	is_cinematic_mode = (enabled != 0);

	if (!is_cinematic_mode)
		Render::setDistanceScale(source_distance_scale);
	else
	{
		if (VRPlayer::get())
			VRPlayer::get()->getPlayer()->setEnabled(false);
	}
}

void CameraCinematic::changedQualitySettings()
{
	source_distance_scale = Render::getDistanceScale();
	last_fov = 0.0f;
}
