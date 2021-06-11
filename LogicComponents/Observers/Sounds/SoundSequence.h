#pragma once
#include "../Observer.h"
#include <UnigineSounds.h>


class SoundSequence : public Observer
{
public:
	COMPONENT(SoundSequence, Observer);
	COMPONENT_UPDATE(update);

	PROP_NAME("sound_sequence");
	PROP_AUTOSAVE(0);

	PROP_PARAM(File, begin_sound);
	PROP_PARAM(File, loop_sound);
	PROP_PARAM(File, end_sound);
	PROP_PARAM(Float, volume);
	PROP_PARAM(Node, pivot);

	void disable() override;

private:
	void update();

	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;

	void init_sound(const char *file_name, SoundSourcePtr &sound_source, int is_loop = 0);
	void stop_all_sounds();

	SoundSourcePtr begin_source;
	SoundSourcePtr loop_source;
	SoundSourcePtr end_source;

	int function_result;
};