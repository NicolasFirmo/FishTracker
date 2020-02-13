#include "ROI.h"
#include "Instrumentation/Profile.h"

namespace ft {

	ROI::ROI(cv::Mat& drawingMat, const wxString& name) :
		m_DrawingMat(drawingMat), m_Name(name), m_Rect(cv::Rect(0, 0, 0, 0))
	{
	}

	void ROI::Render(double scale)
	{
		FT_PROFILE_FUNCTION();
		cv::rectangle(m_DrawingMat, cv::Rect(m_Rect.x * scale, m_Rect.y * scale, m_Rect.width * scale, m_Rect.height * scale), m_Color, m_IsSelected ? 2 : 1);
	}

}