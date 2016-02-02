#ifndef HTMLENGINE_H
#define HTMLENGINE_H

/*
* HTMLEngine uses the library Awesomium to use a browser engine as a drawing medium.
* This is used to grab generated HTML, CSS and JavaScript needed to create the menu
* This also handles callbacks from/to the JavaScript and C++ functions.
*/

#include <vector>

namespace Awesomium{
	class WebView;
	class WebCore;
};

class AWEMenuDispatch;

class HTMLEngine
{
	private:
		std::vector<Awesomium::WebView*> webviews;
		std::vector<AWEMenuDispatch*> webviews_js_method_handler;
		Awesomium::WebCore* webcore;
		int active;
	public:
		HTMLEngine();
		~HTMLEngine();

		Awesomium::WebView * addView();
		Awesomium::WebView * addView(int width, int height);
		Awesomium::WebView * activeWebView(int c_webview);

		void attachDataSource(Awesomium::WebView * webview, std::string pak_location, std::string name);
		void addCustomDispatch(Awesomium::WebView * webview, AWEMenuDispatch * amd);
		void attachGlobalJS(Awesomium::WebView * webview, std::string object_name);
		void attachMethodJS(Awesomium::WebView * webview, std::string method_name, std::string object_name, bool hasResult = false);

		void update();
		void wait();
};

#endif