#include "GrabLogic.h"


REGISTER_COMPONENT(GrabLogic);


void GrabLogic::disabled()
{
	if (obj)
		obj->setEnabled(0);
}

void GrabLogic::on_init()
{
	grab_handler_id = -1;
	throw_handler_id = -1;

	if (!interactive_object)
	{
#if DEBUG
		Log::error("Grab Logic error: interactive object is null, please check node %s\n", node->getName());
#endif
		return;
	}

	obj = getComponent<VRInteractable>(interactive_object.get());
	if (!obj)
	{
#if DEBUG
		Log::error("Grab Logic error: interactive object not contain VRInteractable component, please check node %s\n", node->getName());
#endif
		return;
	}

	grab_handler_id = obj->setOnGrabItListener(std::bind(&GrabLogic::on_grab_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	throw_handler_id = obj->setOnThrowItListener(std::bind(&GrabLogic::on_throw_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	setInitialState();
}

void GrabLogic::on_shutdown()
{
	if (grab_handler_id != -1 && obj)
		obj->removeOnSelectItListener(grab_handler_id);
}

void GrabLogic::on_set_initial_state()
{
	if (obj)
		obj->setEnabled(1);
}

void GrabLogic::on_grab_handler(VRInteractable *const interactable_obj, VRPlayer *player, int hand_num)
{
	set_state((States)final_state.get());

#if DEBUG
	Log::message("GRAB: %s is %d\n", node->getName(), getState());
#endif
}

void GrabLogic::on_throw_handler(VRInteractable *const interactable_obj, VRPlayer *player, int hand_num)
{
	set_state((States)initial_state.get());

#if DEBUG
	Log::message("GRAB: %s is %d\n", node->getName(), getState());
#endif
}