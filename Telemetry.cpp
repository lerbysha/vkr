#include "Telemetry.h"
#include "ProjectUtils.h"
#include <UnigineFileSystem.h>
#include <UnigineGame.h>
#include <UnigineApp.h>
#include <UnigineEditor.h>
#include <UnigineInput.h>

#define ATTACH_TEXT_TO_TOP_RIGHT_CORNER_OF_SCREEN // or else text will be attached to the top right corner of the bound box
#define MAX_DISTANCE_ATTACHED_TEXT 0.2f // 1.0f - 100% of the screen

void Telemetry::init()
{
	String info_background_texture = "";

	PropertyPtr assets = World::getNodeByName("telemetry_data")->getProperty();
	if (assets)
	{
		PropertyParameterPtr param = assets->getParameterPtr("info_background");
		if (param->isExist())
			info_background_texture = param->getValueFile();
	}

	// create WidgetBackground
	info_background = WidgetSprite::create(Gui::get(), FileSystem::resolvePartialVirtualPath(info_background_texture));
	info_background->setColor(vec4(1, 1, 1, 0));
	Gui::get()->addChild(info_background, Gui::ALIGN_OVERLAP);

	// create WidgetLabel
	info_label = WidgetLabel::create(Gui::get(), "");
	info_label->setFontSize(16);
	info_label->setFontOutline(1);
	info_label->setFontRich(1);
	Gui::get()->addChild(info_label, Gui::ALIGN_OVERLAP);

	// enable Visualizer
	Visualizer::setEnabled(1);

	// enable root node "telemetry_data"
	NodePtr inf_node = World::getNodeByName("telemetry_data");
	inf_node->setEnabled(1);

	// get "telemetry_player" node
	NodePtr camera_point = World::getNodeByName("telemetry_player");
	player = Game::getPlayer();
	camera_backup = player->getWorldTransform();
	player->setWorldTransform(camera_point->getWorldTransform());

	PlayerPtr telemetry_player = checked_ptr_cast<Player>(camera_point);
	player->setFov(telemetry_player->getFov());
	player->setZNear(telemetry_player->getZNear());
	player->setZFar(telemetry_player->getZFar());

	// find all nodes with property "telemetry" and make them invisible
	nodes = find_nodes_with_property("telemetry");
	for (int i = 0; i < nodes.size(); i++)
	{
		ObjectPtr obj = checked_ptr_cast<Object>(nodes[i]);
		if (obj)
		{
			for (int k = 0; k < obj->getNumSurfaces(); k++)
			{
				obj->setCollision(0, k);
				obj->setCastShadow(0, k);
				obj->setCastWorldShadow(0, k);
				obj->setViewportMask(0, k);
			}
		}
	}
}

void Telemetry::update()
{
	// play opening animation
	if (opening < 1)
	{
		float ifps = Game::getIFps();
		opening = saturate(opening + 0.25f * ifps);

		// move camera
		player->setWorldPosition(player->getWorldPosition() - Vec3(player->getWorldDirection() * ifps * (1.0f - opening) * 25.0f));

		// show bound boxes (waving effect)
		float offset = 0;
		float step = 1.0f / itof(nodes.size());
		for (int i = 0; i < nodes.size(); i++)
		{
			offset += step;
			float p = ease_out_quart(0, 1, 1.0f - Math::abs(1.0f - saturate(opening * 2.0f - offset) * 2.0f));
			if (p > 0)
			{
				BoundBox b;
				UNIGINE_BOUND_BOX bb = nodes[i]->getWorldBoundBox();
				b.set(vec3(bb.getMin()), vec3(float(bb.getMax().x), float(bb.getMax().y), float(lerp(bb.getMin().z, bb.getMax().z, (Scalar)p))));
				vec3 size = b.getMax() - b.getMin();
				vec3 center = b.getMin() + size * 0.5f;

				// show solid bound box
				visualizer->renderSolidBox(size, Mat4(translate(center)), vec4(0, 1, 0, 0.15f));

				// show wireframe bound box
				visualizer->renderBoundBox(b, Mat4::IDENTITY, vec4(1, 1, 1, 1));
			}
		}
	}

	// check intersection from mouse position to telemetry objects
	check_intersections();

	// show / hide bounding box and information text
	update_information();
}

void Telemetry::check_intersections()
{
	Vec3 pos = get_position_of_screen();
	vec3 dir = get_direction_from_mouse();

	ObjectPtr hit_obj = World::getIntersection(pos, pos + Vec3(dir * 1000.0f), 1, intersection);
	if (hit_obj)
	{
		PropertyPtr prop;
		if (is_have_a_property("telemetry", hit_obj, prop))
		{
			next_node = hit_obj;

			PropertyParameterPtr parameter = prop->getParameterPtr("text");
			if (parameter)
				next_text = parameter->getValueString();

			return;
		}
	}
	next_node = null_node;
}

