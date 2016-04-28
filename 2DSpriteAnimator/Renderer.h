#ifndef RENDERER_H
#define RENDERER_H

struct SDL_Window;
class HTMLEngine;
class fipImage;
class AppEvent;

typedef void* SDL_GLContext;
typedef unsigned int GLuint;

class Renderer
{
	private:

		GLuint ld_img;

		void renderGrid(int xoffset, int yoffset, int size);
		SDL_Window * window;
		SDL_Window * window_tool;

		SDL_GLContext gContext_main;
		SDL_GLContext gContext_tool;

		HTMLEngine * htmlengine;
		fipImage * fip_image;
		AppEvent * appevent;
		bool switcher;
		int width;
		int height;
		bool quit;

		unsigned int aba;

	public:
		Renderer(SDL_Window * window, SDL_Window * window_tool, HTMLEngine * htmlengine, AppEvent * appevent, int width, int height) : window(window), window_tool(window_tool), htmlengine(htmlengine), appevent(appevent), width(width), height(height) { switcher = false; }
		~Renderer(){}

		void setContexts(SDL_GLContext gContext0, SDL_GLContext gContext1)
		{
			gContext_main = gContext0;
			gContext_tool = gContext1;
		}

		static int render_wrapper(void *data);
		static int render_wrapper_tool(void *data);
		int render(SDL_Window * window);
		int render_tool(SDL_Window * window);
		void setImage(fipImage * fip_image);
		void exit();
};

#endif