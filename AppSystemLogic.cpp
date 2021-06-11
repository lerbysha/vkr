#include "AppSystemLogic.h"
#include "Framework/Components/Players/VRPlayer.h"
#include <UnigineApp.h>
#include <UnigineConsole.h>
#include <UnigineSounds.h>

#include <UnigineComponentSystem.h>

// System logic, it exists during the application life cycle.
// These methods are called right after corresponding system script's (UnigineScript) methods.

int AppSystemLogic::init() {
	// Write here code to be called on engine initialization.
	
	// update application window even if window is hidden
	// (because Vive menu switches focus to itself)
	if (VRPlayer::isVRPluginLoaded())
		App::setBackgroundUpdate(true);

#ifdef DISABLE_CONSOLE
	Console::setLock(1);
#endif

	// load world
	Console::run("world_load oil_refinery/oil_refinery");

	const char * video_app = !strcmp(Engine::get()->getVideoApp(), "opengl") ? "OpenGL" : "DirectX";
	App::setTitle(String::format("UNIGINE Oil Refinery Demo (%s)", video_app).get());

	ComponentSystem::get()->initialize();
	Unigine::Console::run("render_auxiliary 1");

	return 1;
}
