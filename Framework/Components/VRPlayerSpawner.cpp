#include "VRPlayerSpawner.h"

#include "Players/VRPlayerPC.h"
#include "Players/VRPlayerOpenVR.h"
#include "Players/VRPlayerOculus.h"

REGISTER_COMPONENT(VRPlayerSpawner);

using namespace Unigine;
using namespace Math;

void VRPlayerSpawner::init()
{
	Engine *engine = Engine::get();

	// check OpenVR headset
	if (engine->findPlugin("AppVive") != -1
		|| engine->findPlugin("AppVarjo") != -1)
	{
		// show vive controls
		open_vr_basestation_0->setEnabled(1);
		open_vr_basestation_1->setEnabled(1);
		open_vr_controller_0->setEnabled(1);
		open_vr_controller_1->setEnabled(1);
		open_vr_teleport_marker->setEnabled(1);
		open_vr_obstacles->setEnabled(0);

		// add node
		PlayerDummyPtr player = PlayerDummy::create();
		init_player(player);

		// add component
		VRPlayerOpenVR* vr_player_open_vr = ComponentSystem::get()->addComponent<VRPlayerOpenVR>(player);
		vr_player_open_vr->allowed_ray_material = allowed_ray_material;
		vr_player_open_vr->forbidden_ray_material = forbidden_ray_material;
		vr_player_open_vr->post_material = post_material;
		vr_player_open_vr->basestation_0 = open_vr_basestation_0;
		vr_player_open_vr->basestation_1 = open_vr_basestation_1;
		vr_player_open_vr->controller_0 = open_vr_controller_0;
		vr_player_open_vr->controller_1 = open_vr_controller_1;
		vr_player_open_vr->teleport_marker = open_vr_teleport_marker;
		vr_player_open_vr->teleport_marker_mat = open_vr_teleport_marker_mat;
		vr_player_open_vr->obstacles = open_vr_obstacles;
		vr_player_open_vr->hand_force_multiplier = open_vr_hand_force_multiplier;
		vr_player_open_vr->controller_buffer_count = open_vr_controller_buffer_count;
		vr_player_open_vr->interaction_collision_mask = open_vr_interaction_collision_mask;
		vr_player_open_vr->left_controller_colliders = open_vr_left_controller_colliders;
		vr_player_open_vr->right_controller_colliders = open_vr_right_controller_colliders;
		vr_player_open_vr->teleport_controller = open_vr_teleport_controller;
		vr_player_open_vr->teleport_button = open_vr_teleport_button;
		vr_player_open_vr->grab_button = open_vr_grab_button;
		vr_player_open_vr->use_button = open_vr_use_button;
		vr_player_open_vr->turn_controller = open_vr_turn_controller;
		vr_player_open_vr->turn_axis = open_vr_turn_axis;
		vr_player_open_vr->move_controller = open_vr_move_controller;
		vr_player_open_vr->move_axis_x = open_vr_move_axis_x;
		vr_player_open_vr->move_axis_y = open_vr_move_axis_y;
		vr_player = vr_player_open_vr;
	}
	else
	{
		// hide vive controls
		open_vr_basestation_0->setEnabled(0);
		open_vr_basestation_1->setEnabled(0);
		open_vr_controller_0->setEnabled(0);
		open_vr_controller_1->setEnabled(0);
		open_vr_teleport_marker->setEnabled(0);
		open_vr_obstacles->setEnabled(0);
	}

	// check Oculus VR headset
	if (engine->findPlugin("AppOculus") != -1)
	{
		// show oculus controls
		oculus_warning_message->setEnabled(1);
		oculus_success_message->setEnabled(1);
		oculus_controller_0->setEnabled(1);
		oculus_controller_1->setEnabled(1);
		oculus_teleport_marker->setEnabled(1);
		oculus_obstacles->setEnabled(0);

		// add node
		PlayerDummyPtr player = PlayerDummy::create();
		init_player(player);

		// add component
		VRPlayerOculus* vr_player_oculus = ComponentSystem::get()->addComponent<VRPlayerOculus>(player);
		vr_player_oculus->allowed_ray_material = allowed_ray_material;
		vr_player_oculus->forbidden_ray_material = forbidden_ray_material;
		vr_player_oculus->post_material = post_material;
		vr_player_oculus->warning_message = oculus_warning_message;
		vr_player_oculus->success_message = oculus_success_message;
		vr_player_oculus->controller_0 = oculus_controller_0;
		vr_player_oculus->controller_1 = oculus_controller_1;
		vr_player_oculus->teleport_marker = oculus_teleport_marker;
		vr_player_oculus->teleport_marker_mat = oculus_teleport_marker_mat;
		vr_player_oculus->obstacles = oculus_obstacles;
		vr_player_oculus->hand_force_multiplier = oculus_hand_force_multiplier;
		vr_player_oculus->controller_buffer_count = oculus_controller_buffer_count;
		vr_player_oculus->interaction_collision_mask = oculus_interaction_collision_mask;
		vr_player_oculus->left_controller_colliders = oculus_left_controller_colliders;
		vr_player_oculus->right_controller_colliders = oculus_right_controller_colliders;
		vr_player_oculus->teleport_controller = oculus_teleport_controller;
		vr_player_oculus->teleport_button = oculus_teleport_button;
		vr_player_oculus->grab_button = oculus_grab_button;
		vr_player_oculus->use_button = oculus_use_button;
		vr_player_oculus->turn_controller = oculus_turn_controller;
		vr_player_oculus->turn_axis = oculus_turn_axis;
		vr_player_oculus->move_controller = oculus_move_controller;
		vr_player_oculus->move_axis_x = oculus_move_axis_x;
		vr_player_oculus->move_axis_y = oculus_move_axis_y;
		vr_player = vr_player_oculus;
	}
	else
	{
		// hide oculus controls
		oculus_warning_message->setEnabled(0);
		oculus_success_message->setEnabled(0);
		oculus_controller_0->setEnabled(0);
		oculus_controller_1->setEnabled(0);
		oculus_teleport_marker->setEnabled(0);
		oculus_obstacles->setEnabled(0);
	}

	// if VR is not loaded
	if (engine->findPlugin("AppVive") == -1 &&
		engine->findPlugin("AppOculus") == -1 &&
		engine->findPlugin("AppVarjo") == -1)
	{
		// add node
		PlayerActorPtr player = PlayerActor::create();
		init_player(player);

		// add component
		VRPlayerPC *vr_player_pc = ComponentSystem::get()->addComponent<VRPlayerPC>(player);
		vr_player_pc->post_material = post_material;
		vr_player_pc->move_speed = move_speed;
		vr_player_pc->acceleration = acceleration;
		vr_player_pc->damping = damping;
		vr_player_pc->hand_length = hand_length;
		vr_player_pc->throw_force = throw_force;
		vr_player_pc->mouse_arrow_sprite = pc_mouse_arrow_sprite;
		vr_player = vr_player_pc;
	}
}

void VRPlayerSpawner::init_player(const PlayerPtr &player)
{
	// copy transform
	player->setWorldTransform(node->getWorldTransform());

	if (node->getType() >= Node::PLAYER_BEGIN &&
		node->getType() <= Node::PLAYER_END)
	{
		PlayerPtr player_ref = checked_ptr_cast<Player>(node);
		
		// copy basic parameters
		player->setFov(player_ref->getFov());
		player->setZNear(player_ref->getZNear());
		player->setZFar(player_ref->getZFar());
		player->setPostMaterials(player_ref->getPostMaterials());
	}

	// unparent current node 
	// (cause vr_group can be our parent and we don't want to turn off
	// components that are children to us)
	node->setWorldParent(NodePtr());
}
