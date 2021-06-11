#include "InteractableHandle.h"
#include <UnigineGame.h>


const auto MIN_SLIDE_SOUND_TIME = 0.2f;

using namespace Unigine;
using namespace Math;

/*static*/ const Vec3 InteractableHandle::ACTIVE_AXIS[4] = { {0.0, 1.0, 1.0}, { 1.0, 0.0, 1.0 }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0, 1.0 } };

REGISTER_COMPONENT(InteractableHandle);


template<int size, uint8_t logic_mode = 0, class V>
V clamp(const V &v, const V &min, const V &max, InteractableHandle::ClampState &out_was_clamped)
{
	out_was_clamped = logic_mode == 0 ? InteractableHandle::ClampState::None : InteractableHandle::ClampState::Clamp;
	V ret = v;
	for (auto i = 0; i < size; ++i)
	{
		if (v[i] < min[i])
		{
			ret[i] = min[i];

			if (logic_mode == 0) out_was_clamped = InteractableHandle::ClampState::MinClamp;
		}
		else if (v[i] > max[i])
		{
			ret[i] = max[i];

			if (logic_mode == 0) out_was_clamped = InteractableHandle::ClampState::MaxClamp;
		}
		else if (logic_mode != 0) out_was_clamped = InteractableHandle::ClampState::None;
	}
	return ret;
}

void InteractableHandle::init()
{
	obj = checked_ptr_cast<Object>(node);

	body = node->getObjectBodyRigid();
	if (body) body->setFreezable(0);

	if (sound_loop && FileSystem::isFileExist(sound_loop.get()))
	{
		sound = SoundSource::create(sound_loop);
		sound->setPosition(node->getWorldPosition());
		sound->setMinDistance(sound_min_dist);
		sound->setMaxDistance(sound_max_dist);
		sound->setGain(volume.get());
		sound->setLoop(1);
		sound->setOcclusion(0);
		sound->stop();
		slide_sound_timer = 0;
	}

	is_grabbed = 0;

	linear_move.speed_buffer.resize(10, 0.0f);
	angular_move.speed_buffer.resize(10, 0.0f);

	is_vr_mode = VRPlayer::isVRPluginLoaded();
}

void InteractableHandle::update()
{
	if (!is_vr_mode)
		return;

	HandleState handle_state = HandleState::NONE;

	if (!is_grabbed && inertial_enabled.get())
	{
		if (change_position.get() && linear_move.apply_inertia)
		{
			handle_state = MOVE;

			Vec3 pos = node->getPosition();
			Vec3 clamped_pos = clamp(pos, Vec3(handle_min_pos), Vec3(handle_max_pos));
			if (length2(clamped_pos - pos) > UNIGINE_EPSILON)
			{
				// stop it! we touch some border!
				linear_move.speed = 0.0f;
				linear_move.apply_inertia = false;

				node->setPosition(clamped_pos);
				handle_state = STOP;
			}
			else
			{
				// smooth slow down the handle if user dont touch it
				linear_move.speed -= linear_move.speed_sign * linear_acceleration.get() * Game::getIFps();
				linear_move.apply_inertia = (linear_move.speed_signbit == std::signbit(linear_move.speed));

				if (!linear_move.apply_inertia) handle_state = NONE;
			}


			if (body)
				body->setLinearVelocity(linear_move.direction * linear_move.speed);
			else
				node->setWorldPosition(node->getWorldPosition() + (Vec3)linear_move.direction * linear_move.speed  * Game::getIFps());
		}

		if (change_rotation.get() && angular_move.apply_inertia)
		{
			auto rot = node->getRotation() * quat(angular_move.direction, angular_move.speed * Game::getIFps());

			angular_move.speed -= angular_move.speed_sign * angular_acceleration * Game::getIFps();
			angular_move.apply_inertia = (angular_move.speed_signbit == std::signbit(angular_move.speed));

			handle_state = MOVE;

			vec3 angles = clamp<3>(decomposeRotationXYZ(rot.getMat3()), min_rot.get(), max_rot.get(), angular_move.was_clamped);
			if (angular_move.was_clamped != InteractableHandle::ClampState::None)
			{
				rot = quat(angles.x, angles.y, angles.z);
				angular_move.apply_inertia = false;

				if (angular_move.was_clamped == InteractableHandle::ClampState::MaxClamp)
				{
					SoundManager::get()->playSound(
						max_clamp_sound,
						node->getWorldPosition(),
						volume.get(), // volume
						Game::getRandomFloat(0.9f, 1.1f),
						sound_min_dist,
						sound_max_dist);
				}
				else if (angular_move.was_clamped == InteractableHandle::ClampState::MinClamp)
				{
					SoundManager::get()->playSound(
						min_clamp_sound,
						node->getWorldPosition(),
						volume.get(), // volume
						Game::getRandomFloat(0.9f, 1.1f),
						sound_min_dist,
						sound_max_dist);
				}

				handle_state = STOP;
			}
			node->setRotation(rot);
		}
	}

	// change pitch of sound if user uses "Slow Motion" effect
	if (sound) sound->setPitch(Game::getScale());

	switch (handle_state)
	{
	case STOP:
		if (sound) sound->stop();
		SoundManager::get()->playSound(
			sound_stop,
			node->getWorldPosition(),
			volume.get(), // volume
			Game::getRandomFloat(0.9f, 1.1f),
			sound_min_dist,
			sound_max_dist);
		break;
	case MOVE:
		if (sound) sound->play();
		break;
	case NONE:
	default:
		if(sound) sound->stop();
		break;
	}
}

