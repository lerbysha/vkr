#include "SelectionLogic.h"
#include <UnigineInput.h>


REGISTER_COMPONENT(SelectionLogic);


void SelectionLogic::disabled()
{
	if (obj)
		obj->setEnabled(0);
}

void SelectionLogic::on_init()
{
	handler_id = -1;

	if (!interactive_object)
	{
#if DEBUG
		Log::error("Selection Logic error: interactive object is null, please check node %s\n", node->getName());
#endif
		return;
	}

	obj = getComponent<VRInteractable>(interactive_object.get());
	if (!obj)
	{
#if DEBUG
		Log::error("Selection Logic error: interactive object not contain VRInteractable component, please check node %s\n", node->getName());
#endif
		return;
	}

	handler_id = obj->setOnSelectItListener(std::bind(&SelectionLogic::on_select_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	setInitialState();
}

void SelectionLogic::on_shutdown()
{
	if (handler_id != -1 && obj)
		obj->removeOnSelectItListener(handler_id);
}

void SelectionLogic::on_set_initial_state()
{
	if (obj)
		obj->setEnabled(1);
}

void SelectionLogic::on_select_handler(VRInteractable *const interactable_obj, VRPlayer *player, int hand_num)
{
	if (VRPlayer::isVRPluginLoaded())
		set_state((States)final_state.get());
	else
	{
		if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
			set_state((States)final_state.get());
	}

#if DEBUG
	Log::message("SELECTION: %s is %d\n", node->getName(), getState());
#endif
}