#include "VRPlayerOpenVR.h"
#include <UnigineVisualizer.h>
#include <UnigineGame.h>
#include "../../Utils.h"
#include <plugins/UnigineAppVive.h>

#define HIDE_BASESTATIONS

REGISTER_COMPONENT(VRPlayerOpenVR);

using namespace Unigine;
using namespace Math;
using namespace Plugins;

void VRPlayerOpenVR::init()
{
	VRPlayer::init();
	vive = AppVive::get();
	find_devices();
	load_input_config();
	init_player();
	load_player_height(0);
	landPlayerTo(player->getWorldPosition(), player->getWorldDirection());
	controllers_init();
	basestation[0] = checked_ptr_cast<NodeReference>(basestation_0.get());
	basestation[1] = checked_ptr_cast<NodeReference>(basestation_1.get());
#ifdef HIDE_BASESTATIONS
	basestation[0]->setEnabled(0);
	basestation[1]->setEnabled(0);
#endif
	teleport_init();
	grab_init();
}

void VRPlayerOpenVR::update()
{
	find_devices();

	controller_valid[0] = CONTROLLER_DEVICE_0 != -1 && vive->isDeviceConnected(CONTROLLER_DEVICE_0) && vive->isPoseValid(CONTROLLER_DEVICE_0);
	controller_valid[1] = CONTROLLER_DEVICE_1 != -1 && vive->isDeviceConnected(CONTROLLER_DEVICE_1) && vive->isPoseValid(CONTROLLER_DEVICE_1);

	// if system menu is open
	if ((controller_valid[0] && vive->getControllerButtonPressed(CONTROLLER_DEVICE_0, AppVive::BUTTON_SYSTEM)) ||
		(controller_valid[1] && vive->getControllerButtonPressed(CONTROLLER_DEVICE_1, AppVive::BUTTON_SYSTEM)))
	{
		for (int i = 0; i < 2; i++)
		{
			controller_ref[i]->setEnabled(0);
			teleport_button_pressed[i] = 0;
		}
		teleport_marker->setEnabled(0);
		teleport_ray->setEnabled(0);

		return;
	}

	Mat4 player_transform = player->getWorldTransform();
	Mat4 hmd_transform = Mat4(vive->getDeviceTransform(HMD_DEVICE_0));
	Mat4 hmd_transform_world = player_transform * hmd_transform;
	Vec3 head_offset =
		(vive->isDeviceConnected(HMD_DEVICE_0) && HMD_DEVICE_0 != -1) ?
		player_transform.getTranslate() - hmd_transform_world.getTranslate() :
		Vec3::ZERO;
	head_offset.z = 0;

	head->setWorldTransform(hmd_transform_world);

	if (!is_vive_controlles)
	{
		// move and rotate head
		vec2 axis;
		axis.x = getControllerAxis(move_controller, move_axis_x);
		axis.y = getControllerAxis(move_controller, move_axis_y);
		move_update_touch(axis, player_transform * Mat4(vive->getDeviceTransform(move_controller == 0 ? CONTROLLER_DEVICE_0 : CONTROLLER_DEVICE_1)), hmd_transform_world);

		axis.x = getControllerAxis(turn_controller, turn_axis);
		axis.y = 0;
		turn_update_touch(axis, hmd_transform_world);
	}

#ifndef HIDE_BASESTATIONS
	basestation_update(0, player_transform, BASESTATION_DEVICE_0);
	basestation_update(1, player_transform, BASESTATION_DEVICE_1);
#endif
	controller_update(0, player_transform, CONTROLLER_DEVICE_0);
	controller_update(1, player_transform, CONTROLLER_DEVICE_1);

	if (teleport_controller == 2)
	{
		teleport_update(0, getControllerButton(0, teleport_button), head_offset);
		teleport_update(1, getControllerButton(1, teleport_button), head_offset);
	}
	else
		teleport_update(teleport_controller, getControllerButton(teleport_controller, teleport_button), head_offset);

	move_update(hmd_transform_world);
	collisions_update(hmd_transform_world, head_offset);

	grab_update(0, controller_valid[0], getControllerAxis(0, grab_button), getControllerButtonDown(0, grab_button));
	grab_update(1, controller_valid[1], getControllerAxis(1, grab_button), getControllerButtonDown(1, grab_button));

	quat rot = player->getRotation();
	if (CONTROLLER_DEVICE_0 != -1)
	{
		push_hand_linear_velocity(0, rot * vive->getDeviceVelocity(CONTROLLER_DEVICE_0) * hand_force_multiplier);
		push_hand_angular_velocity(0, rot * vive->getDeviceAngularVelocity(CONTROLLER_DEVICE_0));
	}
	if (CONTROLLER_DEVICE_1 != -1)
	{
		push_hand_linear_velocity(1, rot * vive->getDeviceVelocity(CONTROLLER_DEVICE_1) * hand_force_multiplier);
		push_hand_angular_velocity(1, rot * vive->getDeviceAngularVelocity(CONTROLLER_DEVICE_1));
	}

	update_gui();
}

