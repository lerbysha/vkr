#include "TransformationLogic.h"
#include "../../Framework/Components/Players/VRPlayerPC.h"


REGISTER_COMPONENT(TransformationLogic);


void TransformationLogic::on_init()
{
	if (!object)
	{
#if DEBUG
		Log::error("Tranformation Logic error: object is null, please check %s\n", node->getName());
#endif
		return;
	}

	is_vr_mode = VRPlayer::isVRPluginLoaded();
	is_hold = false;
	current_time = 1;
	vr_player = nullptr;

	movable = getComponent<ObjMovable>(object.get());
	if (!is_vr_mode && movable)
	{
		// for pc need only logic of grab
		addComponent<ObjSelectable>(object);
		removeComponent<ObjMovable>(object);

		movable = nullptr;
	}

	interactable = getComponent<VRInteractable>(object.get());
	if (interactable)
	{
		grab_handler_id = interactable->setOnGrabItListener(std::bind(&TransformationLogic::on_grab_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		throw_handler_id = interactable->setOnThrowItListener(std::bind(&TransformationLogic::on_throw_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	setInitialState();
}

void TransformationLogic::on_shutdown()
{
	if (interactable)
	{
		interactable->removeOnGrabItListener(grab_handler_id);
		interactable->removeOnThrowItListener(throw_handler_id);
	}
}

void TransformationLogic::on_set_initial_state()
{
	current_time = 1;

	object.get()->setPosition((Vec3)initial_position.get());
	object.get()->setRotation(quat(initial_rotation.get().x, initial_rotation.get().y, initial_rotation.get().z));

	is_hold = false;
	interpolation_completed = true;
}

void TransformationLogic::on_grab_handler(VRInteractable *const obj, VRPlayer *player, int in_hand_num)
{
	vr_player = player;
	hand_num = in_hand_num;

	if (is_vr_mode)
		is_hold = true;
	else
	{
		is_hold = !is_hold;

		from_pos = is_hold ? (Vec3)initial_position.get() : (Vec3)final_position.get();
		to_pos = is_hold ? (Vec3)final_position.get() : (Vec3)initial_position.get();

		from_rot = is_hold ? quat(initial_rotation.get().x, initial_rotation.get().y, initial_rotation.get().z) : quat(final_rotation.get().x, final_rotation.get().y, final_rotation.get().z);
		to_rot = is_hold ? quat(final_rotation.get().x, final_rotation.get().y, final_rotation.get().z) : quat(initial_rotation.get().x, initial_rotation.get().y, initial_rotation.get().z);

		current_time = 1 - current_time;
		interpolation_completed = false;

		if (is_hold)
			SoundManager::get()->playSound(final_sound.get(), object.get()->getWorldPosition(), 1, 1, 0.2f, 100.0f);
		else
			SoundManager::get()->playSound(initial_sound.get(), object.get()->getWorldPosition(), 1, 1, 0.2f, 100.0f);
	}
}

void TransformationLogic::on_throw_handler(VRInteractable *const obj, VRPlayer *player, int in_hand_num)
{
	if (is_vr_mode)
		is_hold = false;
}

void TransformationLogic::update()
{
	update_state();

	if (is_vr_mode)
	{
		if (!is_hold && use_vr_interpolation.get())
		{
			if (getState() == (States)final_state.get())
			{
				to_pos = (Vec3)final_position.get();
				to_rot = quat(final_rotation.get().x, final_rotation.get().y, final_rotation.get().z);
			}
			else if (getState() == (States)initial_state.get())
			{
				to_pos = (Vec3)initial_position.get();
				to_rot = quat(initial_rotation.get().x, initial_rotation.get().y, initial_rotation.get().z);
			}

			float k = saturate(speed_reaction * Game::getIFps());
			object.get()->setPosition(lerp(object->getPosition(), to_pos, k));
			object.get()->setRotation(slerp(object->getRotation(), to_rot, k));
		}
	}
	else
	{
		if (current_time < 1)
		{
			object.get()->setPosition(lerp(from_pos, to_pos, current_time));
			object.get()->setRotation(slerp(from_rot, to_rot, current_time));

			current_time += Game::getIFps() / pc_interpolation_time.get();

			if (current_time >= 1)
			{
				VRPlayerPC *pc_player = dynamic_cast<VRPlayerPC *>(vr_player);
				if (pc_player)
					pc_player->throwObject(0);
			}
		}
	}
}

void TransformationLogic::update_state()
{
	bool is_ready = true;

	if (use_position.get())
	{
		Vec3 pos = object.get()->getPosition();

		if (min_position.get().x > pos.x || pos.x > max_position.get().x ||
			min_position.get().y > pos.y || pos.y > max_position.get().y ||
			min_position.get().z > pos.z || pos.z > max_position.get().z)
		{
			is_ready = false;
		}
	}

	if (is_ready && use_rotation.get())
	{
		quat rot = object.get()->getRotation();
		vec3 euler_rot;

		euler_rot.x = rot.getAngle(object.get()->getDirection(AXIS_X));
		euler_rot.y = rot.getAngle(object.get()->getDirection(AXIS_Y));
		euler_rot.z = rot.getAngle(object.get()->getDirection(AXIS_Z));

		if (min_rotation.get().x > euler_rot.x || euler_rot.x > max_rotation.get().x ||
			min_rotation.get().y > euler_rot.y || euler_rot.y > max_rotation.get().y ||
			min_rotation.get().z > euler_rot.z || euler_rot.z > max_rotation.get().z)
		{
			is_ready = false;
		}
	}

	if (is_ready)
	{
		if (!movable || (movable && vr_player && vr_player->getHandState(hand_num) == VRPlayer::HAND_THROW))
			set_state((States)final_state.get());
	}
	else
		set_state((States)initial_state.get());
}