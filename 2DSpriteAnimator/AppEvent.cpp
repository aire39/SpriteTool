#include "AppEvent.h"
#include "HTMLEngine.h"
#include <Windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_mutex.h>
#include <Awesomium/WebView.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/BitmapSurface.h>
#include <FreeImagePlus.h>
#include <iostream>

AppEvent::AppEvent(SDL_Window * window, HTMLEngine * htmlengine, Awesomium::WebView * wv_main)
{
	this->control_path = 0;
	this->window = window;
	this->htmlengine = htmlengine;
	this->wv_main = wv_main;
	this->ww = 800;
	this->hh = 600;
	this->selection_box = false;
	cx = 0.0f;
	cy = 0.0f;
	lmcx = 0;
	lmcy = 0;
	wi = 160.0f;
	he = 240.0f;
	mutex = SDL_CreateMutex();
}

void AppEvent::setImage(fipImage * fip_image)
{
	this->fip_image = fip_image;
}

void AppEvent::default_set(const SDL_Event &e)
{
	if (e.type == SDL_QUIT)
	{
		quit = true;
		//m_app->exit();
	}

	if (e.type == SDL_DROPFILE)
	{
		std::cout << "Drop File: " << e.drop.file << std::endl;
		cImgPath = e.drop.file;
		img_uploaded = true;
		done_loading = false;
		SDL_free(e.drop.file);
	}
}

void AppEvent::set1(const SDL_Event &e)
{


	if (e.type == SDL_MOUSEMOTION)
	{
		if (m_wrp == true && e.button.button == SDL_BUTTON_MIDDLE)
		{
			SDL_WarpMouseInWindow(window, ww / 2, hh / 2);
			cx += e.motion.x - (ww / 2);
			cy += e.motion.y - (hh / 2);

			Awesomium::JSValue jso_window = wv_main->ExecuteJavascriptWithResult(Awesomium::WSLit("window"), Awesomium::WSLit(""));
			Awesomium::JSArray jso_props;
			jso_props.Push(Awesomium::JSValue((int)cx));
			jso_props.Push(Awesomium::JSValue((int)cy));
			jso_window.ToObject().InvokeAsync(Awesomium::WSLit("updateCamMovement"), jso_props);
		}
		else if (m_wrp2 == true)
		{
			SDL_WarpMouseInWindow(window, ww / 2, hh / 2);
			scale_z += (e.motion.y - (hh / 2.0f)) / 1000.0f;
			std::cout << scale_z << std::endl;
		}
		else
		{
			wv_main->InjectMouseMove(e.motion.x, e.motion.y);
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			wv_main->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Left);
		if (e.button.button == SDL_BUTTON_MIDDLE) {
			lmcx = e.motion.x;
			lmcy = e.motion.y;
			SDL_WarpMouseInWindow(window, ww / 2, hh / 2);
			SDL_ShowCursor(SDL_DISABLE);
			m_wrp = true;
		}
		if (e.button.button == SDL_BUTTON_RIGHT) {
			m_wrp2 = true;
			lmcx = e.motion.x;
			lmcy = e.motion.y;
			SDL_WarpMouseInWindow(window, ww / 2, hh / 2);
			SDL_ShowCursor(SDL_DISABLE);
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			wv_main->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Left);

		if (e.button.button == SDL_BUTTON_MIDDLE || e.button.button == SDL_BUTTON_RIGHT) {
			SDL_ShowCursor(SDL_ENABLE);
			SDL_WarpMouseInWindow(window, lmcx, lmcy);
			m_wrp = false;
			m_wrp2 = false;
		}
	}
}

void AppEvent::set2(const SDL_Event &e)
{

}

void AppEvent::control()
{
	// Window controls such as Keyboard, Mouse, etc is hadnled here.
	//

	while (quit == false)
	{
		SDL_Event e;

		while (SDL_PollEvent(&e))
		{
			default_set(e);

			if (control_path == 0)
				set1(e);

			if (control_path == 1)
				set2(e);


			if (img_uploaded == true)
			{
				SDL_mutexP(mutex);
				fip_image->clear();
				if (fip_image->load(cImgPath.c_str()) == false)
				{
					std::cout << "Failed to load image! : " << cImgPath.c_str() << std::endl;
				}
				fip_image->flipVertical();

				transp = fip_image->isTransparent();
				wi = (float)fip_image->getWidth();
				he = (float)fip_image->getHeight();
				SDL_mutexV(mutex);
				done_loading = true;
			}

			htmlengine->update();
			while (wv_main->IsLoading())
				htmlengine->update();
		}

		SDL_Delay(16);
	}

	SDL_DestroyMutex(mutex);
}


void AppEvent::onOpenFile(Awesomium::WebView * caller, const Awesomium::JSArray& arg)
{

	if (((Awesomium::BitmapSurface*)wv_main->surface())->is_dirty() == false)
	{
		#ifdef _WIN32

		OPENFILENAME ofn;       // common dialog box structure
		char szFile[260];       // buffer for file name
		HWND hwnd;              // owner window
		HANDLE hf = NULL;       // file handle

		// Initialize OPENFILENAME
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window, &wmInfo);
		hwnd = wmInfo.info.win.window;

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "All\0*.*\0png\0*.png\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box.

		if (GetOpenFileName(&ofn) == TRUE)
			hf = CreateFile(ofn.lpstrFile,
			GENERIC_READ,
			0,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			(HANDLE)NULL);

		cImgPath = ofn.lpstrFile;
		if (cImgPath.length() <= 0)
			return;

		img_uploaded = true;
		done_loading = false;

		std::cout << "Open File: " << cImgPath << std::endl;

		CloseHandle(hf);
		#else
		#endif


	}

}

void AppEvent::setControlPath(int id)
{
	control_path = id;
}

int AppEvent::eventFilter(void * pthis, const SDL_Event * evt)
{
	if (evt->type == SDL_WINDOWEVENT)
	{
		if (evt->window.event == SDL_WINDOWEVENT_RESIZED)
		{
			ww = evt->window.data1;
			hh = evt->window.data2;
			updateViewPort = true;
			wv_main->Resize(ww, hh);
		}
	}

	return 1;
}

void AppEvent::exit()
{
	quit = true;
}