#include "PaintTimer.h"
#include "MainFrame.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

namespace ft {

	PaintTimer::PaintTimer(MainFrame* parent) : wxTimer(), m_MainFrame(parent)
	{}

	void PaintTimer::Start()
	{
		wxTimer::Start(10);
	}

	void PaintTimer::Notify()
	{
		FT_PROFILE_FUNCTION();
		m_MainFrame->m_FishFramesMutex.lock();
		{
			FT_PROFILE_SCOPE("OnIdle: Inside the lock");
			for (auto& fishFrame : m_MainFrame->m_FishFrames)
			{
				FT_ASSERT(fishFrame->m_FishPanel, "FishPanel of this FishFrame is nullptr!");
				if (!(fishFrame->m_Closing))
					fishFrame->m_FishPanel->PaintNow();
			}
		}
		m_MainFrame->m_FishFramesMutex.unlock();
	}

} // namespace ft