#include "RawData.h"
#include "Instrumentation/Profile.h"


namespace ft {

	RawData::RawData(const std::vector<std::unique_ptr<ROI>>& ROIs, const std::unique_ptr<Target>& fish) :
		m_ROIs(ROIs), m_Fish(fish)
	{
		FT_PROFILE_FUNCTION();
	}

	void RawData::UpdateRow(double timeInMilliseconds)
	{
		FT_PROFILE_FUNCTION();
		Range range;

		range.targetPosition = m_Fish->GetCurrentPosition();

		for (auto& ROI : m_ROIs)
		{
			if (ROI->GetMode() != ROIMode::COUNTING)
			{
				range.ROIsTargetPresence[ROI->GetName()] = ROIState::NONE;
				continue;
			}

			if (ROI->GetRect().contains(m_Fish->GetCurrentPosition()))
			{
				bool validCount = true;
				for (auto& otherROI : m_ROIs)
				{
					if (otherROI->GetMode() == ROIMode::UNCOUNTING &&
						otherROI->GetRect().contains(m_Fish->GetCurrentPosition()))
					{
						range.ROIsTargetPresence[ROI->GetName()] = ROIState::TARGET_NOT_PRESENT;
						validCount = false;
						break;
					}
				}
				if (validCount)
					range.ROIsTargetPresence[ROI->GetName()] = ROIState::TARGET_PRESENT;
			}
			else
				range.ROIsTargetPresence[ROI->GetName()] = ROIState::TARGET_NOT_PRESENT;
		}

		m_Rows[timeInMilliseconds] = range;
	}

	void RawData::OutPut(std::ostream& os) const
	{ // Try to optimize latter
		FT_PROFILE_FUNCTION();
		os << "Time in ms\tFish position";

		for (auto& ROI : m_ROIs)
			os << '\t' << ROI->GetName();

		for (auto& row : m_Rows)
		{
			os << '\n' << row.first << '\t' << row.second.targetPosition;

			for (auto& ROI : m_ROIs)
			{
				os << '\t';
				int ROIState;
				try
				{
					ROIState = (int)row.second.ROIsTargetPresence.at(ROI->GetName());
				}
				catch (const std::out_of_range & oor)
				{
					os << (int)ROIState::NONE;
					break;
				}
				os << ROIState;
			}
		}
	}

} // namespace ft

