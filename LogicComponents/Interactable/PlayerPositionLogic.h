#pragma once
#include "../LogicObject.h"
#include "../../Framework/Components/VRPlayerSpawner.h"


class PlayerPositionLogic : public LogicObject
{
public:
	COMPONENT(PlayerPositionLogic, LogicObject);
	COMPONENT_UPDATE(update);

	PROP_NAME("player_position_logic");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, spawn_point);
	PROP_PARAM(Vec3, initial_position);
	PROP_PARAM(Vec3, final_position);

private:
	void on_init() override;

	void update();

	NodePtr player_head;
};