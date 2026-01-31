// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddMoves(mint _nMoves) noexcept
	{
		if constexpr (t_Options.m_bStats)
			mp_Stats.m_nElementMoves += _nMoves;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddShiftMoves(mint _nMoves) noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			mp_Stats.m_nElementMoves += _nMoves;
			mp_Stats.m_nShiftMoves += _nMoves;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddRebalanceMoves(mint _nMoves) noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			mp_Stats.m_nElementMoves += _nMoves;
			mp_Stats.m_nRebalanceMoves += _nMoves;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddResizeMoves(mint _nMoves) noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			mp_Stats.m_nElementMoves += _nMoves;
			mp_Stats.m_nResizeMoves += _nMoves;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddRebalanceWindow(mint _nElements, mint _nSegments) noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			mp_Stats.m_nRebalanceWindowElements += _nElements;
			mp_Stats.m_nRebalanceWindowSegments += _nSegments;

			if (_nSegments > mp_Stats.m_nRebalanceWindowMaxSegments)
				mp_Stats.m_nRebalanceWindowMaxSegments = _nSegments;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddRebalance() noexcept
	{
		if constexpr (t_Options.m_bStats)
			++mp_Stats.m_nRebalances;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddResize() noexcept
	{
		if constexpr (t_Options.m_bStats)
			++mp_Stats.m_nResizes;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddEvenRedistribute() noexcept
	{
		if constexpr (t_Options.m_bStats)
			++mp_Stats.m_nEvenRedistributes;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddAdaptiveRedistribute() noexcept
	{
		if constexpr (t_Options.m_bStats)
			++mp_Stats.m_nAdaptiveRedistributes;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddMarkedIntervals(mint _nIntervals, mint _nIntervalElements, mint _nSegments) noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			mp_Stats.m_nMarkedIntervals += _nIntervals;
			mp_Stats.m_nMarkedIntervalElements += _nIntervalElements;
			mp_Stats.m_nMarkedSegments += _nSegments;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddAdaptiveIntervalKinds
		(
			mint _nTwoElementIntervals
			, mint _nWholeSegmentIntervals
			, mint _nBackwardRunLengthTriggered
			, mint _nForwardRunLengthTriggered
			, mint _nTwoElementIntervalFailures
		)
		noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			mp_Stats.m_nAdaptiveTwoElementIntervals += _nTwoElementIntervals;
			mp_Stats.m_nAdaptiveWholeSegmentIntervals += _nWholeSegmentIntervals;
			mp_Stats.m_nAdaptiveBackwardRunLengthTriggered += _nBackwardRunLengthTriggered;
			mp_Stats.m_nAdaptiveForwardRunLengthTriggered += _nForwardRunLengthTriggered;
			mp_Stats.m_nAdaptiveTwoElementIntervalFailures += _nTwoElementIntervalFailures;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddAdaptiveSplitNoIntervals() noexcept
	{
		if constexpr (t_Options.m_bStats)
			++mp_Stats.m_nAdaptiveSplitNoIntervals;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddAdaptiveSplitTooBig() noexcept
	{
		if constexpr (t_Options.m_bStats)
			++mp_Stats.m_nAdaptiveSplitTooBig;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddAdaptiveWindow
		(
			mint _nTimestamps
			, mint _nSegmentsWithTimestamps
			, mint _nSegmentsAnyAbovePercentile
			, mint _nSegmentsHalfAbovePercentile
			, mint _nSegmentsThreeQuarterAbovePercentile
			, mint _nHotSegments
			, bool _bPercentileIsMax
		)
		noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			++mp_Stats.m_nAdaptiveWindows;

			if (_nTimestamps > 0)
			{
				++mp_Stats.m_nAdaptiveWindowsWithTimestamps;
				mp_Stats.m_nAdaptiveTotalTimestamps += _nTimestamps;
			}

			mp_Stats.m_nAdaptiveSegmentsWithTimestamps += _nSegmentsWithTimestamps;
			mp_Stats.m_nAdaptiveSegmentsAnyAbovePercentile += _nSegmentsAnyAbovePercentile;
			mp_Stats.m_nAdaptiveSegmentsHalfAbovePercentile += _nSegmentsHalfAbovePercentile;
			mp_Stats.m_nAdaptiveSegmentsThreeQuarterAbovePercentile += _nSegmentsThreeQuarterAbovePercentile;
			mp_Stats.m_nAdaptiveHotSegments += _nHotSegments;

			if (_bPercentileIsMax)
				++mp_Stats.m_nAdaptivePercentileIsMax;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddDetectorRemap(mint _nSegments, mint _nOverlapChecks, mint _nTransfers) noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			++mp_Stats.m_nDetectorRemapWindows;
			mp_Stats.m_nDetectorRemapSegments += _nSegments;
			mp_Stats.m_nDetectorRemapOverlapChecks += _nOverlapChecks;
			mp_Stats.m_nDetectorRemapTransfers += _nTransfers;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddRebalanceDecision
		(
			bool _bInsertTrigger
			, mint _iFirstViolationLevel
			, mint _iChosenLevel
		)
		noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			if (_bInsertTrigger)
				++mp_Stats.m_nRebalanceInsertTriggers;
			else
				++mp_Stats.m_nRebalanceDeleteTriggers;

			mint iChosenClamped = fg_Min(fg_Max(_iChosenLevel, mint(0)), CStats::mc_nRebalanceLevelHistSize - 1);
			mint iFirstClamped = fg_Min(fg_Max(_iFirstViolationLevel, mint(0)), CStats::mc_nRebalanceLevelHistSize - 1);

			mp_Stats.m_nRebalanceChosenLevelSum += _iChosenLevel;
			mp_Stats.m_nRebalanceFirstViolationLevelSum += _iFirstViolationLevel;
			++mp_Stats.m_RebalanceChosenLevelHist[iChosenClamped];
			++mp_Stats.m_RebalanceFirstViolationLevelHist[iFirstClamped];
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_StatsAddRebalanceAlloc(bool _bHeap) noexcept
	{
		if constexpr (t_Options.m_bStats)
		{
			if (_bHeap)
				++mp_Stats.m_nRebalanceHeapAllocs;
			else
				++mp_Stats.m_nRebalanceStackAllocs;
		}
	}
}
