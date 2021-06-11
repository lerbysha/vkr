#include "CameraShake.h"


REGISTER_COMPONENT(CameraShake);


void CameraShake::render()
{
	if (current_time > 0)
	{
		float ifps = Game::getIFps();
		Vec3 pos = (Vec3)initial_position.get() + delta_position * Game::getRandomFloat(0.0f, 1.0f);

		PlayerPtr current_player = Game::getPlayer();
		Mat4 t = current_player->getWorldTransform();
		
		if (is_player_actor)
		{
			current_player->setWorldTransform(translate(t.getColumn3(3) + pos) * rotation(current_player->getCamera()->getIModelview()));
			current_player->flushTransform();
		}
		else
		{
			setTransformPosition(t, t.getColumn3(3) + pos);
			current_player->setWorldTransform(t);
		}

		current_time -= ifps;
	}
}

void CameraShake::on_init()
{
	delta_position = (Vec3)final_position.get() - (Vec3)initial_position.get();
	current_time = 0;

	is_player_actor = false;
}

void CameraShake::on_activate()
{
	current_time = duration.get();

	is_player_actor = false;
	if (checked_ptr_cast<PlayerActor>(Game::getPlayer()))
		is_player_actor = true;
}

void CameraShake::on_deactivate()
{
	current_time = 0;
}