void Telemetry::update_information()
{
	// check
	if (!current_node)
	{
		if (next_node)
		{
			current_node = next_node;
			info_label->setText(next_text.get());
			info_label->arrange();
		}
		else
			return;
	}

	// calculations
	if (current_node != next_node)
	{
		// hide
		show_percent = saturate(show_percent - 2.5f * Game::getIFps());

		if (show_percent <= 0)
		{
			current_node = next_node;
			info_label->setText(next_text.get());
			info_label->arrange();

			if (!next_node)
			{
				info_background->setColor(vec4(1, 1, 1, 0));
				info_label->setFontColor(vec4(1, 1, 1, 0));
				return;
			}
		}
	}
	else
	{
		// show
		show_percent = saturate(show_percent + 2.5f * Game::getIFps());
	}

	// visualizers
	BoundBox b;
	UNIGINE_BOUND_BOX bb = current_node->getWorldBoundBox();
	float p = ease_out_quart(0, 1, show_percent);
	b.set(vec3(bb.getMin()), vec3(float(bb.getMax().x), float(bb.getMax().y), float(lerp(bb.getMin().z, bb.getMax().z, (Scalar)p))));
	vec3 size = b.getMax() - b.getMin();
	vec3 center = b.getMin() + size * 0.5f;
	vec4 main_color = vec4(1, 1, 1, 1);

	// show solid bound box
	visualizer->renderSolidBox(size, Mat4(translate(center)), vec4(0, 1, 0, 0.15f));

	// show wireframe bound box
	visualizer->renderBoundBox(b, Mat4::IDENTITY, main_color);

	// put text to the top right corner of the bound box (or top right corner of the screen)
	vec3 rect_min, rect_max;
	get_normalized_screen_rectangle(b, rect_min, rect_max);
#ifdef ATTACH_TEXT_TO_TOP_RIGHT_CORNER_OF_SCREEN
	int pos_x = App::getWidth() - info_label->getWidth() - 10;
	int pos_y = 10;
#else
	int pos_x = ftoi(rect_max.x * App::getWidth());
	int pos_y = ftoi(rect_min.y * App::getHeight());
	pos_x = clamp(pos_x, 10, App::getWidth() - info_label->getWidth() - 10);
	pos_y = clamp(pos_y, 10, App::getHeight() - info_label->getHeight() - 10);
#endif
	info_label->setPosition(pos_x, pos_y);
	vec3 label_pos = vec3(itof(pos_x) / itof(App::getWidth()), itof(pos_y) / itof(App::getHeight()), 0);

	// show line between two circles
	float circle_radius = 0.01f;
	vec3 point0 = getNormalizedScreenPosition(center, Game::getPlayer()->getCamera());
	vec3 point1 = vec3(label_pos.x - circle_radius, label_pos.y + itof(info_label->getFontSize() / 2 - 2) / itof(App::getHeight()), 0);
	vec3 p0 = get_vertex_in_circle(point0, circle_radius, 6, 2);
	vec3 p1 = get_vertex_in_circle(point1, circle_radius, 6, 5);

#ifdef ATTACH_TEXT_TO_TOP_RIGHT_CORNER_OF_SCREEN
	float len = length(point1 - point0);
	float max_len = min(length(rect_max - rect_min) * 0.5f, MAX_DISTANCE_ATTACHED_TEXT);
	if (len > max_len)
	{
		len = max_len;
		
		float min_x = point1.x;
		float min_y = point1.y;
		float max_y = 1.0f - point1.y - itof(info_label->getHeight()) / itof(App::getHeight());
		point1 = point0 + normalize(point1 - point0) * len;
		point1.x = min(point1.x, min_x); // clamp text to right corner
		point1.y = clamp(point1.y, min_y, max_y); // clamp text between top and bottom of the screen
		
		p1 = get_vertex_in_circle(point1, circle_radius, 6, 5);

		// reposition of the text
		info_label->setPosition(
			ftoi((point1.x + circle_radius) * App::getWidth()), 
			ftoi(point1.y * App::getHeight() - info_label->getFontSize() / 2 + 2));
	}
#endif
	
	render_circle(point0, circle_radius, 6, main_color, saturate(show_percent * 2.0f));
	render_circle(point1, circle_radius, 6, main_color, show_percent);
	visualizer->renderLine2D(vec2(p0), vec2(p0 + normalize(p1 - p0) * show_percent * length(p1 - p0)), main_color);
	
	// show text
	info_background->setWidth(info_label->getWidth() + 10);
	info_background->setHeight(info_label->getHeight() + 10);
	info_background->arrange();
	info_background->setPosition(info_label->getPositionX() - 5, info_label->getPositionY() - 5);
	info_background->setColor(vec4(1, 1, 1, 0.75f * p));
	info_label->setFontColor(vec4(1, 1, 1, p));
}

