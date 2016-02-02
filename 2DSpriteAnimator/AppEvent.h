#ifndef APPEVENT_H
#define APPEVENT_H

#include <string>

struct SDL_Window;

namespace Awesomium {
	class WebView;
	class JSArray;
};

class fipImage;
class HTMLEngine;
struct SDL_mutex;
union SDL_Event;

class AppEvent
{
	private:

		SDL_Window * window;
		std::string cImgPath;
		SDL_mutex * mutex;
		Awesomium::WebView * wv_main;

		bool quit;
		bool img_uploaded;
		bool done_loading;
		bool m_wrp;
		bool m_wrp2;
		bool transp;
		bool updateViewPort;

		int wi;
		int he;
		int ww;
		int hh;
		float cx;
		float cy;
		int lmcx;
		int lmcy;
		int scale_z;

		fipImage * fip_image;
		HTMLEngine * htmlengine;

	public:
		AppEvent(SDL_Window * window, HTMLEngine * htmlengine, Awesomium::WebView * wv_main);
		int eventFilter(void * pthis, const SDL_Event * evt);
		void setImage(fipImage * fip_image);
		void control();
		void onOpenFile(Awesomium::WebView * caller, const Awesomium::JSArray& arg);

		int getMouseX()
		{
			return cx;
		}

		int getMouseY()
		{
			return cy;
		}

		void exit();
};

#endif