#pragma once
#include <UnigineWidgets.h>
#include "../Framework/Components/Players/VRPlayer.h"
#include "../Framework/Components/VRPlayerSpawner.h"
#include "../LogicComponents/Missions/Mission.h"
#include "../LogicComponents/Accidents/Accident.h"
#include "../LogicComponents/Observers/Observer.h"
#include <UnigineComponentSystem.h>


class LogicController : public Unigine::ComponentBase
{
public:
	COMPONENT(LogicController, Unigine::ComponentBase);

	COMPONENT_INIT(init, 3);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, player_spawn_node)
	PROP_PARAM(Node, start_point);
	PROP_PARAM(Node, floor_node);

	// logic control methods
	void start(int is_training_mode);
	void stop();
	void restart(int is_training_mode);
	bool isActive();

protected:
	virtual void on_init() {};
	virtual void on_shutdown() {};
	virtual void on_start(int is_training_mode) {};
	virtual void on_stop() {};
	virtual void on_restart(int is_training_mode) {};

	virtual void on_mission_finished(Mission *mission);
	virtual void on_accident(Accident *accident);

	void init();
	void shutdown();

	VRPlayer *vr_player;
	VRPlayerSpawner *spawner;
	float default_hand_length;
	int training_mode;

private:
	bool is_active;
	const unsigned int teleportation_mask = 128;

	// all needed logic objects
	Mission *mission;
	Vector<Accident *> accidents;

	// event handlers
	void on_accident_state_changed_handler(LogicObject *object, LogicObject::States object_state);
	void on_mission_state_changed_handler(LogicObject *object, LogicObject::States object_state);

	int mission_handler_id = -1;
	Vector<int> accidents_handlers_id;
};