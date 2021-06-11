#pragma once
#include "../../LogicComponents/LogicObject.h"
#include "../Objects/ObjEarthingIndicator.h"


class ObjEarthingLogic : public LogicObject
{
public:
	COMPONENT(ObjEarthingLogic, LogicObject);

	PROP_NAME("obj_earthing_logic");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, indicator);

private:
	void on_init() override;
	void on_shutdown() override;

	void on_touch_handler();

	ObjEarthingIndicator *obj_indicator;
	int handler_id;
};