void VRPlayerOpenVR::setLock(bool lock)
{
	vive->setHeadPositionLock(lock);
	vive->setHeadRotationLock(lock);
}

void VRPlayerOpenVR::setPlayerPosition(const Vec3 &pos)
{
	put_head_to_position(vive->getDeviceTransform(HMD_DEVICE_0), pos);
}

void VRPlayerOpenVR::landPlayerTo(const Vec3 &position, const vec3 &direction)
{
	land_player_to(vive->getDeviceTransform(HMD_DEVICE_0), position, direction);
}

void VRPlayerOpenVR::basestation_update(int num, const Mat4 &player_transform, int device_id)
{
	if (device_id != -1 && vive->isDeviceConnected(device_id))
	{
		mat4 transform = vive->getDeviceTransform(device_id);
		basestation[num]->setWorldTransform(player_transform * Mat4(transform));
		basestation[num]->setEnabled(1);
	}
	else
		basestation[num]->setEnabled(0);
}

void VRPlayerOpenVR::controller_update(int num, const Mat4 &player_transform, int device_id)
{
	if (device_id != -1 && vive->isDeviceConnected(device_id))
	{
		mat4 transform = vive->getDeviceTransform(device_id);
		controller[num]->setWorldTransform(player_transform * Mat4(transform) * Mat4(rotateX(-90.0f)));

		// update buttons
		float trigger_position = vive->getControllerAxis(device_id, 1).x;
		set_controller_trigger_position(controller[num], trigger_position);

		if (controller[num]->isEnabled())
			controller_ref[num]->setEnabled(1);
	}
	else if (controller[num]->isEnabled())
		controller_ref[num]->setEnabled(0);
}

void VRPlayerOpenVR::set_controller_trigger_position(NodeReferencePtr &in_controller, float position)
{
	NodePtr root = in_controller->getReference();
	int pivot_id = root->findChild("vive_trigger");
	if (pivot_id != -1)
	{
		NodePtr pivot_node = root->getChild(pivot_id);
		pivot_node->setTransform(Mat4(translate(0.0f, -0.039f, -0.018f) * rotateX(-position * 20.0f)));
	}
}

void VRPlayerOpenVR::find_devices()
{
	int curr_tracking = 0;
	int curr_controller = 0;
	for (int i = 0; i < vive->getMaxTrackedDeviceCount(); i++)
	{
		int deviceType = vive->getDeviceType(i);
		switch (deviceType)
		{
		case AppVive::DEVICE_TRACKING:
			if (curr_tracking == 0)
				BASESTATION_DEVICE_0 = i;
			else
				BASESTATION_DEVICE_1 = i;
			curr_tracking++;
			break;
		case AppVive::DEVICE_CONTROLLER:
			if (curr_controller == 0)
				CONTROLLER_DEVICE_0 = i;
			else
				CONTROLLER_DEVICE_1 = i;
			curr_controller++;

			switch (vive->getControllerRole(i))
			{
			case AppVive::CONTROLLER_ROLE_LEFT_HAND:  CONTROLLER_DEVICE_0 = i; break;
			case AppVive::CONTROLLER_ROLE_RIGHT_HAND: CONTROLLER_DEVICE_1 = i; break;
			default: break;
			}
			break;
		case AppVive::DEVICE_HMD:
			HMD_DEVICE_0 = i;
			break;
		default:
			break;
		}
	}
}

