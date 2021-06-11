#pragma once
#include <UnigineComponentSystem.h>
#include "../../Framework/Components/Players/VRPlayer.h"


using namespace Unigine;


class Inventory : public Unigine::ComponentBase
{
public:
	COMPONENT(Inventory, Unigine::ComponentBase);
	COMPONENT_INIT(init, 1);
	COMPONENT_POST_UPDATE(render);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_NAME("inventory");
	PROP_AUTOSAVE(1);

	PROP_PARAM(Node, vr_points_node);
	PROP_PARAM(Node, pc_points_node);
	PROP_PARAM(Node, vr_left_plane);
	PROP_PARAM(Node, vr_right_plane);
	PROP_PARAM(Node, pc_plane);

	int grabPoint(const Vec3 &item_position, Vec3 &out);
	void releasePoint(int point_id);

private:
	void init();
	void render();
	void shutdown();

	VRPlayer *vr_player;

	float height_under_head = 0.6f;
	float angle_of_reaction = 60.0f;
	float speed_of_reaction = 2.5f;
	float pc_position_speed_reaction = 40;
	float vr_position_speed_reaction = 30;
	int is_vr_mode;

	Vector<Math::Vec3> points;
	Vector<bool> points_state;
};