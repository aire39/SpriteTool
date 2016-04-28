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

enum {CONTROL_DEFAULT, CONTROL_SELBOX};

class AppEvent
{
	private:

		SDL_Window * window;
		std::string cImgPath;
		SDL_mutex * mutex;
		Awesomium::WebView * wv_main;

		char control_path;

		bool quit;
		bool img_uploaded;
		bool done_loading;
		bool m_wrp;
		bool m_wrp2;
		bool transp;
		bool updateViewPort;
		bool selection_box;

		int wi;
		int he;
		int ww;
		int hh;
		int cx;
		int cy;
		int lmcx;
		int lmcy;
		int scale_z;

		fipImage * fip_image;
		HTMLEngine * htmlengine;

		void default_set(const SDL_Event &e);
		void set1(const SDL_Event &e);
		void set2(const SDL_Event &e);

	public:
		AppEvent(SDL_Window * window, HTMLEngine * htmlengine, Awesomium::WebView * wv_main);
		int eventFilter(void * pthis, const SDL_Event * evt);
		void setImage(fipImage * fip_image);
		void control();
		void onOpenFile(Awesomium::WebView * caller, const Awesomium::JSArray& arg);

		void setControlPath(int id);

		int getMouseX()
		{
			return cx;
		}

		int getMouseY()
		{
			return cy;
		}

		bool isSelectionBox()
		{
			return selection_box;
		}

		void exit();
};

#endif