void InteractableHandle::update_linear_buffer(bool force)
{
	linear_move.speed_buffer.push_back(linear_move.speed);
	linear_move.speed_buffer.pop_front();
}

void InteractableHandle::update_angular_buffer(bool force)
{
	quat rot = node->getRotation();

	float a1 = rot.getAngle(angular_move.direction);
	float a2 = last_handle_rot.getAngle(angular_move.direction);
	float delta = a1 - a2;

	angular_move.speed_buffer.push_back(delta / Game::getIFps());
	angular_move.speed_buffer.pop_front();
	last_handle_rot = rot;
}

void InteractableHandle::grabIt(VRPlayer *player, int hand_num)
{
	if (!is_vr_mode)
	{
		VRInteractable::grabIt(player, hand_num);
		return;
	}

	// handle_state = NONE;

	SoundManager::get()->playSound(
		sound_start,
		node->getWorldPosition(),
		volume.get(),
		Game::getRandomFloat(0.8f, 1.2f),
		sound_min_dist,
		sound_max_dist);

	last_handle_pos = node->getWorldPosition();
	if (player->getHandDegreesOfFreedom(hand_num) < 6) // PC
	{
		PlayerPtr p = Game::getPlayer();
		Vec3 p1 = p->getCamera()->getPosition();
		Vec3 p2 = p1 + Vec3(p->getViewDirection()) * 1;
		ObjectPtr hit_obj = World::getIntersection(p1, p2, 1, intersection);
		if (hit_obj)
		{
			grab_hand_pos = intersection->getPoint();
		}
		length_hand_pc = length(grab_hand_pos - player->getHead()->getWorldPosition());
	}
	else //VR
		grab_hand_pos = player->getHandNode(hand_num)->getWorldPosition();

	grab_obj_pos = node->getWorldPosition();
	grab_rotate = node->getRotation();

	// set ignoring of the handle from player's raycast (intersections)
	for (int i = 0; i < obj->getNumSurfaces(); i++)
		obj->setIntersection(0, i);

	last_handle_rot = grab_rotate;

	is_grabbed = 1;

	VRInteractable::grabIt(player, hand_num);
}

