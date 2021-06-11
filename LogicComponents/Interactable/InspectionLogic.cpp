#include "InspectionLogic.h"


REGISTER_COMPONENT(InspectionLogic);


void InspectionLogic::disabled()
{
	if (obj)
		obj->setEnabled(0);
}

void InspectionLogic::on_init()
{
	handler_id = -1;

	if (!inspectable_object)
	{
#if DEBUG
		Log::error("Inspectable Logic error: inspectable object is null, please check node %s\n", node->getName());
#endif
		return;
	}

	obj = getComponent<ObjInspectable>(inspectable_object.get());
	if (!obj)
	{
#if DEBUG
		Log::error("Inspectable Logic error: inspectable object not contain ObjInspectable component, please check node %s\n", node->getName());
#endif
		return;
	}

	handler_id = obj->setOnInspectItListener(std::bind(&InspectionLogic::on_inspect_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	setInitialState();
}

void InspectionLogic::on_shutdown()
{
	if (handler_id != -1 && obj)
		obj->removeOnSelectItListener(handler_id);
}

void InspectionLogic::on_set_initial_state()
{
	if (obj)
		obj->setEnabled(1);
}

void InspectionLogic::on_inspect_handler(VRInteractable *const interactable_obj, VRPlayer *player, int status)
{
	if (status)
		set_state((States)final_state.get());
	else
		set_state((States)initial_state.get());

#if DEBUG
	Log::message("INSPECTION: %s is %d\n", node->getName(), getState());
#endif
}