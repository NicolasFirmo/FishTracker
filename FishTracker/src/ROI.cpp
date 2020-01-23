#include "ROI.h"

namespace ft {

	ROI::ROI(cv::Mat& parentMat, const wxString& name) :
		m_ParentMat(parentMat), m_Name(name), m_Rect(cv::Rect(0, 0, 0, 0))
	{
	}

	void ROI::Render(double scale)
	{
		cv::rectangle(m_ParentMat, cv::Rect(m_Rect.x * scale, m_Rect.y * scale, m_Rect.width * scale, m_Rect.height * scale), m_Color, m_IsSelected ? 2 : 1);
	}

}