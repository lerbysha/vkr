#include "ObjMouseMovable.h"
#include <UnigineInput.h>


REGISTER_COMPONENT(ObjMouseMovable);


void ObjMouseMovable::grabIt(VRPlayer* player, int hand_num)
{
	if (VRPlayer::isVRPluginLoaded())
	{
		ObjMovable::grabIt(player, hand_num);
		return;
	}

	local_transform = player->getHead()->getIWorldTransform() * node->getWorldTransform();
	grab_x = App::getWidth() / 2.0f;
	grab_y = App::getHeight() / 2.0f;
	offset = Vec3::ZERO;

	ObjMovable::grabIt(player, hand_num);
}

void ObjMouseMovable::holdIt(VRPlayer *player, int hand_num)
{
	if (VRPlayer::isVRPluginLoaded())
	{
		ObjMovable::holdIt(player, hand_num);
		return;
	}

	if (use_handy_pos)
	{
		float ifps = Game::getIFps() / Game::getScale();

		ivec2 mouse_pos = Input::getMouseCoord();
		offset.x = (mouse_pos.x - grab_x) / App::getWidth();
		offset.y = (grab_y - mouse_pos.y) / App::getHeight();
		offset.z -= 0.02f * Input::getMouseWheel();

		setTransformRotation(local_transform, handy_rot_quat);
		float k = saturate(handy_pos_factor * ifps);
		setTransformPosition(local_transform, lerp(local_transform.getTranslate(), (Vec3)handy_pos.get() + offset, k));

		Mat4 result = player->getHead()->getWorldTransform() * local_transform;
		node->setWorldTransform(result);
	}

	VRInteractable::holdIt(player, hand_num);
}

void ObjMouseMovable::movable_init()
{
	handy_rot_quat = Math::quat(handy_rot_euler.get().x, handy_rot_euler.get().y, handy_rot_euler.get().z);
}