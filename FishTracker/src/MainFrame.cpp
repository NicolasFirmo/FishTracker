#include "MainFrame.h"
#include "Core.h"
#include "Instrumentation/ScopeTimer.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_BUTTON(10001, MainFrame::OnButtonClickedEvent)
EVT_BUTTON(10002, MainFrame::OnSave)
wxEND_EVENT_TABLE()

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Save Color"), m_ColorMat(cv::Mat(cv::Size(512, 512), CV_8UC3, cv::Scalar(255, 255, 255)))
{
	CreateStatusBar();

	m_Btn1 = new wxButton(this, 10001, "Set Color");
	m_Btn2 = new wxButton(this, 10002, "Save in a file");
	m_Txt1 = new wxTextCtrl(this, wxID_ANY, "255,255,255");

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(m_Btn1, 2, wxEXPAND | (wxALL & ~wxBOTTOM), 10);
	sizer->Add(m_Txt1, 3, wxEXPAND | (wxALL & ~wxBOTTOM), 10);
	sizer->Add(m_Btn2, 2, wxEXPAND | wxALL, 10);

	this->SetSizer(sizer);
	sizer->Layout();
}

MainFrame::~MainFrame()
{
}

void MainFrame::OnButtonClickedEvent(wxCommandEvent& e)
{
	ft::ScopeTimer t(this);
	auto txt = m_Txt1->GetValue();
	if ("" == txt)
		return;

	unsigned char rgb[3];
	GetColor(rgb);
	if (rgb)
		m_ColorMat = cv::Mat(cv::Size(512, 512), CV_8UC3, cv::Scalar(rgb[2], rgb[1], rgb[0]));

	cv::namedWindow("Test", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL);
	cv::imshow("Test", m_ColorMat);

	e.Skip();
}

void MainFrame::OnSave(wxCommandEvent& e)
{
	wxFileDialog saveFileDialog(this, _("Imagem"), "", "",
		"JPEG files (*.jpeg)|*.jpeg|PNG files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return; // the user changed idea...

	auto filepath = (std::string)saveFileDialog.GetPath();
	cv::imwrite(filepath, m_ColorMat);
}

void MainFrame::GetColor(unsigned char* rgb)
{
	std::string txt = (std::string)m_Txt1->GetValue();
	if ("" == txt)
	{
		return;
	}
	int pos = 0;
	for (int i = 0; i < 2; i++)
	{
		int endPos = txt.find(',', pos);
		if (endPos == std::string::npos)
		{
			wxLogError("The color you choose is invalid! separete your rbg values with ',' (comma).");
			rgb = nullptr;
			return;
		}
		rgb[i] = (unsigned char)std::stoi(txt.substr(pos, endPos - pos));
		pos = endPos + 1;
	}

	rgb[2] = (char)std::stoi(txt.substr(pos));
}