#pragma once
#include <UnigineWorld.h>
#include <UnigineSounds.h>
#include "../../Framework/Components/VRInteractable.h"
#include "../../Framework/Utils.h"
#include "../../Framework/SoundManager.h"
#include <cmath>
#include <deque>

class InteractableHandle : public VRInteractable
{
public:
	COMPONENT(InteractableHandle, VRInteractable);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// property
	PROP_NAME("handle_test");
	PROP_AUTOSAVE(1);

	// parameters
	PROP_PARAM(File, sound_start);
	PROP_PARAM(File, sound_loop);
	PROP_PARAM(File, sound_stop);
	PROP_PARAM(File, min_clamp_sound);
	PROP_PARAM(File, max_clamp_sound);
	PROP_PARAM(Float, volume, 1);
	PROP_PARAM(Float, sound_min_dist, 0.2f);
	PROP_PARAM(Float, sound_max_dist, 1000.0f);

	PROP_PARAM(Toggle, change_position);
	PROP_PARAM(Vec3, handle_min_pos);
	PROP_PARAM(Vec3, handle_max_pos);
	PROP_PARAM(Toggle, change_rotation);
	PROP_PARAM(Switch, axis, 0, "X,Y,Z,XYZ");
	PROP_PARAM(Vec3, min_rot);
	PROP_PARAM(Vec3, max_rot)

	PROP_PARAM(Toggle, inertial_enabled, 0);
	PROP_PARAM(Float, angular_acceleration, 0.0f);
	PROP_PARAM(Float, linear_acceleration, 0.0f);

	enum ClampState { None, Clamp, MinClamp, MaxClamp };

	// interact methods
	void grabIt(VRPlayer *player, int hand_num) override;
	void holdIt(VRPlayer *player, int hand_num) override;
	void throwIt(VRPlayer *player, int hand_num) override;

protected:
	void init();
	void update();

	void update_linear_buffer(bool force = false);
	void update_angular_buffer(bool force = false);

private:
	enum HandleState { NONE, MOVE, STOP };

	struct InertialParams
	{
		vec3 direction;
		float speed = 0.0f;
		std::deque<float> speed_buffer;
		bool apply_inertia = false;
		bool speed_signbit = false;
		float speed_sign = 0.0f;
		ClampState was_clamped = ClampState::None;
	};

	static const Unigine::Math::Vec3 ACTIVE_AXIS[4];

	Unigine::ObjectPtr obj;
	Unigine::BodyRigidPtr body;
	
	Unigine::SoundSourcePtr sound;

	float slide_sound_timer = 0;
	bool border_reached = false;
	Unigine::Math::Vec3 last_handle_pos;
	Unigine::Math::quat last_handle_rot;
	Unigine::Math::Vec3 grab_hand_pos; // position of the hand when it grab that object
	Unigine::Math::Vec3 grab_obj_pos; // position of the object when it was grabbed
	Unigine::Math::quat grab_rotate; //rotation of the node when it was grabbed
	Unigine::Math::Scalar length_hand_pc; 

	int is_grabbed;
	int is_vr_mode;

	InertialParams linear_move;
	InertialParams angular_move;
	Vec3 last_hand_pos;
	Vec3 hand_dir;

	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();
};