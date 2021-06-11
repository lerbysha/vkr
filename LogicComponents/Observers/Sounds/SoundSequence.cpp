#include "SoundSequence.h"


REGISTER_COMPONENT(SoundSequence);


void SoundSequence::disable()
{
	stop_all_sounds();
	
	Observer::disable();
	function_result = last_result;
}

void SoundSequence::update()
{
	if (!loop_source)
		return;

	if (!begin_source)
	{
		if (function_result && loop_source && !loop_source->isPlaying())
		{
			stop_all_sounds();
			loop_source->play();
		}

		return;
	}

	if (function_result && begin_source && !begin_source->isPlaying() && loop_source && !loop_source->isPlaying())
	{
		stop_all_sounds();
		loop_source->play();
	}
}

void SoundSequence::on_init()
{
	if (begin_sound)
		init_sound(begin_sound.get(), begin_source);

	if (loop_sound)
		init_sound(loop_sound.get(), loop_source, 1);

	if (end_sound)
		init_sound(end_sound.get(), end_source);
}

void SoundSequence::on_activate()
{
	stop_all_sounds();

	if (begin_source)
		begin_source->play();

	function_result = 1;
}

void SoundSequence::on_deactivate()
{
	stop_all_sounds();

	if (end_source)
		end_source->play();

	function_result = 0;
}

void SoundSequence::init_sound(const char *file_name, SoundSourcePtr &sound_source, int is_loop)
{
	if (FileSystem::isFileExist(file_name))
	{
		sound_source = SoundSource::create(file_name);
		sound_source->setLoop(is_loop);
		sound_source->setOcclusion(0);
		sound_source->setMinDistance(2.0f);
		sound_source->setMaxDistance(15.0f);
		sound_source->setParent(node);

		if (pivot)
			sound_source->setWorldPosition(pivot.get()->getWorldPosition());

		sound_source->setGain(volume.get());
		sound_source->setPlayOnEnable(0);
	}
}

void SoundSequence::stop_all_sounds()
{
	if (begin_source)
	{
		begin_source->stop();
		begin_source->setTime(0);
	}

	if (loop_source)
	{
		loop_source->stop();
		loop_source->setTime(0);
	}

	if (end_source)
	{
		end_source->stop();
		end_source->setTime(0);
	}
}