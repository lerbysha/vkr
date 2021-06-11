#include "Inventory.h"
#include "../../Framework/Components/VRPlayerSpawner.h"


REGISTER_COMPONENT(Inventory);


int Inventory::grabPoint(const Vec3 &item_position, Vec3 &out)
{
	int id = -1;
	double min_delta = UNIGINE_INFINITY;

	for (int i = 0; i < points.size(); i++)
		if (!points_state[i])
		{
			double delta = (item_position - points[i]).length2();
			if (delta < min_delta)
			{
				min_delta = delta;
				id = i;
			}
		}

	if (id != -1)
	{
		out = points[id];
		points_state[id] = true;
	}

	return id;
}

void Inventory::releasePoint(int point_id)
{
	if (0 <= point_id && point_id < points_state.size())
		points_state[point_id] = false;
}

void Inventory::init()
{
	NodePtr spawn_node = World::getNodeByName("spawn_point");
	if (!spawn_node)
	{
#if DEBUG
		Log::error("INVENTORY ERROR: spawn point not found\n");
#endif
		return;
	}

	VRPlayerSpawner *spawner = ComponentSystem::get()->getComponent<VRPlayerSpawner>(spawn_node);
	if (!spawner)
	{
#if DEBUG
		Log::error("INVENTORY ERROR: spawn point not contain VRPlayerSpawner component\n");
#endif
		return;
	}

	if (VRPlayer::isVRPluginLoaded() && !vr_points_node)
	{
#if DEBUG
		Log::error("INVENTORY ERROR: vr points node is null\n");
#endif
		return;
	}

	if (!VRPlayer::isVRPluginLoaded() && !pc_points_node)
	{
#if DEBUG
		Log::error("INVENTORY ERROR: pc points node is null\n");
#endif
		return;
	}

	vr_player = spawner->getSpawnedPlayer();

	if (VRPlayer::isVRPluginLoaded())
	{
		vr_left_plane.get()->setEnabled(true);
		vr_right_plane.get()->setEnabled(true);
		pc_plane.get()->setEnabled(false);

		for (int i = 0; i < vr_points_node.get()->getNumChildren(); i++)
		{
			Vec3 position = vr_points_node.get()->getChild(i)->getPosition();
			points.push_back(position);
			points_state.push_back(false);
		}
	}
	else
	{
		vr_left_plane.get()->setEnabled(false);
		vr_right_plane.get()->setEnabled(false);
		pc_plane.get()->setEnabled(true);

		for (int i = 0; i < pc_points_node.get()->getNumChildren(); i++)
		{
			Vec3 position = pc_points_node.get()->getChild(i)->getPosition();
			points.push_back(position);
			points_state.push_back(false);
		}
	}

	is_vr_mode = VRPlayer::isVRPluginLoaded();
}

void Inventory::render()
{
	float ifps = Game::getIFps();

	if (is_vr_mode)
		node->setWorldPosition(vr_player->getHead()->getWorldPosition() + Vec3::DOWN * height_under_head);
	else
	{
		//float k = saturate(pc_position_speed_reaction * ifps);
		//node->setWorldPosition(lerp(node->getWorldPosition(), vr_player->getHead()->getWorldPosition() + Vec3::DOWN * height_under_head, k));

		node->setWorldPosition(vr_player->getHead()->getWorldPosition() + Vec3::DOWN * height_under_head);
	}

	vec3 head_dir = vr_player->getHead()->getWorldDirection();
	vec3 inventory_dir = node->getWorldDirection(AXIS_Y);

	head_dir.z = 0;
	head_dir.normalize();

	inventory_dir.z = 0;
	inventory_dir.normalize();

	if (is_vr_mode)
	{
		if (Math::getAngle(head_dir, inventory_dir) > angle_of_reaction)
		{
			float k = saturate(speed_of_reaction * ifps);
			inventory_dir = Math::lerp(inventory_dir, head_dir, k).normalize();
			node->setWorldDirection(inventory_dir, vec3::UP, AXIS_Y);
		}
	}
	else
		node->setWorldDirection(head_dir, vec3::UP, AXIS_Y);

}

void Inventory::shutdown()
{
	points.clear();
	points_state.clear();
}