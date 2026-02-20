#include "Source/Essential/SampleFramework.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    std::unique_ptr<Engine::Framework> game = std::make_unique<SampleFramework>();

    game->Run();

	return 0;
}

