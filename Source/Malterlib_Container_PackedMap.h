// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Core/Core>
#include <Mib/Bit/Bit>
#include <Mib/Storage/Optional>

#include "Malterlib_Container_PackedMap_Options.h"
#include "Malterlib_Container_PackedMap_Iterator.h"

namespace NMib::NContainer
{
	// Result type for insert operations
	template <typename t_CKey, typename t_CValue>
	struct TCPackedMapResult
	{
		mark_nodebug constexpr inline_small bool f_WasCreated() const noexcept ;
		mark_nodebug constexpr inline_small t_CKey const &f_Key() const noexcept;
		mark_nodebug constexpr inline_small t_CValue &f_Value() noexcept;
		mark_nodebug constexpr inline_small t_CValue const &f_Value() const noexcept;
		mark_nodebug constexpr inline_small operator t_CValue &() noexcept;
		mark_nodebug constexpr inline_small t_CValue &f_GetResult() noexcept;
		mark_nodebug constexpr inline_small t_CValue &operator * () noexcept;

		t_CKey const &m_Key;
		t_CValue &m_Value;
		bool m_bInserted;
	};

	// Reference wrapper for key-value pair access
	template <typename t_CKey, typename t_CValue>
	struct TCPackedMapRef
	{
		mark_nodebug constexpr inline_small t_CKey const &f_Key() const noexcept;
		mark_nodebug constexpr inline_small t_CValue &f_Value() noexcept;
		mark_nodebug constexpr inline_small t_CValue const &f_Value() const noexcept;

		// Implicit conversion to value for convenience
		mark_nodebug constexpr inline_small operator t_CValue &() noexcept;
		mark_nodebug constexpr inline_small operator t_CValue const &() const noexcept;

		t_CKey const &m_Key;
		t_CValue &m_Value;
	};

	// Forward declaration
	template
	<
		typename t_CKey
		, typename t_CValue
		, typename t_CCompare = NMib::CSort_Default
		, typename t_CAllocator = NMib::NMemory::CAllocator_Heap
		, CPackedMapOptions t_Options = {}
	>
	struct TCPackedMap;

