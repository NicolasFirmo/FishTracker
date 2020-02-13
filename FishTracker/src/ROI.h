#pragma once
#include "Core.h"

namespace ft {

	enum ROIMode
	{
		UNACTIVE = 0, COUNTING, UNCOUNTING
	};

	class ROI
	{
	public:
		ROI(cv::Mat& drawingMat, const wxString& name);

		inline const wxString& GetName() const { return m_Name; }
		inline void SetName(const std::string& name) { m_Name = name; }

		inline ROIMode GetMode() const { return m_Mode; }
		inline void SetMode(ROIMode mode) {
			m_Mode = mode; switch (m_Mode)
			{
			case ft::UNACTIVE:
				m_Color = cv::Scalar_<uchar>(200, 200, 200);
				break;
			case ft::COUNTING:
				m_Color = cv::Scalar_<uchar>(0, 255, 50);
				break;
			case ft::UNCOUNTING:
				m_Color = cv::Scalar_<uchar>(255, 50, 0);
				break;
			default:
				FT_ASSERT(false, "Unknow ROIMode");
				break;
			}
		}

		inline cv::Rect GetRect() const { return m_Rect; }
		void SetRect(cv::Rect rect, cv::Size frameSize) {
			if (rect.x < 0) { rect.width += rect.x; rect.x = 0; }
			if (rect.y < 0) { rect.height += rect.y; rect.y = 0; }
			if (rect.x + rect.width > frameSize.width) rect.width = frameSize.width - rect.x;
			if (rect.y + rect.height > frameSize.height) rect.height = frameSize.height - rect.y;

			m_Rect = rect;
		}

		inline void Select() { m_IsSelected = true; }
		inline void Unselect() { m_IsSelected = false; }

		void Render(double scale);

		ft::ROI& operator =(const ft::ROI& ROI) { return const_cast<ft::ROI&>(ROI); }
	private:
		bool m_IsSelected = false;
		cv::Mat& m_DrawingMat;
		wxString m_Name;
		cv::Rect m_Rect;
		ROIMode m_Mode = ROIMode::UNACTIVE;
		cv::Scalar_<uchar> m_Color = cv::Scalar_<uchar>(200, 200, 200);
	};

} // namespace ft
