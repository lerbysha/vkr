#include "MoveLogic.h"


REGISTER_COMPONENT(MoveLogic);


void MoveLogic::disabled()
{
	if (obj_movable)
		obj_movable->setEnabled(0);
}

void MoveLogic::on_init()
{
	grab_id = -1;
	hold_id = -1;
	throw_id = -1;

	if (!interactive_object)
	{
#if DEBUG
		Log::error("Move Logic error: interactive object is null, please check node %s\n", node->getName());
#endif
		return;
	}

	obj_movable = getComponent<ObjMovable>(interactive_object.get());
	if (!obj_movable)
	{
#if DEBUG
		Log::error("Move Logic error: interactive object not contain ObjMovable component, please check node %s\n", node->getName());
#endif
		return;
	}

	grab_id = obj_movable->setOnGrabItListener(std::bind(&MoveLogic::on_grab_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	hold_id = obj_movable->setOnHoldItListener(std::bind(&MoveLogic::on_hold_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	throw_id = obj_movable->setOnThrowItListener(std::bind(&MoveLogic::on_throw_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	if (use_initial_position.get())
		default_transform = obj_movable->getNode()->getWorldTransform();

	setInitialState();
}

void MoveLogic::on_set_initial_state()
{
	if (obj_movable)
	{
		obj_movable->setEnabled(1);

		if (use_initial_position.get())
			obj_movable->getNode()->setWorldTransform(default_transform);
	}
}

void MoveLogic::on_shutdown()
{
	if (grab_id != -1 && obj_movable)
		obj_movable->removeOnGrabItListener(grab_id);

	if (hold_id != -1 && obj_movable)
		obj_movable->removeOnHoldItListener(hold_id);

	if (throw_id != -1 && obj_movable)
		obj_movable->removeOnThrowItListener(throw_id);
}

void MoveLogic::on_grab_handler(VRInteractable *const obj, VRPlayer *player, int hand_num)
{
	set_state(States::Grabbed);

#if DEBUG
	Log::message("MOVABLE: %s is %d\n", node->getName(), getState());
#endif
}

void MoveLogic::on_hold_handler(VRInteractable *const obj, VRPlayer *player, int hand_num)
{
	set_state(States::Held);

//#if DEBUG
//	Log::message("MOVABLE: %s is %d\n", node->getName(), getState());
//#endif
}

void MoveLogic::on_throw_handler(VRInteractable *const obj, VRPlayer *player, int hand_num)
{
	set_state(States::Released);

#if DEBUG
	Log::message("MOVABLE: %s is %d\n", node->getName(), getState());
#endif
}