void VRPlayerOpenVR::load_input_config()
{
	PropertyPtr config = Properties::findProperty("open_vr_input");
	if (config)
	{
		PropertyParameterPtr binds = config->getParameterPtr("binds");
		if (binds)
		{
			auto switch_to_button = [](int button)
			{
				switch (button)
				{
				case 0: return static_cast<int>(AppVive::BUTTON_SYSTEM);
				case 1: return static_cast<int>(AppVive::BUTTON_APPLICATIONMENU);
				case 2: return static_cast<int>(AppVive::BUTTON_GRIP);
				case 3: return static_cast<int>(AppVive::BUTTON_DPAD_LEFT);
				case 4: return static_cast<int>(AppVive::BUTTON_DPAD_UP);
				case 5: return static_cast<int>(AppVive::BUTTON_DPAD_RIGHT);
				case 6: return static_cast<int>(AppVive::BUTTON_DPAD_DOWN);
				case 7: return static_cast<int>(AppVive::BUTTON_A);
				case 8: return static_cast<int>(AppVive::BUTTON_AXIS0);
				case 9: return static_cast<int>(AppVive::BUTTON_AXIS1);
				case 10: return static_cast<int>(AppVive::BUTTON_AXIS2);
				case 11: return static_cast<int>(AppVive::BUTTON_AXIS3);
				case 12: return static_cast<int>(AppVive::BUTTON_AXIS4);
				case 13: return static_cast<int>(AppVive::BUTTON_STEAMVR_TOUCHPAD);
				case 14: return static_cast<int>(AppVive::BUTTON_STEAMVR_TRIGGER);
				case 15: return static_cast<int>(AppVive::BUTTON_DASHBOARD_BACK);
				default: return -1;
				}
			};

			auto switch_to_axis = [](int axis)
			{
				switch (axis)
				{
				case 0: return static_cast<int>(AppVive::AXIS_NONE);
				case 1: return static_cast<int>(AppVive::AXIS_TRACKPAD);
				case 2: return static_cast<int>(AppVive::AXIS_JOYSTICK);
				case 3: return static_cast<int>(AppVive::AXIS_TRIGGER);
				default: return -1;
				}
			};

			String left_controller_name = vive->getDeviceModelNumber(CONTROLLER_DEVICE_0);
			String right_controller_name = vive->getDeviceModelNumber(CONTROLLER_DEVICE_1);

			if (left_controller_name.contains("vive", 0) || right_controller_name.contains("vive", 0))
				is_vive_controlles = true;

			int presets_count = binds->getNumChildren();
			int i = 0;
			for (; i < presets_count; i++)
			{
				PropertyParameterPtr preset = binds->getChild(i);
				String left_name = preset->getChild("left_controller_model_name")->getValueString();
				String right_name = preset->getChild("right_controller_model_name")->getValueString();

				if (!strcmp(left_controller_name, left_name) && !strcmp(right_controller_name, right_name))
				{
					buttons_binds.insert(VRPlayer::XA, switch_to_button(preset->getChild("xa_button")->getValueInt()));
					buttons_binds.insert(VRPlayer::YB, switch_to_button(preset->getChild("yb_button")->getValueInt()));
					buttons_binds.insert(VRPlayer::MENU, switch_to_button(preset->getChild("menu_button")->getValueInt()));
					buttons_binds.insert(VRPlayer::GRIP, switch_to_button(preset->getChild("grip_button")->getValueInt()));
					buttons_binds.insert(VRPlayer::TRIGGER, switch_to_button(preset->getChild("trigger_button")->getValueInt()));

					axes_binds.insert(VRPlayer::TRIGGER, switch_to_axis(preset->getChild("trigger_axis")->getValueInt()));
					axes_binds.insert(VRPlayer::GRIP, switch_to_axis(preset->getChild("grip_axis")->getValueInt()));
					axes_binds.insert(VRPlayer::STICK_X, switch_to_axis(preset->getChild("stick_xy_axis")->getValueInt()));
					axes_binds.insert(VRPlayer::STICK_Y, switch_to_axis(preset->getChild("stick_xy_axis")->getValueInt()));

					break;
				}
			}

			if (i == presets_count)
			{
				for (i = 0; i < presets_count; i++)
				{
					PropertyParameterPtr preset = binds->getChild(i);
					String left_name = preset->getChild("left_controller_model_name")->getValueString();
					String right_name = preset->getChild("right_controller_model_name")->getValueString();

					if (!strcmp(left_name, "default") && !strcmp(right_name, "default"))
					{
						buttons_binds.insert(VRPlayer::XA, switch_to_button(preset->getChild("xa_button")->getValueInt()));
						buttons_binds.insert(VRPlayer::YB, switch_to_button(preset->getChild("yb_button")->getValueInt()));
						buttons_binds.insert(VRPlayer::MENU, switch_to_button(preset->getChild("menu_button")->getValueInt()));
						buttons_binds.insert(VRPlayer::GRIP, switch_to_button(preset->getChild("grip_button")->getValueInt()));
						buttons_binds.insert(VRPlayer::TRIGGER, switch_to_button(preset->getChild("trigger_button")->getValueInt()));

						axes_binds.insert(VRPlayer::TRIGGER, switch_to_axis(preset->getChild("trigger_axis")->getValueInt()));
						axes_binds.insert(VRPlayer::GRIP, switch_to_axis(preset->getChild("grip_axis")->getValueInt()));
						axes_binds.insert(VRPlayer::STICK_X, switch_to_axis(preset->getChild("stick_xy_axis")->getValueInt()));
						axes_binds.insert(VRPlayer::STICK_Y, switch_to_axis(preset->getChild("stick_xy_axis")->getValueInt()));

						break;
					}
				}
			}
		}
	}
}

