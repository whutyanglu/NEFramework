#ifndef MSYK_UI_CEF_BOX_H__
#define MSYK_UI_CEF_BOX_H__

#include "shared/cef/cef_browser_control.h"
#include "dynamic_box.h"

class CefBox : public DynamicBox
{
public:
	CefBox(std::string url = "");

	void Navigate(const std::string &url, shared::js_callback cb = nullptr, std::string message_name = "", std::string message = "");
	void ExecuteJavaScript(const std::string& code, const std::string& script_url, int start_line = 0);
	virtual void OnInit();
	virtual void SetVisible(bool visible = true);

	bool CanGoBack();
	bool CanGoForward();
	void GoBack();
	void GoForward();
	void Refresh();
	
private:
	std::string url_;
	shared::CefBrowserControl *browser_ = NULL;
};


#endif // MSYK_UI_CEF_BOX_H__