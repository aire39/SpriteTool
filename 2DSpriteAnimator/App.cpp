#include "App.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <FreeImagePlus.h>
#include <Awesomium/BitmapSurface.h>
#include <iostream>

#include "Renderer.h"
#include "HTMLEngine.h"
#include "AweMenuDispatch.h"
#include "AppEvent.h"

App::App(int width, int height) : width (width), height(height) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
}

App::~App() 
{
	renderer->exit();
	appevent->exit();
	SDL_WaitThread(sdl_thread_tool, NULL);
	SDL_WaitThread(sdl_thread, NULL);
	delete appevent;
	delete renderer;
	delete htmlengine;
	SDL_DestroyWindow(window_tool);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

App * App::instance(int width, int height)
{
	static App app(width, height);
	return &app;
}

void App::init() {

	// Load Test Image
	//

	fip_image = new fipImage();
	if (fip_image->load("G:\\Projects\\2DSpriteAnimator\\Debug\\mmx1xsprites.png") == false)
	{
		std::cout << "Unable to load image! (FI)." << std::endl;
		return;
	}
	fip_image->flipVertical();

	// Iniitialize Awesomium and load url page into web view (offscreen buffer).
	// Call update until page has been fully loaded into buffer for use.

	this->htmlengine = new HTMLEngine();

	Awesomium::WebView * webview = htmlengine->addView(width, height);
	htmlengine->attachDataSource(webview, "HTML/menu/pak_menu.pak", "AppMenu");
	htmlengine->wait();

	htmlengine->attachGlobalJS(webview, "app");
	htmlengine->attachMethodJS(webview, "sayHello", "app");
	htmlengine->attachMethodJS(webview, "openFile", "app");
	htmlengine->attachMethodJS(webview, "Quit", "app");
	webview->SetTransparent(true);
	webview->LoadURL(Awesomium::WebURL(Awesomium::WSLit("asset://AppMenu/index.html")));
	htmlengine->wait();

	// Create SDL window and surface for drawing and set the screen/surface to white
	//

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, GL_TRUE);

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{ 
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}

	window_tool = SDL_CreateWindow("App Tool", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window_tool == NULL)
	{
		std::cout << "Window cannot be created. Error: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, GL_TRUE);

	window = SDL_CreateWindow("App", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		std::cout << "Window cannot be created. Error: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, GL_TRUE);

	// Set default graphics surface and color
	//

	SDL_Surface * screen_surface = SDL_GetWindowSurface(window);
	SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));

	// Enable Drag and drop SDL feature
	//

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	// Set event filter for window resize
	//

	//SDL_SetEventFilter((SDL_EventFilter)appevent->eventFilter, NULL); // Window resize

	// Initialize event handling and HTMLEngine menu method dispatch object.
	//

	appevent = new AppEvent(window, htmlengine, getWebView(0));
	htmlengine->addCustomDispatch(webview, new AWEMenuDispatch(appevent));
	appevent->setImage(fip_image);
}

Awesomium::WebView * App::getWebView(int view)
{
	return htmlengine->activeWebView(view);
}

SDL_Window * App::getWindow()
{
	return window;
}

HTMLEngine * App::getHTMLEngine()
{
	return htmlengine;
}

void App::run()
{
	this->renderer = new Renderer(window, window_tool, htmlengine, appevent, width, height);
	this->renderer->setImage(fip_image);

	SDL_GLContext gContext1 = SDL_GL_CreateContext(window_tool);
	if (gContext1 == NULL)
	{
		std::cout << "Unable to create OpenGL (3.3) context..." << std::endl;
	}

	SDL_GLContext gContext0 = SDL_GL_CreateContext(window);
	if (gContext0 == NULL)
	{
		std::cout << "Unable to create OpenGL (3.3) context..." << std::endl;
	}

	this->renderer->setContexts(gContext0, gContext1);

	SDL_GL_MakeCurrent(window_tool, NULL);
	SDL_GL_MakeCurrent(window, NULL);

	sdl_thread = SDL_CreateThread(Renderer::render_wrapper, "renderThread", renderer);
	Sleep(20);
	sdl_thread_tool = SDL_CreateThread(Renderer::render_wrapper_tool, "renderThread_Tool", renderer);

	appevent->control();
}