void InteractableHandle::holdIt(VRPlayer *player, int hand_num)
{
	if (!is_vr_mode)
	{
		VRInteractable::holdIt(player, hand_num);
		return;
	}

	NodePtr hand = player->getHandNode(hand_num);

	// change and clamp position
	if (change_position.get())
	{
		Vec3 handle_pos = last_handle_pos;

		Vec3 hand_offset = hand->getWorldPosition() - grab_hand_pos;
		Vec3 prev_pos = node->getWorldPosition();
		Vec3 target_pos = grab_obj_pos + hand_offset;										// world coordinates
		NodePtr parent = node->getParent();
		if (parent)
		{
			target_pos = parent->getIWorldTransform() * target_pos;							// local coordinates
			target_pos = clamp<3, 1>(target_pos, Vec3(handle_min_pos.get()), Vec3(handle_max_pos.get()), linear_move.was_clamped);		// clamp local coords
			target_pos = parent->getWorldTransform() * target_pos;							// return to world
		}
		else
			target_pos = clamp<3, 1>(target_pos, Vec3(handle_min_pos.get()), Vec3(handle_max_pos.get()), linear_move.was_clamped);

		handle_pos = target_pos;

		// calculate velocity
		vec3 target_speed = vec3(target_pos - prev_pos) / Game::getIFps();
		linear_move.speed = target_speed.length();
		linear_move.direction = target_speed.normalize();

		if (body)
			body->setLinearVelocity(target_speed);
		else
		{
			node->setWorldPosition(target_pos);
			if (inertial_enabled) update_linear_buffer();
		}

		// slide sound
		if (length(handle_pos - last_handle_pos) / Game::getIFps() >= 0.3f)
			slide_sound_timer = MIN_SLIDE_SOUND_TIME;

		last_handle_pos = handle_pos;

		if (linear_move.was_clamped != InteractableHandle::ClampState::None)
		{
			if (!border_reached)
			{
				SoundManager::get()->playSound(
					sound_stop,
					node->getWorldPosition(),
					1, // volume
					Game::getRandomFloat(0.9f, 1.1f),
					sound_min_dist,
					sound_max_dist);
				border_reached = true;
			}
		}
		else border_reached = false;
	}

	// change rotation
	if (change_rotation.get())
	{
		Vec3 hand_pos;
		if (player->getHandDegreesOfFreedom(hand_num) < 6) // PC
		{
			PlayerPtr p = Game::getPlayer();
			hand_pos = p->getCamera()->getPosition() + Vec3(p->getViewDirection()) * length_hand_pc;
		}
		else // VR
			hand_pos = hand->getWorldPosition();

		Vec3 current_active_axis = ACTIVE_AXIS[axis.get()];

		auto iwt = node->getParent() ? node->getParent()->getIWorldTransform() : Mat4::IDENTITY;
		auto node_pos = node->getPosition();
		auto local_hand_pos = iwt * hand_pos - node_pos;
		auto local_grab_hand_pos = iwt * grab_hand_pos - node_pos;

		auto new_rot_vec = local_hand_pos * current_active_axis;
		auto old_rot_vec = local_grab_hand_pos * current_active_axis;
		auto rot_vec = cross(new_rot_vec, old_rot_vec);
		float angle = Math::getAngle((vec3)old_rot_vec, (vec3)new_rot_vec, (vec3)rot_vec);
		auto rot = quat(vec3(rot_vec), angle) * grab_rotate;

		vec3 angles = clamp<3>(decomposeRotationXYZ(rot.getMat3()), min_rot.get(), max_rot.get(), angular_move.was_clamped);
		auto d_angles = angles - decomposeRotationXYZ(node->getRotation().getMat3());
		rot = quat(angles.x, angles.y, angles.z);

		node->setRotation(rot);

		if (Math::abs(d_angles.x) >= 1.0f || Math::abs(d_angles.y) >= 1.0f || Math::abs(d_angles.z) >= 1.0f)
			slide_sound_timer = MIN_SLIDE_SOUND_TIME;

		if (angular_move.was_clamped != InteractableHandle::ClampState::None)
		{
			if (!border_reached)
			{
				SoundManager::get()->playSound(
					sound_stop,
					node->getWorldPosition(),
					volume.get(), // volume
					Game::getRandomFloat(0.9f, 1.1f),
					sound_min_dist,
					sound_max_dist);
				border_reached = true;

				if (angular_move.was_clamped == InteractableHandle::ClampState::MaxClamp)
				{
					SoundManager::get()->playSound(
						max_clamp_sound,
						node->getWorldPosition(),
						volume.get(), // volume
						Game::getRandomFloat(0.9f, 1.1f),
						sound_min_dist,
						sound_max_dist);
				}
				else if (angular_move.was_clamped == InteractableHandle::ClampState::MinClamp)
				{
					SoundManager::get()->playSound(
						min_clamp_sound,
						node->getWorldPosition(),
						volume.get(), // volume
						Game::getRandomFloat(0.9f, 1.1f),
						sound_min_dist,
						sound_max_dist);
				}
			}
		}
		else border_reached = false;

		if (inertial_enabled) update_angular_buffer();

		last_handle_pos = hand->getWorldPosition();
	}

	// play slide sound
	if (slide_sound_timer > 0)
	{
		if (sound) sound->play();
		player->vibrateController(hand_num, 1.0f);
		slide_sound_timer = saturate(slide_sound_timer - Game::getIFps());
	}
	else
	{
		if (sound) sound->stop();
		player->vibrateController(hand_num, 0);
	}

	last_hand_pos = hand->getWorldPosition();

	VRInteractable::holdIt(player, hand_num);
}

