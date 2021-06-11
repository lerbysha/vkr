#include "InteractiveMode.h"


void InteractiveMode::on_init()
{
	if (!checklist)
	{
#if DEBUG
		Log::error("GENERATOR LOGIC ERROR: checklist is null, please check %s\n", node->getName());
#endif
		return;
	}

	checklist_item = getComponent<ChecklistInvertoryItem>(checklist.get());
	if (!checklist_item)
	{
#if DEBUG
		Log::error("GENERATOR LOGIC ERROR: checklist not contain ChecklistInventoryItem component\n");
#endif
		return;
	}

	NodePtr item = World::getNodeByName("stopwatch");
	if (!item)
	{
#if DEBUG
		Log::error("INTERACTIVE MODE ERROR: stopwatch not found\n");
#endif
		return;
	}

	stopwatch = getComponent<InventoryItem>(item);
	if (!stopwatch)
	{
#if DEBUG
		Log::error("INTERACTIVE MODE ERROR: stopwatch not contain InventoryItem component\n");
#endif
		return;
	}

	item = World::getNodeByName("indicator_earthing");
	if (!item)
	{
#if DEBUG
		Log::error("INTERACTIVE MODE ERROR: indicator earthing not found\n");
#endif
		return;
	}

	indicator = getComponent<InventoryItem>(item);
	if (!indicator)
	{
#if DEBUG
		Log::error("INTERACTIVE MODE ERROR: indicator earthing not contain InventoryItem component\n");
#endif
		return;
	}

	restart_handler_id = -1;

	if (interactive_borders)
		interactive_borders.get()->setEnabled(0);

	if (inventory)
		inventory.get()->setEnabled(0);
}

void InteractiveMode::grab_checklist()
{
	if (vr_player->isVRPluginLoaded())
	{
		VRPlayerVR *vr = dynamic_cast<VRPlayerVR *>(vr_player);
		if (vr)
		{
			vr->throwObject(0);
			vr->throwObject(1);

			vr->grabObject(0, checked_ptr_cast<Object>(checklist.get()), 0);
		}
	}
	else
	{
		VRPlayerPC *pc = dynamic_cast<VRPlayerPC *>(vr_player);
		if (pc)
		{
			pc->throwObject(0);
			pc->grabObject(0, checked_ptr_cast<Object>(checklist.get()), 0.6f);
		}
	}
}

void InteractiveMode::throw_objects()
{
	if (vr_player->isVRPluginLoaded())
	{
		VRPlayerVR *vr = dynamic_cast<VRPlayerVR *>(vr_player);
		if (vr)
		{
			vr->throwObject(0);
			vr->throwObject(1);
		}
	}
	else
	{
		VRPlayerPC *pc = dynamic_cast<VRPlayerPC *>(vr_player);
		if (pc)
			pc->throwObject(0);
	}
}

void InteractiveMode::on_mission_finished(Mission *finished_mission)
{
	checklist_item->showMissionFinised(finished_mission->getDescription());
	grab_checklist();
}

void InteractiveMode::on_accident(Accident *accident)
{
	checklist_item->showAccident(accident->getDescription());
	grab_checklist();
}

void InteractiveMode::on_restart_handler()
{
	restart(training_mode);
}