void Telemetry::shutdown()
{
	Gui::get()->removeChild(info_background);
	Gui::get()->removeChild(info_label);
	ControlsApp::setMouseEnabled(0);

	// disable root node "telemetry_data"
	NodePtr inf_node = World::getNodeByName("telemetry_data");
	inf_node->setEnabled(0);

	// restore camera position
	if (player)
		player->setWorldTransform(camera_backup);
}

float Telemetry::ease_out_quart(float start, float end, float value)
{
	value--;
	end -= start;
	return -end * (value * value * value * value - 1) + start;
}

void Telemetry::render_circle(const vec3 &point, float radius, int edges, const vec4 &color, float animation)
{
	float aspect = itof(App::getHeight()) / itof(App::getWidth());

	float last_x = point.x, last_y = point.y;
	for (int i = 0; i <= ftoi(edges * animation); i++)
	{
		float s = itof(i) / itof(edges);
		float x = point.x + Math::sin(s * 2.0f * UNIGINE_PI) * radius * aspect;
		float y = point.y + Math::cos(s * 2.0f * UNIGINE_PI) * radius;

		if (i > 0)
			visualizer->renderLine2D(vec2(last_x, last_y), vec2(x, y), color);

		last_x = x;
		last_y = y;
	}
}

vec3 Telemetry::get_nearest_vertex_in_circle(const vec3 &point, float radius, int edges, const vec3 &target)
{
	float min_dist2 = length2(vec2(target.x, target.y) - vec2(point.x, point.y));
	vec3 result = point;

	float aspect = itof(App::getHeight()) / itof(App::getWidth());
	for (int i = 0; i < edges; i++)
	{
		float s = itof(i) / itof(edges);
		float x = point.x + Math::sin(s * 2.0f * UNIGINE_PI) * radius * aspect;
		float y = point.y + Math::cos(s * 2.0f * UNIGINE_PI) * radius;

		float len2 = length2(vec2(x, y) - vec2(target.x, target.y));
		if (len2 < min_dist2)
		{
			min_dist2 = len2;
			result = vec3(x, y, 0);
		}
	}

	return result;
}

vec3 Telemetry::get_vertex_in_circle(const vec3 &point, float radius, int edges, int target_edge)
{
	float aspect = itof(App::getHeight()) / itof(App::getWidth());
	float s = saturate(itof(target_edge) / itof(edges));
	float x = point.x + Math::sin(s * 2.0f * UNIGINE_PI) * radius * aspect;
	float y = point.y + Math::cos(s * 2.0f * UNIGINE_PI) * radius;
	return vec3(x, y, 0);
}

void Telemetry::get_normalized_screen_rectangle(const BoundBox &bb, vec3 &min, vec3 &max)
{
	CameraPtr cam = Game::getPlayer()->getCamera();
	
	vec3 p0 = getNormalizedScreenPosition(vec3(bb.getMin().x, bb.getMin().y, bb.getMin().z), cam);
	vec3 p1 = getNormalizedScreenPosition(vec3(bb.getMin().x, bb.getMax().y, bb.getMin().z), cam);
	vec3 p2 = getNormalizedScreenPosition(vec3(bb.getMax().x, bb.getMin().y, bb.getMin().z), cam);
	vec3 p3 = getNormalizedScreenPosition(vec3(bb.getMax().x, bb.getMax().y, bb.getMin().z), cam);
	vec3 p4 = getNormalizedScreenPosition(vec3(bb.getMin().x, bb.getMin().y, bb.getMax().z), cam);
	vec3 p5 = getNormalizedScreenPosition(vec3(bb.getMin().x, bb.getMax().y, bb.getMax().z), cam);
	vec3 p6 = getNormalizedScreenPosition(vec3(bb.getMax().x, bb.getMin().y, bb.getMax().z), cam);
	vec3 p7 = getNormalizedScreenPosition(vec3(bb.getMax().x, bb.getMax().y, bb.getMax().z), cam);

	min = Math::min(p0, p1);
	min = Math::min(min, p2);
	min = Math::min(min, p3);
	min = Math::min(min, p4);
	min = Math::min(min, p5);
	min = Math::min(min, p6);
	min = Math::min(min, p7);

	max = Math::max(p0, p1);
	max = Math::max(max, p2);
	max = Math::max(max, p3);
	max = Math::max(max, p4);
	max = Math::max(max, p5);
	max = Math::max(max, p6);
	max = Math::max(max, p7);
}