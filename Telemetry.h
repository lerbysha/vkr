#pragma once

#include <UnigineNode.h>
#include <UniginePlayers.h>
#include <UnigineVector.h>
#include <UnigineMap.h>
#include <UnigineWorld.h>
#include <UnigineWidgets.h>
#include <UnigineVisualizer.h>

#include "Mode.h"

using namespace Unigine;
using namespace Math;

class Telemetry: public Mode
{
public:
	virtual ~Telemetry() {}

	void init();
	void update();
	void shutdown();

private:
	WidgetSpritePtr info_background;
	WidgetLabelPtr info_label;
	WorldIntersectionPtr intersection = WorldIntersection::create();

	Vector<NodePtr> nodes; // nodes with property "telemetry"

	PlayerPtr player;
	Mat4 camera_backup;
	float opening = 0;

	Visualizer * visualizer;
	NodePtr null_node, current_node, next_node;
	String next_text;
	float show_percent = 0;

	void check_intersections();
	void update_information();
	float ease_out_quart(float start, float end, float value);
	void render_circle(const vec3 &point, float radius, int edges, const vec4 &color, float animation = 1.0f);
	vec3 get_nearest_vertex_in_circle(const vec3 &point, float radius, int edges, const vec3 &target);
	vec3 get_vertex_in_circle(const vec3 &point, float radius, int edges, int target_edge);
	void get_normalized_screen_rectangle(const BoundBox &bb, vec3 &min, vec3 &max);
};