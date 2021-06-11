#pragma once

#include <UnigineNode.h>
#include <UnigineVector.h>
#include <UnigineCamera.h>

using namespace Unigine;
using namespace Math;

Vector<NodePtr> find_nodes_with_property(const char * property_name);
bool is_have_a_property(const char * property_name, const NodePtr &node);
bool is_have_a_property(const char * property_name, const NodePtr &node, PropertyPtr &prop);
Vec3 get_position_of_screen();
vec3 get_direction_from_mouse();
vec3 getNormalizedScreenPosition(const vec3 &point, const CameraPtr &cam);