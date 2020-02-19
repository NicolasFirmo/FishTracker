#include "Scale.h"
#include "Core.h"
#include "Instrumentation/Profile.h"

namespace ft {
	Scale::Scale(const cv::Point2f& p0, const cv::Point2f& pf, double lenghtInMillimeters, const cv::Scalar_<uchar>& color) :
		m_P0(p0), m_Pf(pf), m_LenghtInMillimeters(lenghtInMillimeters), m_Color(color)
	{
	}
	double Scale::GetPixelsPerMillimeters() const
	{
		FT_PROFILE_FUNCTION();
		double cx_2 = pow((double)(m_Pf.x - m_P0.x), 2);
		double cy_2 = pow((double)(m_Pf.y - m_P0.y), 2);
		return sqrt((cx_2)+(cy_2)) / m_LenghtInMillimeters;
	}
	void Scale::Render(cv::Mat& drawingMat, double scale)
	{
		FT_PROFILE_FUNCTION();
		cv::Point  scaledP0(m_P0.x * scale, m_P0.y * scale);
		cv::Point  scaledPf(m_Pf.x * scale, m_Pf.y * scale);
		FT_ASSERT(scaledP0.x >= 0 && scaledP0.y >= 0 && scaledP0.x < drawingMat.cols && scaledP0.y < drawingMat.rows, "Point out of drawingMat bounds!");
		FT_ASSERT(scaledPf.x >= 0 && scaledPf.y >= 0 && scaledPf.x < drawingMat.cols && scaledPf.y < drawingMat.rows, "Point out of drawingMat bounds!");
		cv::line(drawingMat, scaledP0, scaledPf, m_Color, 2);
	}
} // namespace ft