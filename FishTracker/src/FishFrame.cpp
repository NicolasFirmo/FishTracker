#include "App.h"
#include "IDs.h"
#include "FishFrame.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"
#include "ROI.h"

wxBEGIN_EVENT_TABLE(ft::FishFrame, wxFrame)
EVT_CLOSE(ft::FishFrame::OnClose)
EVT_SIZE(ft::FishFrame::OnSize)

EVT_BUTTON(FT_ID_PLAY, ft::FishFrame::OnPlay)
EVT_BUTTON(FT_ID_PAUSE, ft::FishFrame::OnPause)
EVT_BUTTON(FT_ID_FASTFOWARD, ft::FishFrame::OnFastFoward)

EVT_BUTTON(FT_ID_ADDROI, ft::FishFrame::OnAddROI)
EVT_BUTTON(FT_ID_DELETEROI, ft::FishFrame::OnDeleteROI)
EVT_BUTTON(FT_ID_UNACTIVEROIMODE, ft::FishFrame::OnUnactiveROIMode)
EVT_BUTTON(FT_ID_COUNTROIMODE, ft::FishFrame::OnCountROIMode)
EVT_BUTTON(FT_ID_UNCOUNTROIMODE, ft::FishFrame::OnUncountROIMode)
EVT_LIST_ITEM_SELECTED(FT_ID_ROILIST, ft::FishFrame::OnSelectedROI)
EVT_LIST_ITEM_DESELECTED(FT_ID_ROILIST, ft::FishFrame::OnDeselectedROI)
wxEND_EVENT_TABLE()

namespace ft
{

