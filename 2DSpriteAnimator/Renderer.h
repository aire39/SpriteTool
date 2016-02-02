#ifndef RENDERER_H
#define RENDERER_H

struct SDL_Window;
class HTMLEngine;
class fipImage;
class AppEvent;

class Renderer
{
	private:

		void renderGrid(int xoffset, int yoffset, int size);
		SDL_Window * window;
		HTMLEngine * htmlengine;
		fipImage * fip_image;
		AppEvent * appevent;
		int width;
		int height;
		bool quit;

	public:
		Renderer(SDL_Window * window, HTMLEngine * htmlengine, AppEvent * appevent, int width, int height) : window(window), htmlengine(htmlengine), appevent(appevent), width(width), height(height) {}
		~Renderer(){}

		static int render_wrapper(void *data);
		int render(SDL_Window * window);
		void setImage(fipImage * fip_image);
		void exit();
};

#endif