#include "PlayerPositionLogic.h"


REGISTER_COMPONENT(PlayerPositionLogic);


void PlayerPositionLogic::on_init()
{
	if (!spawn_point)
	{
#if DEBUG
		Log::error("Player Position Logic error: spawn point is null, please check %s\n", node->getName());
#endif
		return;
	}

	VRPlayerSpawner *spawner = getComponent<VRPlayerSpawner>(spawn_point.get());
	if (!spawner)
	{
#if DEBUG
		Log::error("Player Position Logic error: spawn point not contain VRPlayerSpawner component, please check %s\n", node->getName());
#endif
		return;
	}

	VRPlayer *vr_player = spawner->getSpawnedPlayer();
	if (vr_player)
		player_head = vr_player->getHead();

	if (!player_head)
	{
#if DEBUG
		Log::error("Player Position Logic error: can't get head node of player, please check %s\n", node->getName());
#endif
		return;
	}

	setInitialState();
}

void PlayerPositionLogic::update()
{
	Vec3 head_pos = player_head->getWorldPosition();

	if (initial_position.get().x < head_pos.x && head_pos.x < final_position.get().x &&
		initial_position.get().y < head_pos.y && head_pos.y < final_position.get().y &&
		initial_position.get().z < head_pos.z && head_pos.z < final_position.get().z)
	{
		set_state((States)final_state.get());
	}
	else
	{
		set_state((States)initial_state.get());
	}
}