#ifndef MSYK_UI_LOGIN_FORM_H__
#define MSYK_UI_LOGIN_FORM_H__

#include "user/teacher.h"

using login_cb = std::function<void()>;
using login_form_close = std::function<void()>;

class LoginForm final : public ui::WindowImplBase
{
public:
	LoginForm(login_cb cb_login = nullptr, login_form_close cb_close = nullptr);
	~LoginForm();

	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetWindowClassName() const;
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
	static  std::wstring GetWindowId();

private:
	void DoLogin();
	void OnLogin(bool, std::string, std::string, std::string, const std::vector<school> &);
	void ShowError(std::wstring txt = L"", bool visible = true);
	
private:
	login_cb cb_login_ = nullptr;
	login_form_close cb_form_close_ = nullptr;
	ui::RichEdit *edt_username_ = NULL;
	ui::RichEdit *edt_password_ = NULL;
};

#endif //~MSYK_UI_LOGIN_FORM_H__