#include "Renderer.h"
#include "App.h"
#include "HTMLEngine.h"
#include "AppEvent.h"

#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <FreeImagePlus.h>
#include <Awesomium/WebView.h>
#include <Awesomium/BitmapSurface.h>

#include <iostream>

int Renderer::render_wrapper(void *data)
{
	Renderer * renderer = (Renderer*)data;
	renderer->render(renderer->window);
	return 0;
}

int Renderer::render_wrapper_tool(void *data)
{
	Renderer * renderer = (Renderer*)data;
	renderer->render_tool(renderer->window_tool);
	return 0;
}

void Renderer::renderGrid(int xoffset, int yoffset, int size)
{
	const float squareSize = 32.0f;
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(0.15f, 0.15f, 0.15f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPushMatrix();

	for (int i = 0; i < (int)(ceil(800.0f / squareSize) + 1 + size); i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < (int)(ceil(600.0f / squareSize) + 1 + size); j++)
		{
			glVertex2f(j*squareSize - squareSize - xoffset, i*squareSize + squareSize - yoffset);
			glVertex2f(j*squareSize - squareSize - xoffset, i*squareSize - yoffset);
			glVertex2f(j*squareSize - xoffset, i*squareSize + squareSize - yoffset);
			glVertex2f(j*squareSize - xoffset, i*squareSize - yoffset);
		}
		glEnd();
	}

	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.25f, 0.25f, 0.25f);
	const int step = (int)squareSize;
	for (int i = 0; i < (int)(ceil(800.0f / squareSize) + 2 + size); i++)
	{
		glBegin(GL_QUADS);
		for (int j = 0; j < (int)(ceil(600.0f / squareSize) + 2 + size); j++)
		{
			glVertex2f(j*squareSize - squareSize - floor((xoffset / step)*squareSize), i*squareSize + squareSize - floor((yoffset / step)*squareSize));
			glVertex2f(j*squareSize - squareSize - floor((xoffset / step)*squareSize), i*squareSize - floor((yoffset / step)*squareSize));
			glVertex2f(j*squareSize - floor((xoffset / step)*squareSize), i*squareSize - floor((yoffset / step)*squareSize));
			glVertex2f(j*squareSize - floor((xoffset / step)*squareSize), i*squareSize + squareSize - floor((yoffset / step)*squareSize));
		}
		glEnd();
	}
	glPopMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glColor3f(1.0f, 1.0f, 1.0f);
}

