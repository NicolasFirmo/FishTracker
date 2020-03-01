#pragma once
#include "ROI.h"
#include "Tracking.h"
#include "Scale.h"

namespace ft {

	enum class ROIState
	{
		NONE = 0,
		TARGET_NOT_PRESENT,
		TARGET_PRESENT
	};

	struct Range
	{
		cv::Point targetPosition;
		std::unordered_map<wxString, ROIState> ROIsTargetPresence;
	};

	class RawData
	{
	public:
		RawData(const std::vector<std::unique_ptr<ROI>>& ROIs,
			const std::unique_ptr<Target>& fish);

		void UpdateRow(double timeInMilliseconds);

		inline const std::map<double, Range>& GetRows() const { return m_Rows; }

		void OutPut(std::ostream& os) const;

	private:
		std::map<double, Range> m_Rows;

		const std::vector<std::unique_ptr<ROI>>& m_ROIs;
		const std::unique_ptr<Target>& m_Fish;
	};

} // namespace ft
