#include "HTMLEngine.h"
#include <Awesomium\WebCore.h>
#include <Awesomium\WebConfig.h>
#include <Awesomium\WebView.h>
#include <Awesomium\DataPak.h>
#include <Awesomium\STLHelpers.h>
#include "AWEMenuDispatch.h"

HTMLEngine::HTMLEngine() {
	active = -1;
	webcore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
}

HTMLEngine::~HTMLEngine()
{
	for (int i = 0; i < webviews.size(); i++)
		webviews[i]->Destroy();

	for (int i = 0; i < webviews_js_method_handler.size(); i++)
		delete webviews_js_method_handler[i];

	webcore->Shutdown();
}

Awesomium::WebView * HTMLEngine::addView()
{
	// Add a webview or tab if thinking in terms of an actual modern browser.
	//

	Awesomium::WebView * n_webview = webcore->CreateWebView(800, 600);
	n_webview->SetTransparent(true);
	webviews.push_back(n_webview);
	if (webviews.size() == 1)
		active = 0;
	return n_webview;
}

Awesomium::WebView * HTMLEngine::addView(int width, int height)
{
	Awesomium::WebView * n_webview = webcore->CreateWebView(width, height);
	n_webview->SetTransparent(true);
	webviews.push_back(n_webview);
	if (webviews.size() == 1)
		active = 0;
	return n_webview;
}

Awesomium::WebView * HTMLEngine::activeWebView(int c_webview)
{
	// Set active view to focus on. This also return the active webview.
	//

	active = c_webview;
	if (c_webview < 0 || c_webview > webviews.size())
		return NULL;

	return webviews[c_webview];
}

void HTMLEngine::attachDataSource(Awesomium::WebView * webview, std::string pak_location, std::string name)
{
	// This is a way to use local web files such as local HTML, CSS, and JavaScript files that get compiled
	// into one file (*.pak). It can attach itself to any webview.
	//

	Awesomium::DataSource * data_source_menu = new Awesomium::DataPakSource(Awesomium::WSLit(pak_location.c_str()));
	webview->session()->AddDataSource(Awesomium::WSLit(name.c_str()), data_source_menu);
}

void HTMLEngine::addCustomDispatch(Awesomium::WebView * webview, AWEMenuDispatch * amd)
{
	// How to handle JavaScript functions made to call C++ functions.
	//

	webviews_js_method_handler.push_back(amd);
	webview->set_js_method_handler(amd);
}

void HTMLEngine::attachGlobalJS(Awesomium::WebView * webview, std::string object_name)
{
	webview->CreateGlobalJavascriptObject(Awesomium::WSLit(object_name.c_str()));

	Sleep(500); // Idle is here because Awesomium works asynchronously and doesn't have a very good way to deal with knowing
				// when an operation has completed. This shuoldn't really take long but I've ran into otherwise and the way
				// to deal with it can be tricky though there seems to be an easy solution for C#. Just giving time to communicate
				// with awesomium since it runs on it's on process.
}

void HTMLEngine::attachMethodJS(Awesomium::WebView * webview, std::string method_name, std::string object_name, bool hasResult)
{
	Awesomium::JSValue result = webview->ExecuteJavascriptWithResult(Awesomium::WSLit(object_name.c_str()), Awesomium::WSLit(""));
	Awesomium::JSObject object = result.ToObject();
	object.SetCustomMethod(Awesomium::WSLit(method_name.c_str()), hasResult);
}

void HTMLEngine::update()
{
	webcore->Update();
}

void HTMLEngine::wait()
{
	for (int i = 0; i < webviews.size(); i++)
	while (webviews[i]->IsLoading())
		webcore->Update();
}