	FishFrame::FishFrame(const std::string& videoPath) : wxFrame(nullptr, wxID_ANY, "Inspector", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
		m_Cap(videoPath),
		m_SleepDuration(std::chrono::microseconds(0)),
		m_FrameTimePoint(std::chrono::high_resolution_clock::now()), m_SleepTimePoint(std::chrono::high_resolution_clock::now())
	{
		FT_PROFILE_FUNCTION();
		if (m_Cap.isOpened())
		{
			CreateStatusBar();

			m_Cap.read(m_CapFrame);

			m_VideoFrameDuration = std::chrono::nanoseconds((long long)(1000000000.0 / m_Cap.get(cv::CAP_PROP_FPS)));

			m_OriginalFrameSize.width = (int)m_Cap.get(cv::CAP_PROP_FRAME_WIDTH);
			m_OriginalFrameSize.height = (int)m_Cap.get(cv::CAP_PROP_FRAME_HEIGHT);

			int displayWidth, displayHeight;
			wxDisplaySize(&displayWidth, &displayHeight);
			int wFactor = (m_OriginalFrameSize.width / displayWidth + 1);
			int hFactor = (m_OriginalFrameSize.height / (displayHeight - 50) + 1);
			int maxFactor = wFactor > hFactor ? wFactor : hFactor;
			int initialFrameWidth = m_OriginalFrameSize.width / maxFactor;
			int initialFrameHeight = m_OriginalFrameSize.height / maxFactor;

			m_FishPanel = new FishPanel(this);

			m_PlayBtn = new wxButton(this, FT_ID_PLAY, "PLAY", wxDefaultPosition, wxSize(0, m_ButtonHeight));
			m_PauseBtn = new wxButton(this, FT_ID_PAUSE, "PAUSE", wxDefaultPosition, wxSize(0, m_ButtonHeight));
			m_FastFowardBtn = new wxButton(this, FT_ID_FASTFOWARD, "FF", wxDefaultPosition, wxSize(0, m_ButtonHeight));

			m_ROITxt = new wxTextCtrl(this, wxID_ANY, "Region", wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_AddROIBtn = new wxButton(this, FT_ID_ADDROI, "ADD ROI", wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_DeleteROIBtn = new wxButton(this, FT_ID_DELETEROI, "DELETE ROI", wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_UnactiveROIModeBtn = new wxButton(this, FT_ID_UNACTIVEROIMODE, "DEACTIVATE ROI", wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_CountROIModeBtn = new wxButton(this, FT_ID_COUNTROIMODE, "SET TO COUNT", wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_UncountROIModeBtn = new wxButton(this, FT_ID_UNCOUNTROIMODE, "SET TO UNCOUNT", wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_ROILst = new wxListView(this, FT_ID_ROILIST);
			m_ROILst->InsertColumn(0, "Regions", 0, 150);

			wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
			wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* v2Sizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* h2Sizer = new wxBoxSizer(wxHORIZONTAL);

			hSizer->Add(m_PlayBtn, 1, wxEXPAND);
			hSizer->Add(m_PauseBtn, 1, wxEXPAND);
			hSizer->Add(m_FastFowardBtn, 1, wxEXPAND);

			vSizer->Add(m_FishPanel, 1, wxEXPAND);
			vSizer->Add(hSizer, 0, wxEXPAND);

			v2Sizer->Add(m_ROITxt, 0, wxEXPAND);
			v2Sizer->Add(m_AddROIBtn, 0, wxEXPAND);
			v2Sizer->Add(m_DeleteROIBtn, 0, wxEXPAND);
			v2Sizer->Add(m_UnactiveROIModeBtn, 0, wxEXPAND);
			v2Sizer->Add(m_CountROIModeBtn, 0, wxEXPAND);
			v2Sizer->Add(m_UncountROIModeBtn, 0, wxEXPAND);
			v2Sizer->Add(m_ROILst, 1, wxEXPAND);

			h2Sizer->Add(vSizer, 1, wxEXPAND);
			h2Sizer->Add(v2Sizer, 0, wxEXPAND);

			SetSizer(h2Sizer);
			h2Sizer->Layout();

			SetClientSize(wxSize(m_RightPanelWidth + initialFrameWidth, m_ButtonHeight + initialFrameHeight));
			SetMinClientSize(GetMinClientSize() + wxSize(m_RightPanelWidth + 10, m_ButtonHeight + 10));

			m_VideoAvaliable = true;
			m_FishThread = std::make_unique<std::thread>(&FishFrame::Run, this);

			m_UnactiveROIModeBtn->Disable();
			m_CountROIModeBtn->Disable();
			m_UncountROIModeBtn->Disable();
			m_DeleteROIBtn->Disable();
		}
	}
	FishFrame::~FishFrame()
	{
		// it's better to do any thread cleanup in the OnClose()
		// event handler, rather than in the destructor.
		// This is because the event loop for a top-level window is not
		// active anymore when its destructor is called and if the thread
		// sends events when ending, they won't be processed unless
		// you ended the thread from OnClose.
		// See @ref overview_windowdeletion for more info.
	}

	void FishFrame::Run()
	{
		FT_PROFILE_FUNCTION();
		while (m_VideoAvaliable)
			if (m_VideoPlaying)
			{
				FT_PROFILE_SCOPE("Run: Frame");
				FT_FUNCTION_TIMER_STATUS(microseconds, this);
				{
					FT_PROFILE_SCOPE("Run: m_Cap.read(m_CapFrame);");
					m_Cap.read(m_CapFrame); // Main Bottleneck
				}
				if (m_CapFrame.empty())
				{
					wxLogInfo("End of the video");
					wxGetApp().ActivateRenderLoop(false);
					m_VideoPlaying = false;
					m_VideoAvaliable = false;
					return;
				}

				if (m_VideoFastFoward)
					continue;
				{
					FT_PROFILE_SCOPE("Run: Sleeping");
					m_SleepDuration = m_VideoFrameDuration - (std::chrono::high_resolution_clock::now() - m_FrameTimePoint) - ((m_FrameTimePoint - m_SleepTimePoint) - m_SleepDuration);
					m_SleepTimePoint = std::chrono::high_resolution_clock::now();
					std::this_thread::sleep_for(m_SleepDuration);
					m_FrameTimePoint = std::chrono::high_resolution_clock::now();
				}
			}
			else
				std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	void FishFrame::OnClose(wxCloseEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_Closing = true;
		m_VideoAvaliable = false;

		if (m_FishThread)
			m_FishThread->join();
		if (m_AddROIThread)
			m_AddROIThread->join();

		wxGetApp().ActivateRenderLoop(false);

		Destroy();
		evt.Skip(); // don't stop event, we still want window to close
	}

	void ft::FishFrame::OnSize(wxSizeEvent& evt)
	{
		FT_PROFILE_FUNCTION();

		m_ResizeHandled = false;
		m_FishPanel->PaintNow();

		evt.Skip();
	}

	void FishFrame::OnPlay(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (!m_VideoAvaliable)
		{
			wxLogInfo("There is no video loaded");
			return;
		}
		wxGetApp().ActivateRenderLoop(true);
		m_VideoFastFoward = false;
		m_VideoPlaying = true;
		m_SleepTimePoint = std::chrono::high_resolution_clock::now() - m_VideoFrameDuration;
		m_FrameTimePoint = std::chrono::high_resolution_clock::now();
	}
	void FishFrame::OnPause(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (!m_VideoAvaliable)
		{
			wxLogInfo("There is no video loaded");
			return;
		}
		wxGetApp().ActivateRenderLoop(false);
		m_VideoPlaying = false;
	}
	void FishFrame::OnFastFoward(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (!m_VideoAvaliable)
		{
			wxLogInfo("There is no video loaded");
			return;
		}
		wxGetApp().ActivateRenderLoop(true);
		m_VideoPlaying = true;
		m_VideoFastFoward = true;
	}

	void ft::FishFrame::OnAddROI(wxCommandEvent& evt)
	{
		FT_PROFILE_SCOPE("FishFrame::OnAddROI: getting name");

		wxString userEntry = m_ROITxt->GetValue();
		wxListItem item;
		if (m_ROILst->FindItem(-1, (userEntry)) != wxNOT_FOUND)
		{
			int i = 0;
			while (m_ROILst->FindItem(-1, wxString::Format(userEntry + "%d", ++i)) != wxNOT_FOUND);
			item.SetText(wxString::Format(userEntry + "%d", i));
		}
		else
			item.SetText(userEntry);

		static long i = 0;
		item.SetId(i++);
		item.SetColumn(0);
		long itemID = m_ROILst->InsertItem(item);

		m_ROIs.push_back(std::make_unique<ROI>(m_FishPanel->m_SizeCorrected, item.GetText()));

		m_AddROIThread.release();
		m_AddROIThread = std::make_unique<std::thread>([this, itemID]() {
			FT_PROFILE_SCOPE("FishFrame::OnAddROI: setting rect");
			m_AddROIBtn->Disable();
			SetCursor(wxCursor(wxCURSOR_CROSS));

			while (!(wxGetMouseState().LeftIsDown()) || !(GetClientRect().Contains(ScreenToClient(wxGetMousePosition()))))
			{
				if (m_Closing) // It only works because it immediately returns
					return;
				if (wxGetMouseState().RightIsDown() || wxGetKeyState(wxKeyCode::WXK_ESCAPE))
				{
					m_ROIs.pop_back();
					m_ROILst->DeleteItem(itemID);
					SetCursor(wxCursor(wxCURSOR_DEFAULT));
					m_AddROIBtn->Enable();
					return;
				}
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}
			auto mousePos1 = ScreenToClient(wxGetMousePosition());

			while ((wxGetMouseState().LeftIsDown()))
			{
				FT_PROFILE_SCOPE("FishFrame::OnAddROI: update rect size");
				auto mousePos2 = ScreenToClient(wxGetMousePosition());
				int left = mousePos1.x < mousePos2.x ? mousePos1.x : mousePos2.x;
				int top = mousePos1.y < mousePos2.y ? mousePos1.y : mousePos2.y;
				int width = left == mousePos1.x ? mousePos2.x - mousePos1.x : mousePos1.x - mousePos2.x;
				int height = top == mousePos1.y ? mousePos2.y - mousePos1.y : mousePos1.y - mousePos2.y;
				m_ROIs.back()->SetRect(cv::Rect((left - m_FishPanel->m_FrameLeftCoord) / m_FishPanel->m_MinorDimensionFactor, (top - m_FishPanel->m_FrameTopCoord) / m_FishPanel->m_MinorDimensionFactor, width / m_FishPanel->m_MinorDimensionFactor, height / m_FishPanel->m_MinorDimensionFactor));
				if (!m_VideoPlaying)
					m_FishPanel->PaintNow();
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			SetCursor(wxCursor(wxCURSOR_DEFAULT));
			m_AddROIBtn->Enable();

			}
		);
	}

	void ft::FishFrame::OnDeleteROI(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_ROIs.erase(m_ROIs.begin() + m_ROILst->GetFirstSelected());
		m_ROILst->DeleteItem(m_ROILst->GetFirstSelected());
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();
	}

	void ft::FishFrame::OnSelectedROI(wxListEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_UnactiveROIModeBtn->Enable();
		m_CountROIModeBtn->Enable();
		m_UncountROIModeBtn->Enable();
		m_DeleteROIBtn->Enable();
		m_ROIs[m_ROILst->GetFirstSelected()]->Select();
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();

		evt.Skip();
	}

	void ft::FishFrame::OnDeselectedROI(wxListEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_UnactiveROIModeBtn->Disable();
		m_CountROIModeBtn->Disable();
		m_UncountROIModeBtn->Disable();
		m_DeleteROIBtn->Disable();
		for (auto&& ROI : m_ROIs)
			ROI->Unselect();
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();

		evt.Skip();
	}

	void ft::FishFrame::OnUnactiveROIMode(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_ROILst->SetItemBackgroundColour(m_ROILst->GetFirstSelected(), m_ROILst->GetBackgroundColour());
		m_ROIs[m_ROILst->GetFirstSelected()]->SetMode(ROIMode::UNACTIVE);
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();
	}

	void ft::FishFrame::OnCountROIMode(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_ROILst->SetItemBackgroundColour(m_ROILst->GetFirstSelected(), wxColor(0, 255, 50));
		m_ROIs[m_ROILst->GetFirstSelected()]->SetMode(ROIMode::COUNTING);
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();
	}

	void ft::FishFrame::OnUncountROIMode(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_ROILst->SetItemBackgroundColour(m_ROILst->GetFirstSelected(), wxColor(255, 50, 0));
		m_ROIs[m_ROILst->GetFirstSelected()]->SetMode(ROIMode::UNCOUNTING);
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();
	}

} // namespace ft
