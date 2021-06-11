#include "GeneratorLogic.h"
#include "../../Framework/Components/Players/VRPlayerPC.h"
#include "../../Framework/Components/Players/VRPlayerVR.h"
#include <UnigineInput.h>


REGISTER_COMPONENT(GeneratorLogic);


void GeneratorLogic::on_start(int is_training_mode)
{
	if (inventory)
	{
		inventory.get()->setWorldPosition(vr_player->getHead()->getWorldPosition());
		inventory.get()->setEnabled(1);
	}

	spawner->hand_length = 3.0f;

	grab_checklist();

	// disable stopwatch and indicator
	stopwatch->getNode()->setEnabled(0);
	indicator->getNode()->setEnabled(0);

	stopwatch->releasePoint();
	indicator->releasePoint();

	NodePtr fan = World::getNodeByName("generator_fan_logic");
	if (fan)
		fan->setEnabled(1);

	if (restart_handler_id != -1)
		checklist_item->removeOnRestartListener(restart_handler_id);

	restart_handler_id = checklist_item->setOnRestartListener(std::bind(&GeneratorLogic::on_restart_handler, this));

	checklist_item->setEnabledChecklist(1);
	checklist_item->setMissionInfo(this);

	if (task_helpers)
	{
		Vector<TaskHelper *> observers;
		getComponentsInChildren<TaskHelper>(task_helpers.get(), observers);
		for (int i = 0; i < observers.size(); i++)
			observers[i]->setTrainingMode(is_training_mode);
	}

	if (interactive_borders)
		interactive_borders.get()->setEnabled(1);
}

void GeneratorLogic::on_stop()
{
	if (inventory)
		inventory.get()->setEnabled(0);

	spawner->hand_length = default_hand_length;

	NodePtr fan = World::getNodeByName("generator_fan_logic");
	if (fan)
		fan->setEnabled(0);

	throw_objects();

	if (restart_handler_id != -1)
		checklist_item->removeOnRestartListener(restart_handler_id);

	checklist_item->setEnabledChecklist(0);

	if (interactive_borders)
		interactive_borders.get()->setEnabled(0);
}

void GeneratorLogic::items_update()
{
	if (!isActive())
		return;

	if (!vr_player->isVRPluginLoaded())
		if (Input::isKeyDown(Input::KEY_DIGIT_1))
		{
			VRPlayerPC *pc = dynamic_cast<VRPlayerPC *>(vr_player);
			if (pc)
			{
				if (pc->getHandState(0) != VRPlayer::HAND_FREE && pc->getGrabNode(0) == checklist.get())
					pc->throwObject(0);
				else
				{
					pc->throwObject(0);
					pc->grabObject(0, checked_ptr_cast<Object>(checklist.get()), 0.6f);
				}
			}
		}
}