	// Main TCPackedMap class
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	struct TCPackedMap
	{
		using CKey = t_CKey;
		using CValue = t_CValue;
		using CRef = TCPackedMapRef<t_CKey, t_CValue>;

		static_assert
			(
				noexcept(fg_GetType<t_CCompare const &>()(fg_GetType<t_CKey const &>(), fg_GetType<t_CKey const &>()))
				, "TCPackedMap requires key comparisons to be noexcept"
			)
		;

		template <typename tf_CProbeKey>
		constexpr static void fsp_StaticAssertNothrowCompareProbeToStoredKey() noexcept
		{
			using CProbeKey = NTraits::TCRemoveReferenceAndQualifiers<tf_CProbeKey>;
			static_assert
				(
					noexcept(fg_GetType<t_CCompare const &>()(fg_GetType<CProbeKey const &>(), fg_GetType<t_CKey const &>()))
					, "TCPackedMap requires heterogeneous key comparisons (_Key, stored key) to be noexcept"
				)
			;
		}

		template <typename tf_CProbeKey>
		constexpr static void fsp_StaticAssertNothrowCompareStoredKeyToProbe() noexcept
		{
			using CProbeKey = NTraits::TCRemoveReferenceAndQualifiers<tf_CProbeKey>;
			static_assert
				(
					noexcept(fg_GetType<t_CCompare const &>()(fg_GetType<t_CKey const &>(), fg_GetType<CProbeKey const &>()))
					, "TCPackedMap requires heterogeneous key comparisons (stored key, _Key) to be noexcept"
				)
			;
		}

		using CIterator = TCPackedMapIterator<TCPackedMap, false, false>;
		using CIteratorConst = TCPackedMapIterator<TCPackedMap, true, false>;
		using CIteratorReverse = TCPackedMapIterator<TCPackedMap, false, true>;
		using CIteratorReverseConst = TCPackedMapIterator<TCPackedMap, true, true>;

		using CKeyIterator = TCPackedMapKeyIterator<TCPackedMap, false, false>;
		using CKeyIteratorConst = TCPackedMapKeyIterator<TCPackedMap, true, false>;
		using CKeyIteratorReverse = TCPackedMapKeyIterator<TCPackedMap, false, true>;
		using CKeyIteratorReverseConst = TCPackedMapKeyIterator<TCPackedMap, true, true>;

		using CKeyValueIterator = TCPackedMapKeyValueIterator<TCPackedMap, false, false>;
		using CKeyValueIteratorConst = TCPackedMapKeyValueIterator<TCPackedMap, true, false>;
		using CKeyValueIteratorReverse = TCPackedMapKeyValueIterator<TCPackedMap, false, true>;
		using CKeyValueIteratorReverseConst = TCPackedMapKeyValueIterator<TCPackedMap, true, true>;

		struct CFormatOptions
		{
			bool m_bSingleLine = false;
			bool m_bBrackets = true;
		};

		struct CStats
		{
			constexpr static mint mc_nRebalanceLevelHistSize = 32;

			mint m_nElementMoves = 0;
			mint m_nShiftMoves = 0;
			mint m_nRebalanceMoves = 0;
			mint m_nResizeMoves = 0;
			mint m_nRebalances = 0;
			mint m_nResizes = 0;
			mint m_nRebalanceWindowElements = 0;
			mint m_nRebalanceWindowSegments = 0;
			mint m_nRebalanceWindowMaxSegments = 0;
			mint m_nEvenRedistributes = 0;
			mint m_nAdaptiveRedistributes = 0;
			mint m_nMarkedIntervals = 0;
			mint m_nMarkedIntervalElements = 0;
			mint m_nMarkedSegments = 0;
			mint m_nAdaptiveTwoElementIntervals = 0;
			mint m_nAdaptiveWholeSegmentIntervals = 0;
			mint m_nAdaptiveBackwardRunLengthTriggered = 0;
			mint m_nAdaptiveForwardRunLengthTriggered = 0;
			mint m_nAdaptiveTwoElementIntervalFailures = 0;
			mint m_nAdaptiveSplitNoIntervals = 0;
			mint m_nAdaptiveSplitTooBig = 0;
			mint m_nAdaptiveWindows = 0;
			mint m_nAdaptiveWindowsWithTimestamps = 0;
			mint m_nAdaptiveTotalTimestamps = 0;
			mint m_nAdaptiveSegmentsWithTimestamps = 0;
			mint m_nAdaptiveSegmentsAnyAbovePercentile = 0;
			mint m_nAdaptiveSegmentsHalfAbovePercentile = 0;
			mint m_nAdaptiveSegmentsThreeQuarterAbovePercentile = 0;
			mint m_nAdaptiveHotSegments = 0;
			mint m_nAdaptivePercentileIsMax = 0;
			mint m_nDetectorRemapWindows = 0;
			mint m_nDetectorRemapSegments = 0;
			mint m_nDetectorRemapOverlapChecks = 0;
			mint m_nDetectorRemapTransfers = 0;
			mint m_nRebalanceHeapAllocs = 0;
			mint m_nRebalanceStackAllocs = 0;
			mint m_nRebalanceInsertTriggers = 0;
			mint m_nRebalanceDeleteTriggers = 0;
			mint m_nRebalanceChosenLevelSum = 0;
			mint m_nRebalanceFirstViolationLevelSum = 0;
			mint m_RebalanceChosenLevelHist[mc_nRebalanceLevelHistSize] = {};
			mint m_RebalanceFirstViolationLevelHist[mc_nRebalanceLevelHistSize] = {};
		};

		// Key accessor helpers
		struct CKeys
		{
			constexpr CKeys(TCPackedMap &_Map) noexcept;

			constexpr CKeyIterator f_GetIterator() noexcept;
			constexpr CKeyIteratorReverse f_GetIteratorReverse() noexcept;

			template <typename tf_CKey>
			constexpr CKeyIterator f_GetIterator(tf_CKey &&_Key) noexcept;

		protected:
			TCPackedMap &mp_Map;
		};

		struct CKeysConst
		{
			constexpr CKeysConst(TCPackedMap const &_Map) noexcept;

			constexpr CKeyIteratorConst f_GetIterator() const noexcept;
			constexpr CKeyIteratorReverseConst f_GetIteratorReverse() const noexcept;

			template <typename tf_CKey>
			constexpr CKeyIteratorConst f_GetIterator(tf_CKey &&_Key) const noexcept;

		private:
			TCPackedMap const &mp_Map;
		};

		struct CEntries
		{
			constexpr CEntries(TCPackedMap &_Map) noexcept;

			constexpr CKeyValueIterator f_GetIterator() noexcept;
			constexpr CKeyValueIteratorReverse f_GetIteratorReverse() noexcept;

			template <typename tf_CKey>
			constexpr CKeyValueIterator f_GetIterator(tf_CKey &&_Key) noexcept;

		protected:
			TCPackedMap &mp_Map;
		};

		struct CEntriesConst
		{
			constexpr CEntriesConst(TCPackedMap const &_Map) noexcept;

			constexpr CKeyValueIteratorConst f_GetIterator() const noexcept;
			constexpr CKeyValueIteratorReverseConst f_GetIteratorReverse() const noexcept;

			template <typename tf_CKey>
			constexpr CKeyValueIteratorConst f_GetIterator(tf_CKey &&_Key) const noexcept;

		private:
			TCPackedMap const &mp_Map;
		};

		constexpr TCPackedMap() noexcept;
		constexpr TCPackedMap(TCPackedMap &&_Other) noexcept
			(
				NTraits::cIsNothrowMoveConstructible<t_CAllocator>
				&& NTraits::cIsNothrowMoveConstructible<t_CCompare>
			)
		;
		constexpr TCPackedMap(TCPackedMap const &_Other);
		constexpr ~TCPackedMap();

		template <typename ...tfp_CParams>
		constexpr TCPackedMap(CAllocatorConstructTag const &, tfp_CParams && ...p_Params);

		template <typename ...tfp_CParams>
		constexpr TCPackedMap(CCompareConstructTag const &, tfp_CParams && ...p_Params);

		template <typename ...tfp_CAllocatorParams, typename ...tfp_CCompareParams>
		constexpr TCPackedMap
			(
				CAllocatorConstructTag &&
				, CCompareConstructTag &&
				, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
				, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
			)
		;

		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
		constexpr TCPackedMap(TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other);

		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
		constexpr TCPackedMap(TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> &&_Other);

		constexpr TCPackedMap &operator = (TCPackedMap const &_Other);
		constexpr TCPackedMap &operator = (TCPackedMap &&_Other);

		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
		constexpr TCPackedMap &operator = (TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other);

		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
		constexpr TCPackedMap &operator = (TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> &&_Other);

		constexpr void f_Clear(bool _bKeepBacking = false);
		constexpr mint f_GetLen() const noexcept;
		constexpr mint f_GetCapacity() const noexcept;
		constexpr bool f_IsEmpty() const noexcept;

		// Exception safety for insertion is limited:
		// if key/value construction, element moves, or rebalancing done as part of insertion throws,
		// the only guarantee is that no undefined behavior occurs and no memory leaks occur.
		// After such an exception the map must be considered invalid until it is destroyed or cleared.
		template <typename tf_CKey, typename ...tfp_CParam>
		constexpr TCPackedMapResult<t_CKey, t_CValue> f_Insert(tf_CKey &&_Key, tfp_CParam && ...p_Params);

		// Bulk load (expects sorted unique keys)
		// _fInsert is called with a functor: _fInsert([](auto &&_Key, auto &&_Value) { ... })
		// The caller invokes the functor exactly _Count times with key-value pairs
		// The map must not be accessed from within the populate functor.
		template <typename tf_CPopulate>
		constexpr void f_BulkLoad(mint _Count, tf_CPopulate &&_fPopulate);

		// Bulk insert (expects sorted unique keys, may be non-empty)
		// Same interface as f_BulkLoad: _fPopulate receives an insert functor
		// The map must not be accessed from within the populate functor.
		// Same exception-safety guarantee as f_Insert().
		template <typename tf_CPopulate>
		constexpr void f_BulkInsert(mint _Count, tf_CPopulate &&_fPopulate);

		// Find operations
		template <typename tf_CKey>
		constexpr bool f_Exists(tf_CKey &&_Key) const noexcept;

		template <typename tf_CKey>
		constexpr t_CValue const *f_FindEqual(tf_CKey &&_Key) const noexcept;
		template <typename tf_CKey>
		constexpr t_CValue *f_FindEqual(tf_CKey &&_Key) noexcept;

		template <typename tf_CKey>
		constexpr t_CValue *f_FindSmallestGreaterThanEqual(tf_CKey &&_Key) noexcept;
		template <typename tf_CKey>
		constexpr t_CValue const *f_FindSmallestGreaterThanEqual(tf_CKey &&_Key) const noexcept;

		template <typename tf_CKey>
		constexpr t_CValue *f_FindLargestLessThanEqual(tf_CKey &&_Key) noexcept;
		template <typename tf_CKey>
		constexpr t_CValue const *f_FindLargestLessThanEqual(tf_CKey &&_Key) const noexcept;

		constexpr t_CValue *f_FindSmallest() noexcept;
		constexpr t_CValue const *f_FindSmallest() const noexcept;

		constexpr t_CValue *f_FindLargest() noexcept;
		constexpr t_CValue const *f_FindLargest() const noexcept;

		constexpr t_CKey const *f_FindSmallestKey() const noexcept;
		constexpr t_CKey const *f_FindLargestKey() const noexcept;

		// Remove operations
		// Same exception-safety guarantee as f_Insert().
		template <typename tf_CKey>
		constexpr bool f_Remove(tf_CKey &&_Key);

		// Same exception-safety guarantee as f_Insert().
		constexpr void f_Remove(t_CValue *_pValue);
		// Same exception-safety guarantee as f_Insert().
		constexpr void f_Remove(t_CValue const *_pValue);

		// Iterator access
		constexpr CIterator f_GetIterator() noexcept;
		constexpr CIteratorConst f_GetIterator() const noexcept;
		constexpr CIteratorReverse f_GetIteratorReverse() noexcept;
		constexpr CIteratorReverseConst f_GetIteratorReverse() const noexcept;

		template <typename tf_CKey>
		constexpr CIterator f_GetIterator(tf_CKey &&_Key) noexcept;
		template <typename tf_CKey>
		constexpr CIteratorConst f_GetIterator(tf_CKey &&_Key) const noexcept;

		template <typename tf_CKey>
		constexpr CIterator f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) noexcept;
		template <typename tf_CKey>
		constexpr CIteratorConst f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const noexcept;

