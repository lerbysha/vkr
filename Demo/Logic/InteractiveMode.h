#pragma once
#include "../LogicController.h"
#include "../Inventory/ChecklistInvertoryItem.h"


class InteractiveMode : public LogicController
{
public:
	COMPONENT(InteractiveMode, LogicController);

	PROP_PARAM(Node, inventory);
	PROP_PARAM(Node, checklist);
	PROP_PARAM(Node, task_helpers);
	PROP_PARAM(Node, interactive_borders);

protected:
	void on_init() override;

	InventoryItem *stopwatch;
	InventoryItem *indicator;

	ChecklistInvertoryItem *checklist_item;

	void on_mission_finished(Mission *mission) override;
	void on_accident(Accident *accident) override;

	void on_restart_handler();
	int restart_handler_id;

	void grab_checklist();
	void throw_objects();
};