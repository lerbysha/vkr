#include "ObjEarthingLogic.h"
#include <UnigineInput.h>


REGISTER_COMPONENT(ObjEarthingLogic);


void ObjEarthingLogic::on_init()
{
	handler_id = -1;

	if (!indicator)
	{
#if DEBUG
		Log::error("Obj Earthing Logic error: indicator is null, please check node %s\n", node->getName());
#endif
		return;
	}

	obj_indicator = getComponent<ObjEarthingIndicator>(indicator.get());
	if (!obj_indicator)
	{
#if DEBUG
		Log::error("Obj Earthing Logic error: indicator not contain ObjEarthingIndicator component, please check node %s\n", node->getName());
#endif
		return;
	}

	handler_id = obj_indicator->setOnEarthingTouchListener(std::bind(&ObjEarthingLogic::on_touch_handler, this));

	setInitialState();
}

void ObjEarthingLogic::on_shutdown()
{
	if (handler_id != -1 && obj_indicator)
		obj_indicator->removeOnEarthingTouchListener(handler_id);
}

void ObjEarthingLogic::on_touch_handler()
{
	set_state((States)final_state.get());
}