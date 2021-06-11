#ifndef __APP_WORLD_LOGIC_H__
#define __APP_WORLD_LOGIC_H__

#include <UnigineLogic.h>
#include <UnigineStreams.h>
#include "Framework/Components/VRPlayerSpawner.h"
#include "Mode.h"
#include "Demo/Logic/GeneratorLogic.h"
#include "Demo/Logic/UdhLogic.h"

class AppWorldLogic : public Unigine::WorldLogic
{
private:
	GeneratorLogic *generator_logic;
	UdhLogic *udh_logic;
	Mode *telemetry = nullptr;

	bool interactive_player_init;
	VRPlayerSpawner *spawner;
	PlayerPtr cinematic_player;

	void exitMode(Mode *& mode);

public:
	void setFreeMode();
	void setGeneratorInteractiveMode(int training_mode);
	void setUdhInteractiveMode(int training_mode);
	void setTelemetryMode();

public:
	AppWorldLogic();
	
	int init() override;
	int update() override;
	int shutdown() override;
};

#endif // __APP_WORLD_LOGIC_H__
