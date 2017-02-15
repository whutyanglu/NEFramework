#include "stdafx.h"
#include "login_form.h"
#include "shared/closure.h"
#include "api/net_api.h"

LoginForm::LoginForm(login_cb cb_login, login_form_close cb_close)
	: cb_login_(cb_login)
	, cb_form_close_(cb_close)
{

}

LoginForm::~LoginForm()
{
}

std::wstring LoginForm::GetSkinFile()
{
	return L"login_form.xml";
}

std::wstring LoginForm::GetSkinFolder()
{
	return L"login";
}

std::wstring LoginForm::GetWindowClassName() const
{
	return L"LoginForm";
}

void LoginForm::InitWindow()
{
	ui::Button *btn_close = dynamic_cast<ui::Button*>(pm_.FindControl(L"btn_close"));
	btn_close->AttachClick([this](const ui::EventArgs &arg) {
		if (cb_form_close_) {
			cb_form_close_();
		}
		Close();
		return true;
	});

	ui::Button *btn_login = dynamic_cast<ui::Button*>(pm_.FindControl(L"btn_login"));
	btn_login->AttachClick([this](const ui::EventArgs &arg) {
		DoLogin();
		return true;
	});

	edt_username_ = dynamic_cast<ui::RichEdit*>(pm_.FindControl(L"edt_username"));
	edt_password_ = dynamic_cast<ui::RichEdit*>(pm_.FindControl(L"edt_password"));
}

void LoginForm::OnFinalMessage(HWND hWnd)
{
	pm_.Cleanup();
}

std::wstring LoginForm::GetWindowId()
{
	return L"loginform_window_id";
}

void LoginForm::DoLogin()
{
	using namespace shared;
	using namespace std::placeholders;
	std::string user_name = edt_username_->GetUTF8Text();
	std::string password = edt_password_->GetUTF8Text();

	if (user_name.empty() ) {
		ShowError(L"用户名不能为空");
		return;
	}

	if (password.empty()) {
		ShowError(L"密码不能为空");
		return;
	}

	ShowError(L"", false);

	std::shared_ptr<NetApi> ptr(new NetApi());
	ptr->Login(user_name, password, 
		ToWeakCallback(nbase::Bind(&LoginForm::OnLogin, this, _1, _2, _3, _4, _5)));
}

void LoginForm::OnLogin(bool succ, std::string mesage, std::string name, std::string id, const std::vector<school>& schools)
{
	Post2UI(ToWeakCallback([this,succ, mesage, name, id, schools]() {
		if (!succ) {
			ShowError(nbase::UTF8ToUTF16(mesage));
		}
		else {
			Teacher::GetInstance()->SetId(id);
			Teacher::GetInstance()->SetName(name);
			Teacher::GetInstance()->SetSchools(schools);
			Close();
			if (cb_login_) {
				cb_login_();
			}
		}
	}));
}

void LoginForm::ShowError(std::wstring txt, bool visible)
{
	ui::Label *label_error = dynamic_cast<ui::Label *>(pm_.FindControl(L"txt_error"));
	label_error->SetText(txt);
	label_error->SetVisible(visible);
}
