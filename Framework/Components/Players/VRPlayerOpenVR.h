#pragma once
#include <UnigineEngine.h>
#include <UnigineEditor.h>
#include <UniginePlayers.h>
#include <UnigineLogic.h>

#include "VRPlayerVR.h"

namespace Unigine
{
	namespace Plugins
	{
		class AppVive;
	}
}

class VRPlayerOpenVR : public VRPlayerVR
{
public:
	COMPONENT(VRPlayerOpenVR, VRPlayerVR);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// property
	PROP_NAME("player_vive");
	PROP_AUTOSAVE(1);

	// parameters
	PROP_PARAM(Node, basestation_0);
	PROP_PARAM(Node, basestation_1);

	// player
	void setLock(bool lock) override;
	void setPlayerPosition(const Unigine::Math::Vec3 &pos) override;
	void landPlayerTo(const Unigine::Math::Vec3 &position, const Unigine::Math::vec3 &direction) override;

	// hands
	Unigine::Math::Mat4 getControllerTransform(int controller_num) override;
	int getControllerButton(int controller_num, int button) override;
	float getControllerAxis(int controller_num, int button) override;
	void vibrateController(int controller_num, float amplitude = 1.0f) override;
	int getUseButtonState(int controller_num) override;
	int getUseButtonDown(int controller_num) override;

protected:
	// main loop
	void init();
	void update();

private:
	void set_controller_trigger_position(Unigine::NodeReferencePtr &controller, float position);
	void find_devices();
	void load_input_config();

	Unigine::Map<int, int> buttons_binds;
	Unigine::Map<int, int> axes_binds;

	void basestation_update(int num, const Unigine::Math::Mat4 &player_transform, int device_id);
	void controller_update(int num, const Unigine::Math::Mat4 &player_transform, int device_id);

	int vive_getControllerDPadPressed(int device, int button);

	Unigine::Plugins::AppVive *vive = nullptr;
	Unigine::NodeReferencePtr basestation[CONTROLLER_COUNT];

	int BASESTATION_DEVICE_0 = -1,
		BASESTATION_DEVICE_1 = -1,
		CONTROLLER_DEVICE_0 = -1,
		CONTROLLER_DEVICE_1 = -1,
		HMD_DEVICE_0 = -1;

	bool is_vive_controlles = false;

	float odiscrete_timer = 0;

	void move_update_touch(const Unigine::Math::vec2 &axis,
		const Unigine::Math::Mat4 &controller_transform,
		const Unigine::Math::Mat4 &head_transform);

	void turn_update_touch(const Unigine::Math::vec2 &axis, const Unigine::Math::Mat4 &head_transform);
};