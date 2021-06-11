#include "AppWorldLogic.h"
#include <UnigineInterpreter.h>
#include <UnigineInterface.h>
#include <UnigineApp.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineEditor.h>

#include "Telemetry.h"

AppWorldLogic::AppWorldLogic()
{
	generator_logic = nullptr;
	udh_logic = nullptr;
}

int AppWorldLogic::init()
{
	Unigine::Console::run("render_auxiliary 1");

	interactive_player_init = VRPlayer::isVRPluginLoaded() ? true : false;

	NodePtr spawner_node = World::getNodeByName("spawn_point");
	if (!spawner_node)
	{
		Log::error("APP WORLD LOGIC ERROR: spawn point not found\n");
		return 0;
	}

	spawner = ComponentSystem::get()->getComponent<VRPlayerSpawner>(spawner_node);
	if (!spawner)
	{
		Log::error("APP WORLD LOGIC ERROR: spawn point node not contain VRPLayerSpawner component\n");
		return 0;
	}

	NodePtr cinematic_node = World::getNodeByName("cinematic_player");
	if (!cinematic_node)
	{
		Log::error("APP WORLD LOGIC ERROR: cinematic player not found\n");
		return 0;
	}

	cinematic_player = checked_ptr_cast<Player>(cinematic_node);
	if (!cinematic_player)
	{
		Log::error("APP WORLD LOGIC ERROR: can't cast cinematic player from cinematic node\n");
		return 0;
	}

	NodePtr logic_layer = World::getNodeByName("logic_layer");
	if (!logic_layer)
	{
		Log::error("APP WORLD LOGIC ERROR: logic layer not found\n");
		return 0;
	}

	generator_logic = ComponentSystem::get()->getComponentInChildren<GeneratorLogic>(logic_layer);
	if (!generator_logic)
	{
		Log::error("APP WORLD LOGIC ERROR: generator logic component not found\n");
		return 0;
	}

	udh_logic = ComponentSystem::get()->getComponentInChildren<UdhLogic>(logic_layer);
	if (!udh_logic)
	{
		Log::error("APP WORLD LOGIC ERROR: udh logic component not found\n");
		return 0;
	}

	return 1;
}

void AppWorldLogic::setFreeMode()
{
	// disable other modes
	exitMode(telemetry);

	// disable VRPlayer
	// (otherwise keys Q and E would not work right in Fly Over mode)
	if (VRPlayer::get())
		VRPlayer::get()->getPlayer()->setEnabled(false);

	if (generator_logic && generator_logic->isInitialized() && generator_logic->isActive())
		generator_logic->stop();

	if (udh_logic && udh_logic->isInitialized() && udh_logic->isActive())
		udh_logic->stop();
}

void AppWorldLogic::setGeneratorInteractiveMode(int training_mode)
{
	// disable other modes
	exitMode(telemetry);

	// enable VRPlayer
	if (VRPlayer::get())
		VRPlayer::get()->getPlayer()->setEnabled(true);

	if (udh_logic && udh_logic->isActive())
		udh_logic->stop();

	// enable interactive mode
	if (!generator_logic)
		return;

	if (generator_logic->isActive())
		generator_logic->stop();

	generator_logic->start(training_mode);
	ControlsApp::setMouseEnabled(false);
}

void AppWorldLogic::setUdhInteractiveMode(int training_mode)
{
	// disable other modes
	exitMode(telemetry);

	// enable VRPlayer
	if (VRPlayer::get())
		VRPlayer::get()->getPlayer()->setEnabled(true);

	if (generator_logic && generator_logic->isActive())
		generator_logic->stop();

	// enable interactive mode
	if (!udh_logic)
		return;

	if (udh_logic->isActive())
		udh_logic->stop();

	udh_logic->start(training_mode);
}


void AppWorldLogic::setTelemetryMode()
{
	// disable VRPlayer
	// (otherwise keys Q and E would not work right in Fly Over mode)
	if (VRPlayer::get())
		VRPlayer::get()->getPlayer()->setEnabled(false);

	// disable other modes
	if (generator_logic && generator_logic->isActive())
		generator_logic->stop();

	if (udh_logic && udh_logic->isActive())
		udh_logic->stop();

	// enable telemetry mode
	if (telemetry)
		return;

	telemetry = new Telemetry();
	telemetry->init();
}

void AppWorldLogic::exitMode(Mode *& mode)
{
	if (!mode)
		return;

	mode->shutdown();
	delete mode;
	mode = nullptr;
}

int AppWorldLogic::update()
{
	// soft mouse for non-training modes
	if (!generator_logic->isActive() && !udh_logic->isActive())
	{
		if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON_LEFT) && !Gui::get()->isActive())
			ControlsApp::setMouseEnabled(true);
		else
			ControlsApp::setMouseEnabled(false);
	}

	if (!interactive_player_init && spawner->isInitialized())
	{
		Game::setPlayer(cinematic_player);
		if (VRPlayer::get())
			VRPlayer::get()->getPlayer()->setEnabled(false);

		interactive_player_init = true;
	}

	if (telemetry)
		telemetry->update();

	if (Input::isKeyDown(Input::KEY_F8))
		Console::run("world_reload");

	return 1;
}

int AppWorldLogic::shutdown()
{
	generator_logic = nullptr;
	udh_logic = nullptr;

	return 1;
}