void InteractableHandle::throwIt(VRPlayer *player, int hand_num)
{
	/*if (!is_vr_mode)
	{
		VRInteractable::throwIt(player, hand_num);
		return;
	}*/

	if (sound) sound->stop();

	if ((change_rotation.get() && !angular_move.was_clamped) || (change_position.get() && !linear_move.was_clamped))
	{
		SoundManager::get()->playSound(
			sound_stop,
			node->getWorldPosition(),
			Game::getScale(), // volume
			Game::getRandomFloat(0.9f, 1.1f),
			sound_min_dist,
			sound_max_dist);
	}

	// handle_state = NONE;

	// revert changes
	for (int i = 0; i < obj->getNumSurfaces(); i++)
		obj->setIntersection(1, i);

	if (inertial_enabled)
	{
		if (change_position.get())
		{
			update_linear_buffer(true);

			linear_move.speed = 0;
			for (size_t i = 0; i < linear_move.speed_buffer.size(); i++)
				linear_move.speed += linear_move.speed_buffer[i];
			linear_move.speed /= linear_move.speed_buffer.size();

			linear_move.apply_inertia = true;
			linear_move.speed_signbit = std::signbit(linear_move.speed);
			linear_move.speed_sign = linear_move.speed_signbit ? -1.0f : 1.0f;

			linear_move.speed_buffer.clear();
			linear_move.speed_buffer.resize(10, 0.0f);
		}

		if (change_rotation.get())
		{
			update_angular_buffer(true);

			angular_move.speed = 0;
			for (size_t i = 0; i < angular_move.speed_buffer.size(); i++)
				angular_move.speed += angular_move.speed_buffer[i];
			angular_move.speed /= angular_move.speed_buffer.size();

			angular_move.apply_inertia = true;
			angular_move.speed_signbit = std::signbit(angular_move.speed);
			angular_move.speed_sign = angular_move.speed_signbit ? -1.0f : 1.0f;

			angular_move.speed_buffer.clear();
			angular_move.speed_buffer.resize(10, 0.0f);

			hand_dir = player->getHandNode(hand_num)->getWorldPosition() - last_hand_pos;
			hand_dir = hand_dir.normalize();

			// for XYZ not work
			angular_move.direction = vec3(1, 1, 1) - (vec3)ACTIVE_AXIS[axis.get()];
		}
	}

	is_grabbed = 0;

	VRInteractable::throwIt(player, hand_num);
}