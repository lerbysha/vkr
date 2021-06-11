#include "UdhLogic.h"
#include "../../Framework/Components/Players/VRPlayerPC.h"
#include "../../Framework/Components/Players/VRPlayerVR.h"
#include <UnigineInput.h>


REGISTER_COMPONENT(UdhLogic);


void UdhLogic::on_start(int is_training_mode)
{
	if (inventory)
	{
		inventory.get()->setWorldPosition(vr_player->getHead()->getWorldPosition());
		inventory.get()->setEnabled(1);
	}

	spawner->hand_length = default_hand_length;

	grab_checklist();

	if (restart_handler_id != -1)
		checklist_item->removeOnRestartListener(restart_handler_id);

	restart_handler_id = checklist_item->setOnRestartListener(std::bind(&UdhLogic::on_restart_handler, this));

	// enable stopwatch and indicator
	stopwatch->getNode()->setEnabled(true);
	stopwatch->setEnabled(true);
	stopwatch->grabPoint();

	indicator->getNode()->setEnabled(true);
	indicator->setEnabled(true);
	indicator->grabPoint();

	checklist_item->setEnabledChecklist(1);
	checklist_item->setMissionInfo(this);

	for (int i = 0; i < breakdowns_components.size(); i++)
	{
		breakdowns_components[i]->initial_state = static_cast<int>(LogicObject::States::NotBroken);
		breakdowns_components[i]->setInitialState();
	}

	int index = Game::getRandomInt(0, breakdowns_components.size());
	breakdowns_components[index]->initial_state = static_cast<int>(LogicObject::States::Broken);
	breakdowns_components[index]->setInitialState();

#if DEBUG
	Log::message("%s is broken\n", breakdowns_components[index]->getNode()->getName());
#endif

	if (task_helpers)
	{
		Vector<TaskHelper *> observers;
		getComponentsInChildren<TaskHelper>(task_helpers.get(), observers);
		for (int i = 0; i < observers.size(); i++)
			observers[i]->setTrainingMode(is_training_mode);
	}

	inspection_manager.get()->setEnabled(1);
	manager->force_enable = is_training_mode ? false : true;

	if (interactive_borders)
		interactive_borders.get()->setEnabled(1);
}

void UdhLogic::on_stop()
{
	if (inventory)
		inventory.get()->setEnabled(0);

	throw_objects();

	if (restart_handler_id != -1)
		checklist_item->removeOnRestartListener(restart_handler_id);

	checklist_item->setEnabledChecklist(0);

	// disable stopwatch and indicator
	stopwatch->setEnabled(0);
	stopwatch->releasePoint();

	indicator->setEnabled(0);
	indicator->releasePoint();

	inspection_manager.get()->setEnabled(0);
	manager->force_enable = false;

	if (interactive_borders)
		interactive_borders.get()->setEnabled(0);
}

void UdhLogic::on_init()
{
	InteractiveMode::on_init();

	if (!breakdowns)
	{
#if DEBUG
		Log::error("UDH LOGIC ERROR: breakdowns node is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!inspection_manager)
	{
#if DEBUG
		Log::error("UDH LOGIC ERROR: inspection manager node is null, please check %s\n", node->getName());
#endif
		return;
	}

	manager = getComponent<InspectionManager>(inspection_manager.get());
	if (!manager)
	{
#if DEBUG
		Log::error("UDH LOGIC ERROR: inspection manager node not contain InspectionManager component, please check %s\n", node->getName());
#endif
		return;
	}

	inspection_manager.get()->setEnabled(0);

	getComponentsInChildren<LogicObject>(breakdowns.get(), breakdowns_components);
	for (int i = 0; i < breakdowns_components.size(); i++)
		if (!breakdowns_components[i]->isEnabled())
		{
			breakdowns_components.remove(i);
			i--;
		}
}

void UdhLogic::on_shutdown()
{
	breakdowns_components.clear();
}

void UdhLogic::items_update()
{
	if (!isActive())
		return;

	if (!vr_player->isVRPluginLoaded())
	{
		VRPlayerPC *pc = dynamic_cast<VRPlayerPC *>(vr_player);
		if (!pc)
			return;

		if (Input::isKeyDown(Input::KEY_DIGIT_1))
		{
			if (pc->getHandState(0) != VRPlayer::HAND_FREE && pc->getGrabNode(0) == checklist.get())
				pc->throwObject(0);
			else
			{
				pc->throwObject(0);
				pc->grabObject(0, checked_ptr_cast<Object>(checklist.get()), 0.6f);
			}
		}

		if (Input::isKeyDown(Input::KEY_DIGIT_2))
		{
			if (pc->getHandState(0) != VRPlayer::HAND_FREE && pc->getGrabNode(0) == indicator->getNode())
				pc->throwObject(0);
			else
			{
				pc->throwObject(0);
				pc->grabObject(0, checked_ptr_cast<Object>(indicator->getNode()), 0.6f);
			}
		}

		if (Input::isKeyDown(Input::KEY_DIGIT_3))
		{
			if (pc->getHandState(0) != VRPlayer::HAND_FREE && pc->getGrabNode(0) == stopwatch->getNode())
				pc->throwObject(0);
			else
			{
				pc->throwObject(0);
				pc->grabObject(0, checked_ptr_cast<Object>(stopwatch->getNode()), 0.6f);
			}
		}
	}
}