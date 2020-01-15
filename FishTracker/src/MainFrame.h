#pragma once

class MainFrame : public wxFrame
{
public:
	MainFrame();
	~MainFrame();

	void OnButtonClickedEvent(wxCommandEvent &e);
	void OnSave(wxCommandEvent &e);

public:
	wxButton *m_Btn1 = nullptr;
	wxButton *m_Btn2 = nullptr;
	wxTextCtrl *m_Txt1 = nullptr;

	std::ofstream m_Txtfile;

	wxDECLARE_EVENT_TABLE();

private:
	cv::Mat m_ColorMat;
	void GetColor(unsigned char *rgb);
};