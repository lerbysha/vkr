#pragma once
#include <UniginePlayers.h>
#include <UnigineComponentSystem.h>
#include "Players/VRPlayer.h"

// spawns VRPlayer at node's position
class VRPlayerSpawner : public Unigine::ComponentBase
{
public:
	COMPONENT(VRPlayerSpawner, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	// property
	PROP_NAME("vr_player_spawner");

	// Common
	PROP_PARAM(Material, allowed_ray_material, "Allowed Ray Matreial", "", "Common");
	PROP_PARAM(Material, forbidden_ray_material, "Forbidden Ray Matreial", "", "Common");
	PROP_PARAM(Material, post_material, "Post Matreial", "", "Common");

	// OpenVR group
	PROP_PARAM(Node, open_vr_basestation_0, "Base station 0", "", "OpenVR");
	PROP_PARAM(Node, open_vr_basestation_1, "Base station 1", "", "OpenVR");
	PROP_PARAM(Node, open_vr_controller_0, "Controller 0", "", "OpenVR");
	PROP_PARAM(Node, open_vr_controller_1, "Controller 1", "", "OpenVR");
	PROP_PARAM(Node, open_vr_teleport_marker, "Teleport Marker", "", "OpenVR");
	PROP_PARAM(Material, open_vr_teleport_marker_mat, "Teleport Marker Material", "", "OpenVR");
	PROP_PARAM(Node, open_vr_obstacles, "Obstacles", "", "OpenVR");
	PROP_PARAM(Float, open_vr_hand_force_multiplier, 1.3f, "Hand Force Multiplier", "", "OpenVR");	// getHandLinearVelocity = velocity * hand_force_multiplier
	PROP_PARAM(Int, open_vr_controller_buffer_count, 4, "Controller Buffer Count", "", "OpenVR");
	PROP_PARAM(Mask, open_vr_interaction_collision_mask, 0, "Interaction Collision Mask", "", "OpenVR");
	PROP_ARRAY(Node, open_vr_left_controller_colliders, "Left Controller Colliders", "", "OpenVR")
	PROP_ARRAY(Node, open_vr_right_controller_colliders, "Right Controller Colliders", "", "OpenVR")
	PROP_PARAM(Switch, open_vr_teleport_controller, 0, "LEFT,RIGHT,LEFT AND RIGHT", "Teleport Controller", "", "OpenVR");
	PROP_PARAM(Switch, open_vr_teleport_button, 0, "STICK X,STICK Y,TRIGGER,GRIP,XA,YB,MENU,THUMB", "Teleport Button", "", "OpenVR");
	PROP_PARAM(Switch, open_vr_grab_button, 0, "STICK X,STICK Y,TRIGGER,GRIP,XA,YB,MENU,THUMB", "Grab Button", "", "OpenVR");
	PROP_PARAM(Switch, open_vr_use_button, 0, "STICK X,STICK Y,TRIGGER,GRIP,XA,YB,MENU,THUMB", "Use Button", "", "OpenVR");
	PROP_PARAM(Switch, open_vr_turn_controller, 0, "LEFT,RIGHT", "Turn Controller", "", "OpenVR");
	PROP_PARAM(Switch, open_vr_turn_axis, 0, "STICK X,STICK Y", "Turn Axis", "", "OpenVR");
	PROP_PARAM(Switch, open_vr_move_controller, 0, "LEFT,RIGHT", "Move Controller", "", "OpenVR");
	PROP_PARAM(Switch, open_vr_move_axis_x, 0, "STICK X,STICK Y", "Move Axis X", "", "OpenVR");
	PROP_PARAM(Switch, open_vr_move_axis_y, 0, "STICK X,STICK Y", "Move Axis Y", "", "OpenVR");

	// OCULUS group
	PROP_PARAM(Node, oculus_warning_message, "Warning Message", "", "Oculus");
	PROP_PARAM(Node, oculus_success_message, "Success Message", "", "Oculus");
	PROP_PARAM(Node, oculus_controller_0, "Controller 0", "", "Oculus");
	PROP_PARAM(Node, oculus_controller_1, "Controller 1", "", "Oculus");
	PROP_PARAM(Node, oculus_teleport_marker, "Teleport Marker", "", "Oculus");
	PROP_PARAM(Material, oculus_teleport_marker_mat, "Teleport Market Material", "", "Oculus");
	PROP_PARAM(Node, oculus_obstacles, "Obstacles", "", "Oculus");
	PROP_PARAM(Float, oculus_hand_force_multiplier, 1.3f, "Hand Force Multiplier", "", "Oculus");	// getHandLinearVelocity = velocity * hand_force_multiplier
	PROP_PARAM(Int, oculus_controller_buffer_count, 4, "Controller Buffer Count", "", "Oculus");
	PROP_PARAM(Mask, oculus_interaction_collision_mask, 0, "Interaction Collision Mask", "", "Oculus");
	PROP_ARRAY(Node, oculus_left_controller_colliders, "Left Controller Colliders", "", "Oculus");
	PROP_ARRAY(Node, oculus_right_controller_colliders, "Right Controller Colliders", "", "Oculus");
	PROP_PARAM(Switch, oculus_teleport_controller, 0, "LEFT,RIGHT,LEFT AND RIGHT", "Teleport Controller", "", "Oculus");
	PROP_PARAM(Switch, oculus_teleport_button, 0, "STICK X,STICK Y,TRIGGER,GRIP,XA,YB,MENU,THUMB", "Teleport Button", "", "Oculus");
	PROP_PARAM(Switch, oculus_grab_button, 0, "STICK X,STICK Y,TRIGGER,GRIP,XA,YB,MENU,THUMB", "Grab Button", "", "Oculus");
	PROP_PARAM(Switch, oculus_use_button, 0, "STICK X,STICK Y,TRIGGER,GRIP,XA,YB,MENU,THUMB", "Use Button", "", "Oculus");
	PROP_PARAM(Switch, oculus_turn_controller, 0, "LEFT,RIGHT", "Turn Controller", "", "Oculus");
	PROP_PARAM(Switch, oculus_turn_axis, 0, "STICK X,STICK Y", "Turn Axis", "", "Oculus");
	PROP_PARAM(Switch, oculus_move_controller, 0, "LEFT,RIGHT", "Move Controller", "", "Oculus");
	PROP_PARAM(Switch, oculus_move_axis_x, 0, "STICK X,STICK Y", "Move Axis X", "", "Oculus");
	PROP_PARAM(Switch, oculus_move_axis_y, 0, "STICK X,STICK Y", "Move Axis Y", "", "Oculus");

	// PC group
	PROP_PARAM(Float, move_speed, 2.5f, "Move Speed", "", "PC");	// speed of the player
	PROP_PARAM(Float, acceleration, 8.0f, "Acceleration", "", "PC");	// acceleration of the player
	PROP_PARAM(Float, damping, 8.0f, "Damping", "", "PC");		// damping of the player
	PROP_PARAM(Float, hand_length, 1.3f, "Hand Length", "", "PC");	// uses for grab objects
	PROP_PARAM(Float, throw_force, 7.0f, "Throw Force", "", "PC");	// start velocity of grabbed object
	PROP_PARAM(File, pc_mouse_arrow_sprite, "", "PC Mouse Arrow", "", "PC");

	// return spawned player's component
	UNIGINE_INLINE VRPlayer* getSpawnedPlayer() const { return vr_player; }

protected:
	void init();

private:
	void init_player(const Unigine::PlayerPtr &player);

	VRPlayer* vr_player;
};