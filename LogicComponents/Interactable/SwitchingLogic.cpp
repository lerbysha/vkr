#include "SwitchingLogic.h"


REGISTER_COMPONENT(SwitchingLogic);


void SwitchingLogic::disabled()
{
	if (obj_switch)
		obj_switch->setEnabled(0);
}

void SwitchingLogic::on_init()
{
	handler_id = -1;

	if (!switch_node)
	{
#if DEBUG
		Log::error("Switching Logic error: switch node is null, please check node %s\n", node->getName());
#endif
		return;
	}

	obj_switch = getComponent<ObjSwitch>(switch_node.get());
	if (!obj_switch)
	{
#if DEBUG
		Log::error("Switching Logic error: switch node not contain ObjSwitch component, please check node %s\n", node->getName());
#endif
		return;
	}

	handler_id = obj_switch->setOnUseItListener(std::bind(&SwitchingLogic::on_use_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	setInitialState();
}

void SwitchingLogic::on_shutdown()
{
	if (handler_id != -1 && obj_switch)
		obj_switch->removeOnUseItListener(handler_id);
}

void SwitchingLogic::on_set_initial_state()
{
	if (obj_switch)
	{
		obj_switch->setEnabled(1);
		obj_switch->setDefault(activated.get());
	}
}

void SwitchingLogic::on_use_handler(VRInteractable *const obj, VRPlayer *player, int hand_num)
{
	if (getState() == (States)initial_state.get())
		set_state((States)final_state.get());
	else
		set_state((States)initial_state.get());

#if DEBUG
	Log::message("SWITCH: %s is %d\n", node->getName(), getState());
#endif
}