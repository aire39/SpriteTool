#include "App.h"

int main(int argc, char *argv[])
{
	const int window_width  = 800;
	const int window_height = 600;

	// Initialize and Run Application
	App * m_app = App::instance(window_width, window_height);
	m_app->init();
	m_app->run();

	return 0;

}