		template <typename tf_CKey>
		constexpr CIterator f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) noexcept;
		template <typename tf_CKey>
		constexpr CIteratorConst f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const noexcept;

		constexpr CKeys f_Keys() noexcept;
		constexpr CKeysConst f_Keys() const noexcept;
		constexpr CEntries f_Entries() noexcept;
		constexpr CEntriesConst f_Entries() const noexcept;

		// Stream support
		template <typename tf_CStream>
		constexpr void f_Feed(tf_CStream &_Stream) const;

		template <typename tf_CStream>
		constexpr void f_Consume(tf_CStream &_Stream);

		// Format support
		template <typename tf_COutput>
		constexpr void f_Format(tf_COutput &_Output, CFormatOptions const &_Options = {}) const;

		// Scan-optimized traversal (clusters across segment pairs)
		template <typename tf_FOnEntry>
		constexpr void f_Scan(tf_FOnEntry &&_fOnEntry) const;

		// Debug validation (returns false on invariant violation)
		constexpr bool f_Debug_Validate(NStr::CStr *_pOutError = nullptr) const;

		// Stats access (only valid if mc_bStats is true)
		constexpr CStats const &f_GetStats() const noexcept
			requires(t_Options.m_bStats)
		;
		constexpr void f_ResetStats() noexcept
			requires(t_Options.m_bStats)
		;

		// Same exception-safety guarantee as f_Insert().
		template <typename tf_CKey, typename ...tfp_CParam>
		constexpr t_CValue &operator[] (tf_CKey &&_Key, tfp_CParam && ...p_Params);

		template <typename tf_CKey>
		constexpr t_CValue const &operator[] (tf_CKey &&_Key) const;

		// Same exception-safety guarantee as f_Insert().
		template <typename tf_CKey, typename ...tfp_CParam>
		constexpr TCPackedMapResult<t_CKey, t_CValue> operator () (tf_CKey &&_Key, tfp_CParam && ...p_Params);

		// Comparison operators
		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
		constexpr bool operator == (TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other) const noexcept
			(
				noexcept(fg_GetType<t_CKey const &>() == fg_GetType<tf_CKey const &>())
				&& noexcept(fg_GetType<t_CValue const &>() == fg_GetType<tf_CValue const &>())
			)
		;

		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
		constexpr auto operator <=> (TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other) const noexcept
			(
				noexcept(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
				&& noexcept(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
			)
		;

		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
		constexpr auto f_CompareLexicographical(TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other) const noexcept
			(
				noexcept(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
				&& noexcept(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
			)
		;

		constexpr static pfp64 mc_LeafLowerBound = t_Options.m_LeafLowerBound; // Leaf lower bound (prevents underflow, ensures O(log² n) amortized)
		constexpr static pfp64 mc_RootLowerBound = t_Options.m_RootLowerBound; // Root lower bound
		constexpr static pfp64 mc_LeafUpperBound = t_Options.m_LeafUpperBound; // Leaf upper bound (full segment)
		constexpr static pfp64 mc_RootUpperBound = t_Options.m_RootUpperBound; // Root upper bound
		constexpr static pfp64 mc_ResizeThreshold = t_Options.m_ResizeThreshold; // Shrink threshold

	protected:
		template <typename, typename, typename, typename, CPackedMapOptions>
		friend struct TCPackedMap;

		template <typename, bool, bool>
		friend struct TCPackedMapIterator;

		template <typename, bool, bool>
		friend struct TCPackedMapKeyIterator;

		template <typename, bool, bool>
		friend struct TCPackedMapKeyValueIterator;

		static_assert(t_Options.m_MinSegments > 0, "PackedMap mc_MinSegments must be greater than 0");
		static_assert(t_Options.m_DetectorQueueSize > 0, "PackedMap mc_DetectorQueueSize must be greater than 0");
		static_assert(t_Options.m_MaxCapacityBits < sizeof(mint) * 8, "PackedMap mc_MaxCapacityBits must be smaller than the number of bits in mint");

		constexpr static uint16 mcp_MaxCalibratorLevels = t_Options.m_MaxCapacityBits; // Supports up to 2^mc_MaxCapacityBits segments

		struct CReciprocalTable
		{
			constexpr CReciprocalTable() noexcept;

			mint m_Values[mcp_MaxCalibratorLevels] = {};
		};

		// Marked interval in element positions (global positions within the window)
		// Per paper: either a 2-element interval (kbwd/kfwd hotspot) or whole segment range
		struct CMarkedInterval
		{
			mint m_iStartElem; // Start element position (inclusive)
			mint m_iEndElem; // End element position (exclusive)
			smint m_SignedScore; // +1 insert-heavy, -1 delete-heavy
		};

		constexpr static mint mcp_DetectorQueueSize = t_Options.m_DetectorQueueSize; // Number of timestamps per segment (ring buffer Q)
		constexpr static uint16 mcp_RunLengthCap = 1000; // Saturation cap for backward/forward run-length counters
		constexpr static uint16 mcp_NetBalanceCap = 1000; // Saturation cap for sc counter
		constexpr static uint16 mcp_RunLengthHotnessThreshold = 8; // θSC threshold for run-length counters

		// Fixed-point density representation: density = ScaledValue / mcp_DensityScale
		// All runtime density math uses integer arithmetic with this scale factor.
		constexpr static mint mcp_DensityScaleBits = fg_Min(mint(20), mint(sizeof(void *) * 8) / 2 - 1);
		constexpr static mint mcp_DensityScale = mint(1) << mcp_DensityScaleBits;
		// Truncate (floor) scaled constants so they are always <= the exact float value.
		// This ensures fsp_ScaledMulCeil for lower bounds and fsp_ScaledMulFloor for upper
		// bounds produce element counts identical to the original floating-point code.
		constexpr static mint mcp_LeafLowerBoundScaled = (mint)(mc_LeafLowerBound * mcp_DensityScale);
		constexpr static mint mcp_RootLowerBoundScaled = (mint)(mc_RootLowerBound * mcp_DensityScale);
		constexpr static mint mcp_LeafUpperBoundScaled = (mint)(mc_LeafUpperBound * mcp_DensityScale);
		constexpr static mint mcp_RootUpperBoundScaled = (mint)(mc_RootUpperBound * mcp_DensityScale);
		constexpr static mint mcp_ResizeThresholdScaled = (mint)(mc_ResizeThreshold * mcp_DensityScale);

		constexpr static bool mcp_bUseReciprocalDivide = false; // sizeof(void *) >= 8; // Not safe for 32 bit
		// Use fixed-point density math when max capacity exceeds pfp64 precision (53-bit mantissa).
		// Max capacity ≈ 2^mc_MaxCapacityBits / mc_RootUpperBound; safe for float when < 2^52.
		constexpr static bool mcp_bUseFixedPoint = [] -> bool
			{
				if (t_Options.m_MaxCapacityBits >= 63)
					return true;
				return (pfp64)(uint64(1) << t_Options.m_MaxCapacityBits) >= mc_RootUpperBound * (pfp64)(uint64(1) << 52);
			}
			()
		;

		// Density bound monotonicity: lower bounds increase toward root, upper bounds decrease.
		// This guarantees the interpolation deltas have known signs, enabling branchless unsigned arithmetic.
		static_assert(mc_LeafLowerBound >= 0.0, "Leaf lower bound must be >= 0.0");
		static_assert(mc_RootLowerBound >= 0.0, "Root lower bound must be >= 0.0");
		static_assert(mc_LeafUpperBound >= 0.0, "Leaf upper bound must be >= 0.0");
		static_assert(mc_RootUpperBound > 0.0, "Root upper bound must be > 0.0");
		static_assert(mc_RootUpperBound <= 1.0, "Root upper bound must be <= 1.0");
		static_assert(mc_ResizeThreshold >= 0.0, "Resize threshold must be >= 0.0");
		static_assert(mc_ResizeThreshold <= 1.0, "Resize threshold must be <= 1.0");
		static_assert(mc_RootLowerBound >= mc_LeafLowerBound, "Lower bound must increase from leaf to root");
		static_assert(mc_LeafUpperBound >= mc_RootUpperBound, "Upper bound must decrease from leaf to root");
		static_assert(mc_LeafUpperBound >= mc_LeafLowerBound, "Upper bound must be >= lower bound at leaf");
		static_assert(mc_RootUpperBound >= mc_RootLowerBound, "Upper bound must be >= lower bound at root");
		static_assert(mc_LeafUpperBound <= 1.0, "Leaf upper bound must be <= 1.0");
		static_assert(mcp_RootUpperBoundScaled > 0, "Root upper bound must remain positive after fixed-point scaling");

		// Precomputed unsigned deltas for interpolation (signs are known at compile time)
		constexpr static mint mcp_LowerBoundDeltaScaled = mcp_RootLowerBoundScaled - mcp_LeafLowerBoundScaled; // >= 0
		constexpr static mint mcp_UpperBoundDeltaScaled = mcp_LeafUpperBoundScaled - mcp_RootUpperBoundScaled; // >= 0

		constexpr static uint16 mcp_Fanout = 8; // Branching factor (cache line friendly)
		constexpr static uint16 mcp_FanoutBits = gc_HighestBitSet<mcp_Fanout>; // log2(fanout)
		constexpr static uint16 mcp_MaxLevels = (t_Options.m_MaxCapacityBits + mcp_FanoutBits - 1) / mcp_FanoutBits; // Maximum index depth (supports up to fanout^MaxLevels segments)

		// Reciprocal table for divide-free threshold interpolation.
		// Replaces division by Divisor in [1, mcp_MaxCalibratorLevels) with
		// multiply+shift: floor(|x| / d) == (|x| * R[d]) >> S.
		// The shift is sized so that the intermediate |x| * R[d] fits in signed mint.
		// Max dividend: mcp_DensityScale * (mcp_MaxCalibratorLevels - 1).
		// Exact when max dividend <= 2^mcp_ReciprocalShift.
		constexpr static mint mcp_ReciprocalShift = (mint)sizeof(mint) * 8 - 2 - mcp_DensityScaleBits - gc_HighestBitSet<mcp_MaxCalibratorLevels>;

		constexpr static CReciprocalTable mcp_ReciprocalTable = {};

		constexpr static bool mcp_bIndexStoresKeys = NTraits::cIsNothrowCopyConstructible<t_CKey> && NTraits::cIsNothrowCopyAssignable<t_CKey>;

		// Exception safety: noexcept traits for specialization
		constexpr static bool mcp_bNothrowElementMove = NTraits::cIsNothrowMoveConstructible<t_CKey> && NTraits::cIsNothrowMoveConstructible<t_CValue>;
		constexpr static bool mcp_bNothrowElementCopy = NTraits::cIsNothrowCopyConstructible<t_CKey> && NTraits::cIsNothrowCopyConstructible<t_CValue>;

		// Segment metadata - per-segment count and detector state
		constexpr static bool mcp_bDetectorStoresKeys = t_Options.m_bAdaptive && t_Options.m_bDetectorStoresKeys && NTraits::cIsNothrowCopyConstructible<t_CKey> && NTraits::cIsNothrowCopyAssignable<t_CKey>;

		// Scratch buffers may reuse the map allocator only when it is stateless and default-constructible.
		// Stateful allocators can require constructor arguments or impose allocation constraints.
		using CScratchAllocator = TCConditional<NTraits::cIsEmpty<t_CAllocator> && NTraits::cIsConstructibleWith<t_CAllocator>, t_CAllocator, NMemory::CAllocator_Heap>;

		// Static allocator sizing for rebalance temporaries (covers up to ~64 segments without heap)
		constexpr static mint mcp_nMarkedIntervalStaticSize = 64 * sizeof(CMarkedInterval);

		using CMarkedIntervalAllocator = NMemory::TCAllocator_Static<mcp_nMarkedIntervalStaticSize, 16, CScratchAllocator>;
		using CMarkedIntervalVector = TCVector<CMarkedInterval, CMarkedIntervalAllocator>;

		// Precomputed density bounds for a given calibrator level, independent of capacity.
		struct CLevelDensityBounds
		{
			using CThreshold = TCConditional<!mcp_bUseFixedPoint, pfp64, mint>;

			CThreshold m_LowerBound;
			CThreshold m_UpperBound;
		};

		struct CDetectorKeys
		{
			NStorage::TCOptional<t_CKey> m_BackwardRunLengthValue;  // Stored successor key (kbwd.value)
			NStorage::TCOptional<t_CKey> m_ForwardRunLengthValue;  // Stored predecessor key (kfwd.value)
		};

		struct CDetectorKeys_StoreSlot
		{
			NStorage::TCOptional<mint> m_BackwardRunLengthSlot;
			NStorage::TCOptional<mint> m_ForwardRunLengthSlot;
		};

		using CDetectorKeysStorage = TCConditional<mcp_bDetectorStoresKeys, CDetectorKeys, CDetectorKeys_StoreSlot>;

		struct CSegmentMeta_Adaptive
		{
			CDetectorKeysStorage m_DetectorKeys;
			uint32 m_Timestamps[mcp_DetectorQueueSize] = {};
			uint16 m_Count = 0;
			uint16 m_BackwardRunLength = 0;
			uint16 m_ForwardRunLength = 0;
			int16 m_NetBalance = 0;
			int8 m_LastOpSign = 0; // +1 insert, -1 delete, 0 unknown
			uint8 m_iQueueHead = 0;
			uint8 m_QueueCount = 0;
		};

		struct CSegmentMeta_NonAdaptive
		{
			uint16 m_Count = 0;
		};

		using CSegmentMeta = TCConditional<t_Options.m_bAdaptive, CSegmentMeta_Adaptive, CSegmentMeta_NonAdaptive>;

		struct CStats_Empty
		{
		};

		using CStatsStorage = TCConditional<t_Options.m_bStats, CStats, CStats_Empty>;

		using CIndexEntry = TCConditional<mcp_bIndexStoresKeys, t_CKey, mint>;
		using CIndexValidStorage = TCConditional<mcp_bIndexStoresKeys, uint8 *, CEmpty>;
		using CCalibratorCount = mint;

		using CTimeCounterStorage = TCConditional<t_Options.m_bAdaptive, uint32, CEmpty>;

		// Main data header - all arrays in one allocation
		struct CPackedMapData
		{
			constexpr inline_small void f_SetIndexEntryValid(mint _iEntry, bool _bValid) noexcept;
			constexpr inline_small bool f_GetIndexEntryValid(mint _iEntry) const noexcept;
			constexpr inline_small void f_DestroyIndexEntries() noexcept;
			constexpr inline_small void f_SetIndexEntry(mint _iEntry, CIndexEntry const &_Entry) noexcept;
			constexpr inline_small void f_ClearIndexEntry(mint _iEntry) noexcept;

			// Cached pointers (set on allocation)
			t_CKey *m_pKeys = nullptr;
			t_CValue *m_pValues = nullptr;
			CIndexEntry *m_pIndex = nullptr;                              // All index entries (all levels packed)
			mint *m_pLevelOffsets = nullptr;                              // Offset into m_pIndex for each level
			DMibNoUniqueAddress CIndexValidStorage m_IndexValidStorage{}; // Validity bitmask for key-stored entries (1 bit per entry)
			CCalibratorCount *m_pCalibratorCounts = nullptr;              // Calibrator tree counts (all levels packed)
			mint *m_pCalibratorOffsets = nullptr;                         // Offset into m_pCalibratorCounts for each level
			CSegmentMeta *m_pSegmentMeta = nullptr;

			mint m_nElements = 0;                                         // Total element count
			mint m_nSegments = 0;                                         // Number of segments
			mint m_AllocSize = 0;                                         // Size of allocation
			mint m_Capacity = 0;                                          // Total capacity (m_nSegments * SegmentSize)
			mint m_nStaticIndexTotalEntries = 0;                          // Total entries across all levels
			mint m_nCalibratorTreeTotalEntries = 0;                       // Total entries across all levels (~2*nSegments)
			DMibNoUniqueAddress CTimeCounterStorage m_TimeCounter = {};   // Global monotonic counter for detector timestamps
			uint16 m_nStaticIndexLevels = 0;                              // Number of levels (1 = leaf only)
			uint16 m_nCalibratorTreeLevels = 0;                           // Number of levels (ceil(log2(nSegments)) + 1)
		};

		// Static index helpers (multi-level B+-tree-like index)
		struct CIndexLayout
		{
			mint m_LevelSizes[mcp_MaxLevels];       // Size of each level
			mint m_LevelOffsets[mcp_MaxLevels];     // Offset for each level in packed array
			mint m_nTotalEntries;                   // Total entries across all levels
			uint16 m_nLevels;                       // Number of levels
		};

		// Binary search within segment
		struct CSearchResult
		{
			mint m_iLocalPos;                       // Position within segment (relative to first element)
			bool m_bExists;                         // Whether key was found
		};

		// Find global position (segment + local position)
		struct CFindResult
		{
			mint m_iSegment;
			mint m_iLocalPos;                       // Position relative to first element in segment
			mint m_iSlot;                           // Absolute slot index in keys/values array
			bool m_bExists;
		};

		// Calibrator tree layout (binary tree of element counts for O(log n) density queries)
		struct CCalibratorLayout
		{
			mint m_LevelSizes[mcp_MaxCalibratorLevels];        // Size of each level
			mint m_LevelOffsets[mcp_MaxCalibratorLevels];      // Offset for each level in packed array
			mint m_nTotalEntries;                              // Total entries across all levels
			uint16 m_nLevels;                                  // Number of levels
		};

		template <typename ...tfp_CAllocatorParams, typename ...tfp_CCompareParams, mint ...tp_IndicesAllocator, mint ...tp_IndicesCompare>
		constexpr TCPackedMap
			(
				NMeta::TCIndices<tp_IndicesAllocator...> const &_IndexSequenceAllocator
				, NMeta::TCIndices<tp_IndicesCompare...> const &_IndexSequuenceCompare
				, CAllocatorConstructTag &&
				, CCompareConstructTag &&
				, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
				, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
			)
		;

		// floor(density * capacity) — dispatches to fixed-point or float based on mcp_bUseFixedPoint
		constexpr static mint fsp_ScaledMulFloor(mint _ScaledDensity, mint _Capacity) noexcept;

		// ceil(density * capacity) — dispatches to fixed-point or float based on mcp_bUseFixedPoint
		constexpr static mint fsp_ScaledMulCeil(mint _ScaledDensity, mint _Capacity) noexcept;

		// ceil(nElements / density) — dispatches to fixed-point or float based on mcp_bUseFixedPoint
		constexpr static mint fsp_ScaledDivCeil(mint _nElements, mint _ScaledDensity) noexcept;

		// Exact unsigned integer division via reciprocal multiply for small dividends.
		// Precondition: _Dividend <= DensityScale * (MaxCalibratorLevels - 1), _Divisor in [1, MaxCalibratorLevels)
		constexpr static mint fsp_ReciprocalDivide(mint _Dividend, mint _Divisor) noexcept;

		// Remove helper (shared by key and pointer remove overloads)
		constexpr void fp_RemoveAtPosition(CPackedMapData *_pData, mint _iSegment, mint _iLocalPos);
		constexpr void fp_Clear(bool _bKeepBacking, t_CAllocator &_Allocator);

		// Segment helpers
		constexpr static mint fsp_GetSegmentStart(mint _iSegment) noexcept;
		constexpr static mint fsp_GetSegmentEnd(mint _iSegment) noexcept;
		constexpr static bool fsp_IsOddSegment(mint _iSegment) noexcept;
		constexpr static mint fsp_GetSegmentFirstSlot(mint _iSegment, mint _Count) noexcept;

		// Allocation helpers
		constexpr mint fp_CalculateAllocSize(mint _nSegments, mint &o_Alignment) const noexcept;
		constexpr void fp_AllocInitial(mint _nSegments = 0);
		constexpr void fp_ReserveForElementCount(mint _nElements);
		constexpr void fp_Resize(mint _nNewSegments);
		constexpr void fp_SetupCachedPointers(CPackedMapData *_pData) noexcept;

		constexpr static CIndexLayout fsp_ComputeIndexLayout(mint _nSegments) noexcept;
		constexpr static mint fsp_StaticIndexSizeBytes(mint _nSegments) noexcept;
		template <typename tf_CKey>
		constexpr static mint fsp_FindSegmentForKey(CPackedMapData const *_pData, t_CCompare const &_Compare, tf_CKey const &_Key) noexcept;
		constexpr static mint fsp_FindPrevNonEmptySegment(CPackedMapData const *_pData, mint _iSegment) noexcept;
		constexpr static mint fsp_FindNextNonEmptySegment(CPackedMapData const *_pData, mint _iSegment) noexcept;
		constexpr static mint fsp_FindLeftmostValidLeaf(CPackedMapData const *_pData, mint _iLevel, mint _iEntry) noexcept;
		constexpr static mint fsp_FindRightmostValidLeaf(CPackedMapData const *_pData, mint _iLevel, mint _iEntry) noexcept;
		constexpr void fp_RebuildStaticIndex(CPackedMapData *_pData) noexcept;
		constexpr void fp_RebuildStaticIndexRange(CPackedMapData *_pData, mint _iStartSeg, mint _iEndSeg) noexcept;
		constexpr void fp_UpdateStaticIndexEntry(CPackedMapData *_pData, mint _iSegment) noexcept;
		constexpr void fp_BuildIndexLevel(CPackedMapData *_pData, mint _iLevel) noexcept;
		constexpr bool fp_CompareIndexEntry(CPackedMapData const *_pData, mint _iEntry, t_CKey const &_Key) const noexcept;  // Returns true if entry < key

		// Validity bitmask helpers (for key-stored entries)
		constexpr static mint fsp_IndexValidityArraySizeBytes(mint _nTotalEntries) noexcept;

		template <typename tf_CKey>
		constexpr static CSearchResult fsp_BinarySearchInSegment(CPackedMapData const *_pData, t_CCompare const &_Compare, mint _iSegment, tf_CKey const &_Key) noexcept;

		template <typename tf_CKey>
		constexpr static CFindResult fsp_Find(CPackedMapData const *_pData, t_CCompare const &_Compare, tf_CKey const &_Key) noexcept;

		template <typename tf_CKey>
		constexpr static mint fsp_FindEqualSlot(CPackedMapData const *_pData, t_CCompare const &_Compare, tf_CKey const &_Key) noexcept;

		constexpr static mint fsp_ComputeRank(CPackedMapData const *_pData, mint _iSegment, mint _iLocalPos) noexcept;
		constexpr static mint fsp_FindSlotByRank(CPackedMapData const *_pData, mint _Rank) noexcept;

		// Shift operations
		constexpr mint fp_ShiftRightInSegment(CPackedMapData *_pData, mint _iSegment, mint _iLocalPos);
		constexpr void fp_ShiftLeftInSegment(CPackedMapData *_pData, mint _iSegment, mint _iLocalPos);

		// Calibrator tree helpers (density thresholds by level)
		constexpr mint fp_CalibratorLevelCount(mint _nSegments) const noexcept;
		constexpr void fp_GetLevelThresholds(mint _iLevel, mint _nLevels, mint &o_ScaledLowerBound, mint &o_ScaledUpperBound) const noexcept;
		constexpr mint fp_GetLevelLowerThreshold(mint _iLevel, mint _nLevels) const noexcept;
		constexpr mint fp_GetLevelUpperThreshold(mint _iLevel, mint _nLevels) const noexcept;

		// Compute density bounds for a given level (independent of capacity).
		constexpr CLevelDensityBounds fp_GetLevelDensityBounds(mint _iLevel, mint _nLevels) const noexcept;

		// Apply precomputed density bounds to a specific capacity.
		constexpr void fp_ElementBoundsFromDensity(CLevelDensityBounds const &_DensityBounds, mint _Capacity, mint &o_nMinElements, mint &o_nMaxElements) const noexcept;

		// Compute element bounds directly for a given level and capacity.
		// When mcp_bUseFixedPoint=false, uses the original direct pfp64 computation (no scaled integer round-trip).
		constexpr void fp_GetLevelElementBounds(mint _iLevel, mint _nLevels, mint _Capacity, mint &o_nMinElements, mint &o_nMaxElements) const noexcept;

		constexpr CCalibratorLayout fp_ComputeCalibratorLayout(mint _nSegments) const noexcept;
		constexpr mint fp_CalibratorTreeSizeBytes(mint _nSegments) const noexcept;
		constexpr void fp_InitializeCalibratorCounts(CPackedMapData *_pData) noexcept;
		constexpr void fp_UpdateCalibratorCountsRange(CPackedMapData *_pData, mint _iStartSeg, mint _iEndSeg) noexcept;
		constexpr void fp_UpdateCalibratorCountOnDelta(CPackedMapData *_pData, mint _iSegment, int32 _nDelta) noexcept;
		constexpr mint fp_GetCalibratorWindowCount(CPackedMapData const *_pData, mint _iLevel, mint _iWindowIndex) const noexcept;

		// Rebalancing
		constexpr bool fp_SegmentNeedsRebalance(CPackedMapData const *_pData, mint _iSegment) const noexcept;
		constexpr bool fp_SegmentNeedsRebalanceAfterDelete(CPackedMapData const *_pData, mint _iSegment) const noexcept;
		constexpr void fp_RebalanceFromSegment(CPackedMapData *_pData, mint _iSegment);
		constexpr NStorage::TCTuple<mint, mint, mint> fp_FindRebalanceWindow(CPackedMapData const *_pData, mint _iSegment) const noexcept;
		constexpr void fp_EvenRedistribute(CPackedMapData *_pData, mint _iStartSeg, mint _iEndSeg, mint _iWindowLevel, mint _nGlobalLevels);
		constexpr bool fp_ValidateRedistributeDensityBounds(CPackedMapData const *_pData, mint _iStartSeg, mint _iEndSeg, mint _nTotalElements, mint _nGlobalLevels, ch8 const *_pLabel) const noexcept;

		// Adaptive rebalancing (optional, implements RMA paper's Detector + Calibrator tree)
		constexpr void fp_RecordInsertion(CPackedMapData *_pData, mint _iSegment, t_CKey const *_pPrevKey, t_CKey const *_pNextKey) noexcept;
		constexpr void fp_RecordDeletion(CPackedMapData *_pData, mint _iSegment, t_CKey const *_pPrevKey, t_CKey const *_pNextKey) noexcept;
		constexpr void fp_ResetDetectorState(CPackedMapData *_pData, mint _iSegment);
		constexpr void fp_CopyDetectorStateScaled(CSegmentMeta &_Out, CSegmentMeta const &_In, mint _Overlap, mint _SourceCount);
		constexpr bool fp_IsSegmentMarked(CPackedMapData const *_pData, mint _iSegment) const noexcept;

		constexpr void fp_FindMarkedIntervals(CPackedMapData const *_pData, mint _iStartSeg, mint _iEndSeg, CMarkedIntervalVector &_OutIntervals);
		constexpr void fp_AdaptiveRedistribute(CPackedMapData *_pData, mint _iStartSeg, mint _iEndSeg, mint _iWindowLevel, mint _nGlobalLevels);

		constexpr bool fp_GetPrevNextKeys(CPackedMapData const *_pData, mint _iSegment, mint _iLocalPos, t_CKey const *&_pPrevKey, t_CKey const *&_pNextKey) const noexcept;
		constexpr void fp_AdjustDetectorSlotsOnShift(CPackedMapData *_pData, mint _iSegment, mint _iStartSlot, mint _iEndSlot, mint _Delta) noexcept;
		constexpr void fp_ClearDetectorSlotAt(CPackedMapData *_pData, mint _iSegment, mint _iSlot) noexcept;

		// Stats helpers
		constexpr void fp_StatsAddMoves(mint _nMoves) noexcept;
		constexpr void fp_StatsAddShiftMoves(mint _nMoves) noexcept;
		constexpr void fp_StatsAddRebalanceMoves(mint _nMoves) noexcept;
		constexpr void fp_StatsAddResizeMoves(mint _nMoves) noexcept;
		constexpr void fp_StatsAddRebalanceWindow(mint _nElements, mint _nSegments) noexcept;
		constexpr void fp_StatsAddRebalance() noexcept;
		constexpr void fp_StatsAddResize() noexcept;
		constexpr void fp_StatsAddEvenRedistribute() noexcept;
		constexpr void fp_StatsAddAdaptiveRedistribute() noexcept;
		constexpr void fp_StatsAddMarkedIntervals(mint _nIntervals, mint _nIntervalElements, mint _nSegments) noexcept;
		constexpr void fp_StatsAddAdaptiveIntervalKinds
			(
				mint _nTwoElementIntervals
				, mint _nWholeSegmentIntervals
				, mint _nBackwardRunLengthTriggered
				, mint _nForwardRunLengthTriggered
				, mint _nTwoElementIntervalFailures
			)
			noexcept
		;
		constexpr void fp_StatsAddAdaptiveSplitNoIntervals() noexcept;
		constexpr void fp_StatsAddAdaptiveSplitTooBig() noexcept;
		constexpr void fp_StatsAddAdaptiveWindow
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
		;
		constexpr void fp_StatsAddDetectorRemap(mint _nSegments, mint _nOverlapChecks, mint _nTransfers) noexcept;
		constexpr void fp_StatsAddRebalanceDecision(bool _bInsertTrigger, mint _iFirstViolationLevel, mint _iChosenLevel) noexcept;
		constexpr void fp_StatsAddRebalanceAlloc(bool _bHeap) noexcept;

		// Density-bounded recursive distribution of elements across segments
		constexpr void fp_DistributeElements(mint *_pTargets, mint _iStart, mint _iEnd, mint _nElements, mint _iLocalLevel, mint _iWindowLevel, mint _nGlobalLevels) const noexcept;

		constexpr static mint mcp_SegmentSize = [] -> mint
			{
				if constexpr (t_Options.m_SegmentBytes > 0)
				{
					constexpr mint c_ElementBytes = (mint)sizeof(t_CKey) + (mint)sizeof(t_CValue);
					constexpr mint c_ByBytes = (c_ElementBytes > 0) ? (t_Options.m_SegmentBytes / c_ElementBytes) : (mint)0;
					return (c_ByBytes > 0) ? c_ByBytes : (mint)1;
				}
				else
					return t_Options.m_SegmentSize;
			}
			()
		;
		static_assert(mcp_SegmentSize > 0, "PackedMap segment size must be greater than 0");
		static_assert(mcp_SegmentSize <= TCLimitsInt<uint16>::mc_Max, "PackedMap segment size exceeds the 16-bit per-segment count storage");

		// Internal data
		DMibNoUniqueAddress CStatsStorage mp_Stats;
		DMibNoUniqueAddress t_CAllocator mp_Allocator;
		DMibNoUniqueAddress t_CCompare mp_Compare;
		CPackedMapData *mp_pData = nullptr;
	};

	// Type alias for default options (same as TCPackedMap with default template arguments)
	template <typename t_CKey, typename t_CValue, typename t_CCompare = NMib::CSort_Default, typename t_CAllocator = NMib::NMemory::CAllocator_Heap>
	using TCPackedMapDefault = TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>;
}

// Include implementations
#include "PackedMap/Malterlib_Container_PackedMap.hpp"
