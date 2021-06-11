#include "CameraAttachComponent.h"
#include <UnigineGame.h>

REGISTER_COMPONENT(CameraAttachComponent);


void CameraAttachComponent::init()
{
	need_follow_pos = follow_position.get() > 0;
	offset = Math::Vec3(position_offset.get());
	need_follow_rot = follow_rotation.get() > 0;
	rot = Math::composeRotationXYZ(rotation_offset.get()).getRotate();
	local_offset = is_local_offset.get() > 0;
}

void CameraAttachComponent::update()
{
	if (!need_follow_pos && !need_follow_rot)
		return;

	auto player = Game::getPlayer();
	if (player)
	{
		auto new_transform = node->getWorldTransform();
		auto transform = player->getWorldTransform();
		if (need_follow_rot)
		{
			Math::quat new_rot;
			new_rot = transform.getRotate() * rot;
			new_transform = Math::Mat4(new_rot, new_transform.getTranslate());
		}
		if (need_follow_pos)
		{
			Math::Vec3 new_pos;
			if (local_offset)
			{
				mul(new_pos, transform, offset);
			}
			else
			{
				new_pos = transform.getTranslate() + offset;
			}
			new_transform.setColumn3(3, new_pos);
		}

		node->setWorldTransform(new_transform);
	}
}