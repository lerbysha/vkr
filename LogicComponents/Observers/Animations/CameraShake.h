#pragma once
#include "../Observer.h"
#include "../../../Framework/Components/Players/VRPlayer.h"
#include <UnigineGame.h>
#include "../../../Framework/Components/VRPlayerSpawner.h"


class CameraShake : public Observer
{
public:
	COMPONENT(CameraShake, Observer);
	COMPONENT_UPDATE(render);

	PROP_NAME("camera_shake");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Float, duration);
	PROP_PARAM(Vec3, initial_position);
	PROP_PARAM(Vec3, final_position);

private:
	void render();

	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;

	PlayerPtr player;

	Vec3 delta_position;
	float current_time;

	bool is_player_actor;
};