#include "Tracking.h"
#include "Core.h"
#include "Instrumentation/Profile.h"

namespace ft {

	Target::Target(cv::Mat& inputMat, cv::Mat& drawingMat, uint64_t intensitySumThreshold, uchar movementDiferenceThreshold, const cv::Vec3b& lowColor, const cv::Vec3b& highColor, const cv::Size& scanningAreaMinSize) :
		m_InputMat(inputMat), m_DrawingMat(drawingMat),
		m_CurrentPosition(inputMat.cols / 2, inputMat.rows / 2), m_ScanningAreaRect(m_CurrentPosition - cv::Point(scanningAreaMinSize.width / 2, scanningAreaMinSize.height / 2), scanningAreaMinSize), m_ScanningAreaMinSize(scanningAreaMinSize),
		m_IntensitySumThreshold(intensitySumThreshold), m_MovementDiferenceThreshold(movementDiferenceThreshold), m_HighColor(highColor), m_LowColor(lowColor),
		m_CrossColor(255, 0, 0), m_ScanningAreaColor(0, 255, 0)
	{
		FT_PROFILE_FUNCTION();
		FT_ASSERT(m_ScanningAreaRect.tl().x > 0 && m_ScanningAreaRect.tl().y > 0 && m_ScanningAreaRect.br().x <= inputMat.cols && m_ScanningAreaRect.br().y <= inputMat.rows, "Scanning area out of InputMat bounds!");
		m_InputMat.copyTo(m_BackGroundMat);

		// Debug ------------
		m_WhatIsFish = cv::Mat::zeros(inputMat.rows, inputMat.cols, CV_8UC1);
		// ------------------
	}

	void Target::Detect()
	{
		FT_PROFILE_FUNCTION();

		cv::absdiff(m_InputMat, m_BackGroundMat, m_MovementMat);

		cv::Point_<uint64_t> centerOfIntensity(0, 0);
		uint64_t intensitySum = 0;

		cv::Vec3b* pInput;
		cv::Vec3b* pMovement;
		// Debug --------
		uchar* pWIF;
		// --------------
		for (uint64_t i = m_ScanningAreaRect.tl().y; i < m_ScanningAreaRect.br().y; i++)
		{
			pInput = m_InputMat.ptr<cv::Vec3b>(i);
			pMovement = m_MovementMat.ptr<cv::Vec3b>(i);
			pWIF = m_WhatIsFish.ptr<uchar>(i);
			for (uint64_t j = m_ScanningAreaRect.tl().x; j < m_ScanningAreaRect.br().x; j++)
			{

				uint64_t movementDiferenceIntensity = pMovement[j][0] + pMovement[j][1] + pMovement[j][2];
				if (!(pInput[j][0] > m_LowColor[0] &&
					pInput[j][1] > m_LowColor[1] &&
					pInput[j][2] > m_LowColor[2] &&

					pInput[j][0] < m_HighColor[0] &&
					pInput[j][1] < m_HighColor[1] &&
					pInput[j][2] < m_HighColor[2] &&

					movementDiferenceIntensity > m_MovementDiferenceThreshold)
					)
				{
					// Debug --------
					pWIF[j] = 0;
					// --------------
					continue;
				}
				// Debug --------
				pWIF[j] = UCHAR_MAX;
				// --------------

				intensitySum += movementDiferenceIntensity;
				centerOfIntensity += cv::Point_<uint64_t>(j * movementDiferenceIntensity, i * movementDiferenceIntensity);
			}
		}
		if (intensitySum < m_IntensitySumThreshold)
		{
			m_TargetFound = false;
		}
		else
		{
			m_TargetFound = true;
			m_CurrentPosition = cv::Point((int)(centerOfIntensity.x / intensitySum), (int)(centerOfIntensity.y / intensitySum));
		}

		UpdateScanningAreaRect();
	}

	void Target::UpdateBackGround()
	{
		FT_PROFILE_FUNCTION();

		// TEMPORARY IMPLEMENTATION
		cv::Vec3b* pInput;
		cv::Vec3b* pBackground;
		uchar* pWIF;
		for (int i = 0; i < m_BackGroundMat.rows; i++)
		{
			pInput = m_InputMat.ptr<cv::Vec3b>(i);
			pBackground = m_BackGroundMat.ptr<cv::Vec3b>(i);
			pWIF = m_WhatIsFish.ptr<uchar>(i);

			for (int j = 0; j < m_BackGroundMat.cols; j++)
				if (!pWIF[j])
					pBackground[j] = pInput[j];
		}
	}

