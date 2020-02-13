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
		cv::Rect scaledRect = cv::Rect(m_Rect.x * scale, m_Rect.y * scale, m_Rect.width * scale, m_Rect.height * scale);
		FT_ASSERT(scaledRect.tl().x >= 0 && scaledRect.tl().y >= 0 && scaledRect.br().x < m_DrawingMat.cols && scaledRect.br().y < m_DrawingMat.rows, "Rect out of m_DrawingMat bounds!");
		cv::rectangle(m_DrawingMat, scaledRect, m_Color, m_IsSelected ? 2 : 1);
	}

}