void VRPlayerOpenVR::vibrateController(int controller_num, float amplitude)
{
	if (xpad360->isAvailable())
	{
		if (controller_num == 0) xpad360->setLeftMotor(amplitude * 0.2f);
		if (controller_num == 1) xpad360->setRightMotor(amplitude * 0.2f);
	}

	if (amplitude > 0)
	{
		if (controller_num == 0) vive->setControllerVibration(CONTROLLER_DEVICE_0, 100);
		if (controller_num == 1) vive->setControllerVibration(CONTROLLER_DEVICE_1, 100);
	}
}

int VRPlayerOpenVR::getUseButtonState(int controller_num)
{
	return getControllerButton(controller_num, use_button);
}

int VRPlayerOpenVR::getUseButtonDown(int controller_num)
{
	return getControllerButtonDown(controller_num, use_button);
}

void VRPlayerOpenVR::move_update_touch(const vec2 &axis, const Mat4 &controller_transform, const Mat4 &head_transform)
{
	if (Math::abs(axis.x) > dead_zone || Math::abs(axis.y) > dead_zone)
	{
		auto move_dir = controller_transform.getRotate() * Vec3(axis.x, 0, -axis.y);
		move_dir.z = 0.0f;
		auto move_vec = head_transform.getTranslate() + move_dir * Game::getIFps() / Game::getScale();

		static const auto MOVE_OBSTACLE_DIST = 0.4;
		auto wi = WorldIntersection::create();
		if (World::getIntersection(head_transform.getTranslate(), move_vec + move_dir.normalize() * Scalar(MOVE_OBSTACLE_DIST), 1, wi)) return;

		landPlayerTo(move_vec, vec3(-head_transform.getAxisZ()));
	}
}

void VRPlayerOpenVR::turn_update_touch(const vec2 &axis, const Mat4 &head_transform)
{
	if (Math::abs(axis.x) > dead_zone)
	{
		odiscrete_timer -= Game::getIFps() / Game::getScale();
		if (odiscrete_timer < 0)
		{
			quat rot = quat(0, 0, -Math::sign(axis.x) * 75.0f * step);
			Vec3 offset0 = head_transform.getTranslate() - player->getWorldPosition();
			Vec3 offset1 = rot * offset0;
			player->setPosition(player->getPosition() + offset0 - offset1);
			player->setRotation(rot * player->getRotation());

			odiscrete_timer = step;
		}
	}
	else
		odiscrete_timer = 0;
}

Mat4 VRPlayerOpenVR::getControllerTransform(int controller_num)
{
	return Mat4(controller_num == 0 ? vive->getDeviceTransform(CONTROLLER_DEVICE_0) : vive->getDeviceTransform(CONTROLLER_DEVICE_1));
}

