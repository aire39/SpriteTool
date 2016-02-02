#ifndef APP_H
#define APP_H


namespace Awesomium {
	class WebView;
}

struct SDL_Window;
struct SDL_Thread;
class HTMLEngine;
class fipImage;
class Renderer;
class AppEvent;

class App
{
	private:
		int width;
		int height;
		
		SDL_Window * window;
		SDL_Thread * sdl_thread;
		
		App(int width, int height);

		HTMLEngine * htmlengine;
		Renderer   * renderer;
		AppEvent   * appevent;

		fipImage * fip_image; // Test image

	public:
		~App();

		static App * instance(int width = 800, int height = 600);
		void init();
		void run();

		Awesomium::WebView * getWebView(int view);
		SDL_Window * getWindow();
		HTMLEngine * getHTMLEngine();

};



#endif