	void Target::DrawPositionCross(double scale)
	{
		FT_PROFILE_FUNCTION();
		FT_ASSERT(!m_DrawingMat.empty(), "m_DrawingMat is empty!");
		FT_ASSERT(m_DrawingMat.type() == CV_8UC3, "Target::DrawPositionCross(double scale) only accepts CV_8UC3 mats");

		int nRows = m_DrawingMat.rows;
		int nCols = m_DrawingMat.cols;
		int crossCenterX = (int)(m_CurrentPosition.x * scale);
		int crossCenterY = (int)(m_CurrentPosition.y * scale);
		int crossTop = (crossCenterY - 10) < 0 ? 0 : (crossCenterY - 10);
		int crossLeft = (crossCenterX - 10) < 0 ? 0 : (crossCenterX - 10);
		int crossBottom = (crossCenterY + 10) > m_DrawingMat.rows ? m_DrawingMat.rows : (crossCenterY + 10);
		int crossRight = (crossCenterX + 10) > m_DrawingMat.cols ? m_DrawingMat.cols : (crossCenterX + 10);

		cv::Vec3b* p;
		for (int i = crossTop; i < crossBottom; i++)
		{
			p = m_DrawingMat.ptr<cv::Vec3b>(i);
			p[crossCenterX] = m_CrossColor;
		}
		p = m_DrawingMat.ptr<cv::Vec3b>(crossCenterY);
		for (int i = crossLeft; i < crossRight; i++)
			p[i] = m_CrossColor;
	}

	void Target::DrawScannigArea(double scale)
	{
		FT_PROFILE_FUNCTION();
		FT_ASSERT(!m_DrawingMat.empty(), "m_DrawingMat is empty!");
		FT_ASSERT(m_DrawingMat.type() == CV_8UC3, "Target::DrawPositionCross(double scale) only accepts CV_8UC3 mats");

		cv::rectangle(m_DrawingMat, cv::Rect(m_ScanningAreaRect.x * scale, m_ScanningAreaRect.y * scale, m_ScanningAreaRect.width * scale, m_ScanningAreaRect.height * scale), m_ScanningAreaColor);
	}

	void Target::UpdateScanningAreaRect()
	{
		FT_PROFILE_FUNCTION();

		if (m_TargetFound)
		{
			m_ScanningAreaRect.width = m_ScanningAreaMinSize.width;
			m_ScanningAreaRect.height = m_ScanningAreaMinSize.height;
		}
		else
		{
			m_ScanningAreaRect.width += 30;
			m_ScanningAreaRect.height += 30;
			if (m_ScanningAreaRect.width >= m_InputMat.cols) m_ScanningAreaRect.width = m_InputMat.cols - 1;
			if (m_ScanningAreaRect.height >= m_InputMat.rows) m_ScanningAreaRect.height = m_InputMat.rows - 1;
		}

		m_ScanningAreaRect.x = m_CurrentPosition.x - (m_ScanningAreaRect.width / 2);
		m_ScanningAreaRect.y = m_CurrentPosition.y - (m_ScanningAreaRect.height / 2);

		if (m_ScanningAreaRect.tl().x < 0)m_ScanningAreaRect.x = 0;
		else if (m_ScanningAreaRect.br().x >= m_InputMat.cols)m_ScanningAreaRect.x = m_InputMat.cols - m_ScanningAreaRect.width - 1;
		if (m_ScanningAreaRect.tl().y < 0)m_ScanningAreaRect.y = 0;
		else if (m_ScanningAreaRect.br().y >= m_InputMat.rows)m_ScanningAreaRect.y = m_InputMat.rows - m_ScanningAreaRect.height - 1;

		FT_ASSERT(m_ScanningAreaRect.tl().x >= 0 && m_ScanningAreaRect.tl().y >= 0 && m_ScanningAreaRect.br().x < m_InputMat.cols && m_ScanningAreaRect.br().y < m_InputMat.rows, "Scanning area out of m_InputMat bounds!");
	}

} // namespace ft