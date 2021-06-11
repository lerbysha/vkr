#pragma once
#include <UnigineWidgets.h>
#include "InteractiveMode.h"
#include "../Inventory/ChecklistInvertoryItem.h"
#include "../TaskHelper.h"
#include "../../LogicComponents/Interactable/InspectionManager.h"


class UdhLogic : public InteractiveMode
{
public:
	COMPONENT(UdhLogic, InteractiveMode);
	COMPONENT_UPDATE(items_update)

	PROP_NAME("udh_logic");
	PROP_AUTOSAVE(1);

	PROP_PARAM(Node, breakdowns);
	PROP_PARAM(Node, inspection_manager);

private:
	void on_init() override;
	void on_start(int is_training_mode) override;
	void on_stop() override;
	void on_shutdown();

	void items_update();

	Vector<LogicObject *> breakdowns_components;
	InspectionManager *manager;
};