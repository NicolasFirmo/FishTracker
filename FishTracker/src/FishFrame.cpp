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

EVT_SLIDER(FT_ID_VIDEO_POSITION, ft::FishFrame::OnVideoPositionChange)
EVT_SLIDER(FT_ID_SUM_THRESHOLD, ft::FishFrame::OnSumThresholdChange)
EVT_SLIDER(FT_ID_MOVEMENT_THRESHOLD, ft::FishFrame::OnMovementThresholdChange)

EVT_BUTTON(FT_ID_SET_SCALE, ft::FishFrame::OnSetScale)

wxEND_EVENT_TABLE()

namespace ft
{

	const int FishFrame::m_RightPanelWidth = 150;
	const int FishFrame::m_ButtonHeight = 30;

	FishFrame::FishFrame(MainFrame* parent, const std::string& videoPath) : wxFrame(nullptr, wxID_ANY, _("Inspector"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
		m_Parent(parent), m_Cap(videoPath),
		m_SleepDuration(std::chrono::microseconds(0)),
		m_FrameTimePoint(std::chrono::high_resolution_clock::now()), m_SleepTimePoint(std::chrono::high_resolution_clock::now())
	{
		FT_PROFILE_FUNCTION();
		FT_ASSERT(m_Cap.get(cv::CAP_PROP_FRAME_WIDTH) >= FT_MIN_FRAME_WIDTH && m_Cap.get(cv::CAP_PROP_FRAME_HEIGHT) >= FT_MIN_FRAME_HEIGHT, "Video Frame is too small!");

		if (m_Cap.isOpened())
		{
			CreateStatusBar();

			m_Cap.read(m_CapFrame);
			m_CapFrame.copyTo(m_ToRenderFrame);

			m_VideoFrameDuration = std::chrono::nanoseconds((long long)(1000000000.0 / m_Cap.get(cv::CAP_PROP_FPS)));
			if (m_VideoFrameDuration < std::chrono::milliseconds(5))
				m_VideoFrameDuration = std::chrono::nanoseconds((long long)(1000000000.0 / 30));

			m_OriginalFrameSize.width = (int)m_Cap.get(cv::CAP_PROP_FRAME_WIDTH);
			m_OriginalFrameSize.height = (int)m_Cap.get(cv::CAP_PROP_FRAME_HEIGHT);
			m_FrameCount = (int)m_Cap.get(cv::CAP_PROP_FRAME_COUNT);

			int displayWidth, displayHeight;
			wxDisplaySize(&displayWidth, &displayHeight);
			int wFactor = (m_OriginalFrameSize.width / displayWidth + 1);
			int hFactor = (m_OriginalFrameSize.height / (displayHeight - 50) + 1);
			int maxFactor = wFactor > hFactor ? wFactor : hFactor;
			int initialFrameWidth = m_OriginalFrameSize.width / maxFactor;
			int initialFrameHeight = m_OriginalFrameSize.height / maxFactor;

			m_FishPanel = new FishPanel(this);

			m_PlayBtn = new wxButton(this, FT_ID_PLAY, _("PLAY"), wxDefaultPosition, wxSize(0, m_ButtonHeight));
			m_PauseBtn = new wxButton(this, FT_ID_PAUSE, _("PAUSE"), wxDefaultPosition, wxSize(0, m_ButtonHeight));
			m_FastFowardBtn = new wxButton(this, FT_ID_FASTFOWARD, _("FF"), wxDefaultPosition, wxSize(0, m_ButtonHeight));

			m_ROITxt = new wxTextCtrl(this, wxID_ANY, _("Region"), wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_AddROIBtn = new wxButton(this, FT_ID_ADDROI, _("ADD ROI"), wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_DeleteROIBtn = new wxButton(this, FT_ID_DELETEROI, _("DELETE ROI"), wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_UnactiveROIModeBtn = new wxButton(this, FT_ID_UNACTIVEROIMODE, _("DEACTIVATE ROI"), wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_CountROIModeBtn = new wxButton(this, FT_ID_COUNTROIMODE, _("SET TO COUNT"), wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_UncountROIModeBtn = new wxButton(this, FT_ID_UNCOUNTROIMODE, _("SET TO UNCOUNT"), wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));
			m_ROILst = new wxListView(this, FT_ID_ROILIST);
			m_ROILst->InsertColumn(0, _("Regions"), 0, 150);

			m_SliderVideoPosition = new wxSlider(this, FT_ID_VIDEO_POSITION, 0, 0, m_FrameCount);
			m_SliderSumThreshold = new wxSlider(this, FT_ID_SUM_THRESHOLD, std::sqrt(FT_SUM_THRESHOLD_MAX) / 2, 1, std::sqrt(FT_SUM_THRESHOLD_MAX));
			m_SliderMovementThreshold = new wxSlider(this, FT_ID_MOVEMENT_THRESHOLD, UCHAR_MAX / 2, 1, UCHAR_MAX);

			m_SetScaleBtn = new wxButton(this, FT_ID_SET_SCALE, _("SET SCALE"), wxDefaultPosition, wxSize(m_RightPanelWidth, m_ButtonHeight));

			wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
			wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* v2Sizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* h2Sizer = new wxBoxSizer(wxHORIZONTAL);

			hSizer->Add(m_PlayBtn, 1, wxEXPAND);
			hSizer->Add(m_PauseBtn, 1, wxEXPAND);
			hSizer->Add(m_FastFowardBtn, 1, wxEXPAND);

			vSizer->Add(m_FishPanel, 1, wxEXPAND);
			vSizer->Add(hSizer, 0, wxEXPAND);
			vSizer->Add(m_SliderVideoPosition, 0, wxEXPAND);

			v2Sizer->Add(m_ROITxt, 0, wxEXPAND);
			v2Sizer->Add(m_AddROIBtn, 0, wxEXPAND);
			v2Sizer->Add(m_DeleteROIBtn, 0, wxEXPAND);
			v2Sizer->Add(m_UnactiveROIModeBtn, 0, wxEXPAND);
			v2Sizer->Add(m_CountROIModeBtn, 0, wxEXPAND);
			v2Sizer->Add(m_UncountROIModeBtn, 0, wxEXPAND);
			v2Sizer->Add(m_ROILst, 1, wxEXPAND);
			v2Sizer->Add(m_SliderSumThreshold, 0, wxEXPAND);
			v2Sizer->Add(m_SliderMovementThreshold, 0, wxEXPAND);

			v2Sizer->Add(m_SetScaleBtn, 0, wxEXPAND);

			h2Sizer->Add(vSizer, 1, wxEXPAND);
			h2Sizer->Add(v2Sizer, 0, wxEXPAND);

			m_Fish = std::make_unique<Target>(m_CapFrame, m_FishPanel->m_SizeCorrected, m_SliderSumThreshold->GetValue(), m_SliderMovementThreshold->GetValue(),
				cv::Vec3b(45, 49, 46), cv::Vec3b(123, 127, 125), cv::Size(FT_MIN_FRAME_WIDTH, FT_MIN_FRAME_HEIGHT));

			// Maybe Create only when user starts inspection?
			m_RawData = std::make_unique<RawData>(m_ROIs, m_Fish);

			m_BackgroundUpdateRect = m_Fish->GetScanningAreaRect();

			m_VideoAvaliable = true;
			m_FishThread = std::make_unique<std::thread>(&FishFrame::Run, this);

			SetSizer(h2Sizer);
			h2Sizer->Layout();

			SetClientSize(wxSize(m_RightPanelWidth + initialFrameWidth, m_ButtonHeight + initialFrameHeight));
			SetMinClientSize(GetMinClientSize() + wxSize(m_RightPanelWidth + 10, m_ButtonHeight + 10));

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

	void FishFrame::Update()
	{
		if (!m_Closing)
			m_FishPanel->PaintNow();

		m_SliderVideoPosition->SetValue(m_CurrentFramePosition);

		if (!m_EndOfTheVideoHandled)
		{
			m_EndOfTheVideoHandled = true;
			wxLogInfo(_("End of the video"));
		}

		if (IsActive())
		{
			if (!m_FinishedAddingROIHandled)
			{
				m_FinishedAddingROIHandled = true;
				SetCursor(wxCursor(wxCURSOR_DEFAULT));
				m_AddROIBtn->Enable();
			}
			else if (!m_FinishedSettingScaleHandled)
			{
				m_FinishedSettingScaleHandled = true;
				SetCursor(wxCursor(wxCURSOR_DEFAULT));
				m_SetScaleBtn->Enable();

				if (!m_SetLineLenghtHandled)
				{
					m_SetScaleLenFrame = new SetScaleLenFrame(this);
					m_SetScaleLenFrame->Show();
				}
			}
			wxLogStatus(this, _("Frame Took: %lldns"), m_CurrentFrameDuration);
		}
	}

	void FishFrame::Run()
	{
		FT_PROFILE_FUNCTION();
		while (m_VideoAvaliable)
			if (m_VideoPlaying)
			{
				FT_PROFILE_SCOPE("Run: Frame");
				ScopeTimerReference<int64_t*> timer(&m_CurrentFrameDuration);
				//FT_FUNCTION_TIMER_STATUS(microseconds, this); // Only for dev
				m_CapMutex.lock();
				double CurrentVideoTime = m_Cap.get(cv::CAP_PROP_POS_MSEC);
				m_CurrentFramePosition = m_Cap.get(cv::CAP_PROP_POS_FRAMES);
				{
					FT_PROFILE_SCOPE("Run: m_Cap.read(m_CapFrame);");
					m_Cap.read(m_CapFrame); // Main Bottleneck
				}
				m_CapMutex.unlock();
				if (m_CapFrame.empty())
				{
					m_EndOfTheVideoHandled = false;
					m_VideoPlaying = false;
					m_VideoEnded = true;
					continue;
				}

				if (!(m_BackgroundUpdateRect.contains(m_Fish->GetCurrentPosition())))
				{
					FT_PROFILE_SCOPE("Run: Update the background");
					m_Fish->UpdateBackGround();
					m_BackgroundUpdateRect = m_Fish->GetScanningAreaRect();
				}

				m_Fish->Detect();
				m_RawData->UpdateRow(CurrentVideoTime);

				SetRenderFrame();

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
				std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}

	void FishFrame::OnClose(wxCloseEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_Closing = true;
		if (m_VideoEnded)
		{
			wxMessageDialog closeDialog(this, _("There's still content to be analized!\nExit anyway?"), _("Exiting Inspection"), wxYES_NO);
			if (closeDialog.ShowModal() != wxID_YES)
			{
				m_Closing = false;
				return;
			}
		}
		m_VideoAvaliable = false;

		m_Parent->m_FishFramesMutex.lock();

		m_Parent->m_FishFrames.erase(m_Parent->m_FishFrames.begin() + m_Index);
		for (size_t i = m_Index; i < m_Parent->m_FishFrames.size(); i++)
			m_Parent->m_FishFrames[i]->SetIndex(i);

		m_Parent->m_FishFramesMutex.unlock();

		if (m_FishThread)
			m_FishThread->join();
		if (m_AddROIThread)
			m_AddROIThread->join();
		if (m_SetScaleThread)
			m_SetScaleThread->join();

		// TEMPORARY --------------------------------
		std::ofstream ofs("test.tsv", std::ios::app);
		m_RawData->OutPut(ofs);
		ofs.flush();
		ofs.close();
		// ------------------------------------------

		Destroy();
		evt.Skip(); // don't stop event, we still want window to close
	}

	void FishFrame::OnSize(wxSizeEvent& evt)
	{
		FT_PROFILE_FUNCTION();

		m_ResizeHandled = false;
		m_FishPanel->PaintNow();

		evt.Skip();
	}

	void FishFrame::OnPlay(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (m_VideoEnded)
		{
			wxLogInfo(_("Video reached the end!"));
			return;
		}
		m_VideoFastFoward = false;
		m_VideoPlaying = true;
		m_SleepTimePoint = std::chrono::high_resolution_clock::now() - m_VideoFrameDuration;
		m_FrameTimePoint = std::chrono::high_resolution_clock::now();
	}
	void FishFrame::OnPause(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (m_VideoEnded)
		{
			wxLogInfo(_("Video reached the end!"));
			return;
		}
		m_VideoPlaying = false;
	}
	void FishFrame::OnFastFoward(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (m_VideoEnded)
		{
			wxLogInfo(_("Video reached the end!"));
			return;
		}
		m_VideoPlaying = true;
		m_VideoFastFoward = true;
	}

	void FishFrame::OnAddROI(wxCommandEvent& evt)
	{
		FT_PROFILE_SCOPE("FishFrame::OnAddROI: getting name");

		m_AddROIBtn->Disable();
		SetCursor(wxCursor(wxCURSOR_CROSS));

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

		item.SetId(m_ROIs.size());
		item.SetColumn(0);
		long itemID = m_ROILst->InsertItem(item);

		m_ROIs.push_back(std::make_unique<ROI>(m_FishPanel->m_SizeCorrected, item.GetText()));

		m_AddROIThread.release();
		m_AddROIThread = std::make_unique<std::thread>([this, itemID]() {
			FT_PROFILE_SCOPE("FishFrame::OnAddROI: setting rect");
			while (!(wxGetMouseState().LeftIsDown()) || !(GetClientRect().Contains(ScreenToClient(wxGetMousePosition()))))
			{
				if (m_Closing) // It only works because it immediately returns
					return;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			auto mousePos1 = ScreenToClient(wxGetMousePosition());

			while ((wxGetMouseState().LeftIsDown()))
			{
				FT_PROFILE_SCOPE("FishFrame::OnAddROI: update rect size");
				if (wxGetMouseState().RightIsDown() || wxGetKeyState(wxKeyCode::WXK_ESCAPE))
				{
					m_ROIs.pop_back();
					m_ROILst->DeleteItem(itemID);
					m_FinishedAddingROIHandled = false;
					return;
				}
				auto mousePos2 = ScreenToClient(wxGetMousePosition());
				int left = mousePos1.x < mousePos2.x ? mousePos1.x : mousePos2.x;
				int top = mousePos1.y < mousePos2.y ? mousePos1.y : mousePos2.y;
				int width = left == mousePos1.x ? mousePos2.x - mousePos1.x : mousePos1.x - mousePos2.x;
				int height = top == mousePos1.y ? mousePos2.y - mousePos1.y : mousePos1.y - mousePos2.y;
				m_ROIs.back()->SetRect(cv::Rect((left - m_FishPanel->m_FrameLeftCoord) / m_FishPanel->m_MinorDimensionFactor,
					(top - m_FishPanel->m_FrameTopCoord) / m_FishPanel->m_MinorDimensionFactor,
					width / m_FishPanel->m_MinorDimensionFactor,
					height / m_FishPanel->m_MinorDimensionFactor),
					m_OriginalFrameSize);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			m_FinishedAddingROIHandled = false;

			}
		);
	}

	void FishFrame::OnDeleteROI(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		long item = m_ROILst->GetFirstSelected();
		m_ROIs.erase(m_ROIs.begin() + item);
		m_ROILst->DeleteItem(item);
		if (item < m_ROILst->GetItemCount())
		{
			m_ROILst->Select(item);
		}
		else
		{
			m_UnactiveROIModeBtn->Disable();
			m_CountROIModeBtn->Disable();
			m_UncountROIModeBtn->Disable();
			m_DeleteROIBtn->Disable();
		}

		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();
	}

	void FishFrame::OnSelectedROI(wxListEvent& evt)
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

	void FishFrame::OnDeselectedROI(wxListEvent& evt)
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

	void FishFrame::OnUnactiveROIMode(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_ROILst->SetItemBackgroundColour(m_ROILst->GetFirstSelected(), m_ROILst->GetBackgroundColour());
		m_ROIs[m_ROILst->GetFirstSelected()]->SetMode(ROIMode::UNACTIVE);
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();
	}

	void FishFrame::OnCountROIMode(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_ROILst->SetItemBackgroundColour(m_ROILst->GetFirstSelected(), wxColor(0, 255, 50));
		m_ROIs[m_ROILst->GetFirstSelected()]->SetMode(ROIMode::COUNTING);
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();
	}

	void FishFrame::OnUncountROIMode(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_ROILst->SetItemBackgroundColour(m_ROILst->GetFirstSelected(), wxColor(255, 50, 0));
		m_ROIs[m_ROILst->GetFirstSelected()]->SetMode(ROIMode::UNCOUNTING);
		if (!m_VideoPlaying)
			m_FishPanel->PaintNow();
	}

	void ft::FishFrame::OnVideoPositionChange(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		FT_ASSERT(m_SliderSumThreshold->GetValue() > 0, "Video Position became negative!");

		m_CapMutex.lock();
		m_CurrentFramePosition = m_SliderVideoPosition->GetValue();
		m_Cap.set(cv::CAP_PROP_POS_FRAMES, m_CurrentFramePosition);
		m_Cap.read(m_CapFrame);
		SetRenderFrame();
		m_VideoEnded = false;
		m_CapMutex.unlock();
	}

	void FishFrame::OnSumThresholdChange(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		FT_ASSERT(m_SliderSumThreshold->GetValue() > 0, "SliderSumThreshold became negative!");
		auto value = m_SliderSumThreshold->GetValue();
		m_Fish->SetIntensitySumThreshold(value * value);
	}

	void FishFrame::OnMovementThresholdChange(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		FT_ASSERT(m_SliderMovementThreshold->GetValue() > 0 && m_SliderMovementThreshold->GetValue() <= UCHAR_MAX, "SliderMovementThreshold outof uchar bounds!");
		auto value = m_SliderMovementThreshold->GetValue();
		m_Fish->SetMovementDiferenceThreshold(value);
	}

	void FishFrame::OnSetScale(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_Scale.release();
		m_Scale = std::make_unique<Scale>();

		m_SetScaleBtn->Disable();
		SetCursor(wxCursor(wxCURSOR_CROSS));

		m_SetScaleThread.release();
		m_SetScaleThread = std::make_unique<std::thread>([this]() {
			FT_PROFILE_SCOPE("FishFrame::OnSetScale: setting scale");

			while (!(wxGetMouseState().LeftIsDown()) || !(GetClientRect().Contains(ScreenToClient(wxGetMousePosition()))))
			{
				if (m_Closing) // It only works because it immediately returns
					return;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			auto mousePos1 = ScreenToClient(wxGetMousePosition());
			m_Scale->SetP0({ (float)((mousePos1.x - m_FishPanel->m_FrameLeftCoord) / m_FishPanel->m_MinorDimensionFactor) ,
							 (float)((mousePos1.y - m_FishPanel->m_FrameTopCoord) / m_FishPanel->m_MinorDimensionFactor) });

			while ((wxGetMouseState().LeftIsDown()))
			{
				FT_PROFILE_SCOPE("FishFrame::OnSetScale: update scale");
				if (wxGetMouseState().RightIsDown() || wxGetKeyState(wxKeyCode::WXK_ESCAPE))
				{
					m_Scale.release();
					m_FinishedSettingScaleHandled = false;
					return;
				}
				auto mousePos2 = ScreenToClient(wxGetMousePosition());
				m_Scale->SetPf({ (float)((mousePos2.x - m_FishPanel->m_FrameLeftCoord) / m_FishPanel->m_MinorDimensionFactor) ,
								 (float)((mousePos2.y - m_FishPanel->m_FrameTopCoord) / m_FishPanel->m_MinorDimensionFactor) });
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			m_SetLineLenghtHandled = false;
			m_FinishedSettingScaleHandled = false;
			}
		);
	}

	void ft::FishFrame::SetRenderFrame()
	{
		FT_PROFILE_FUNCTION();
		// Debug ----------
		//m_ToRenderFrame.setTo(0);
		//m_Fish->m_MovementMat.copyTo(m_ToRenderFrame, m_Fish->m_WhatIsFish);
		m_CapFrame.copyTo(m_ToRenderFrame);
		//cv::rectangle(m_ToRenderFrame, m_BackgroundUpdateRect, cv::Vec3b(255, 0, 255));
		// ----------------
	}

} // namespace ft
