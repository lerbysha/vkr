#pragma once
#include <UnigineComponentSystem.h>

using namespace Unigine;

class CameraAttachComponent : public Unigine::ComponentBase
{
public:	
	COMPONENT(CameraAttachComponent, Unigine::ComponentBase);
	PROP_NAME("CameraAttachComponent");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Toggle, follow_position, 1);
	PROP_PARAM(Vec3, position_offset);
	PROP_PARAM(Toggle, is_local_offset, 1);
	PROP_PARAM(Toggle, follow_rotation, 0);
	PROP_PARAM(Vec3, rotation_offset);

private:
	void init();
	void update();

	bool need_follow_pos = true;
	bool local_offset = true;
	Math::Vec3 offset = Math::Vec3::ZERO;
	bool need_follow_rot = false;
	Math::quat rot = Math::quat::ZERO;
};

