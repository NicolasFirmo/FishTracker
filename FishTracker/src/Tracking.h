#pragma once

namespace ft {

	class Target
	{
	public:
		Target(cv::Mat& inputMat, cv::Mat& drawingMat, uint64_t intensitySumThreshold, uchar movementDiferenceThreshold, const cv::Vec3b& lowColor, const cv::Vec3b& highColor, const cv::Size& scanningAreaSize);

		void Detect();
		void UpdateBackGround();

		void DrawPositionCross(double scale);
		void DrawScannigArea(double scale);

		inline void SetIntensitySumThreshold(unsigned long ist) { m_IntensitySumThreshold = ist; }
		inline unsigned long GetIntensitySumThreshold() const { return m_IntensitySumThreshold; }
		inline void SetMovementDiferenceThreshold(uchar mvt) { m_MovementDiferenceThreshold = mvt; }
		inline uchar GetMovementDiferenceThreshold() const { return m_MovementDiferenceThreshold; }

		inline void SetHighColor(const cv::Vec3b& highColor) { m_HighColor = highColor; }
		inline const cv::Vec3b& GetHighColor() const { return m_HighColor; }
		inline void SetLowColor(const cv::Vec3b& lowColor) { m_LowColor = lowColor; }
		inline const cv::Vec3b& GetLowColor() const { return m_LowColor; }

		inline void SetCrossColor(const cv::Vec3b& crossColor) { m_CrossColor = crossColor; }
		inline const cv::Vec3b& GetCrossColor() const { return m_CrossColor; }
		inline void SetScanningAreaColor(const cv::Vec3b& scanningAreaColor) { m_ScanningAreaColor = scanningAreaColor; }
		inline const cv::Vec3b& GetScanningAreaColor() const { return m_ScanningAreaColor; }

		inline void SetCurrentPosition(const cv::Point& currentPosition) { m_CurrentPosition = currentPosition; }
		inline const cv::Point& GetCurrentPosition() const { return m_CurrentPosition; }
		inline void SetScanningAreaRect(const cv::Rect& scanningAreaRect) { m_ScanningAreaRect = scanningAreaRect; }
		inline const cv::Rect& GetScanningAreaRect() const { return m_ScanningAreaRect; }

		inline void SetScanningAreaMinSize(const cv::Size& scanningAreaMinSize) { m_ScanningAreaMinSize = scanningAreaMinSize; }
		inline const cv::Size& GetScanningAreaMinSize() const { return m_ScanningAreaMinSize; }
	private:
		void UpdateScanningAreaRect();
	public:
		cv::Mat& m_InputMat;
		cv::Mat& m_DrawingMat;

		// Debug --------------------
		cv::Mat m_WhatIsFish;
		cv::Mat m_BackGroundMat;
		cv::Mat m_MovementMat;
		// --------------------------

		bool m_TargetFound = false;
	private:

		uint64_t m_IntensitySumThreshold;
		uchar m_MovementDiferenceThreshold;

		cv::Vec3b m_HighColor;
		cv::Vec3b m_LowColor;

		cv::Point m_CurrentPosition;
		cv::Vec3b m_CrossColor;

		cv::Rect m_ScanningAreaRect;
		cv::Size m_ScanningAreaMinSize;

		cv::Vec3b m_ScanningAreaColor;
	};

} // namespace ft