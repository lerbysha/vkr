#include "ObjSwitch.h"
#include <UnigineGame.h>
#include <UnigineEditor.h>
#include "../../SoundManager.h"

REGISTER_COMPONENT(ObjSwitch);

using namespace Unigine;
using namespace Math;


void ObjSwitch::setDefault(int condition)
{
	enabled = condition;

	if (change_position)
		node->setPosition(enabled ? (Math::Vec3)enable_position.get() : (Math::Vec3)disable_position.get());

	if (change_rotation)
		node->setRotation(enabled ? enable_rotation : disable_rotation);

	stop_all_sounds();

	time = 1;
}

void ObjSwitch::init_sound(const char *file_name, SoundSourcePtr &sound_source)
{
	if (FileSystem::isFileExist(file_name))
	{
		sound_source = SoundSource::create(file_name);
		sound_source->setLoop(0);
		sound_source->setOcclusion(0);
		sound_source->setMinDistance(2.0f);
		sound_source->setMaxDistance(15.0f);
		sound_source->setParent(node);
		sound_source->setPosition(Vec3::ZERO);
		sound_source->setPlayOnEnable(0);
	}
}

void ObjSwitch::stop_all_sounds()
{
	if (enable_sound)
		enable_sound->stop();

	if (loop_sound)
		loop_sound->stop();

	if (disable_sound)
		disable_sound->stop();
}

void ObjSwitch::init()
{
	// convert euler's vec3 to rotation quat
	vec3 dis_rot = disable_rotation_euler;
	vec3 en_rot = enable_rotation_euler;
	disable_rotation = quat(dis_rot.x, dis_rot.y, dis_rot.z);
	enable_rotation = quat(en_rot.x, en_rot.y, en_rot.z);

	if (enable_sound_file)
		init_sound(enable_sound_file.get(), enable_sound);

	if (loop_sound_file)
		init_sound(loop_sound_file.get(), loop_sound);

	if (disable_sound_file)
		init_sound(disable_sound_file.get(), disable_sound);

	// init value is "disabled" by default
	setDefault(0);
}

void ObjSwitch::update()
{
	// animation
	if (time < 1)
	{
		time = saturate(time + (Game::getIFps() / Game::getScale()) / animation_duration.get());

		if (play_pingpong)
		{
			float percent = 1.0f - Math::abs(time - 0.5f) * 2.0f; // 0 -> 1 -> 0

			if (change_position)
				node->setPosition(Vec3(lerp(time < 0.5f ? from_pos : Vec3(disable_position), Vec3(enable_position), percent)));
			if (change_rotation)
			{
				if (use_quaternions)
					node->setRotation(slerp(time < 0.5f ? from_rot : disable_rotation, enable_rotation, percent));
				else
				{
					vec3 cur_rot = lerp(time < 0.5f ? from_rot_euler : disable_rotation_euler, enable_rotation_euler, percent);
					node->setRotation(quat(cur_rot.x, cur_rot.y, cur_rot.z));
				}
			}
		}
		else
		{
			if (change_position)
				node->setPosition(Vec3(lerp(from_pos, to_pos, time)));
			if (change_rotation)
			{
				if (use_quaternions)
					node->setRotation(slerp(from_rot, to_rot, time));
				else
				{
					vec3 cur_rot = lerp(from_rot_euler, to_rot_euler, time);
					node->setRotation(quat(cur_rot.x, cur_rot.y, cur_rot.z));
				}
			}
		}

		if (time >= 1 && enabled && loop_sound && !loop_sound->isPlaying())
		{
			loop_sound->setLoop(1);
			loop_sound->play();
		}
	}
}

void ObjSwitch::useIt(VRPlayer* player, int hand_num)
{
	// rotate switcher
	enabled = !enabled;

	from_pos = Vec3(enabled ? disable_position : enable_position);
	from_rot = enabled ? disable_rotation : enable_rotation;
	from_rot_euler = enabled ? disable_rotation_euler : enable_rotation_euler;

	to_pos = Vec3(enabled ? enable_position : disable_position);
	to_rot = enabled ? enable_rotation : disable_rotation;
	to_rot_euler = enabled ? enable_rotation_euler : disable_rotation_euler;

	float new_time = 1 - time;

	// play sound
	if (enabled && enable_sound)
	{
		stop_all_sounds();

		float time_offset = (enable_sound->getLength() - animation_duration.get()) * new_time;
		if (time_offset < 0)
			time_offset = 0;

		Sound::renderWorld(1);
		enable_sound->setTime(time_offset);
		enable_sound->play();
	}

	if (!enabled && disable_sound)
	{
		stop_all_sounds();

		Sound::renderWorld(1);

		float time_offset = (disable_sound->getLength() - animation_duration.get()) * new_time;
		if (time_offset < 0)
			time_offset = 0;

		disable_sound->setTime(time_offset);
		disable_sound->play();
	}

	time = new_time;

	VRInteractable::useIt(player, hand_num);
}