int VRPlayerOpenVR::getControllerButton(int controller_num, int button)
{
	if (!controller_valid[controller_num])
		return 0;

	int device = (controller_num == 0 ? CONTROLLER_DEVICE_0 : CONTROLLER_DEVICE_1);

	switch (button)
	{
	case STICK_X: return Math::abs(vive->getControllerAxis(device, axes_binds[STICK_X]).x) > 0.5f;
	case STICK_Y: return Math::abs(vive->getControllerAxis(device, axes_binds[STICK_Y]).y) > 0.5f;
	case TRIGGER: return vive->getControllerButtonPressed(device, static_cast<AppVive::BUTTON>(buttons_binds[TRIGGER])) ||
		vive->getControllerAxis(device, axes_binds[TRIGGER]).x > 0.5f;
	case GRIP: return vive->getControllerButtonPressed(device, static_cast<AppVive::BUTTON>(buttons_binds[GRIP])) ||
		vive->getControllerAxis(device, axes_binds[GRIP]).x > 0.5f;
	case XA:
		if (buttons_binds[XA] < AppVive::BUTTON_DPAD_LEFT || AppVive::BUTTON_DPAD_DOWN < buttons_binds[XA])
			return vive->getControllerButtonPressed(device, static_cast<AppVive::BUTTON>(buttons_binds[XA]));
		return vive_getControllerDPadPressed(device, buttons_binds[XA]);

	case YB:
		if (buttons_binds[YB] < AppVive::BUTTON_DPAD_LEFT || AppVive::BUTTON_DPAD_DOWN < buttons_binds[YB])
			return vive->getControllerButtonPressed(device, static_cast<AppVive::BUTTON>(buttons_binds[YB]));
		return vive_getControllerDPadPressed(device, buttons_binds[YB]);

	case MENU:
		if (buttons_binds[MENU] < AppVive::BUTTON_DPAD_LEFT || AppVive::BUTTON_DPAD_DOWN < buttons_binds[MENU])
			return vive->getControllerButtonPressed(device, static_cast<AppVive::BUTTON>(buttons_binds[MENU]));
		return vive_getControllerDPadPressed(device, buttons_binds[MENU]);
	}

	return 0;
}

float VRPlayerOpenVR::getControllerAxis(int controller_num, int button)
{
	if (!controller_valid[controller_num])
		return 0;

	int device = (controller_num == 0 ? CONTROLLER_DEVICE_0 : CONTROLLER_DEVICE_1);

	switch (button)
	{
	case STICK_X:
		if (axes_binds[STICK_X] == AppVive::AXIS_NONE && vive->getControllerButtonPressed(device, AppVive::BUTTON_STEAMVR_TOUCHPAD))
			return 0.0f;
		return vive->getControllerAxis(device, axes_binds[STICK_X]).x;

	case STICK_Y:
		if (axes_binds[STICK_Y] == AppVive::AXIS_NONE && vive->getControllerButtonPressed(device, AppVive::BUTTON_STEAMVR_TOUCHPAD))
			return 0.0f;
		return vive->getControllerAxis(device, axes_binds[STICK_Y]).y;

	case TRIGGER: return vive->getControllerAxis(device, axes_binds[TRIGGER]).x;
	case GRIP: return (vive->getControllerAxis(device, axes_binds[GRIP])).x;
	case XA: return itof(vive_getControllerDPadPressed(device, static_cast<AppVive::BUTTON>(buttons_binds[XA])));
	case YB: return itof(vive_getControllerDPadPressed(device, static_cast<AppVive::BUTTON>(buttons_binds[YB])));
	case MENU: return itof(vive->getControllerButtonPressed(device, static_cast<AppVive::BUTTON>(buttons_binds[MENU])));
	}

	return 0;
}

int VRPlayerOpenVR::vive_getControllerDPadPressed(int device, int button)
{
	// You might expect that pressing one of the edges of the SteamVR controller touchpad could
	// be detected with a call to k_EButton_DPad_* (BUTTON_DPAD_*), but currently this always returns false.
	// Not sure whether this is SteamVR's design intent, not yet implemented, or a bug.
	// The expected behaviour can be achieved by detecting overall Touchpad press, with Touch-Axis comparison to an edge threshold.

	if (vive->getControllerButtonPressed(device, AppVive::BUTTON_STEAMVR_TOUCHPAD))
	{
		vec2 axis = vive->getControllerAxis(device, AppVive::AXIS_NONE);
		if ((axis.y > 0.3f && button == AppVive::BUTTON_DPAD_UP) ||
			(axis.y < -0.3f && button == AppVive::BUTTON_DPAD_DOWN) ||
			(axis.x > 0.6f && button == AppVive::BUTTON_DPAD_RIGHT) ||
			(axis.x < -0.6f && button == AppVive::BUTTON_DPAD_LEFT))
			return 1;
	}

	return 0;
}