#include "ProjectUtils.h"

#include <UnigineNodes.h>
#include <UnigineEditor.h>
#include <UnigineGame.h>
#include <UniginePlayers.h>
#include <UnigineApp.h>

void find_prop_in_children(const char * property_name, const NodePtr &node, Vector<NodePtr> &result)
{
	if (!node) return;

	if (is_have_a_property(property_name, node))
		result.append(node);

	for (int i = 0; i < node->getNumChildren(); i++)
	{
		find_prop_in_children(property_name, node->getChild(i), result);

		NodeReferencePtr nodeRef = checked_ptr_cast<NodeReference>(node->getChild(i));
		if (nodeRef)
			find_prop_in_children(property_name, nodeRef->getReference(), result);
	}
}

Vector<NodePtr> find_nodes_with_property(const char * property_name)
{
	Vector<NodePtr> result;

	Vector<NodePtr> nodes;
	World::getNodes(nodes);
	for (const auto &node : nodes)
	{
		if (!node->getParent()) // if it is root object
		{
			find_prop_in_children(property_name, node, result);

			NodeReferencePtr nodeRef = checked_ptr_cast<NodeReference>(node);
			if (nodeRef)
				find_prop_in_children(property_name, nodeRef->getReference(), result);
		}
	}

	return result;
}

bool is_have_a_property(const char * property_name, const NodePtr &node)
{
	PropertyPtr prop;
	return is_have_a_property(property_name, node, prop);
}

bool is_have_a_property(const char * property_name, const NodePtr &node, PropertyPtr &prop)
{
	prop = node->getProperty();
	if (!prop)
		return false;

	PropertyPtr p = prop;
	do
	{
		if (strcmp(p->getName(), property_name) == 0)
			return true;

		p = p->getParent();
	} while (p);

	return false;
}

Vec3 get_position_of_screen()
{
	return Game::getPlayer()->getWorldPosition();
}

vec3 get_direction_from_mouse()
{
	PlayerPtr player = Game::getPlayer();
	return player->getDirectionFromScreen(Gui::get()->getMouseX(), Gui::get()->getMouseY(), App::getWidth(), App::getHeight());
}

vec3 getNormalizedScreenPosition(const vec3 &point, const CameraPtr &cam)
{
	float width = static_cast<float>(App::getWidth());
	float height = static_cast<float>(App::getHeight());

	mat4 projection = cam->getProjection();
	mat4 modelview = mat4(cam->getModelview());
	projection.m00 *= height / width;

	vec4 p = projection * vec4(modelview * vec4(point, 1));
	if (p.w > 0)
	{
		return vec3(
			0.5f + p.x * 0.5f / p.w,
			1.0f - (0.5f + p.y * 0.5f / p.w),
			0);
	}
	return vec3(-1, -1, -1);
}
