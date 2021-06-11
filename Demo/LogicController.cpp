#include "LogicController.h"
#include "../Framework/Components/VRInteractable.h"
#include "../Demo/Inventory/ChecklistInvertoryItem.h"
#include "../Framework/Components/Players/VRPlayerPC.h"
#include "../Framework/Components/Players/VRPlayerVR.h"


void LogicController::start(int is_training_mode)
{
	training_mode = is_training_mode;

	// enable all observers components
	Vector<Observer *> all_observers;
	getComponentsInChildren<Observer>(node, all_observers);
	for (int i = 0; i < all_observers.size(); i++)
		if (all_observers[i]->getNode()->isEnabled())
			all_observers[i]->setEnabled(true);

	// enable and init all logic components
	Vector<LogicObject *> all_components;
	getComponentsInChildren<LogicObject>(node, all_components);
	for (int i = 0; i < all_components.size(); i++)
		if (all_components[i]->getNode()->isEnabled())
		{
			all_components[i]->setEnabled(true);
			all_components[i]->setInitialState();
		}

	// after all action with logic objects set tasks in initial state
	Vector<Task *> all_tasks;
	getComponentsInChildren<Task>(node, all_tasks);
	for (int i = 0; i < all_tasks.size(); i++)
		if (all_tasks[i]->isEnabled())
			all_tasks[i]->setInitialState();

	mission_handler_id = mission->setOnStateChangedListener(std::bind(&LogicController::on_mission_state_changed_handler, this, std::placeholders::_1, std::placeholders::_2));

	for (int i = 0; i < accidents.size(); i++)
	{
		int id = accidents[i]->setOnStateChangedListener(std::bind(&LogicController::on_accident_state_changed_handler, this, std::placeholders::_1, std::placeholders::_2));
		accidents_handlers_id.push_back(id);
	}

	if (VRPlayer::isVRPluginLoaded())
	{
		floor_node.get()->setEnabled(1);
		World::updateSpatial();
	}
	else
		floor_node.get()->setEnabled(0);

	vr_player->landPlayerTo(start_point->getWorldTransform());
	Game::setPlayer(vr_player->getPlayer());

	on_start(is_training_mode);

	NodePtr block = World::getNodeByName("container_flyover_collision");
	if (block)
		block->setEnabled(0);

	mission->setInProgress();
	is_active = true;
}

void LogicController::stop()
{
	on_stop();

	floor_node.get()->setEnabled(0);
	World::updateSpatial();

	if (mission_handler_id != -1)
	{
		mission->removeOnStateChangedListener(mission_handler_id);
		mission_handler_id = -1;
	}

	for (int i = 0; i < accidents_handlers_id.size(); i++)
		accidents[i]->removeOnStateChangedListener(accidents_handlers_id[i]);
	accidents_handlers_id.clear();

	// disable all logic components
	Vector<LogicObject *> all_components;
	getComponentsInChildren<LogicObject>(node, all_components);
	for (int i = 0; i < all_components.size(); i++)
		if (all_components[i]->isEnabled())
		{
			all_components[i]->setEnabled(0);
			all_components[i]->setInitialState();
			all_components[i]->disabled();
		}

	// after all action with logic objects set tasks in initial state
	Vector<Task *> all_tasks;
	getComponentsInChildren<Task>(node, all_tasks);
	for (int i = 0; i < all_tasks.size(); i++)
		if (all_tasks[i]->isEnabled())
			all_tasks[i]->setInitialState();

	// disable all observers components
	Vector<Observer *> all_observers;
	getComponentsInChildren<Observer>(node, all_observers);
	for (int i = 0; i < all_observers.size(); i++)
		if (all_observers[i]->isEnabled())
		{
			all_observers[i]->setEnabled(0);
			all_observers[i]->disable();
		}

	NodePtr block = World::getNodeByName("container_flyover_collision");
	if (block)
		block->setEnabled(1);

	is_active = false;
}

void LogicController::restart(int is_training_mode)
{
	training_mode = is_training_mode;

	stop();
	start(is_training_mode);

	on_restart(is_training_mode);
}

bool LogicController::isActive()
{
	return is_active;
}

void LogicController::on_accident_state_changed_handler(LogicObject *object, LogicObject::States object_state)
{
	if (object_state != (LogicObject::States)object->final_state.get())
		return;

	Accident *accident = dynamic_cast<Accident *>(object);
	if (accident)
		on_accident(accident);
}

void LogicController::on_mission_state_changed_handler(LogicObject *object, LogicObject::States object_state)
{
	if (object_state != (LogicObject::States)object->final_state.get())
		return;

	Mission *mission_component = dynamic_cast<Mission *>(object);
	if (mission_component)
		on_mission_finished(mission_component);
}

void LogicController::on_mission_finished(Mission *finished_mission)
{

}

void LogicController::on_accident(Accident *accident)
{

}

void LogicController::init()
{
	if (!player_spawn_node)
	{
#if DEBUG
		Log::error("Logic Controller error: player spawn node is null, please check %s\n", node->getName());
#endif
		return;
	}

	spawner = getComponent<VRPlayerSpawner>(player_spawn_node.get());
	if (!spawner)
	{
#if DEBUG
		Log::error("Logic Controller error: player spawn node not contain VRPlayerSpawner component, please check %s\n", node->getName());
#endif
		return;
	}

	vr_player = spawner->getSpawnedPlayer();
	vr_player->setTeleportationMask(teleportation_mask);
	default_hand_length = spawner->hand_length;

	if (!start_point)
	{
#if DEBUG
		Log::error("Logic Controller error: start point is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!floor_node)
	{
#if DEBUG
		Log::error("Logic Controller error: floor_node is null, please check %s\n", node->getName());
#endif
		return;
	}

	mission = getComponentInChildren<Mission>(node);
	if (!mission)
	{
#if DEBUG
		Log::error("Logic Controller error: node with Mission component not found, please check %s\n", node->getName());
#endif
		return;
	}

	Vector<Accident *> all_accidents;
	getComponentsInChildren<Accident>(node, all_accidents);

	accidents.reserve(all_accidents.size());
	for (int i = 0; i < all_accidents.size(); i++)
		if (all_accidents[i]->isEnabled())
			accidents.push_back(all_accidents[i]);

	is_active = false;
	mission_handler_id = -1;

	on_init();
	stop();
}

void LogicController::shutdown()
{
	on_shutdown();

	stop();

	accidents.clear();
}