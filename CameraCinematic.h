#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineInterface.h>

class CameraCinematic : public Unigine::ComponentBase
{
public:
	COMPONENT(CameraCinematic, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_NAME("camera_cinematic");
	PROP_AUTOSAVE(1);

	PROP_PARAM(Float, source_fov, 60.0f);
	PROP_PARAM(Float, min_fov, 10.0f);
	PROP_PARAM(Float, max_fov, 110.0f);

	static void setCinematicMode(int enabled);
	static void changedQualitySettings();

private:
	void init();
	void update();

	Unigine::PlayerPtr player;
	static float source_distance_scale;

	static bool is_cinematic_mode;
	static float last_fov;
};

