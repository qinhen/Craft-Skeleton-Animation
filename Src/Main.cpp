#define CRAFT_ENGINE_GUI_APPLICATION
#include "MyMainWindow.h"

int main()
{
	using namespace CraftEngine;

	core::setUtf8();

	gui::Application app;
	auto main_window = new MyMainWindow();
	main_window->setWindowTitle(L"CraftSkeletonAnimation");
	main_window->getPalette().setBackgroundImage(gui::GuiRenderSystem::createImage("background.png"));
	main_window->loadModel("./models/pd.X");
	main_window->setUpdateEveryFrame(true);
	UI::sceneWidget->setUpdateEveryFrame(true);

	main_window->exec();
	return 0;
}




