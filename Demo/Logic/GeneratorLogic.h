#pragma once
#include <UnigineWidgets.h>
#include "../LogicController.h"
#include "../Inventory/ChecklistInvertoryItem.h"
#include "../TaskHelper.h"
#include "InteractiveMode.h"


class GeneratorLogic : public InteractiveMode
{
public:
	COMPONENT(GeneratorLogic, InteractiveMode);
	COMPONENT_UPDATE(items_update, 1);

	PROP_NAME("generator_logic");
	PROP_AUTOSAVE(1);

private:
	void on_start(int is_training_mode) override;
	void on_stop() override;

	void items_update();
};