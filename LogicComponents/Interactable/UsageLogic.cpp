#include "UsageLoigc.h"


REGISTER_COMPONENT(UsageLogic);


void UsageLogic::disabled()
{
	if (obj)
		obj->setEnabled(0);
}

void UsageLogic::on_init()
{
	handler_id = -1;

	if (!interactive_object)
	{
#if DEBUG
		Log::error("Usage Logic error: interactive object is null, please check node %s\n", node->getName());
#endif
		return;
	}

	obj = getComponent<VRInteractable>(interactive_object.get());
	if (!obj)
	{
#if DEBUG
		Log::error("Usage Logic error: interactive object not contain VRInteractable component, please check node %s\n", node->getName());
#endif
		return;
	}

	handler_id = obj->setOnUseItListener(std::bind(&UsageLogic::on_use_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	next_use_time = Game::getTime();

	setInitialState();
}

void UsageLogic::on_shutdown()
{
	if (handler_id != -1 && obj)
		obj->removeOnSelectItListener(handler_id);
}

void UsageLogic::on_set_initial_state()
{
	if (obj)
		obj->setEnabled(1);
}

void UsageLogic::on_use_handler(VRInteractable *const interactable_obj, VRPlayer *player, int hand_num)
{
	if (next_use_time > Game::getTime())
		return;

	if (getState() == (States)initial_state.get())
		set_state((States)final_state.get());
	else
		set_state((States)initial_state.get());

	next_use_time = Game::getTime() + use_interval;

#if DEBUG
	Log::message("USAGE: %s is %d\n", node->getName(), getState());
#endif
}