int Renderer::render(SDL_Window * window)
{
	GLuint texID = 0;
	GLuint pWidth = width;
	GLuint pHeight = height;
	int ww = width;
	int hh = height;

	SDL_Window * w = static_cast<SDL_Window*>(window);
	SDL_GL_MakeCurrent(window, gContext_main);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenTextures(1, &ld_img);
	glBindTexture(GL_TEXTURE_2D, ld_img);

	int wi = (float)fip_image->getWidth();
	int he = (float)fip_image->getHeight();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)wi, (GLsizei)he, 0, GL_BGRA, GL_UNSIGNED_BYTE, fip_image->accessPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ww, hh, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	Awesomium::BitmapSurface * wv_surface = static_cast<Awesomium::BitmapSurface*>(htmlengine->activeWebView(0)->surface());
	wv_surface->set_is_dirty(true);
	SDL_GL_MakeCurrent(window, NULL);

	while (quit == false)
	{
		SDL_GL_MakeCurrent(window, NULL);
		while (switcher == true && quit == false);
		SDL_GL_MakeCurrent(window, gContext_main);
		glViewport(0, 0, ww, hh);

		if ((((int)pWidth != (int)ww) || ((int)pHeight != (int)hh)) && (wv_surface != NULL))
		{
			// Width and/or height has changed. Must update texture.
			Sleep(800);
			wv_surface = (Awesomium::BitmapSurface*)htmlengine->activeWebView(0)->surface();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ww, hh, 0, GL_BGRA, GL_UNSIGNED_BYTE, wv_surface->buffer());
			pWidth = ww;
			pHeight = hh;
		}

		/*if (img_uploaded == true)
		{
		if (done_loading == true)
		{
		glBindTexture(GL_TEXTURE_2D, ld_img);
		if (transp == TRUE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)wi, (GLsizei)he, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
		else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)wi, (GLsizei)he, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
		img_uploaded = false;
		}
		}*/

		if (wv_surface->is_dirty())
		{
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ww, hh, GL_BGRA, GL_UNSIGNED_BYTE, wv_surface->buffer());
			if (wv_surface->is_dirty())
				wv_surface->set_is_dirty(false);
		}

		/*if (updateViewPort == true)
		{
		updateViewPort = false;
		glViewport(0, 0, ww, hh);
		}*/

		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 600, 800, 0, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glScalef(scale_z, scale_z, 1.0f);
		glTranslatef(appevent->getMouseX(), appevent->getMouseY(), 0.0f);

		renderGrid(appevent->getMouseX(), appevent->getMouseY(), 50);
		
		glPushMatrix();

		glBindTexture(GL_TEXTURE_2D, ld_img);
		if (fip_image->isValid() == TRUE)
		{
			//if (transp == TRUE)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)wi, (GLsizei)he, GL_BGRA, GL_UNSIGNED_BYTE, fip_image->accessPixels());
			//else
			//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)wi, (GLsizei)he, GL_BGR, GL_UNSIGNED_BYTE, fip_image->accessPixels());
		}
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(wi / 2.0f - (wi / 4.0f), he / 2.0f - (he / 4.0f) + 40.0f, 0.0f);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0, he);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(wi, he);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(wi, 0);
		glEnd();
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);

		glPushMatrix();
			glTranslatef(wi / 2.0f - (wi / 4.0f), he / 2.0f - (he / 4.0f) + 40.0f, 0.0f);
		float pwi = (float)wi / 9.0f;
		float phe = (float)he / 11.0f;

		for (int i = 0; i < 11; i++)
		{
			float ii = (float)he / 11.0f * (float)i;
			for (int j = 0; j < 9; j++)
			{
				float jj = (float)wi / 9.0f * (float)j;

				//std::cout << (j + (i * 8)) << ": " << ((float)j) / 8.0f << std::endl;

				glColor4f(((float)j) / 9.0f, ((float)i) / 11.0f, 0.0f, 0.5f);
				glBegin(GL_TRIANGLE_STRIP);

				glVertex2f(jj, ii + phe);
				glVertex2f(jj, ii);
				glVertex2f(jj + pwi, ii + phe);
				glVertex2f(jj + pwi, ii);

				glEnd();
			}
		}
		glPopMatrix();


		//Render menu
		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, texID);
		glLoadIdentity();
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, 1.0f);
		glEnd();

		SDL_Delay(17);
		SDL_GL_SwapWindow(w);
		switcher ^= true;

	}

	glDeleteTextures(1, &texID);

	return 0;
}

int Renderer::render_tool(SDL_Window * window)
{
	SDL_GL_MakeCurrent(window, gContext_tool);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 600, 800, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	SDL_GL_MakeCurrent(window, NULL);
	while (quit == false)
	{
		SDL_GL_MakeCurrent(window, NULL);
		while (switcher == false && quit == false);
		SDL_GL_MakeCurrent(window, gContext_tool);

		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();

		glPushMatrix();
		
			glTranslatef(0.0f, 0.0f, 0.0f);

			glBindTexture(GL_TEXTURE_2D, ld_img);
			glColor3f(1.0f, 1.0f, 1.0f);
			
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(0, fip_image->getHeight());
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(0, 0);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(fip_image->getWidth(), fip_image->getHeight());
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(fip_image->getWidth(), 0);
			glEnd();

		glPopMatrix();

		////////////////

		glPushMatrix();

		glTranslatef(0.0f, 0.0f, 0.0f);

		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(.0f, 1.0f, 1.0f, 0.5f);

		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(0, fip_image->getHeight()/32.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(fip_image->getWidth() / 32.0f, fip_image->getHeight() / 32.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(fip_image->getWidth() / 32.0f, 0);
		glEnd();

		glPopMatrix();

		SDL_Delay(10);
		SDL_GL_SwapWindow(window);

		switcher ^= true;
	}

	return 0;
}

void Renderer::setImage(fipImage * fip_image)
{
	this->fip_image = fip_image;
}

void Renderer::exit()
{
	quit = true;
}