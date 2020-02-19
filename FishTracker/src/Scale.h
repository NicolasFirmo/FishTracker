#pragma once

namespace ft {

	class Scale
	{
	public:
		Scale(const cv::Point2f& p0 = { 0.0f,0.0f }, const cv::Point2f& pf = { 0.0f,0.0f }, double lenghtInMillimeters = 0, const cv::Scalar_<uchar>& color = { 0,0,255 });

		double GetPixelsPerMillimeters() const;
		void Render(cv::Mat& drawingMat, double scale);

		inline void SetP0(const cv::Point2f& p0) { m_P0 = p0; }
		inline void SetPf(const cv::Point2f& pf) { m_Pf = pf; }
		inline void SetLenghtInMillimeters(double lenghtInMillimeters) { m_LenghtInMillimeters = lenghtInMillimeters; }
	private:
		cv::Point2f m_P0;
		cv::Point2f m_Pf;
		double m_LenghtInMillimeters;
		cv::Scalar_<uchar> m_Color;
	};

} // namespace ft
