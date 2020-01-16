#pragma once

class MainFrame : public wxFrame
{
public:
	MainFrame();
	void OnClose(wxCloseEvent& evt);

	void OnLoadVideo(wxCommandEvent& evt);
	void OnPlay(wxCommandEvent& evt);
	void OnPause(wxCommandEvent& evt);

	void Render();
public:
	wxButton* m_LoadBtn = nullptr;
	wxButton* m_PlayBtn = nullptr;
	wxButton* m_PauseBtn = nullptr;

	wxDECLARE_EVENT_TABLE();
private:
	cv::VideoCapture m_Cap;
	cv::Mat m_VideoMat;

	double m_DeltaTime = 0;

	bool m_VideoLoaded = false;
	bool m_VideoPlaying = false;

	int m_VideoFPS;
};