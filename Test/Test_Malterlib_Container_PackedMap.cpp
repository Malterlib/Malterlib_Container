// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Memory/MemoryManager>
#include <Mib/Container/PackedMap>
#include <Mib/Contract/Contract>
#include <Mib/Cryptography/SecureRandom>
#include <Mib/Atomic/Atomic>
#include <Mib/Stream/Streams/Vector>
#include <Mib/Test/Exception>

namespace
{
	using namespace NMib;
	using namespace NMib::NTraits;
	using namespace NMib::NStr;
	using namespace NMib::NContainer;
	using namespace NMib::NStorage;
	using namespace NMib::NMemory;
	using namespace NMib::NCryptography;
	using namespace NMib::NException;
	using namespace NMib::NStream;

	// Based on paper:
	// https://ir.cwi.nl/pub/28649/28649.pdf

	// Static verification of mcp_MaxLevels and mcp_MaxCalibratorLevels derived from mc_MaxCapacityBits
	template <typename t_CMap>
	struct TCPackedMapConstantsAccess : t_CMap
	{
		constexpr static uint16 mc_Fanout = t_CMap::mcp_Fanout;
		constexpr static uint16 mc_FanoutBits = t_CMap::mcp_FanoutBits;
		constexpr static uint16 mc_MaxLevels = t_CMap::mcp_MaxLevels;
		constexpr static uint16 mc_MaxCalibratorLevels = t_CMap::mcp_MaxCalibratorLevels;
		constexpr static bool mc_bUseFixedPoint = t_CMap::mcp_bUseFixedPoint;
		constexpr static bool mc_bNothrowElementMove = t_CMap::mcp_bNothrowElementMove;
		constexpr static bool mc_bNothrowElementCopy = t_CMap::mcp_bNothrowElementCopy;
		constexpr static umint mc_SegmentSize = t_CMap::mcp_SegmentSize;
		constexpr static umint mc_SegmentMetaSize = sizeof(typename t_CMap::CSegmentMeta);
		constexpr static umint mc_PackedMapDataSize = sizeof(typename t_CMap::CPackedMapData);
		using CCalibratorCountPointer = decltype(((typename t_CMap::CPackedMapData *)nullptr)->m_pCalibratorCounts);
	};

	// Default options: mc_MaxCapacityBits = min(sizeof(void *) * 8 - 2, 51)
	using CDefaultMap = TCPackedMapConstantsAccess<TCPackedMap<int32, int32>>;
	static_assert(CDefaultMap::mc_FanoutBits == 3);
	static_assert(CDefaultMap::mc_MaxCalibratorLevels == fg_Min(sizeof(void *) * 8 - 2, 51));
	static_assert(CDefaultMap::mc_MaxLevels == (fg_Min(sizeof(void *) * 8 - 2, 51) + 2) / 3);
	static_assert(!CDefaultMap::mc_bUseFixedPoint); // Default options should use floating-point density math
	static_assert(cIsSame<CDefaultMap::CCalibratorCountPointer, umint *>);
	static_assert(CDefaultMap::mc_SegmentSize <= TCLimitsInt<uint16>::mc_Max);

	using CDefaultMapNonAdaptive = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bAdaptive = false}>>;
	static_assert(CDefaultMapNonAdaptive::mc_SegmentMetaSize < CDefaultMap::mc_SegmentMetaSize);

	// Custom options: mc_MaxCapacityBits = 32
	using CMap32 = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_MaxCapacityBits = 32}>>;
	static_assert(CMap32::mc_MaxCalibratorLevels == 32);
	static_assert(CMap32::mc_MaxLevels == 11);
	static_assert(!CMap32::mc_bUseFixedPoint); // 2^32 well within pfp64 precision

	// Custom options: mc_MaxCapacityBits = 48
	using CMap48 = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_MaxCapacityBits = 48}>>;
	static_assert(CMap48::mc_MaxCalibratorLevels == 48);
	static_assert(CMap48::mc_MaxLevels == 16);
	static_assert(!CMap48::mc_bUseFixedPoint); // 2^48 within pfp64 precision

	// Custom options: mc_MaxCapacityBits = 3 (exact multiple of FanoutBits)
	using CMap3 = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_MaxCapacityBits = 3}>>;
	static_assert(CMap3::mc_MaxCalibratorLevels == 3);
	static_assert(CMap3::mc_MaxLevels == 1);
	static_assert(!CMap3::mc_bUseFixedPoint);

	// Fixed-point selection: mc_MaxCapacityBits = 52 exceeds pfp64 precision (2^52 >= 0.75 * 2^52)
	using CMap52 = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_MaxCapacityBits = 52}>>;
	static_assert(CMap52::mc_bUseFixedPoint);

	// Fixed-point selection: mc_MaxCapacityBits = 62 far exceeds pfp64 precision
	using CMap62 = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_MaxCapacityBits = 62}>>;
	static_assert(CMap62::mc_bUseFixedPoint);

	// Fixed-point selection: low mc_RootUpperBound (0.3) makes capacity exceed pfp64 precision even at 51 bits
	using CMapLowUpper = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_RootUpperBound = 0.30}>>;
	static_assert(CMapLowUpper::mc_bUseFixedPoint); // 2^51 / 0.30 > 2^52

	// Floating-point with low mc_RootUpperBound but fewer bits stays within pfp64 precision
	using CMapLowUpperSmall = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_MaxCapacityBits = 50, .m_RootUpperBound = 0.50}>>;
	static_assert(!CMapLowUpperSmall::mc_bUseFixedPoint); // 2^50 / 0.50 = 2^51 < 2^52

	using CMapSegmentMax = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = TCLimitsInt<uint16>::mc_Max}>>;
	static_assert(CMapSegmentMax::mc_SegmentSize == TCLimitsInt<uint16>::mc_Max);

	using CMapBytesSegmentMax = TCPackedMapConstantsAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentBytes = ((umint)sizeof(int32) + (umint)sizeof(int32)) * TCLimitsInt<uint16>::mc_Max}>>;
	static_assert(CMapBytesSegmentMax::mc_SegmentSize == TCLimitsInt<uint16>::mc_Max);

	static_assert(sizeof(TCPackedMap<int32, int32>) == sizeof(void *));
	static_assert(cIsSame<decltype(fg_GetType<TCPackedMap<int32, int32> &>().f_FindSmallestKey()), int32 const *>);
	static_assert(cIsSame<decltype(fg_GetType<TCPackedMap<int32, int32> &>().f_FindLargestKey()), int32 const *>);
	static_assert(cIsSame<decltype(fg_GetType<TCPackedMap<int32, int32> const &>().f_FindSmallestKey()), int32 const *>);
	static_assert(cIsSame<decltype(fg_GetType<TCPackedMap<int32, int32> const &>().f_FindLargestKey()), int32 const *>);

	struct CMoveOnlyKey
	{
		int32 m_Value = 0;

		CMoveOnlyKey() = default;
		explicit CMoveOnlyKey(int32 _Value)
			: m_Value(_Value)
		{
		}

		CMoveOnlyKey(CMoveOnlyKey const &) = delete;
		CMoveOnlyKey &operator = (CMoveOnlyKey const &) = delete;

		CMoveOnlyKey(CMoveOnlyKey &&) = default;
		CMoveOnlyKey &operator = (CMoveOnlyKey &&) = default;

		auto operator <=> (CMoveOnlyKey const &_Other) const noexcept = default;
	};

	struct CNonDefaultKey
	{
		int32 m_Value;

		CNonDefaultKey() = delete;
		explicit CNonDefaultKey(int32 _Value) noexcept
			: m_Value(_Value)
		{
		}

		CNonDefaultKey(CNonDefaultKey const &) noexcept = default;
		auto operator = (CNonDefaultKey const &) noexcept -> CNonDefaultKey & = default;
		CNonDefaultKey(CNonDefaultKey &&) noexcept = default;
		auto operator = (CNonDefaultKey &&) noexcept -> CNonDefaultKey & = default;

		auto operator <=> (CNonDefaultKey const &_Other) const noexcept = default;
	};

	struct CThrowingMoveCompare
	{
		CThrowingMoveCompare() = default;
		CThrowingMoveCompare(CThrowingMoveCompare const &) = default;
		CThrowingMoveCompare(CThrowingMoveCompare &&) noexcept(false)
		{
		}

		auto operator = (CThrowingMoveCompare const &) -> CThrowingMoveCompare & = default;
		auto operator = (CThrowingMoveCompare &&) noexcept(false) -> CThrowingMoveCompare & = default;

		auto operator () (int32 _Left, int32 _Right) const noexcept
		{
			return _Left <=> _Right;
		}
	};

	struct CConfigurableThrowingMoveCompare
	{
		static inline bool ms_bThrowOnMoveConstruct = false;
		static inline bool ms_bThrowOnMoveAssign = false;

		static void fs_Reset(bool _bThrowOnMoveConstruct = false, bool _bThrowOnMoveAssign = false)
		{
			ms_bThrowOnMoveConstruct = _bThrowOnMoveConstruct;
			ms_bThrowOnMoveAssign = _bThrowOnMoveAssign;
		}

		CConfigurableThrowingMoveCompare() = default;
		CConfigurableThrowingMoveCompare(CConfigurableThrowingMoveCompare const &) = default;
		CConfigurableThrowingMoveCompare(CConfigurableThrowingMoveCompare &&)
		{
			if (ms_bThrowOnMoveConstruct)
				DMibError("ThrowCompareMoveCtor");
		}

		auto operator = (CConfigurableThrowingMoveCompare const &) -> CConfigurableThrowingMoveCompare & = default;
		auto operator = (CConfigurableThrowingMoveCompare &&) -> CConfigurableThrowingMoveCompare &
		{
			if (ms_bThrowOnMoveAssign)
				DMibError("ThrowCompareMoveAssign");

			return *this;
		}

		template <typename tf_CLeft, typename tf_CRight>
		auto operator () (tf_CLeft const &_Left, tf_CRight const &_Right) const noexcept
		{
			return _Left <=> _Right;
		}
	};

	struct CThrowingMoveAllocator
	{
		CThrowingMoveAllocator() = default;
		CThrowingMoveAllocator(CThrowingMoveAllocator const &) = default;
		CThrowingMoveAllocator(CThrowingMoveAllocator &&) noexcept(false)
		{
		}

		auto operator = (CThrowingMoveAllocator const &) -> CThrowingMoveAllocator & = default;
		auto operator = (CThrowingMoveAllocator &&) noexcept(false) -> CThrowingMoveAllocator & = default;
	};

	struct CThrowingComparisonKey
	{
		static inline bool ms_bThrowOnEqual = false;
		static inline bool ms_bThrowOnCompare = false;

		int32 m_Value = 0;

		static void fs_Reset(bool _bThrowOnEqual = false, bool _bThrowOnCompare = false)
		{
			ms_bThrowOnEqual = _bThrowOnEqual;
			ms_bThrowOnCompare = _bThrowOnCompare;
		}

		CThrowingComparisonKey() = default;
		explicit CThrowingComparisonKey(int32 _Value) noexcept
			: m_Value(_Value)
		{
		}

		bool operator == (CThrowingComparisonKey const &_Other) const
		{
			if (ms_bThrowOnEqual)
				DMibError("ThrowEqualComparison");

			return m_Value == _Other.m_Value;
		}

		auto operator <=> (CThrowingComparisonKey const &_Other) const
		{
			if (ms_bThrowOnCompare)
				DMibError("ThrowThreeWayComparison");

			return m_Value <=> _Other.m_Value;
		}
	};

	struct CThrowingComparisonValue
	{
		static inline bool ms_bThrowOnEqual = false;
		static inline bool ms_bThrowOnCompare = false;

		int32 m_Value = 0;

		static void fs_Reset(bool _bThrowOnEqual = false, bool _bThrowOnCompare = false)
		{
			ms_bThrowOnEqual = _bThrowOnEqual;
			ms_bThrowOnCompare = _bThrowOnCompare;
		}

		CThrowingComparisonValue() = default;
		explicit CThrowingComparisonValue(int32 _Value) noexcept
			: m_Value(_Value)
		{
		}

		bool operator == (CThrowingComparisonValue const &_Other) const
		{
			if (ms_bThrowOnEqual)
				DMibError("ThrowEqualComparisonValue");

			return m_Value == _Other.m_Value;
		}

		auto operator <=> (CThrowingComparisonValue const &_Other) const
		{
			if (ms_bThrowOnCompare)
				DMibError("ThrowThreeWayComparisonValue");

			return m_Value <=> _Other.m_Value;
		}
	};

	struct CThrowingComparisonKeyCompare
	{
		auto operator () (CThrowingComparisonKey const &_Left, CThrowingComparisonKey const &_Right) const noexcept
		{
			return _Left.m_Value <=> _Right.m_Value;
		}
	};

	static_assert(cIsNothrowMoveConstructible<TCPackedMap<int32, int32>>);
	static_assert(!cIsNothrowMoveConstructible<TCPackedMap<int32, int32, CThrowingMoveCompare>>);
	static_assert(!cIsNothrowMoveConstructible<TCPackedMap<int32, int32, CSort_Default, CThrowingMoveAllocator>>);
	using CThrowingComparisonMap = TCPackedMap<CThrowingComparisonKey, CThrowingComparisonValue, CThrowingComparisonKeyCompare>;

	static_assert(noexcept(fg_GetType<TCPackedMap<int32, int32> const &>() == fg_GetType<TCPackedMap<int32, int32> const &>()));
	static_assert(noexcept(fg_GetType<TCPackedMap<int32, int32> const &>() <=> fg_GetType<TCPackedMap<int32, int32> const &>()));
	static_assert(noexcept(fg_GetType<TCPackedMap<int32, int32> const &>().f_CompareLexicographical(fg_GetType<TCPackedMap<int32, int32> const &>())));
	static_assert(!noexcept(fg_GetType<CThrowingComparisonMap const &>() == fg_GetType<CThrowingComparisonMap const &>()));
	static_assert(!noexcept(fg_GetType<CThrowingComparisonMap const &>() <=> fg_GetType<CThrowingComparisonMap const &>()));
	static_assert(!noexcept(fg_GetType<CThrowingComparisonMap const &>().f_CompareLexicographical(fg_GetType<CThrowingComparisonMap const &>())));

	// Stateless allocator that throws after a configurable number of allocations.
	// Since cIsEmpty is true, CScratchAllocator resolves to this type, ensuring
	// scratch buffer allocations (temp keys/values, Targets vector) also go through it.
	struct CFailingAllocator : CAllocator_Base
	{
		enum
		{
			mc_bIsDefault = false
			, mc_Reporting = true
			, mc_CanBeStatic = false
			, mc_bMethodsStatic = true
		};

		static inline umint ms_nAllocAfterFail = TCLimitsInt<umint>::mc_Max;
		static inline umint ms_nAllocations = 0;
		static inline umint ms_nFrees = 0;

		static void fs_Reset(umint _nAllocAfterFail = TCLimitsInt<umint>::mc_Max)
		{
			ms_nAllocAfterFail = _nAllocAfterFail;
			ms_nAllocations = 0;
			ms_nFrees = 0;
		}

		constexpr bool operator == (CFailingAllocator const &) const noexcept
		{
			return true;
		}

		constexpr auto operator <=> (CFailingAllocator const &) const noexcept
		{
			return COrdering_Strong::equal;
		}

		static void fp_CheckAlloc()
		{
			if (ms_nAllocAfterFail >= 0 && ++ms_nAllocations >= ms_nAllocAfterFail)
				DMibError("AllocFail");
		}

		only_parameters_aliased malloc_like static void *f_AllocAligned(umint _Size, umint _Alignment, EAllocationFlag _AllocFlags = EAllocationFlag_None, ENumaNode _NumaNode = ENumaNode_Default)
		{
			fp_CheckAlloc();
			return CAllocator_Heap::f_AllocAligned(_Size, _Alignment, _AllocFlags, _NumaNode);
		}

		only_parameters_aliased malloc_like static void *f_AllocAlignedWithSize(umint &_Size, umint _Alignment, EAllocationFlag _AllocFlags = EAllocationFlag_None, ENumaNode _NumaNode = ENumaNode_Default)
		{
			fp_CheckAlloc();
			return CAllocator_Heap::f_AllocAlignedWithSize(_Size, _Alignment, _AllocFlags, _NumaNode);
		}

		only_parameters_aliased malloc_like static void *f_Alloc(umint _Size, EAllocationFlag _AllocFlags = EAllocationFlag_None, ENumaNode _NumaNode = ENumaNode_Default)
		{
			fp_CheckAlloc();
			return CAllocator_Heap::f_Alloc(_Size, _AllocFlags, _NumaNode);
		}

		only_parameters_aliased malloc_like static void *f_AllocWithSize(umint &_Size, EAllocationFlag _AllocFlags = EAllocationFlag_None, ENumaNode _NumaNode = ENumaNode_Default)
		{
			fp_CheckAlloc();
			return CAllocator_Heap::f_AllocWithSize(_Size, _AllocFlags, _NumaNode);
		}

		only_parameters_aliased static void f_Free(void *_pBlock, umint _Size)
		{
			++ms_nFrees;
			CAllocator_Heap::f_Free(_pBlock, _Size);
		}

		only_parameters_aliased static void f_FreeNoSize(void *_pBlock)
		{
			CAllocator_Heap::f_FreeNoSize(_pBlock);
		}

		static bool f_DeterministicSize()
		{
			return CAllocator_Heap::f_DeterministicSize();
		}

		only_parameters_aliased static umint f_Size(void *_pBlock)
		{
			return CAllocator_Heap::f_Size(_pBlock);
		}

		only_parameters_aliased static umint f_TrySize(void *_pBlock)
		{
			return CAllocator_Heap::f_TrySize(_pBlock);
		}

		static umint f_SizePadded(umint _Size)
		{
			return CAllocator_Heap::f_SizePadded(_Size);
		}

		static bool f_IsStatic(void const *_pBlock)
		{
			return CAllocator_Heap::f_IsStatic(_pBlock);
		}

		only_parameters_aliased malloc_like static void *f_Realloc(void *_pMem, umint &_Size, umint _OldSize, EAllocationFlag _AllocFlags = EAllocationFlag_None, ENumaNode _NumaNode = ENumaNode_Default)
		{
			fp_CheckAlloc();
			return CAllocator_Heap::f_Realloc(_pMem, _Size, _OldSize, _AllocFlags, _NumaNode);
		}

		only_parameters_aliased static void *f_Resize(void *_pMem, umint &_Size, umint _OldSize, EAllocationFlag _AllocFlags = EAllocationFlag_None, ENumaNode _NumaNode = ENumaNode_Default)
		{
			return CAllocator_Heap::f_Resize(_pMem, _Size, _OldSize, _AllocFlags, _NumaNode);
		}

		static bool f_OnlyOneAlloc()
		{
			return CAllocator_Heap::f_OnlyOneAlloc();
		}
	};

	static_assert(cIsEmpty<CFailingAllocator>);
	static_assert(cIsConstructibleWith<CFailingAllocator>);

	// Nothrow-move type with double-destroy detection.
	// Exercises the mcp_bNothrowElementMove path in fp_Resize/fp_EvenRedistribute
	// while catching stale-count bugs that trivially-destructible int32 cannot.
	struct CNothrowMoveTracked
	{
		static inline NAtomic::TCAtomic<umint> ms_nAlive{0};
		int32 m_Value = 0;
		uint32 m_Magic = 0;

		static void fs_Reset()
		{
			ms_nAlive = 0;
		}

		CNothrowMoveTracked() noexcept
		{
			m_Magic = 0xABCD1234;
			++ms_nAlive;
		}

		explicit CNothrowMoveTracked(int32 _Value) noexcept
			: m_Value(_Value)
		{
			m_Magic = 0xABCD1234;
			++ms_nAlive;
		}

		~CNothrowMoveTracked()
		{
			DMibFastCheck(m_Magic == 0xABCD1234);
			m_Magic = 0xDEADDEAD;
			--ms_nAlive;
		}

		CNothrowMoveTracked(CNothrowMoveTracked const &_Other) noexcept
			: m_Value(_Other.m_Value)
		{
			m_Magic = 0xABCD1234;
			++ms_nAlive;
		}

		CNothrowMoveTracked(CNothrowMoveTracked &&_Other) noexcept
			: m_Value(_Other.m_Value)
		{
			m_Magic = 0xABCD1234;
			++ms_nAlive;
		}

		CNothrowMoveTracked &operator = (CNothrowMoveTracked const &) noexcept = default;
		CNothrowMoveTracked &operator = (CNothrowMoveTracked &&) noexcept = default;

		bool operator == (CNothrowMoveTracked const &_Other) const noexcept = default;
		auto operator <=> (CNothrowMoveTracked const &_Other) const noexcept = default;
	};

	static_assert(cIsNothrowMoveConstructible<CNothrowMoveTracked>);
	static_assert(cIsNothrowCopyConstructible<CNothrowMoveTracked>);

	struct CIntKeyProbe
	{
		int32 m_Value = 0;
	};

	struct CTransparentIntCompare
	{
		auto operator () (int32 _Left, int32 _Right) const noexcept
		{
			return _Left <=> _Right;
		}

		auto operator () (int32 _Left, CIntKeyProbe const &_Right) const noexcept
		{
			return _Left <=> _Right.m_Value;
		}

		auto operator () (CIntKeyProbe const &_Left, int32 _Right) const noexcept
		{
			return _Left.m_Value <=> _Right;
		}

		auto operator () (CIntKeyProbe const &_Left, CIntKeyProbe const &_Right) const noexcept
		{
			return _Left.m_Value <=> _Right.m_Value;
		}
	};

	struct CStatefulIntCompare
	{
		bool m_bReverse = false;

		CStatefulIntCompare() = default;
		explicit CStatefulIntCompare(bool _bReverse)
			: m_bReverse(_bReverse)
		{
		}

		auto operator () (int32 _Left, int32 _Right) const noexcept
		{
			return m_bReverse ? (_Right <=> _Left) : (_Left <=> _Right);
		}
	};

	struct CReverseIntCompare
	{
		auto operator () (int32 _Left, int32 _Right) const noexcept
		{
			return _Right <=> _Left;
		}
	};

	struct CSignedUnsignedIntCompare
	{
		auto operator () (int32 _Left, int32 _Right) const noexcept
		{
			return _Left <=> _Right;
		}

		auto operator () (uint32 _Left, uint32 _Right) const noexcept
		{
			return _Left <=> _Right;
		}

		auto operator () (int32 _Left, uint32 _Right) const noexcept
		{
			return (int64)_Left <=> (int64)_Right;
		}

		auto operator () (uint32 _Left, int32 _Right) const noexcept
		{
			return (int64)_Left <=> (int64)_Right;
		}
	};

	struct CStatefulSignedUnsignedIntCompare
	{
		bool m_bReverse = false;

		CStatefulSignedUnsignedIntCompare() = default;
		explicit CStatefulSignedUnsignedIntCompare(bool _bReverse)
			: m_bReverse(_bReverse)
		{
		}

		auto operator () (int32 _Left, int32 _Right) const noexcept
		{
			return m_bReverse ? (_Right <=> _Left) : (_Left <=> _Right);
		}

		auto operator () (uint32 _Left, uint32 _Right) const noexcept
		{
			return m_bReverse ? (_Right <=> _Left) : (_Left <=> _Right);
		}

		auto operator () (int32 _Left, uint32 _Right) const noexcept
		{
			if (m_bReverse)
				return (int64)_Right <=> (int64)_Left;
			else
				return (int64)_Left <=> (int64)_Right;
		}

		auto operator () (uint32 _Left, int32 _Right) const noexcept
		{
			if (m_bReverse)
				return (int64)_Right <=> (int64)_Left;
			else
				return (int64)_Left <=> (int64)_Right;
		}
	};

	struct CDetectMovedFromFreeAllocatorState
	{
		umint m_nAllocations = 0;
		umint m_nFrees = 0;
	};

	struct CDetectMovedFromFreeAllocator : CAllocator_Base
	{
		static inline umint ms_nFreeCallsFromMovedFrom = 0;

		static void fs_Reset() noexcept
		{
			ms_nFreeCallsFromMovedFrom = 0;
		}

		CDetectMovedFromFreeAllocator() = default;

		explicit CDetectMovedFromFreeAllocator(CDetectMovedFromFreeAllocatorState *_pState) noexcept
			: m_pState(_pState)
		{
		}

		CDetectMovedFromFreeAllocator(CDetectMovedFromFreeAllocator const &) = default;

		CDetectMovedFromFreeAllocator(CDetectMovedFromFreeAllocator &&_Other) noexcept
			: m_pState(fg_Exchange(_Other.m_pState, nullptr))
		{
		}

		auto operator = (CDetectMovedFromFreeAllocator const &) -> CDetectMovedFromFreeAllocator & = default;

		auto operator = (CDetectMovedFromFreeAllocator &&_Other) noexcept -> CDetectMovedFromFreeAllocator &
		{
			m_pState = fg_Exchange(_Other.m_pState, nullptr);
			return *this;
		}

		only_parameters_aliased malloc_like void *f_AllocAlignedWithSize(umint &_Size, umint _Alignment, EAllocationFlag _AllocFlags = EAllocationFlag_None, ENumaNode _NumaNode = ENumaNode_Default)
		{
			DMibCheck(m_pState);
			++m_pState->m_nAllocations;
			return CAllocator_Heap::f_AllocAlignedWithSize(_Size, _Alignment, _AllocFlags, _NumaNode);
		}

		only_parameters_aliased malloc_like void *f_AllocAligned(umint _Size, umint _Alignment, EAllocationFlag _AllocFlags = EAllocationFlag_None, ENumaNode _NumaNode = ENumaNode_Default)
		{
			DMibCheck(m_pState);
			++m_pState->m_nAllocations;
			return CAllocator_Heap::f_AllocAligned(_Size, _Alignment, _AllocFlags, _NumaNode);
		}

		only_parameters_aliased void f_Free(void *_pBlock, umint _Size)
		{
			if (m_pState)
				++m_pState->m_nFrees;
			else
				++ms_nFreeCallsFromMovedFrom;

			CAllocator_Heap::f_Free(_pBlock, _Size);
		}

		CDetectMovedFromFreeAllocatorState *m_pState = nullptr;
	};

	template <typename t_CMap>
	void fg_RunInsertAndVerify(umint _nCount)
	{
		t_CMap Map;
		for (umint iTest = 0; iTest < _nCount; ++iTest)
			Map.f_Insert((int32)iTest, (int32)(iTest * 10));

		DMibExpect(Map.f_GetLen(), ==, _nCount);

		umint iExpected = 0;
		for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
		{
			DMibExpect(*Iter, ==, (int32)iExpected)(ETestFlag_Aggregated);
			++iExpected;
		}
		DMibExpect(iExpected, ==, _nCount);
	}

	template <typename t_CMap>
	umint fg_RunInsertStats(umint _nCount)
	{
		t_CMap Map;
		Map.f_ResetStats();

		for (umint iTest = 0; iTest < _nCount; ++iTest)
			Map.f_Insert((int32)iTest, (int32)iTest);

		return Map.f_GetStats().m_nElementMoves;
	}

	template <typename t_CMap>
	void fg_RunRandomStress(umint _nCount)
	{
		t_CMap Map;

		CSecureRandom Random;
		Random.f_InsecureDeterministicReseed(0x12345678u);
		for (umint iTest = 0; iTest < _nCount; ++iTest)
		{
			umint nKey = (umint)(Random.f_GetValue<uint32>() % (uint32)_nCount);
			Map.f_Insert((int32)nKey, (int32)nKey);

			NStr::CStr ValidateError;
			if (!Map.f_Debug_Validate(&ValidateError))
				DMibError(NStr::CStr::CFormat("Insert {}: {}") << iTest << ValidateError);
		}

		// Verify sorted order and uniqueness
		int32 nPrev = -1;
		for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
		{
			DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
			nPrev = *Iter;
		}
	}

	template <typename t_CMap>
	struct TCPackedMapTestAccess : t_CMap
	{
		using t_CMap::t_CMap;
		using t_CMap::mc_RootUpperBound;
		using t_CMap::mcp_SegmentSize;
		using t_CMap::mp_pData;
		using t_CMap::fp_CalibratorLevelCount;
		using t_CMap::fp_GetLevelElementBounds;
		using t_CMap::fsp_FindNextNonEmptySegment;
		using t_CMap::fsp_FindPrevNonEmptySegment;
		using t_CMap::fsp_GetSegmentFirstSlot;
	};

	template <typename t_CMap>
	void fg_ExpectPackedMapValid(t_CMap const &_Map, ch8 const *_pLabel)
	{
		NStr::CStr ValidateError;
		if (!_Map.f_Debug_Validate(&ValidateError))
			DMibError(NStr::CStr::CFormat("{} validation failed: {}") << _pLabel << ValidateError);
	}

	template <typename t_CMap>
	void fg_ExpectIntKeyOrder(t_CMap const &_Map, int32 const *_pExpected, umint _nExpected)
	{
		umint iExpected = 0;
		for (auto Iter = _Map.f_Keys().f_GetIterator(); Iter; ++Iter)
		{
			DMibExpect(iExpected, <, _nExpected)(ETestFlag_Aggregated);
			DMibExpect(*Iter, ==, _pExpected[iExpected])(ETestFlag_Aggregated);
			++iExpected;
		}

		DMibExpect(iExpected, ==, _nExpected);
	}

	template <typename t_CMap>
	auto fg_FindIntGapInsertKeyInSegment(t_CMap const &_Map, umint _iSegment, int32 &o_Key) -> bool
	{
		auto *pData = _Map.mp_pData;
		umint Count = pData->m_pSegmentMeta[_iSegment].m_Count;
		if (Count < 2)
			return false;

		umint iFirst = t_CMap::fsp_GetSegmentFirstSlot(_iSegment, Count);
		for (umint iLocal = 0; iLocal + 1 < Count; ++iLocal)
		{
			int32 LeftKey = pData->m_pKeys[iFirst + iLocal];
			int32 RightKey = pData->m_pKeys[iFirst + iLocal + 1];
			if (RightKey - LeftKey >= 2)
			{
				o_Key = LeftKey + (RightKey - LeftKey) / 2;
				return true;
			}
		}

		return false;
	}

	struct CProjectedBulkInsertPlan
	{
		umint m_nInitialCount = 0;
		int32 m_LeftInsertKey = 0;
		int32 m_RightInsertKey = 0;
		bool m_bFound = false;
	};

	template <typename t_CMap>
	auto fg_FindProjectedBulkInsertPlan(t_CMap &_Map) -> CProjectedBulkInsertPlan
	{
		for (umint nInitial = 16; nInitial <= 23; ++nInitial)
		{
			_Map.f_Clear();
			_Map.f_BulkLoad
				(
					nInitial
					, [&](auto &&_fInsert)
					{
						for (umint iKey = 0; iKey < nInitial; ++iKey)
							_fInsert((int32)(iKey * 10), (int32)(iKey * 100));
					}
				)
			;

			auto *pData = _Map.mp_pData;
			if (!pData || pData->m_nSegments < 2)
				continue;

			if (pfp64(pData->m_nElements + 2) > t_CMap::mc_RootUpperBound * pfp64(pData->m_Capacity))
				continue;

			umint nLevels = _Map.fp_CalibratorLevelCount(pData->m_nSegments);
			umint nMinTwoSegment = 0;
			umint nMaxTwoSegment = 0;
			_Map.fp_GetLevelElementBounds(1, nLevels, 2 * t_CMap::mcp_SegmentSize, nMinTwoSegment, nMaxTwoSegment);

			for (umint iSegment = 0; iSegment + 1 < pData->m_nSegments; ++iSegment)
			{
				umint LeftCount = pData->m_pSegmentMeta[iSegment].m_Count;
				umint RightCount = pData->m_pSegmentMeta[iSegment + 1].m_Count;

				if (LeftCount < 2 || RightCount < 2)
					continue;
				if ((LeftCount + 1 > t_CMap::mcp_SegmentSize) || (RightCount + 1 > t_CMap::mcp_SegmentSize))
					continue;
				if ((LeftCount + RightCount + 2) <= nMaxTwoSegment)
					continue;

				int32 LeftInsertKey = 0;
				int32 RightInsertKey = 0;
				if
				(
					!fg_FindIntGapInsertKeyInSegment(_Map, iSegment, LeftInsertKey)
					|| !fg_FindIntGapInsertKeyInSegment(_Map, iSegment + 1, RightInsertKey)
					|| LeftInsertKey >= RightInsertKey
				)
					continue;

				return
				{
					.m_nInitialCount = nInitial
					, .m_LeftInsertKey = LeftInsertKey
					, .m_RightInsertKey = RightInsertKey
					, .m_bFound = true
				};
			}
		}

		return {};
	}

#if defined(DMibDebug) || defined(DMibSanitizerEnabled)
	constexpr static bool gc_bDoHugeTests = false;
#else
	constexpr static bool gc_bDoHugeTests = true;
#endif

//#define DMibContainerPackedMapDebugStats

	template <typename t_CStats>
	umint fg_PrintPackedMapStats(char const *_pLabel, t_CStats const &_Stats)
	{
		umint nMoves = _Stats.m_nElementMoves;
#ifdef DMibContainerPackedMapDebugStats
		DMibConOut
			(
				"{}: {ns } shift={ns } rebalance={ns } resize={ns } rebals={ns } avgWinSeg={fe2} maxWinSeg={ns }\n"
				, _pLabel, nMoves
				, _Stats.m_nShiftMoves
				, _Stats.m_nRebalanceMoves
				, _Stats.m_nResizeMoves
				, _Stats.m_nRebalances
				, _Stats.m_nRebalances > 0 ? fp64(_Stats.m_nRebalanceWindowSegments) / fp64(_Stats.m_nRebalances) : fp64(0)
				, _Stats.m_nRebalanceWindowMaxSegments
			)
		;
		DMibConOut
			(
				"  markedIntervals={ns } markedElems={ns } markedSegs={ns } noIntervalSplits={ns } tooBigSplits={ns }\n"
				, _Stats.m_nMarkedIntervals
				, _Stats.m_nMarkedIntervalElements
				, _Stats.m_nMarkedSegments
				, _Stats.m_nAdaptiveSplitNoIntervals
				, _Stats.m_nAdaptiveSplitTooBig
			)
		;
		DMibConOut
			(
				"  twoElemInts={ns } wholeSegInts={ns } bwdRLTrig={ns } fwdRLTrig={ns } twoElemFails={ns }\n"
				, _Stats.m_nAdaptiveTwoElementIntervals
				, _Stats.m_nAdaptiveWholeSegmentIntervals
				, _Stats.m_nAdaptiveBackwardRunLengthTriggered
				, _Stats.m_nAdaptiveForwardRunLengthTriggered
				, _Stats.m_nAdaptiveTwoElementIntervalFailures
			)
		;
		DMibConOut
			(
				"  adaptiveWindows={ns } windowsWithTs={ns } totalTs={ns } hotSegs={ns } percIsMax={ns }\n"
				, _Stats.m_nAdaptiveWindows
				, _Stats.m_nAdaptiveWindowsWithTimestamps
				, _Stats.m_nAdaptiveTotalTimestamps
				, _Stats.m_nAdaptiveHotSegments
				, _Stats.m_nAdaptivePercentileIsMax
			)
		;
		DMibConOut
			(
				"  tsSegs={ns } anyAbovePct={ns } halfAbovePct={ns } threeQuarterAbovePct={ns }\n"
				, _Stats.m_nAdaptiveSegmentsWithTimestamps
				, _Stats.m_nAdaptiveSegmentsAnyAbovePercentile
				, _Stats.m_nAdaptiveSegmentsHalfAbovePercentile
				, _Stats.m_nAdaptiveSegmentsThreeQuarterAbovePercentile
			)
		;
		DMibConOut
			(
				"  remapWindows={ns } remapSegs={ns } remapChecks={ns } remapTransfers={ns }\n"
				, _Stats.m_nDetectorRemapWindows
				, _Stats.m_nDetectorRemapSegments
				, _Stats.m_nDetectorRemapOverlapChecks
				, _Stats.m_nDetectorRemapTransfers
			)
		;
		DMibConOut
			(
				"  rebalanceDecision: ins={ns } del={ns } chosenLvlAvg={fe2} firstViolLvlAvg={fe2}\n"
				, _Stats.m_nRebalanceInsertTriggers
				, _Stats.m_nRebalanceDeleteTriggers
				, _Stats.m_nRebalances > 0 ? fp64(_Stats.m_nRebalanceChosenLevelSum) / fp64(_Stats.m_nRebalances) : fp64(0)
				, _Stats.m_nRebalances > 0 ? fp64(_Stats.m_nRebalanceFirstViolationLevelSum) / fp64(_Stats.m_nRebalances) : fp64(0)
			)
		;
		{
			NStr::CStr ChosenHist;
			NStr::CStr ViolHist;
			for (umint iLevel = 0; iLevel < _Stats.mc_nRebalanceLevelHistSize; ++iLevel)
			{
				if (_Stats.m_RebalanceChosenLevelHist[iLevel] > 0)
					ChosenHist += "{}:{ns } "_f << iLevel << _Stats.m_RebalanceChosenLevelHist[iLevel];
				if (_Stats.m_RebalanceFirstViolationLevelHist[iLevel] > 0)
					ViolHist += "{}:{ns } "_f << iLevel << _Stats.m_RebalanceFirstViolationLevelHist[iLevel];
			}
			DMibConOut("  chosenLvlHist={}\n", ChosenHist);
			DMibConOut("  firstViolLvlHist={}\n", ViolHist);
		}
#endif
		return nMoves;
	}

	class CPackedMap_Tests : public NMib::NTest::CTest
	{
	public:
		void f_DoTests()
		{
			DMibTestSuite("Basic")
			{
				DMibTestCategory("Size")
				{
					TCPackedMap<int32, int32> Map;
					static_assert(sizeof(Map) == sizeof(void *), "TCPackedMap should be one pointer in size");
					DMibExpect(Map.f_GetLen(), ==, 0);
					DMibExpectTrue(Map.f_IsEmpty());
				};
				DMibTestCategory("InsertAndFind")
				{
					TCPackedMap<int32, int32> Map;

					// Insert some values
					auto Result1 = Map.f_Insert(5, 50);
					DMibExpectTrue(Result1.m_bInserted);
					DMibExpect(Result1.f_Key(), ==, 5);
					DMibExpect(Result1.f_Value(), ==, 50);

					auto Result2 = Map.f_Insert(3, 30);
					DMibExpectTrue(Result2.m_bInserted);

					auto Result3 = Map.f_Insert(7, 70);
					DMibExpectTrue(Result3.m_bInserted);

					DMibExpect(Map.f_GetLen(), ==, 3);

					// Find values
					{
						DMibTestPath("Find5");
						auto *pValue = Map.f_FindEqual(5);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, 50);
					}
					{
						DMibTestPath("Find3");
						auto *pValue = Map.f_FindEqual(3);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, 30);
					}
					{
						DMibTestPath("Find7");
						auto *pValue = Map.f_FindEqual(7);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, 70);
					}
					{
						DMibTestPath("FindNonExisting");
						auto *pValue = Map.f_FindEqual(10);
						DMibExpectFalse(pValue);
					}
				};
				DMibTestCategory("DuplicateInsert")
				{
					TCPackedMap<int32, int32> Map;

					{
						DMibTestPath("FirstInsert");
						Map.f_Insert(5, 50);
						DMibExpect(Map.f_GetLen(), ==, 1);
					}

					{
						DMibTestPath("DuplicateInsert");
						// Insert duplicate - should return existing
						auto Result = Map.f_Insert(5, 500);
						DMibExpectFalse(Result.m_bInserted);
						DMibExpect(Result.f_Value(), ==, 50); // Original value unchanged
						DMibExpect(Map.f_GetLen(), ==, 1);
					}
				};
				DMibTestCategory("OperatorBracket")
				{
					TCPackedMap<CStr, int32> Map;

					Map["Alpha"] = 1;
					Map["Beta"] = 2;
					Map["Gamma"] = 3;

					DMibExpect(Map.f_GetLen(), ==, 3);
					DMibExpect(*Map.f_FindEqual("Alpha"), ==, 1);
					DMibExpect(*Map.f_FindEqual("Beta"), ==, 2);
					DMibExpect(*Map.f_FindEqual("Gamma"), ==, 3);

					// Modify through bracket
					Map["Beta"] = 20;
					DMibExpect(*Map.f_FindEqual("Beta"), ==, 20);
				};
				DMibTestCategory("HeterogeneousLookup")
				{
					using CMap = TCPackedMap<int32, int32, CTransparentIntCompare>;

					CMap Map;
					Map.f_Insert(10, 100);
					Map.f_Insert(20, 200);
					Map.f_Insert(30, 300);

					DMibAssertTrue(Map.f_FindEqual(CIntKeyProbe{.m_Value = 20}));
					DMibExpect(*Map.f_FindEqual(CIntKeyProbe{.m_Value = 20}), ==, 200);
					DMibExpect(*Map.f_FindSmallestGreaterThanEqual(CIntKeyProbe{.m_Value = 25}), ==, 300);
					DMibExpect(*Map.f_FindLargestLessThanEqual(CIntKeyProbe{.m_Value = 25}), ==, 200);

					auto IterExact = Map.f_GetIterator(CIntKeyProbe{.m_Value = 20});
					DMibAssertTrue(IterExact);
					DMibExpect(*IterExact, ==, 200);

					auto IterGE = Map.f_GetIterator_SmallestGreaterThanEqual(CIntKeyProbe{.m_Value = 25});
					DMibAssertTrue(IterGE);
					DMibExpect(*IterGE, ==, 300);

					auto IterLE = Map.f_GetIterator_LargestLessThanEqual(CIntKeyProbe{.m_Value = 25});
					DMibAssertTrue(IterLE);
					DMibExpect(*IterLE, ==, 200);

					DMibExpectTrue(Map.f_Remove(CIntKeyProbe{.m_Value = 20}));
					DMibExpectFalse(Map.f_FindEqual(20));
					DMibExpectFalse(Map.f_Remove(CIntKeyProbe{.m_Value = 99}));

					fg_ExpectPackedMapValid(Map, "HeterogeneousLookup");
				};
			};
			DMibTestSuite("TemplatedOptions")
			{
				DMibTestCategory("AdaptiveOn_DefaultSegment")
				{
					using CPackedMap = TCPackedMap<int32, int32>;
					fg_RunInsertAndVerify<CPackedMap>(512);
				};
				DMibTestCategory("AdaptiveOff_DefaultSegment")
				{
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bAdaptive = false}>;
					fg_RunInsertAndVerify<CPackedMap>(512);
				};
				DMibTestCategory("AdaptiveOn_SmallSegment")
				{
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 32}>;
					fg_RunInsertAndVerify<CPackedMap>(512);
				};
			};
			DMibTestSuite("Complexity")
			{
				DMibTestCategory("InsertGrowth")
				{
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					umint nMovesN;
					{
						DMibTestPath("Lower");
						nMovesN = fg_RunInsertStats<CPackedMap>(1000);
					}
					umint nMoves2N;
					{
						DMibTestPath("Upper");
						nMoves2N = fg_RunInsertStats<CPackedMap>(2000);
					}

					// Expect sub-quadratic growth: moves for 2N should be less than 4x moves for N
					DMibExpect(nMoves2N, <, nMovesN * 4);
				};
			};
			DMibTestSuite("StressExtended")
			{
				DMibTestCategory("RandomLarge")
				{
					using CPackedMap = TCPackedMap<int32, int32>;
					fg_RunRandomStress<CPackedMap>(5000);
				};
				DMibTestCategory("MoveOnlyKeys")
				{
					using CPackedMap = TCPackedMap<CMoveOnlyKey, int32>;
					CPackedMap Map;
					for (int32 iTest = 0; iTest < 200; ++iTest)
						Map.f_Insert(CMoveOnlyKey(iTest), iTest);

					int32 iExpected = 0;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(Iter->m_Value, ==, iExpected)(ETestFlag_Aggregated);
						++iExpected;
					}
					DMibExpect(iExpected, ==, 200);
				};
			};
			DMibTestSuite("Remove")
			{
				DMibTestCategory("ByKey")
				{
					TCPackedMap<int32, int32> Map;

					for (int32 iTest = 0; iTest < 10; ++iTest)
						Map.f_Insert(iTest, iTest * 10);

					DMibExpect(Map.f_GetLen(), ==, 10);

					{
						DMibTestPath("RemoveExisting");
						bool bRemoved = Map.f_Remove(5);
						DMibExpectTrue(bRemoved);
						DMibExpect(Map.f_GetLen(), ==, 9);
						DMibExpectFalse(Map.f_FindEqual(5));
					}

					{
						DMibTestPath("RemoveNonExisting");
						bool bRemoved = Map.f_Remove(100);
						DMibExpectFalse(bRemoved);
						DMibExpect(Map.f_GetLen(), ==, 9);
					}
				};
				DMibTestCategory("ByPointer")
				{
					DMibTestCategory("Basic")
					{
						TCPackedMap<int32, int32> Map;

						for (int32 iTest = 0; iTest < 10; ++iTest)
							Map.f_Insert(iTest, iTest * 10);

						auto *pValue = Map.f_FindEqual(7);
						DMibAssertTrue(pValue);

						Map.f_Remove(pValue);
						DMibExpect(Map.f_GetLen(), ==, 9);
						DMibExpectFalse(Map.f_FindEqual(7));
					};
					DMibTestCategory("AllElements")
					{
						// Remove every element by pointer, validating after each removal
						TCPackedMap<int32, int32> Map;

						for (int32 iTest = 0; iTest < 200; ++iTest)
							Map.f_Insert(iTest * 3, iTest * 10);

						fg_ExpectPackedMapValid(Map, "BeforePointerRemoveAll");

						for (int32 iTest = 0; iTest < 200; ++iTest)
						{
							int32 Key = iTest * 3;
							auto *pValue = Map.f_FindEqual(Key);
							DMibAssertTrue(pValue)(ETestFlag_Aggregated);
							DMibExpect(*pValue, ==, iTest * 10)(ETestFlag_Aggregated);

							Map.f_Remove(pValue);
							DMibExpect(Map.f_GetLen(), ==, 200 - iTest - 1)(ETestFlag_Aggregated);
							DMibExpectFalse(Map.f_FindEqual(Key))(ETestFlag_Aggregated);
						}

						DMibExpectTrue(Map.f_IsEmpty());
					};
					DMibTestCategory("Interleaved")
					{
						// Interleave pointer-based remove with key-based insert
						TCPackedMap<int32, int32> Map;

						for (int32 iTest = 0; iTest < 100; ++iTest)
							Map.f_Insert(iTest, iTest);

						// Remove even keys by pointer, then re-insert with new values
						for (int32 iTest = 0; iTest < 100; iTest += 2)
						{
							auto *pValue = Map.f_FindEqual(iTest);
							DMibAssertTrue(pValue)(ETestFlag_Aggregated);
							Map.f_Remove(pValue);
						}

						DMibExpect(Map.f_GetLen(), ==, 50);
						fg_ExpectPackedMapValid(Map, "AfterPointerRemoveEvens");

						for (int32 iTest = 0; iTest < 100; iTest += 2)
							Map.f_Insert(iTest, iTest + 1000);

						DMibExpect(Map.f_GetLen(), ==, 100);
						fg_ExpectPackedMapValid(Map, "AfterReinsert");

						// Verify new values
						for (int32 iTest = 0; iTest < 100; iTest += 2)
						{
							auto *pFound = Map.f_FindEqual(iTest);
							DMibAssertTrue(pFound)(ETestFlag_Aggregated);
							DMibExpect(*pFound, ==, iTest + 1000)(ETestFlag_Aggregated);
						}
					};
					DMibTestCategory("FirstAndLast")
					{
						// Remove the first and last elements by pointer
						TCPackedMap<int32, int32> Map;

						for (int32 iTest = 0; iTest < 50; ++iTest)
							Map.f_Insert(iTest, iTest * 10);

						// Remove smallest by pointer
						auto *pSmallest = Map.f_FindSmallest();
						DMibAssertTrue(pSmallest);
						DMibExpect(*pSmallest, ==, 0);
						Map.f_Remove(pSmallest);
						DMibExpect(Map.f_GetLen(), ==, 49);
						DMibExpectFalse(Map.f_FindEqual(0));

						// Remove largest by pointer
						auto *pLargest = Map.f_FindLargest();
						DMibAssertTrue(pLargest);
						DMibExpect(*pLargest, ==, 490);
						Map.f_Remove(pLargest);
						DMibExpect(Map.f_GetLen(), ==, 48);
						DMibExpectFalse(Map.f_FindEqual(49));

						fg_ExpectPackedMapValid(Map, "AfterFirstLastRemove");
					};
					DMibTestCategory("ConstPointer")
					{
						// Remove using const value pointer
						TCPackedMap<int32, int32> Map;

						for (int32 iTest = 0; iTest < 20; ++iTest)
							Map.f_Insert(iTest, iTest * 10);

						int32 const *pValue = ((TCPackedMap<int32, int32> const &)Map).f_FindEqual(15);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, 150);

						Map.f_Remove(pValue);
						DMibExpect(Map.f_GetLen(), ==, 19);
						DMibExpectFalse(Map.f_FindEqual(15));
						fg_ExpectPackedMapValid(Map, "AfterConstPointerRemove");
					};
					DMibTestCategory("AfterRebalance")
					{
						// Build a map large enough to trigger rebalances, then remove by pointer
						TCPackedMap<int32, int32> Map;

						for (int32 iTest = 0; iTest < 500; ++iTest)
							Map.f_Insert(iTest, iTest);

						fg_ExpectPackedMapValid(Map, "BeforeRebalancePointerRemove");

						// Remove a spread of elements by pointer to trigger delete-side rebalances
						for (int32 iTest = 0; iTest < 500; iTest += 3)
						{
							auto *pValue = Map.f_FindEqual(iTest);
							DMibAssertTrue(pValue)(ETestFlag_Aggregated);
							Map.f_Remove(pValue);
						}

						fg_ExpectPackedMapValid(Map, "AfterRebalancePointerRemove");

						// Verify remaining elements
						for (int32 iTest = 0; iTest < 500; ++iTest)
						{
							bool bShouldExist = (iTest % 3) != 0;
							auto *pValue = Map.f_FindEqual(iTest);
							DMibExpect(!!pValue, ==, bShouldExist)(ETestFlag_Aggregated);
						}
					};
					DMibTestCategory("ReverseOrder")
					{
						// Remove all elements by pointer in reverse key order
						TCPackedMap<int32, int32> Map;

						for (int32 iTest = 0; iTest < 150; ++iTest)
							Map.f_Insert(iTest, iTest * 7);

						for (int32 iTest = 149; iTest >= 0; --iTest)
						{
							auto *pValue = Map.f_FindEqual(iTest);
							DMibAssertTrue(pValue)(ETestFlag_Aggregated);
							DMibExpect(*pValue, ==, iTest * 7)(ETestFlag_Aggregated);
							Map.f_Remove(pValue);
						}

						DMibExpectTrue(Map.f_IsEmpty());
					};
					DMibTestCategory("SmallSegments")
					{
						// Use small segment size to exercise more segment boundaries
						using CSmallMap = TCPackedMap<int32, int32, CSort_Default, NMemory::CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4}>;
						CSmallMap Map;

						for (int32 iTest = 0; iTest < 80; ++iTest)
							Map.f_Insert(iTest * 2, iTest);

						fg_ExpectPackedMapValid(Map, "BeforeSmallSegPointerRemove");

						// Remove every other element by pointer
						for (int32 iTest = 0; iTest < 80; iTest += 2)
						{
							int32 Key = iTest * 2;
							auto *pValue = Map.f_FindEqual(Key);
							DMibAssertTrue(pValue)(ETestFlag_Aggregated);
							Map.f_Remove(pValue);
						}

						DMibExpect(Map.f_GetLen(), ==, 40);
						fg_ExpectPackedMapValid(Map, "AfterSmallSegPointerRemove");
					};
					DMibTestCategory("NullPointer")
					{
						// Null pointer should be a no-op, not throw
						TCPackedMap<int32, int32> Map;
						Map.f_Insert(1, 10);

						Map.f_Remove((int32 *)nullptr);
						umint nAfterMutable = Map.f_GetLen();
						DMibExpect(nAfterMutable, ==, 1);

						Map.f_Remove((int32 const *)nullptr);
						umint nAfterConst = Map.f_GetLen();
						DMibExpect(nAfterConst, ==, 1);
					};
					DMibTestCategory("EmptyMap")
					{
						// Remove on empty map with null pointer should be a no-op
						TCPackedMap<int32, int32> Map;
						Map.f_Remove((int32 *)nullptr);
						DMibExpectTrue(Map.f_IsEmpty());
					};
#ifdef DMibContractConfigure_RequireEnabled
					DMibTestCategory("PastCapacity")
					{
						// Pointer outside the values array should violate the pointer precondition
						using CMap = TCPackedMapTestAccess<TCPackedMap<int32, int32>>;
						CMap Map;

						for (int32 iTest = 0; iTest < 10; ++iTest)
							Map.f_Insert(iTest, iTest * 10);

						auto *pData = Map.mp_pData;
						int32 *pBeforeBegin = (int32 *)(uintptr_t(pData->m_pValues) - sizeof(int32));
						int32 *pPastEnd = pData->m_pValues + pData->m_Capacity;

						DMibExpectViolatesRequire(Map.f_Remove(pBeforeBegin), "bAddressInRange && bOnValueBoundary");
						DMibExpectViolatesRequire(Map.f_Remove(pPastEnd), "bAddressInRange && bOnValueBoundary");
						DMibExpect(Map.f_GetLen(), ==, 10);
					};
					DMibTestCategory("GapSlotEvenSegment")
					{
						// Pointer to a gap slot in an even segment should violate the element-range precondition
						// Even segments are packed right, so gaps are at the left side
						using CMap = TCPackedMapTestAccess<TCPackedMap<int32, int32, CSort_Default, NMemory::CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8}>>;
						CMap Map;

						// Insert fewer than segment size elements to ensure gaps exist
						for (int32 iTest = 0; iTest < 4; ++iTest)
							Map.f_Insert(iTest, iTest * 10);

						auto *pData = Map.mp_pData;
						// Segment 0 is even with 4 elements in an 8-slot segment
						// Elements are packed right at [4,5,6,7], gaps at [0,1,2,3]
						umint Count = pData->m_pSegmentMeta[0].m_Count;
						DMibAssert(Count, >, 0);
						DMibAssert(Count, <, (umint)CMap::mcp_SegmentSize);

						// Point to the first slot (a gap in the even segment)
						int32 *pGapSlot = &pData->m_pValues[0];
						umint iFirst = CMap::fsp_GetSegmentFirstSlot(0, Count);

						// Only test if there's actually a gap (first element isn't at slot 0)
						if (iFirst > 0)
						{
							DMibExpectViolatesRequire(Map.f_Remove(pGapSlot), "iSlot >= iFirst");
							DMibExpect(Map.f_GetLen(), ==, 4);
						}
					};
					DMibTestCategory("GapSlotOddSegment")
					{
						// Pointer to a gap slot in an odd segment should violate the element-range precondition
						// Odd segments are packed left, so gaps are at the right side
						using CMap = TCPackedMapTestAccess<TCPackedMap<int32, int32, CSort_Default, NMemory::CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8}>>;
						CMap Map;

						// Insert enough to have elements in segment 1 (odd)
						// With 8-slot segments at 75% density, ~12 elements spans 2+ segments
						for (int32 iTest = 0; iTest < 12; ++iTest)
							Map.f_Insert(iTest, iTest * 10);

						auto *pData = Map.mp_pData;
						// Find an odd segment with a gap
						for (umint iSeg = 1; iSeg < pData->m_nSegments; iSeg += 2)
						{
							umint Count = pData->m_pSegmentMeta[iSeg].m_Count;
							if (Count > 0 && Count < CMap::mcp_SegmentSize)
							{
								// Odd segment packed left: elements at [segStart, segStart+Count-1]
								// Gap at segStart + Count
								umint iGapSlot = iSeg * CMap::mcp_SegmentSize + Count;
								int32 *pGapSlot = &pData->m_pValues[iGapSlot];

								DMibExpectViolatesRequire(Map.f_Remove(pGapSlot), "iLocalPos < Count");
								DMibExpect(Map.f_GetLen(), ==, 12);
								break;
							}
						}
					};
					DMibTestCategory("EmptySegmentSlot")
					{
						// Pointer to a slot in a completely empty segment should violate the non-empty-segment precondition
						using CMap = TCPackedMapTestAccess<TCPackedMap<int32, int32, CSort_Default, NMemory::CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8}>>;
						CMap Map;

						// Insert and remove to create empty segments
						for (int32 iTest = 0; iTest < 40; ++iTest)
							Map.f_Insert(iTest, iTest * 10);

						// Remove elements to potentially empty some segments
						for (int32 iTest = 0; iTest < 40; iTest += 2)
							Map.f_Remove(iTest);

						auto *pData = Map.mp_pData;
						umint nLen = Map.f_GetLen();

						// Find an empty segment
						for (umint iSeg = 0; iSeg < pData->m_nSegments; ++iSeg)
						{
							if (pData->m_pSegmentMeta[iSeg].m_Count == 0)
							{
								int32 *pEmptySlot = &pData->m_pValues[iSeg * CMap::mcp_SegmentSize];
								DMibExpectViolatesRequire(Map.f_Remove(pEmptySlot), "Count > 0");
								DMibExpect(Map.f_GetLen(), ==, nLen);
								break;
							}
						}
					};
#endif
				};
				DMibTestCategory("Clear")
				{
					TCPackedMap<int32, int32> Map;

					for (int32 iTest = 0; iTest < 100; ++iTest)
						Map.f_Insert(iTest, iTest * 10);

					DMibExpect(Map.f_GetLen(), ==, 100);

					Map.f_Clear();
					DMibExpect(Map.f_GetLen(), ==, 0);
					DMibExpectTrue(Map.f_IsEmpty());
				};
				DMibTestCategory("ClearKeepBackingResetsIndex")
				{
					using CMap = TCPackedMapTestAccess<TCPackedMap<int32, int32>>;

					CMap Map;

					for (int32 iTest = 0; iTest < 256; ++iTest)
						Map.f_Insert(iTest, iTest * 10);

					auto *pDataBeforeClear = Map.mp_pData;
					DMibAssertTrue(pDataBeforeClear);
					umint CapacityBeforeClear = pDataBeforeClear->m_Capacity;

					Map.f_Clear(true);

					DMibExpect(Map.f_GetLen(), ==, 0);
					DMibExpectTrue(Map.f_IsEmpty());
					DMibAssertTrue(Map.mp_pData);
					DMibExpectTrue(Map.mp_pData == pDataBeforeClear);
					DMibExpect(Map.mp_pData->m_Capacity, ==, CapacityBeforeClear);

					for (umint iEntry = 0; iEntry < Map.mp_pData->m_nStaticIndexTotalEntries; ++iEntry)
						DMibExpectFalse(Map.mp_pData->f_GetIndexEntryValid(iEntry))(ETestFlag_Aggregated);

					auto Result = Map.f_Insert(5000, 123);
					DMibExpectTrue(Result.m_bInserted);

					auto *pValue = Map.f_FindEqual(5000);
					DMibAssertTrue(pValue);
					DMibExpect(*pValue, ==, 123);
					DMibExpectFalse(Map.f_FindEqual(17));

					fg_ExpectPackedMapValid(Map, "ClearKeepBackingResetsIndex");
				};
			};
			DMibTestSuite("Iteration")
			{
				DMibTestCategory("Forward")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(5, 50);
					Map.f_Insert(3, 30);
					Map.f_Insert(7, 70);
					Map.f_Insert(1, 10);
					Map.f_Insert(9, 90);

					// Iterate in sorted order
					TCVector<int32> Keys;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
						Keys.f_InsertLast(*Iter);

					DMibExpect(Keys.f_GetLen(), ==, 5);
					DMibExpect(Keys[0], ==, 1);
					DMibExpect(Keys[1], ==, 3);
					DMibExpect(Keys[2], ==, 5);
					DMibExpect(Keys[3], ==, 7);
					DMibExpect(Keys[4], ==, 9);
				};
				DMibTestCategory("Reverse")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(5, 50);
					Map.f_Insert(3, 30);
					Map.f_Insert(7, 70);
					Map.f_Insert(1, 10);
					Map.f_Insert(9, 90);

					// Iterate in reverse sorted order
					TCVector<int32> Keys;
					for (auto Iter = Map.f_Keys().f_GetIteratorReverse(); Iter; ++Iter)
						Keys.f_InsertLast(*Iter);

					DMibExpect(Keys.f_GetLen(), ==, 5);
					DMibExpect(Keys[0], ==, 9);
					DMibExpect(Keys[1], ==, 7);
					DMibExpect(Keys[2], ==, 5);
					DMibExpect(Keys[3], ==, 3);
					DMibExpect(Keys[4], ==, 1);
				};
				DMibTestCategory("KeyValue")
				{
					TCPackedMap<CStr, int32> Map;

					Map["Alpha"] = 1;
					Map["Beta"] = 2;
					Map["Gamma"] = 3;

					umint nSum = 0;
					for (auto Iter = Map.f_Entries().f_GetIterator(); Iter; ++Iter)
					{
						auto Ref = *Iter;
						nSum += Ref.f_Value();
					}

					DMibExpect(nSum, ==, 6);
				};
			};
			DMibTestSuite("FindOperations")
			{
				DMibTestCategory("Exists")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(5, 50);
					Map.f_Insert(10, 100);
					Map.f_Insert(15, 150);

					DMibExpectTrue(Map.f_Exists(5));
					DMibExpectTrue(Map.f_Exists(10));
					DMibExpectTrue(Map.f_Exists(15));
					DMibExpectFalse(Map.f_Exists(7));
					DMibExpectFalse(Map.f_Exists(0));
					DMibExpectFalse(Map.f_Exists(20));
				};
				DMibTestCategory("SmallestLargest")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(5, 50);
					Map.f_Insert(3, 30);
					Map.f_Insert(7, 70);
					Map.f_Insert(1, 10);
					Map.f_Insert(9, 90);

					auto *pSmallest = Map.f_FindSmallest();
					DMibAssertTrue(pSmallest);
					DMibExpect(*pSmallest, ==, 10);

					auto *pLargest = Map.f_FindLargest();
					DMibAssertTrue(pLargest);
					DMibExpect(*pLargest, ==, 90);

					auto pSmallestKey = Map.f_FindSmallestKey();
					DMibAssertTrue(pSmallestKey);
					DMibExpect(*pSmallestKey, ==, 1);

					auto pLargestKey = Map.f_FindLargestKey();
					DMibAssertTrue(pLargestKey);
					DMibExpect(*pLargestKey, ==, 9);
				};
				DMibTestCategory("Bounds")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(10, 100);
					Map.f_Insert(20, 200);
					Map.f_Insert(30, 300);
					Map.f_Insert(40, 400);

					// FindSmallestGreaterThanEqual
					{
						DMibTestPath("SGTE_15");
						auto *pValue = Map.f_FindSmallestGreaterThanEqual(15);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, 200); // 20 is smallest >= 15
					}
					{
						DMibTestPath("SGTE_20");
						auto *pValue = Map.f_FindSmallestGreaterThanEqual(20);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, 200); // 20 is smallest >= 20
					}
					{
						DMibTestPath("SGTE_50");
						auto *pValue = Map.f_FindSmallestGreaterThanEqual(50);
						DMibExpectFalse(pValue); // No key >= 50
					}
					// FindLargestLessThanEqual
					{
						DMibTestPath("LLTE_25");
						auto *pValue = Map.f_FindLargestLessThanEqual(25);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, 200); // 20 is largest <= 25
					}
					{
						DMibTestPath("LLTE_20");
						auto *pValue = Map.f_FindLargestLessThanEqual(20);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, 200); // 20 is largest <= 20
					}
					{
						DMibTestPath("LLTE_5");
						auto *pValue = Map.f_FindLargestLessThanEqual(5);
						DMibExpectFalse(pValue); // No key <= 5
					}
				};
				DMibTestCategory("ConstIndexMissingKeyThrows")
				{
					TCPackedMap<int32, int32> Map;
					Map.f_Insert(1, 10);

					auto const &ConstMap = Map;
					DMibExpect(ConstMap[1], ==, 10);
					DMibExpectExceptionType(ConstMap[2], CException);
				};
				DMibTestCategory("NonDefaultConstructibleKey")
				{
					TCPackedMap<CNonDefaultKey, int32> Map;

					Map.f_Insert(CNonDefaultKey(3), 30);
					Map.f_Insert(CNonDefaultKey(1), 10);
					Map.f_Insert(CNonDefaultKey(2), 20);

					auto *pValue = Map.f_FindEqual(CNonDefaultKey(2));
					DMibAssertTrue(pValue);
					DMibExpect(*pValue, ==, 20);
					DMibExpect(Map.f_GetLen(), ==, 3);
					fg_ExpectPackedMapValid(Map, "NonDefaultConstructibleKey");
				};
			};
			DMibTestSuite("CopyMove")
			{
				DMibTestCategory("Copy")
				{
					TCPackedMap<int32, int32> Map1;

					for (int32 iTest = 0; iTest < 50; ++iTest)
						Map1.f_Insert(iTest, iTest * 10);

					TCPackedMap<int32, int32> Map2(Map1);

					DMibExpect(Map2.f_GetLen(), ==, 50);
					for (int32 iTest = 0; iTest < 50; ++iTest)
					{
						auto *pValue = Map2.f_FindEqual(iTest);
						DMibAssertTrue(pValue)(ETestFlag_Aggregated);
						DMibExpect(*pValue, ==, iTest * 10)(ETestFlag_Aggregated);
					}

					// Verify Map1 unchanged
					DMibExpect(Map1.f_GetLen(), ==, 50);
				};
				DMibTestCategory("CopyPreservesStatefulAllocator")
				{
					using CMemoryManagerParams = NMemory::TCMemoryManagerParams<>;
					using CMemoryManager = NMemory::TCMemoryManager<CMemoryManagerParams>;
					using CStatefulAllocator = NMemory::TCAllocator_MemoryManager<CMemoryManagerParams>;
					using CMap = TCPackedMap<int32, int32, CSort_Default, CStatefulAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;

					CMemoryManager MemoryManager{NMemory::CMemoryManagerConfig()};
					CMap Map1(CAllocatorConstructTag(), &MemoryManager);
					Map1.f_Insert(1, 10);
					Map1.f_Insert(2, 20);
					Map1.f_Insert(3, 30);

					CMap Map2(Map1);

					DMibExpect(Map2.f_GetLen(), ==, 3);
					DMibAssertTrue(Map2.f_FindEqual(2));
					DMibExpect(*Map2.f_FindEqual(2), ==, 20);

					auto Result = Map2.f_Insert(4, 40);
					DMibExpectTrue(Result.m_bInserted);
					DMibExpect(*Map2.f_FindEqual(4), ==, 40);

					fg_ExpectPackedMapValid(Map1, "CopyPreservesStatefulAllocator.Source");
					fg_ExpectPackedMapValid(Map2, "CopyPreservesStatefulAllocator.Copy");
				};
				DMibTestCategory("Move")
				{
					TCPackedMap<int32, int32> Map1;

					for (int32 iTest = 0; iTest < 50; ++iTest)
						Map1.f_Insert(iTest, iTest * 10);

					TCPackedMap<int32, int32> Map2(fg_Move(Map1));

					DMibExpect(Map2.f_GetLen(), ==, 50);
					for (int32 iTest = 0; iTest < 50; ++iTest)
					{
						auto *pValue = Map2.f_FindEqual(iTest);
						DMibAssertTrue(pValue)(ETestFlag_Aggregated);
						DMibExpect(*pValue, ==, iTest * 10)(ETestFlag_Aggregated);
					}

					// Map1 should be empty after move
					DMibExpect(Map1.f_GetLen(), ==, 0);
				};
				DMibTestCategory("TemplatedMoveConstructorDoesNotFreeThroughMovedFromAllocator")
				{
					using CSourceMap = TCPackedMap<int32, int32, CSort_Default, CDetectMovedFromFreeAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;
					using CDestinationMap = TCPackedMap<int64, int32, CSort_Default, CDetectMovedFromFreeAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;

					CDetectMovedFromFreeAllocatorState State;
					CDetectMovedFromFreeAllocator::fs_Reset();

					{
						CSourceMap Source(CAllocatorConstructTag(), &State);
						Source.f_Insert(1, 10);
						Source.f_Insert(2, 20);
						Source.f_Insert(3, 30);

						CDestinationMap Destination(fg_Move(Source));

						DMibExpect(Source.f_IsEmpty(), ==, true);
						DMibAssertTrue(Destination.f_FindEqual(1));
						DMibExpect(*Destination.f_FindEqual(1), ==, 10);
						DMibAssertTrue(Destination.f_FindEqual(2));
						DMibExpect(*Destination.f_FindEqual(2), ==, 20);
						DMibAssertTrue(Destination.f_FindEqual(3));
						DMibExpect(*Destination.f_FindEqual(3), ==, 30);
						fg_ExpectPackedMapValid(Destination, "TemplatedMoveConstructorDoesNotFreeThroughMovedFromAllocator");
					}

					DMibExpect(CDetectMovedFromFreeAllocator::ms_nFreeCallsFromMovedFrom, ==, 0);
					DMibExpect(State.m_nFrees, >, 0);
				};
				DMibTestCategory("CopyAssign")
				{
					TCPackedMap<int32, int32> Map1;
					TCPackedMap<int32, int32> Map2;

					for (int32 iTest = 0; iTest < 50; ++iTest)
						Map1.f_Insert(iTest, iTest * 10);

					Map2.f_Insert(100, 1000);
					Map2 = Map1;

					DMibExpect(Map2.f_GetLen(), ==, 50);
					DMibExpectFalse(Map2.f_FindEqual(100)); // Old data cleared
				};
				DMibTestCategory("MoveAssign")
				{
					TCPackedMap<int32, int32> Map1;
					TCPackedMap<int32, int32> Map2;

					for (int32 iTest = 0; iTest < 50; ++iTest)
						Map1.f_Insert(iTest, iTest * 10);

					Map2.f_Insert(100, 1000);
					Map2 = fg_Move(Map1);

					DMibExpect(Map2.f_GetLen(), ==, 50);
					DMibExpect(Map1.f_GetLen(), ==, 0);
				};
				DMibTestCategory("CopyAssignCopiesStatefulComparator")
				{
					using CMap = TCPackedMap<int32, int32, CStatefulIntCompare>;

					CMap Map1(CCompareConstructTag(), true);
					Map1.f_Insert(3, 30);
					Map1.f_Insert(2, 20);
					Map1.f_Insert(1, 10);

					CMap Map2(CCompareConstructTag(), false);
					Map2.f_Insert(100, 1000);
					Map2 = Map1;

					auto *pValue = Map2.f_FindEqual(2);
					DMibAssertTrue(pValue);
					DMibExpect(*pValue, ==, 20);

					auto Result = Map2.f_Insert(4, 40);
					DMibExpectTrue(Result.m_bInserted);

					constexpr static int32 gc_Expected[] = {4, 3, 2, 1};
					fg_ExpectIntKeyOrder(Map2, gc_Expected, 4);
					fg_ExpectPackedMapValid(Map2, "CopyAssignCopiesStatefulComparator");
				};
				DMibTestCategory("TemplatedMoveAssignPreservesStatefulComparator")
				{
					using CSourceMap = TCPackedMap<int32, int32, CStatefulIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_MaxCapacityBits = 12}>;
					using CDestinationMap = TCPackedMap<int32, int32, CStatefulIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;

					CSourceMap Source(CCompareConstructTag(), true);
					Source.f_Insert(3, 30);
					Source.f_Insert(2, 20);
					Source.f_Insert(1, 10);

					CDestinationMap Destination(CCompareConstructTag(), false);
					Destination = fg_Move(Source);

					auto *pValue = Destination.f_FindEqual(3);
					DMibAssertTrue(pValue);
					DMibExpect(*pValue, ==, 30);
					DMibExpect(Source.f_GetLen(), ==, 0);

					auto Result = Destination.f_Insert(4, 40);
					DMibExpectTrue(Result.m_bInserted);

					constexpr static int32 gc_Expected[] = {4, 3, 2, 1};
					fg_ExpectIntKeyOrder(Destination, gc_Expected, 4);
					fg_ExpectPackedMapValid(Destination, "TemplatedMoveAssignPreservesStatefulComparator");
				};
				DMibTestCategory("TemplatedConversionFallsBackWhenComparatorDiffers")
				{
					using CSourceMap = TCPackedMap<int32, int32, CReverseIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_MaxCapacityBits = 12}>;
					using CDestinationMap = TCPackedMap<int32, int32, CStatefulIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;

					CSourceMap Source;
					Source.f_Insert(3, 30);
					Source.f_Insert(2, 20);
					Source.f_Insert(1, 10);

					CDestinationMap Destination(CCompareConstructTag(), false);
					Destination = Source;

					auto *pValue = Destination.f_FindEqual(2);
					DMibAssertTrue(pValue);
					DMibExpect(*pValue, ==, 20);

					auto Result = Destination.f_Insert(4, 40);
					DMibExpectTrue(Result.m_bInserted);

					constexpr static int32 gc_Expected[] = {1, 2, 3, 4};
					fg_ExpectIntKeyOrder(Destination, gc_Expected, 4);
					fg_ExpectPackedMapValid(Destination, "TemplatedConversionFallsBackWhenComparatorDiffers");
				};
				DMibTestCategory("TemplatedConversionFallsBackWhenKeyTypeDiffers")
				{
					using CSourceMap = TCPackedMap<int32, int32, CSignedUnsignedIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_MaxCapacityBits = 12}>;
					using CDestinationMap = TCPackedMap<uint32, int32, CSignedUnsignedIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;

					CSourceMap Source;
					Source.f_Insert(int32(-1), 10);
					Source.f_Insert(int32(0), 20);

					CDestinationMap Destination(Source);

					DMibAssertTrue(Destination.f_FindEqual(uint32(0u)));
					DMibExpect(*Destination.f_FindEqual(uint32(0u)), ==, 20);
					DMibAssertTrue(Destination.f_FindEqual(TCLimitsInt<uint32>::mc_Max));
					DMibExpect(*Destination.f_FindEqual(TCLimitsInt<uint32>::mc_Max), ==, 10);

					constexpr static uint32 gc_Expected[] = {0u, TCLimitsInt<uint32>::mc_Max};
					umint iExpected = 0;
					for (auto Iter = Destination.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(iExpected, <, 2)(ETestFlag_Aggregated);
						DMibExpect(*Iter, ==, gc_Expected[iExpected])(ETestFlag_Aggregated);
						++iExpected;
					}
					DMibExpect(iExpected, ==, 2);
					fg_ExpectPackedMapValid(Destination, "TemplatedConversionFallsBackWhenKeyTypeDiffers");
				};
				DMibTestCategory("TemplatedCopyAssignPreservesComparatorWhenKeyTypeDiffers")
				{
					using CSourceMap = TCPackedMap<int32, int32, CStatefulSignedUnsignedIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_MaxCapacityBits = 12}>;
					using CDestinationMap = TCPackedMap<uint32, int32, CStatefulSignedUnsignedIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;

					CSourceMap Source(CCompareConstructTag(), true);
					Source.f_Insert(int32(-1), 10);
					Source.f_Insert(int32(0), 20);

					CDestinationMap Destination(CCompareConstructTag(), false);
					Destination = Source;

					auto Result = Destination.f_Insert(int32(1u), 30);
					DMibExpectTrue(Result.m_bInserted);

					constexpr static uint32 gc_Expected[] = {TCLimitsInt<uint32>::mc_Max, 1u, 0u};
					umint iExpected = 0;
					for (auto Iter = Destination.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(iExpected, <, 3)(ETestFlag_Aggregated);
						DMibExpect(*Iter, ==, gc_Expected[iExpected])(ETestFlag_Aggregated);
						++iExpected;
					}
					DMibExpect(iExpected, ==, 3);
					fg_ExpectPackedMapValid(Destination, "TemplatedCopyAssignPreservesComparatorWhenKeyTypeDiffers");
				};
				DMibTestCategory("TemplatedMoveAssignPreservesComparatorWhenKeyTypeDiffers")
				{
					using CSourceMap = TCPackedMap<int32, int32, CStatefulSignedUnsignedIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_MaxCapacityBits = 12}>;
					using CDestinationMap = TCPackedMap<uint32, int32, CStatefulSignedUnsignedIntCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;

					CSourceMap Source(CCompareConstructTag(), true);
					Source.f_Insert(int32(-1), 10);
					Source.f_Insert(int32(0), 20);

					CDestinationMap Destination(CCompareConstructTag(), false);
					Destination = fg_Move(Source);

					auto Result = Destination.f_Insert(int32(1u), 30);
					DMibExpectTrue(Result.m_bInserted);

					constexpr static uint32 gc_Expected[] = {TCLimitsInt<uint32>::mc_Max, 1u, 0u};
					umint iExpected = 0;
					for (auto Iter = Destination.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(iExpected, <, 3)(ETestFlag_Aggregated);
						DMibExpect(*Iter, ==, gc_Expected[iExpected])(ETestFlag_Aggregated);
						++iExpected;
					}
					DMibExpect(iExpected, ==, 3);
					fg_ExpectPackedMapValid(Destination, "TemplatedMoveAssignPreservesComparatorWhenKeyTypeDiffers");
				};
			};
			DMibTestSuite("Comparison")
			{
				DMibTestCategory("Equality")
				{
					TCPackedMap<int32, int32> Map1;
					TCPackedMap<int32, int32> Map2;

					for (int32 iTest = 0; iTest < 10; ++iTest)
					{
						Map1.f_Insert(iTest, iTest * 10);
						Map2.f_Insert(iTest, iTest * 10);
					}

					DMibExpectTrue(Map1 == Map2);

					Map2.f_Insert(100, 1000);
					DMibExpectFalse(Map1 == Map2);
				};
				DMibTestCategory("ThreeWay")
				{
					TCPackedMap<int32, int32> Map1;
					TCPackedMap<int32, int32> Map2;

					Map1.f_Insert(1, 10);
					Map1.f_Insert(2, 20);

					Map2.f_Insert(1, 10);
					Map2.f_Insert(3, 30);

					DMibExpectTrue((Map1 <=> Map2) < 0);
					DMibExpectTrue((Map2 <=> Map1) > 0);

					TCPackedMap<int32, int32> Map3 = Map1;
					DMibExpectTrue((Map1 <=> Map3) == 0);
				};
				DMibTestCategory("EmptyMaps")
				{
					TCPackedMap<int32, int32> Map1;
					TCPackedMap<int32, int32> Map2;

					DMibExpectTrue(Map1 == Map2);
					DMibExpectTrue((Map1 <=> Map2) == 0);

					Map1.f_Insert(1, 10);
					DMibExpectFalse(Map1 == Map2);
					DMibExpectTrue((Map1 <=> Map2) > 0);
					DMibExpectTrue((Map2 <=> Map1) < 0);
				};
				DMibTestCategory("EqualityCrossSegment")
				{
					// 500 elements with segment size 64 spans ~8+ segments
					umint const nCount = 500;

					TCPackedMap<int32, int32> Map1;
					TCPackedMap<int32, int32> Map2;

					for (umint iTest = 0; iTest < nCount; ++iTest)
					{
						Map1.f_Insert((int32)iTest, (int32)(iTest * 10));
						Map2.f_Insert((int32)iTest, (int32)(iTest * 10));
					}

					DMibExpectTrue(Map1 == Map2);
					DMibExpectTrue((Map1 <=> Map2) == 0);

					// Differ by one extra key at the end
					Map2.f_Insert((int32)nCount, (int32)(nCount * 10));
					DMibExpectFalse(Map1 == Map2);

					// Differ by value only (same keys), early in the map
					TCPackedMap<int32, int32> Map3;
					for (umint iTest = 0; iTest < nCount; ++iTest)
						Map3.f_Insert((int32)iTest, iTest == 5 ? 999 : (int32)(iTest * 10));

					DMibExpectFalse(Map1 == Map3);

					// Differ by value only, late in the map (last segment)
					TCPackedMap<int32, int32> Map4;
					for (umint iTest = 0; iTest < nCount; ++iTest)
						Map4.f_Insert((int32)iTest, iTest == nCount - 1 ? 999 : (int32)(iTest * 10));

					DMibExpectFalse(Map1 == Map4);
				};
				DMibTestCategory("ThreeWayCrossSegment")
				{
					umint const nCount = 500;

					TCPackedMap<int32, int32> Map1;
					TCPackedMap<int32, int32> Map2;

					for (umint iTest = 0; iTest < nCount; ++iTest)
					{
						Map1.f_Insert((int32)iTest, (int32)(iTest * 10));
						Map2.f_Insert((int32)iTest, (int32)(iTest * 10));
					}

					DMibExpectTrue((Map1 <=> Map2) == 0);

					// Differ by key in the middle
					TCPackedMap<int32, int32> Map3;
					for (umint iTest = 0; iTest < nCount; ++iTest)
					{
						if (iTest == nCount / 2)
							Map3.f_Insert((int32)(iTest + nCount), (int32)(iTest * 10));
						else
							Map3.f_Insert((int32)iTest, (int32)(iTest * 10));
					}

					DMibExpectTrue((Map1 <=> Map3) < 0);
					DMibExpectTrue((Map3 <=> Map1) > 0);

					// Differ by value only (same keys)
					TCPackedMap<int32, int32> Map4;
					for (umint iTest = 0; iTest < nCount; ++iTest)
						Map4.f_Insert((int32)iTest, iTest == nCount / 2 ? (int32)(iTest * 10 + 1) : (int32)(iTest * 10));

					DMibExpectTrue((Map1 <=> Map4) < 0);
					DMibExpectTrue((Map4 <=> Map1) > 0);

					// Different sizes, equal prefix
					TCPackedMap<int32, int32> MapShort;
					for (umint iTest = 0; iTest < nCount / 2; ++iTest)
						MapShort.f_Insert((int32)iTest, (int32)(iTest * 10));

					DMibExpectTrue((MapShort <=> Map1) < 0);
					DMibExpectTrue((Map1 <=> MapShort) > 0);
				};
				DMibTestCategory("Lexicographical")
				{
					// Empty maps
					{
						TCPackedMap<int32, int32> Empty1;
						TCPackedMap<int32, int32> Empty2;
						DMibExpectTrue(Empty1.f_CompareLexicographical(Empty2) == 0);
					}

					// Empty vs non-empty
					{
						TCPackedMap<int32, int32> NonEmpty;
						TCPackedMap<int32, int32> Empty;
						NonEmpty.f_Insert(1, 10);
						DMibExpectTrue(NonEmpty.f_CompareLexicographical(Empty) > 0);
						DMibExpectTrue(Empty.f_CompareLexicographical(NonEmpty) < 0);
					}

					// Equal small maps
					{
						TCPackedMap<int32, int32> MapA;
						TCPackedMap<int32, int32> MapB;
						MapA.f_Insert(1, 10);
						MapB.f_Insert(1, 10);
						DMibExpectTrue(MapA.f_CompareLexicographical(MapB) == 0);
					}

					// Differ by key
					{
						TCPackedMap<int32, int32> MapLow;
						TCPackedMap<int32, int32> MapHigh;
						MapLow.f_Insert(1, 10);
						MapHigh.f_Insert(2, 10);
						DMibExpectTrue(MapLow.f_CompareLexicographical(MapHigh) < 0);
						DMibExpectTrue(MapHigh.f_CompareLexicographical(MapLow) > 0);
					}

					// Differ by value (same key)
					{
						TCPackedMap<int32, int32> MapSmallVal;
						TCPackedMap<int32, int32> MapLargeVal;
						MapSmallVal.f_Insert(1, 10);
						MapLargeVal.f_Insert(1, 20);
						DMibExpectTrue(MapSmallVal.f_CompareLexicographical(MapLargeVal) < 0);
						DMibExpectTrue(MapLargeVal.f_CompareLexicographical(MapSmallVal) > 0);
					}
				};
				DMibTestCategory("LexicographicalCrossSegment")
				{
					umint const nCount = 500;

					TCPackedMap<int32, int32> Map1;
					TCPackedMap<int32, int32> Map2;

					for (umint iTest = 0; iTest < nCount; ++iTest)
					{
						Map1.f_Insert((int32)iTest, (int32)(iTest * 10));
						Map2.f_Insert((int32)iTest, (int32)(iTest * 10));
					}

					// Equal
					DMibExpectTrue(Map1.f_CompareLexicographical(Map2) == 0);

					// Differ by key in the middle
					TCPackedMap<int32, int32> Map3;
					for (umint iTest = 0; iTest < nCount; ++iTest)
					{
						if (iTest == nCount / 2)
							Map3.f_Insert((int32)(iTest + nCount), (int32)(iTest * 10));
						else
							Map3.f_Insert((int32)iTest, (int32)(iTest * 10));
					}

					DMibExpectTrue(Map1.f_CompareLexicographical(Map3) < 0);
					DMibExpectTrue(Map3.f_CompareLexicographical(Map1) > 0);

					// Differ by value only (same keys)
					TCPackedMap<int32, int32> Map4;
					for (umint iTest = 0; iTest < nCount; ++iTest)
						Map4.f_Insert((int32)iTest, iTest == nCount / 2 ? (int32)(iTest * 10 + 1) : (int32)(iTest * 10));

					DMibExpectTrue(Map1.f_CompareLexicographical(Map4) < 0);
					DMibExpectTrue(Map4.f_CompareLexicographical(Map1) > 0);

					// Different sizes, equal prefix (shorter is less)
					TCPackedMap<int32, int32> MapShort;
					for (umint iTest = 0; iTest < nCount / 2; ++iTest)
						MapShort.f_Insert((int32)iTest, (int32)(iTest * 10));

					DMibExpectTrue(MapShort.f_CompareLexicographical(Map1) < 0);
					DMibExpectTrue(Map1.f_CompareLexicographical(MapShort) > 0);

					// Differ by value at the end (last segment): Map1 has 4990, Map5 has 999
					TCPackedMap<int32, int32> Map5;
					for (umint iTest = 0; iTest < nCount; ++iTest)
						Map5.f_Insert((int32)iTest, iTest == nCount - 1 ? 999 : (int32)(iTest * 10));

					DMibExpectTrue(Map1.f_CompareLexicographical(Map5) > 0);
					DMibExpectTrue(Map5.f_CompareLexicographical(Map1) < 0);
				};
				DMibTestCategory("ThrowingKeyComparisonsPropagate")
				{
					CThrowingComparisonMap Map1;
					CThrowingComparisonMap Map2;

					Map1.f_Insert(CThrowingComparisonKey(1), CThrowingComparisonValue(10));
					Map2.f_Insert(CThrowingComparisonKey(1), CThrowingComparisonValue(10));

					CThrowingComparisonKey::fs_Reset(true, false);
					CThrowingComparisonValue::fs_Reset();
					DMibExpectExceptionType((void)(Map1 == Map2), CException);

					CThrowingComparisonKey::fs_Reset(false, true);
					CThrowingComparisonValue::fs_Reset();
					DMibExpectExceptionType((void)(Map1 <=> Map2), CException);
					DMibExpectExceptionType((void)Map1.f_CompareLexicographical(Map2), CException);

					CThrowingComparisonKey::fs_Reset();
					CThrowingComparisonValue::fs_Reset();
				};
				DMibTestCategory("ThrowingValueComparisonsPropagate")
				{
					CThrowingComparisonMap Map1;
					CThrowingComparisonMap Map2;

					Map1.f_Insert(CThrowingComparisonKey(1), CThrowingComparisonValue(10));
					Map2.f_Insert(CThrowingComparisonKey(1), CThrowingComparisonValue(10));

					CThrowingComparisonKey::fs_Reset();
					CThrowingComparisonValue::fs_Reset(true, false);
					DMibExpectExceptionType((void)(Map1 == Map2), CException);

					CThrowingComparisonKey::fs_Reset();
					CThrowingComparisonValue::fs_Reset(false, true);
					DMibExpectExceptionType((void)(Map1 <=> Map2), CException);
					DMibExpectExceptionType((void)Map1.f_CompareLexicographical(Map2), CException);

					CThrowingComparisonKey::fs_Reset();
					CThrowingComparisonValue::fs_Reset();
				};
			};
			DMibTestSuite("Stress")
			{
				DMibTestCategory("ManyInserts")
				{
					TCPackedMap<int32, int32> Map;

					// Insert many elements
					umint const nCount = 1000;
					for (umint iTest = 0; iTest < nCount; ++iTest)
						Map.f_Insert((int32)iTest, (int32)(iTest * 10));

					DMibExpect(Map.f_GetLen(), ==, nCount);

					// Verify all elements are present and sorted
					umint iExpected = 0;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, ==, (int32)iExpected)(ETestFlag_Aggregated);
						++iExpected;
					}
					DMibExpect(iExpected, ==, nCount);
				};
				DMibTestCategory("RandomOrder")
				{
					TCPackedMap<int32, int32> Map;

					// Insert in "random" order (reverse, then forward)
					for (int32 iTest = 99; iTest >= 0; --iTest)
						Map.f_Insert(iTest, iTest * 10);

					DMibExpect(Map.f_GetLen(), ==, 100);

					// Verify sorted order
					int32 iExpected = 0;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, ==, iExpected)(ETestFlag_Aggregated);
						++iExpected;
					}
				};
				DMibTestCategory("InsertRemove")
				{
					TCPackedMap<int32, int32> Map;

					// Insert elements
					for (int32 iTest = 0; iTest < 100; ++iTest)
						Map.f_Insert(iTest, iTest * 10);

					// Remove half
					for (int32 iTest = 0; iTest < 100; iTest += 2)
						Map.f_Remove(iTest);

					DMibExpect(Map.f_GetLen(), ==, 50);

					// Verify remaining elements
					for (int32 iTest = 1; iTest < 100; iTest += 2)
					{
						auto *pValue = Map.f_FindEqual(iTest);
						DMibAssertTrue(pValue)(ETestFlag_Aggregated);
						DMibExpect(*pValue, ==, iTest * 10)(ETestFlag_Aggregated);
					}
				};
			};
			DMibTestSuite("StringKeys")
			{
				DMibTestCategory("Basic")
				{
					TCPackedMap<CStr, CStr> Map;

					Map["Apple"] = "Red";
					Map["Banana"] = "Yellow";
					Map["Grape"] = "Purple";

					DMibExpect(Map.f_GetLen(), ==, 3);
					DMibExpect(*Map.f_FindEqual("Apple"), ==, CStr("Red"));
					DMibExpect(*Map.f_FindEqual("Banana"), ==, CStr("Yellow"));
					DMibExpect(*Map.f_FindEqual("Grape"), ==, CStr("Purple"));
				};
				DMibTestCategory("SortedOrder")
				{
					TCPackedMap<CStr, int32> Map;

					Map["Zebra"] = 1;
					Map["Apple"] = 2;
					Map["Mango"] = 3;

					TCVector<CStr> Keys;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
						Keys.f_InsertLast(*Iter);

					DMibExpect(Keys[0], ==, CStr("Apple"));
					DMibExpect(Keys[1], ==, CStr("Mango"));
					DMibExpect(Keys[2], ==, CStr("Zebra"));
				};
			};
			DMibTestSuite("Options")
			{
				DMibTestCategory("CustomSegmentSize")
				{
					TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 32}> Map;

					for (int32 iTest = 0; iTest < 100; ++iTest)
						Map.f_Insert(iTest, iTest * 10);

					DMibExpect(Map.f_GetLen(), ==, 100);

					// Verify sorted order
					int32 iExpected = 0;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, ==, iExpected)(ETestFlag_Aggregated);
						++iExpected;
					}
				};
				DMibTestCategory("LargeSegmentSize")
				{
					TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 128}> Map;

					for (int32 iTest = 0; iTest < 500; ++iTest)
						Map.f_Insert(iTest, iTest * 10);

					DMibExpect(Map.f_GetLen(), ==, 500);
				};
			};
			DMibTestSuite("RMAPaper")
			{
				DMibTestCategory("SkewedWorkloadLeft")
				{
					// Test sequential insertions at the same location (left edge)
					// This is an adversarial workload that tests adaptive gap placement
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;
					Map.f_ResetStats();

					// Insert 500 elements at the left edge (always inserting smallest key)
					for (int32 iTest = 500; iTest > 0; --iTest)
						Map.f_Insert(iTest, iTest);

					DMibExpect(Map.f_GetLen(), ==, 500);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Validation failed: {}") << ValidateError);

					// Verify sorted order is maintained
					int32 nPrev = 0;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("SkewedWorkloadRight")
				{
					// Test sequential insertions at the right edge
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;
					Map.f_ResetStats();

					// Insert 500 elements at the right edge (always inserting largest key)
					for (int32 iTest = 0; iTest < 500; ++iTest)
						Map.f_Insert(iTest, iTest);

					DMibExpect(Map.f_GetLen(), ==, 500);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Validation failed: {}") << ValidateError);
				};
				DMibTestCategory("SkewedWorkloadMiddle")
				{
					// Test insertions alternating around a hotspot in the middle
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;
					Map.f_ResetStats();

					// Create a hotspot pattern: insert around a central key
					umint const nCenter = 500;
					for (int32 iTest = 0; iTest < 500; ++iTest)
					{
						// Alternating +1/-1 pattern around center to create directional drift
						int32 nKey = (int32)nCenter + (iTest % 2 == 0 ? iTest / 2 : -(iTest / 2 + 1));
						Map.f_Insert(nKey, nKey);
					}

					DMibExpect(Map.f_GetLen(), ==, 500);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Validation failed: {}") << ValidateError);

					// Verify sorted order
					int32 nPrev = -1000000;  // Sufficiently small for test data range
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("ComplexityBoundRandom")
				{
					// Verify that element moves grow sub-quadratically (should be O(n log² n))
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true, .m_DetectorQueueSize = 128}>;

					CSecureRandom Random;
					Random.f_InsecureDeterministicReseed(0x12345678u);

					// Measure for N=500
					umint nMovesSmall;
					{
						DMibTestPath("N500");
						CPackedMap Map;
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 500; ++iTest)
							Map.f_Insert(Random.f_GetValue<int32>(), iTest);

						nMovesSmall = fg_PrintPackedMapStats("Random insert nMovesSmall(500)", Map.f_GetStats());
					}

					// Measure for N=2000
					umint nMovesLarge;
					{
						DMibTestPath("N2000");
						CPackedMap Map;
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 2000; ++iTest)
							Map.f_Insert(Random.f_GetValue<int32>(), iTest);

						nMovesLarge = fg_PrintPackedMapStats("Random insert nMovesLarge(2 000)", Map.f_GetStats());
					}

					// For O(n log² n), when n grows 4x:
					// Moves should grow by approximately 4 * (log(4n))² / (log(n))²
					// Which is approximately 4 * ((log(n) + 2) / log(n))² ≈ 4 to 6x
					// For quadratic O(n²), it would be 16x
					// We check that it's less than 10x to have margin
					DMibExpect(fp64(nMovesLarge) / fp64(nMovesSmall), <, 10.0);

					// Measure for N=2 000 000 (huge) - debug only
					if constexpr (gc_bDoHugeTests)
					{
						DMibTestPath("N2000000");
						CPackedMap Map;
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 2'000'000; ++iTest)
							Map.f_Insert(Random.f_GetValue<int32>(), iTest);

						fg_PrintPackedMapStats("Random insert nMovesLargeHuge(2 000 000)", Map.f_GetStats());
					}

				};
				DMibTestCategory("ComplexityBound")
				{
					// Verify that element moves grow sub-quadratically (should be O(n log² n))
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true, .m_DetectorQueueSize = 128}>;

					// Measure for N=500
					umint nMovesSmall;
					{
						DMibTestPath("N500");
						CPackedMap Map;

						for (int32 iTest = 0; iTest < 500; ++iTest)
							Map.f_Insert(iTest, iTest);

						nMovesSmall = fg_PrintPackedMapStats("Insert nMovesSmall(500)", Map.f_GetStats());
					}

					// Measure for N=2000
					umint nMovesLarge;
					{
						DMibTestPath("N2000");
						CPackedMap Map;
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 2000; ++iTest)
							Map.f_Insert(iTest, iTest);

						nMovesLarge = fg_PrintPackedMapStats("Insert nMovesLarge(2 000)", Map.f_GetStats());
					}

					// For O(n log² n), when n grows 4x:
					// Moves should grow by approximately 4 * (log(4n))² / (log(n))²
					// Which is approximately 4 * ((log(n) + 2) / log(n))² ≈ 4 to 6x
					// For quadratic O(n²), it would be 16x
					// We check that it's less than 10x to have margin
					DMibExpect(fp64(nMovesLarge) / fp64(nMovesSmall), <, 10.0);

					// Measure for N=2 000 000 (huge) - debug only
					if constexpr (gc_bDoHugeTests)
					{
						DMibTestPath("N2000000");
						CPackedMap Map;
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 2'000'000; ++iTest)
							Map.f_Insert(iTest, iTest);

						fg_PrintPackedMapStats("Insert nMovesLargeHuge(2 000 000)", Map.f_GetStats());
					}
				};
				DMibTestCategory("ComplexityBoundEraseRandom")
				{
					// Verify that element moves during random erasure grow sub-quadratically (should be O(n log² n))
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true, .m_DetectorQueueSize = 128}>;

					CSecureRandom Random;
					Random.f_InsecureDeterministicReseed(0xDEADBEEFu);

					auto fShuffledKeys = [&Random](umint _nCount) -> TCVector<int32>
						{
							TCVector<int32> Keys;
							for (umint iTest = 0; iTest < _nCount; ++iTest)
								Keys.f_InsertLast((int32)iTest);
							for (umint iTest = _nCount - 1; iTest > 0; --iTest)
							{
								umint iRandom = (umint)(Random.f_GetValue<uint32>() % (uint32)(iTest + 1));
								int32 Tmp = Keys[iTest];
								Keys[iTest] = Keys[iRandom];
								Keys[iRandom] = Tmp;
							}
							return Keys;
						}
					;

					// Measure for N=500
					umint nMovesSmall;
					{
						DMibTestPath("N500");
						CPackedMap Map;
						for (int32 iTest = 0; iTest < 500; ++iTest)
							Map.f_Insert(iTest, iTest);
						auto Keys = fShuffledKeys(500);
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 500; ++iTest)
							Map.f_Remove(Keys[iTest]);

						nMovesSmall = fg_PrintPackedMapStats("Random remove nMovesSmall(500)", Map.f_GetStats());
					}

					// Measure for N=2000
					umint nMovesLarge;
					{
						DMibTestPath("N2000");
						CPackedMap Map;
						for (int32 iTest = 0; iTest < 2000; ++iTest)
							Map.f_Insert(iTest, iTest);
						auto Keys = fShuffledKeys(2000);
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 2000; ++iTest)
							Map.f_Remove(Keys[iTest]);

						nMovesLarge = fg_PrintPackedMapStats("Random remove nMovesLarge(2 000)", Map.f_GetStats());
					}

					// For O(n log² n), when n grows 4x:
					// Moves should grow by approximately 4 * (log(4n))² / (log(n))²
					// For quadratic O(n²), it would be 16x
					// We check that it's less than 10x to have margin
					DMibExpect(fp64(nMovesLarge) / fp64(nMovesSmall), <, 10.0);

					// Measure for N=2 000 000 (huge)
					if constexpr (gc_bDoHugeTests)
					{
						DMibTestPath("N2000000");
						CPackedMap Map;
						for (int32 iTest = 0; iTest < 2'000'000; ++iTest)
							Map.f_Insert(iTest, iTest);

						auto Keys = fShuffledKeys(2'000'000);
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 2'000'000; ++iTest)
							Map.f_Remove(Keys[iTest]);

						fg_PrintPackedMapStats("Random remove nMovesLargeHuge(2 000 000)", Map.f_GetStats());
					}
				};
				DMibTestCategory("ComplexityBoundErase")
				{
					// Verify that element moves during sequential erasure grow sub-quadratically (should be O(n log² n))
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true, .m_DetectorQueueSize = 128}>;

					// Measure for N=500
					umint nMovesSmall;
					{
						DMibTestPath("N500");
						CPackedMap Map;
						for (int32 iTest = 0; iTest < 500; ++iTest)
							Map.f_Insert(iTest, iTest);
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 500; ++iTest)
							Map.f_Remove(iTest);

						nMovesSmall = fg_PrintPackedMapStats("Remove nMovesSmall(500)", Map.f_GetStats());
					}

					// Measure for N=2000
					umint nMovesLarge;
					{
						DMibTestPath("N2000");
						CPackedMap Map;
						for (int32 iTest = 0; iTest < 2000; ++iTest)
							Map.f_Insert(iTest, iTest);
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 2000; ++iTest)
							Map.f_Remove(iTest);

						nMovesLarge = fg_PrintPackedMapStats("Remove nMovesLarge(2 000)", Map.f_GetStats());
					}

					// For O(n log² n), when n grows 4x:
					// Moves should grow by approximately 4 * (log(4n))² / (log(n))²
					// For quadratic O(n²), it would be 16x
					// We check that it's less than 10x to have margin
					DMibExpect(fp64(nMovesLarge) / fp64(nMovesSmall), <, 10.0);

					// Measure for N=2 000 000 (huge)
					if constexpr (gc_bDoHugeTests)
					{
						DMibTestPath("N2000000");
						CPackedMap Map;
						for (int32 iTest = 0; iTest < 2'000'000; ++iTest)
							Map.f_Insert(iTest, iTest);
						Map.f_ResetStats();
						for (int32 iTest = 0; iTest < 2'000'000; ++iTest)
							Map.f_Remove(iTest);

						fg_PrintPackedMapStats("Remove nMovesLargeHuge(2 000 000)", Map.f_GetStats());
					}
				};
				DMibTestCategory("DensityInvariant")
				{
					// Verify density invariants are maintained after operations
					using CPackedMap = TCPackedMap<int32, int32>;

					CPackedMap Map;

					// Insert enough elements to trigger multiple rebalances
					for (int32 iTest = 0; iTest < 1000; ++iTest)
						Map.f_Insert(iTest, iTest);

					// Validation includes density checks
					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Validation failed: {}") << ValidateError);

					// Remove some elements
					for (int32 iTest = 0; iTest < 500; iTest += 2)
						Map.f_Remove(iTest);

					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Validation after remove failed: {}") << ValidateError);
				};
			};
			DMibTestSuite("ThresholdProfiles")
			{
				DMibTestCategory("DefaultProfile")
				{
					// Test with default thresholds
					TCPackedMap<int32, int32> Map;

					// Insert 1000 elements, verify invariants
					for (int32 iTest = 0; iTest < 1000; ++iTest)
						Map.f_Insert(iTest, iTest);

					DMibExpect(Map.f_GetLen(), ==, 1000);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Default profile validation failed: {}") << ValidateError);

					// Verify sorted order
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("ScanOrientedProfile")
				{
					// Test with paper's scan-oriented thresholds
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, fg_PackedMapOptionsScanOriented()>;

					CPackedMap Map;

					// Insert elements
					for (int32 iTest = 0; iTest < 1000; ++iTest)
						Map.f_Insert(iTest, iTest);

					DMibExpect(Map.f_GetLen(), ==, 1000);

					// Verify sorted order
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
			};
			DMibTestSuite("LowerBoundEnforcement")
			{
				DMibTestCategory("DeleteTriggersRebalance")
				{
					// Insert elements, delete many, verify density invariants
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;
					Map.f_ResetStats();

					// Insert elements
					for (int32 iTest = 0; iTest < 500; ++iTest)
						Map.f_Insert(iTest, iTest);

					// Delete many elements (should trigger lower-bound rebalancing)
					for (int32 iTest = 0; iTest < 400; iTest += 2)
						Map.f_Remove(iTest);

					// Verify invariants after deletion
					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Delete rebalance validation failed: {}") << ValidateError);

					// Verify sorted order is maintained
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("ScanOrientedNoLowerBound")
				{
					// With rho1=0, deletions should not trigger lower-bound rebalance at leaf level
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, fg_PackedMapOptionsScanOriented()>;

					CPackedMap Map;

					// Insert elements
					for (int32 iTest = 0; iTest < 500; ++iTest)
						Map.f_Insert(iTest, iTest);

					// Delete many elements
					for (int32 iTest = 0; iTest < 400; iTest += 2)
						Map.f_Remove(iTest);

					// Verify elements are still accessible
					DMibExpect(Map.f_GetLen(), ==, 300);

					// Verify sorted order
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
			};
			DMibTestSuite("AdaptiveAlgorithm")
			{
				DMibTestCategory("HotIntervalDetection")
				{
					// Test that hot intervals are detected after repeated insertions at same location
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;
					Map.f_ResetStats();

					// Create a hotspot by inserting many elements at the same location
					// Insert base elements first
					for (int32 iTest = 0; iTest < 100; ++iTest)
						Map.f_Insert(iTest * 100, iTest);

					// Now insert many elements around a single location (hotspot)
					int32 nHotspotBase = 10000;
					for (int32 iTest = 0; iTest < 200; ++iTest)
						Map.f_Insert(nHotspotBase + iTest, iTest);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Hot interval validation failed: {}") << ValidateError);

					// Verify all elements present and sorted
					DMibExpect(Map.f_GetLen(), ==, 300);
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("DirectionalDrift")
				{
					// Test directional drift detection - repeated insertions in one direction
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					{
						DMibTestPath("Forward");
						CPackedMap Map;

						// Insert elements with consistent backward drift (decreasing keys)
						for (int32 iTest = 1000; iTest >= 0; --iTest)
							Map.f_Insert(iTest, iTest);

						NStr::CStr ValidateError;
						if (!Map.f_Debug_Validate(&ValidateError))
							DMibError(NStr::CStr::CFormat("Backward drift validation failed: {}") << ValidateError);

						DMibExpect(Map.f_GetLen(), ==, 1001);
					}

					{
						DMibTestPath("Backward");
						CPackedMap Map;

						// Insert elements with consistent forward drift (increasing keys)
						for (int32 iTest = 0; iTest <= 1000; ++iTest)
							Map.f_Insert(iTest, iTest);

						NStr::CStr ValidateError;
						if (!Map.f_Debug_Validate(&ValidateError))
							DMibError(NStr::CStr::CFormat("Forward drift validation failed: {}") << ValidateError);

						DMibExpect(Map.f_GetLen(), ==, 1001);
					}
				};
				DMibTestCategory("SignedCounterStability")
				{
					// Test that signed counter (sc) handles mixed insert/delete sequences
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;

					// Phase 1: Insert elements
					for (int32 iTest = 0; iTest < 500; ++iTest)
						Map.f_Insert(iTest, iTest);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("After initial inserts: {}") << ValidateError);

					// Phase 2: Delete half the elements
					for (int32 iTest = 0; iTest < 500; iTest += 2)
						Map.f_Remove(iTest);

					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("After deletions: {}") << ValidateError);

					DMibExpect(Map.f_GetLen(), ==, 250);

					// Phase 3: Re-insert elements (should trigger sc to go positive again)
					for (int32 iTest = 0; iTest < 500; iTest += 2)
						Map.f_Insert(iTest, iTest * 10);

					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("After re-inserts: {}") << ValidateError);

					DMibExpect(Map.f_GetLen(), ==, 500);

					// Verify all elements present and sorted
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("DensityBoundsAllLevels")
				{
					// Test that density bounds are maintained at all calibrator tree levels
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;

					// Insert enough elements to create multiple calibrator tree levels
					for (int32 iTest = 0; iTest < 2000; ++iTest)
						Map.f_Insert(iTest, iTest);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Initial density bounds: {}") << ValidateError);

					// Delete in a pattern that might violate density bounds
					for (int32 iTest = 0; iTest < 1500; iTest += 3)
						Map.f_Remove(iTest);

					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Post-delete density bounds: {}") << ValidateError);

					// Re-insert to stress the adaptive algorithm
					for (int32 iTest = 2000; iTest < 3000; ++iTest)
						Map.f_Insert(iTest, iTest);

					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Post-reinsert density bounds: {}") << ValidateError);
				};
				DMibTestCategory("AlternatingHotspots")
				{
					// Test with alternating hotspot locations
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;

					// Create initial structure
					for (int32 iTest = 0; iTest < 100; ++iTest)
						Map.f_Insert(iTest * 1000, iTest);

					// Alternate between two hotspot locations
					for (int32 iTest = 0; iTest < 500; ++iTest)
					{
						if (iTest % 2 == 0)
							Map.f_Insert(10000 + iTest, iTest);  // Hotspot A
						else
							Map.f_Insert(50000 + iTest, iTest);  // Hotspot B
					}

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Alternating hotspots validation: {}") << ValidateError);

					// Verify all elements present and sorted
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("MixedOperationsStress")
				{
					// Stress test with random-like mixed operations
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_bStats = true}>;

					CPackedMap Map;
					CSecureRandom Random;
					Random.f_InsecureDeterministicReseed(0xDEADBEEFu);

					// Perform many mixed operations
					for (int32 iTest = 0; iTest < 2000; ++iTest)
					{
						int32 nKey = (int32)(Random.f_GetValue<uint32>() % 5000);
						int32 nOp = (int32)(Random.f_GetValue<uint32>() % 10);

						if (nOp < 7)  // 70% inserts
							Map.f_Insert(nKey, nKey);
						else  // 30% deletes
							Map.f_Remove(nKey);

						// Periodic validation
						if (iTest % 500 == 0)
						{
							NStr::CStr ValidateError;
							if (!Map.f_Debug_Validate(&ValidateError))
								DMibError(NStr::CStr::CFormat("Mixed ops validation at {}: {}") << iTest << ValidateError);
						}
					}

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Final mixed ops validation: {}") << ValidateError);

					// Verify sorted order
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("SmallSegmentAdaptive")
				{
					// Test adaptive algorithm with small segment size
					using CPackedMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 32, .m_bStats = true}>;

					CPackedMap Map;

					// Create hotspots with small segments
					for (int32 iTest = 0; iTest < 500; ++iTest)
						Map.f_Insert(iTest, iTest);

					// Delete from middle to create gaps
					for (int32 iTest = 200; iTest < 300; ++iTest)
						Map.f_Remove(iTest);

					// Re-insert at hotspot
					for (int32 iTest = 200; iTest < 300; ++iTest)
						Map.f_Insert(iTest, iTest * 2);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Small segment adaptive: {}") << ValidateError);

					DMibExpect(Map.f_GetLen(), ==, 500);
				};
			};
			DMibTestSuite("BulkLoad")
			{
				DMibTestCategory("Empty")
				{
					TCPackedMap<int32, int32> Map;
					Map.f_BulkLoad
						(
							0
							, [](auto &&_fInsert)
							{
							}
						)
					;
					DMibExpect(Map.f_GetLen(), ==, 0);
					DMibExpectTrue(Map.f_IsEmpty());
				};
				DMibTestCategory("SingleElement")
				{
					TCPackedMap<int32, int32> Map;
					Map.f_BulkLoad
						(
							1
							, [](auto &&_fInsert)
							{
								_fInsert(42, 420);
							}
						)
					;
					DMibExpect(Map.f_GetLen(), ==, 1);
					auto *pValue = Map.f_FindEqual(42);
					DMibAssertTrue(pValue);
					DMibExpect(*pValue, ==, 420);
				};
				DMibTestCategory("SortedSequence")
				{
					umint const nCount = 500;
					TCPackedMap<int32, int32> Map;
					Map.f_BulkLoad
						(
							nCount
							, [&](auto &&_fInsert)
							{
								for (umint iTest = 0; iTest < nCount; ++iTest)
									_fInsert((int32)iTest, (int32)(iTest * 10));
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, nCount);

					// Verify all elements present with correct values
					for (umint iTest = 0; iTest < nCount; ++iTest)
					{
						DMibTestPath("{}"_f << iTest);
						auto *pValue = Map.f_FindEqual((int32)iTest);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, (int32)(iTest * 10))(ETestFlag_Aggregated);
					}

					// Verify sorted iteration
					int32 nPrev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, nPrev)(ETestFlag_Aggregated);
						nPrev = *Iter;
					}
				};
				DMibTestCategory("Validate")
				{
					// Test with enough elements to span multiple segments and calibrator levels
					umint const nCount = 2000;
					TCPackedMap<int32, int32> Map;
					Map.f_BulkLoad
						(
							nCount
							, [&](auto &&_fInsert)
							{
								for (umint iTest = 0; iTest < nCount; ++iTest)
									_fInsert((int32)iTest, (int32)iTest);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, nCount);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("BulkLoad validation failed: {}") << ValidateError);
				};
				DMibTestCategory("OverwriteExisting")
				{
					// BulkLoad on a non-empty map should clear and reload
					TCPackedMap<int32, int32> Map;
					Map.f_Insert(100, 1000);
					Map.f_Insert(200, 2000);

					Map.f_BulkLoad
						(
							3
							, [](auto &&_fInsert)
							{
								_fInsert(1, 10);
								_fInsert(2, 20);
								_fInsert(3, 30);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, 3);
					DMibExpectFalse(Map.f_FindEqual(100));
					DMibExpectFalse(Map.f_FindEqual(200));
					DMibAssertTrue(Map.f_FindEqual(1));
					DMibExpect(*Map.f_FindEqual(1), ==, 10);
				};
				DMibTestCategory("MoveSemantics")
				{
					TCPackedMap<CMoveOnlyKey, int32> Map;
					Map.f_BulkLoad
						(
							3
							, [](auto &&_fInsert)
							{
								_fInsert(CMoveOnlyKey(1), 10);
								_fInsert(CMoveOnlyKey(2), 20);
								_fInsert(CMoveOnlyKey(3), 30);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, 3);
					DMibAssertTrue(Map.f_FindEqual(CMoveOnlyKey(2)));
					DMibExpect(*Map.f_FindEqual(CMoveOnlyKey(2)), ==, 20);
				};
				DMibTestCategory("CopySemantics")
				{
					TCPackedMap<int32, CStr> Map;
					CStr Value1("Hello");
					CStr Value2("World");
					CStr Value3("Test");
					Map.f_BulkLoad
						(
							3
							, [&](auto &&_fInsert)
							{
								_fInsert(1, Value1);
								_fInsert(2, Value2);
								_fInsert(3, Value3);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, 3);
					DMibAssertTrue(Map.f_FindEqual(1));
					DMibExpect(*Map.f_FindEqual(1), ==, "Hello");

					// Originals should be unchanged (copied, not moved)
					DMibExpect(Value1, ==, "Hello");
					DMibExpect(Value2, ==, "World");
					DMibExpect(Value3, ==, "Test");
				};
				DMibTestCategory("StringKeys")
				{
					TCPackedMap<CStr, int32> Map;
					Map.f_BulkLoad
						(
							4
							, [](auto &&_fInsert)
							{
								_fInsert("alpha", 1);
								_fInsert("beta", 2);
								_fInsert("gamma", 3);
								_fInsert("zeta", 4);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, 4);
					DMibAssertTrue(Map.f_FindEqual("beta"));
					DMibExpect(*Map.f_FindEqual("beta"), ==, 2);
					DMibAssertTrue(Map.f_FindEqual("zeta"));
					DMibExpect(*Map.f_FindEqual("zeta"), ==, 4);
				};
				DMibTestCategory("LargeScale")
				{
					umint const nCount = 10000;
					TCPackedMap<int32, int32> Map;
					Map.f_BulkLoad
						(
							nCount
							, [&](auto &&_fInsert)
							{
								for (umint iTest = 0; iTest < nCount; ++iTest)
									_fInsert((int32)iTest, (int32)(iTest * 3));
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, nCount);

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("BulkLoad large scale validation failed: {}") << ValidateError);

					// Spot check values
					DMibExpect(*Map.f_FindEqual(0), ==, 0);
					DMibExpect(*Map.f_FindEqual(5000), ==, 15000);
					DMibExpect(*Map.f_FindEqual(9999), ==, 29997);
				};
				DMibTestCategory("StatefulAllocatorScratchStorage")
				{
					using CMemoryManagerParams = NMemory::TCMemoryManagerParams<>;
					using CMemoryManager = NMemory::TCMemoryManager<CMemoryManagerParams>;
					using CStatefulAllocator = NMemory::TCAllocator_MemoryManager<CMemoryManagerParams>;
					using CMap = TCPackedMap<int32, int32, CSort_Default, CStatefulAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_MaxCapacityBits = 12}>;

					CMemoryManager MemoryManager{NMemory::CMemoryManagerConfig()};
					CMap Map(CAllocatorConstructTag(), &MemoryManager);
					Map.f_BulkLoad
						(
							3
							, [](auto &&_fInsert)
							{
								_fInsert(1, 10);
								_fInsert(2, 20);
								_fInsert(3, 30);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, 3);
					DMibExpect(*Map.f_FindEqual(2), ==, 20);

					Map.f_BulkInsert
						(
							2
							, [](auto &&_fInsert)
							{
								_fInsert(4, 40);
								_fInsert(5, 50);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, 5);
					DMibExpect(*Map.f_FindEqual(5), ==, 50);
					fg_ExpectPackedMapValid(Map, "StatefulAllocatorScratchStorage");
				};
				DMibTestCategory("InsertAfterBulkLoad")
				{
					// Verify the map is fully functional after bulk load
					TCPackedMap<int32, int32> Map;
					Map.f_BulkLoad
						(
							5
							, [](auto &&_fInsert)
							{
								_fInsert(10, 100);
								_fInsert(20, 200);
								_fInsert(30, 300);
								_fInsert(40, 400);
								_fInsert(50, 500);
							}
						)
					;

					// Insert between existing elements
					auto Result = Map.f_Insert(25, 250);
					DMibExpectTrue(Result.m_bInserted);
					DMibExpect(Map.f_GetLen(), ==, 6);
					DMibExpect(*Map.f_FindEqual(25), ==, 250);

					// Remove and re-insert
					DMibExpectTrue(Map.f_Remove(30));
					DMibExpect(Map.f_GetLen(), ==, 5);
					DMibExpectFalse(Map.f_FindEqual(30));

					NStr::CStr ValidateError;
					if (!Map.f_Debug_Validate(&ValidateError))
						DMibError(NStr::CStr::CFormat("Post-BulkLoad operations validation: {}") << ValidateError);
				};
#ifdef DMibContractConfigure_CheckEnabled
				DMibTestCategory("ContractUnsorted")
				{
					// Keys not in sorted order should trigger contract violation
					TCPackedMap<int32, int32> Map;
					DMibExpectViolatesCheck
						(
							Map.f_BulkLoad
							(
								3
								, [](auto &&_fInsert)
								{
									_fInsert(10, 100);
									_fInsert(5, 50);  // Out of order
									_fInsert(20, 200);
								}
							)
							, "mp_Compare(pKeys[iPrevKeySlot], _Key) < 0"
						)
					;
				};
				DMibTestCategory("ContractInsertCanonicalOrder")
				{
					TCPackedMap<uint32, int32, CSignedUnsignedIntCompare> Map;
					Map.f_Insert(uint32(0u), 0);

					DMibExpectViolatesCheck
						(
							Map.f_Insert(int32(-1), 10)
							, "mp_Compare(pKeys[iInsertSlot], *pNextKey) < 0"
						)
					;
				};
				DMibTestCategory("ContractDuplicate")
				{
					// Duplicate keys should trigger contract violation
					TCPackedMap<int32, int32> Map;
					DMibExpectViolatesCheck
						(
							Map.f_BulkLoad
							(
								3
								, [](auto &&_fInsert)
								{
									_fInsert(10, 100);
									_fInsert(10, 200);  // Duplicate
									_fInsert(20, 300);
								}
							)
							, "mp_Compare(pKeys[iPrevKeySlot], _Key) < 0"
						)
					;
				};
				DMibTestCategory("ContractTooFewInserts")
				{
					// Inserting fewer elements than declared should trigger contract violation
					TCPackedMap<int32, int32> Map;
					DMibExpectViolatesCheck
						(
							Map.f_BulkLoad
							(
								5
								, [](auto &&_fInsert)
								{
									_fInsert(1, 10);
									_fInsert(2, 20);
									// Only 2 of 5 declared
								}
							)
							, "InsertCount == nTotalElements"
						)
					;
				};
				DMibTestCategory("ContractTooManyInserts")
				{
					// Inserting more elements than declared should trigger contract violation
					TCPackedMap<int32, int32> Map;
					DMibExpectViolatesCheck
						(
							Map.f_BulkLoad
							(
								2
								, [](auto &&_fInsert)
								{
									_fInsert(1, 10);
									_fInsert(2, 20);
									_fInsert(3, 30);  // Extra insert beyond declared count
								}
							)
							, "iSeg < nSegments"
						)
					;
				};
#endif
			};
			DMibTestSuite("Stream")
			{
				DMibTestCategory("RejectDuplicateKeysOnConsume")
				{
					CBinaryStreamMemory<> Stream;
					fg_FeedLenToStream(Stream, uint64(3));
					Stream.f_Feed((int32)1);
					Stream.f_Feed((int32)10);
					Stream.f_Feed((int32)1);
					Stream.f_Feed((int32)20);
					Stream.f_Feed((int32)2);
					Stream.f_Feed((int32)30);

					CByteVector RawData = Stream.f_GetVector();
					CBinaryStreamMemoryRef<> Input(RawData);

					TCPackedMap<int32, int32> Map;
					DMibExpectException(Map.f_Consume(Input), DMibErrorInstance("TCPackedMap stream contained a duplicate key"));
				};
				DMibTestCategory("RejectDuplicateKeysRollsBackToEmpty")
				{
					CBinaryStreamMemory<> Stream;
					fg_FeedLenToStream(Stream, uint64(3));
					Stream.f_Feed((int32)1);
					Stream.f_Feed((int32)10);
					Stream.f_Feed((int32)1);
					Stream.f_Feed((int32)20);
					Stream.f_Feed((int32)2);
					Stream.f_Feed((int32)30);

					CByteVector RawData = Stream.f_GetVector();
					CBinaryStreamMemoryRef<> Input(RawData);

					TCPackedMap<int32, int32> Map;
					Map.f_Insert(99, 990);

					DMibExpectException(Map.f_Consume(Input), DMibErrorInstance("TCPackedMap stream contained a duplicate key"));
					DMibExpect(Map.f_GetLen(), ==, 0);
					fg_ExpectPackedMapValid(Map, "RejectDuplicateKeysRollsBackToEmpty");
				};
			};

			DMibTestSuite("Regression")
			{
				DMibTestCategory("KeyedIteratorsStopAtRemainingElements")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(10, 100);
					Map.f_Insert(20, 200);
					Map.f_Insert(30, 300);
					Map.f_Insert(40, 400);

					auto fExpectValueIter = [&](auto _Iter)
						{
							DMibExpect(_Iter.f_GetLen(), ==, 3);
							DMibExpect(*_Iter, ==, 200);
							++_Iter;
							DMibExpect(_Iter.f_GetLen(), ==, 2);
							DMibExpect(*_Iter, ==, 300);
							++_Iter;
							DMibExpect(_Iter.f_GetLen(), ==, 1);
							DMibExpect(*_Iter, ==, 400);
							++_Iter;
							DMibExpectFalse(_Iter);
						}
					;

					auto fExpectKeyIter = [&](auto _Iter)
						{
							DMibExpect(_Iter.f_GetLen(), ==, 3);
							DMibExpect(*_Iter, ==, 20);
							++_Iter;
							DMibExpect(_Iter.f_GetLen(), ==, 2);
							DMibExpect(*_Iter, ==, 30);
							++_Iter;
							DMibExpect(_Iter.f_GetLen(), ==, 1);
							DMibExpect(*_Iter, ==, 40);
							++_Iter;
							DMibExpectFalse(_Iter);
						}
					;

					auto fExpectEntryIter = [&](auto _Iter)
						{
							DMibExpect(_Iter.f_GetLen(), ==, 3);
							{
								auto Ref = *_Iter;
								DMibExpect(Ref.f_Key(), ==, 20);
								DMibExpect(Ref.f_Value(), ==, 200);
							}
							++_Iter;
							DMibExpect(_Iter.f_GetLen(), ==, 2);
							{
								auto Ref = *_Iter;
								DMibExpect(Ref.f_Key(), ==, 30);
								DMibExpect(Ref.f_Value(), ==, 300);
							}
							++_Iter;
							DMibExpect(_Iter.f_GetLen(), ==, 1);
							{
								auto Ref = *_Iter;
								DMibExpect(Ref.f_Key(), ==, 40);
								DMibExpect(Ref.f_Value(), ==, 400);
							}
							++_Iter;
							DMibExpectFalse(_Iter);
						}
					;

					{
						DMibTestPath("Values");
						fExpectValueIter(Map.f_GetIterator(20));
					}
					{
						DMibTestPath("ValuesConst");
						fExpectValueIter(fg_Const(Map).f_GetIterator(20));
					}
					{
						DMibTestPath("Keys");
						fExpectKeyIter(Map.f_Keys().f_GetIterator(20));
					}
					{
						DMibTestPath("KeysConst");
						fExpectKeyIter(fg_Const(Map).f_Keys().f_GetIterator(20));
					}
					{
						DMibTestPath("Entries");
						fExpectEntryIter(Map.f_Entries().f_GetIterator(20));
					}
					{
						DMibTestPath("EntriesConst");
						fExpectEntryIter(fg_Const(Map).f_Entries().f_GetIterator(20));
					}
				};
				DMibTestCategory("KeyedIteratorsCompareByPosition")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(10, 100);
					Map.f_Insert(20, 200);
					Map.f_Insert(30, 300);
					Map.f_Insert(40, 400);

					{
						DMibTestPath("Values");
						auto Iter = Map.f_GetIterator();
						auto Found = Map.f_GetIterator(30);
						{
							DMibTestPath("InitialMismatch");
							DMibExpectFalse(Iter == Found);
						}
						++Iter;
						{
							DMibTestPath("SecondMismatch");
							DMibExpectFalse(Iter == Found);
						}
						++Iter;
						{
							DMibTestPath("Aligned");
							DMibExpectTrue(Iter == Found);
						}
					}
					{
						DMibTestPath("Keys");
						auto Iter = Map.f_Keys().f_GetIterator();
						auto Found = Map.f_Keys().f_GetIterator(30);
						{
							DMibTestPath("InitialMismatch");
							DMibExpectFalse(Iter == Found);
						}
						++Iter;
						{
							DMibTestPath("SecondMismatch");
							DMibExpectFalse(Iter == Found);
						}
						++Iter;
						{
							DMibTestPath("Aligned");
							DMibExpectTrue(Iter == Found);
						}
					}
					{
						DMibTestPath("Entries");
						auto Iter = Map.f_Entries().f_GetIterator();
						auto Found = Map.f_Entries().f_GetIterator(30);
						{
							DMibTestPath("InitialMismatch");
							DMibExpectFalse(Iter == Found);
						}
						++Iter;
						{
							DMibTestPath("SecondMismatch");
							DMibExpectFalse(Iter == Found);
						}
						++Iter;
						{
							DMibTestPath("Aligned");
							DMibExpectTrue(Iter == Found);
						}
					}
				};
				DMibTestCategory("KeyedIteratorsCanMoveBackward")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(10, 100);
					Map.f_Insert(20, 200);
					Map.f_Insert(30, 300);
					Map.f_Insert(40, 400);

					{
						DMibTestPath("Values");
						auto Iter = Map.f_GetIterator(30);
						{
							DMibTestPath("Start");
							DMibExpect(*Iter, ==, 300);
							DMibExpect(Iter.f_GetLen(), ==, 2);
						}
						--Iter;
						{
							DMibTestPath("PrevOne");
							DMibExpect(*Iter, ==, 200);
							DMibExpect(Iter.f_GetLen(), ==, 3);
						}
						--Iter;
						{
							DMibTestPath("PrevTwo");
							DMibExpect(*Iter, ==, 100);
							DMibExpect(Iter.f_GetLen(), ==, 4);
						}
						--Iter;
						{
							DMibTestPath("ClampAtBegin");
							DMibExpect(*Iter, ==, 100);
						}
					}
					{
						DMibTestPath("Keys");
						auto Iter = Map.f_Keys().f_GetIterator(30);
						{
							DMibTestPath("Start");
							DMibExpect(*Iter, ==, 30);
						}
						--Iter;
						{
							DMibTestPath("PrevOne");
							DMibExpect(*Iter, ==, 20);
						}
						--Iter;
						{
							DMibTestPath("PrevTwo");
							DMibExpect(*Iter, ==, 10);
						}
						--Iter;
						{
							DMibTestPath("ClampAtBegin");
							DMibExpect(*Iter, ==, 10);
						}
					}
					{
						DMibTestPath("Entries");
						auto Iter = Map.f_Entries().f_GetIterator(30);
						{
							DMibTestPath("Start");
							auto Ref = *Iter;
							DMibExpect(Ref.f_Key(), ==, 30);
							DMibExpect(Ref.f_Value(), ==, 300);
						}
						--Iter;
						{
							DMibTestPath("PrevOne");
							auto Ref = *Iter;
							DMibExpect(Ref.f_Key(), ==, 20);
							DMibExpect(Ref.f_Value(), ==, 200);
						}
						--Iter;
						{
							DMibTestPath("PrevTwo");
							auto Ref = *Iter;
							DMibExpect(Ref.f_Key(), ==, 10);
							DMibExpect(Ref.f_Value(), ==, 100);
						}
						--Iter;
						{
							DMibTestPath("ClampAtBegin");
							auto Ref = *Iter;
							DMibExpect(Ref.f_Key(), ==, 10);
							DMibExpect(Ref.f_Value(), ==, 100);
						}
					}
				};
				DMibTestCategory("ConstBoundIterators")
				{
					TCPackedMap<int32, int32> Map;

					Map.f_Insert(10, 100);
					Map.f_Insert(20, 200);
					Map.f_Insert(30, 300);
					Map.f_Insert(40, 400);

					auto const &ConstMap = fg_Const(Map);

					{
						DMibTestPath("SmallestGreaterThanEqual");
						TCPackedMap<int32, int32>::CIteratorConst Iter = ConstMap.f_GetIterator_SmallestGreaterThanEqual(15);
						DMibExpectTrue((bool)Iter);
						DMibExpect(*Iter, ==, 200);
					}
					{
						DMibTestPath("LargestLessThanEqual");
						TCPackedMap<int32, int32>::CIteratorConst Iter = ConstMap.f_GetIterator_LargestLessThanEqual(25);
						DMibExpectTrue((bool)Iter);
						DMibExpect(*Iter, ==, 200);
					}
				};
				DMibTestCategory("SparseBoundLookupsSkipEmptySegments")
				{
					using CMap = TCPackedMapTestAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = false, .m_MinSegments = 16, .m_MaxCapacityBits = 12}>>;

					CMap Map;
					Map.f_BulkLoad
						(
							4
							, [](auto &&_fInsert)
							{
								_fInsert(10, 100);
								_fInsert(20, 200);
								_fInsert(30, 300);
								_fInsert(40, 400);
							}
						)
					;

					auto *pData = Map.mp_pData;
					DMibAssertTrue(pData);

					umint iPrevNonEmpty = TCLimitsInt<umint>::mc_Max;
					umint iPrevGapSegment = 0;
					umint iNextGapSegment = 0;
					int32 ProbeKey = 0;
					int32 ExpectedLessValue = 0;
					int32 ExpectedGreaterValue = 0;
					bool bFoundGap = false;

					for (umint iSeg = 0; iSeg < pData->m_nSegments; ++iSeg)
					{
						umint Count = pData->m_pSegmentMeta[iSeg].m_Count;
						if (Count == 0)
							continue;

						if (iPrevNonEmpty != TCLimitsInt<umint>::mc_Max && iSeg - iPrevNonEmpty > 1)
						{
							umint PrevCount = pData->m_pSegmentMeta[iPrevNonEmpty].m_Count;
							umint iPrevFirst = CMap::fsp_GetSegmentFirstSlot(iPrevNonEmpty, PrevCount);
							umint iNextFirst = CMap::fsp_GetSegmentFirstSlot(iSeg, Count);
							int32 PrevKey = pData->m_pKeys[iPrevFirst + PrevCount - 1];
							int32 NextKey = pData->m_pKeys[iNextFirst];

							iPrevGapSegment = iPrevNonEmpty;
							iNextGapSegment = iSeg;
							ProbeKey = PrevKey + (NextKey - PrevKey) / 2;
							ExpectedLessValue = pData->m_pValues[iPrevFirst + PrevCount - 1];
							ExpectedGreaterValue = pData->m_pValues[iNextFirst];
							bFoundGap = true;
							break;
						}

						iPrevNonEmpty = iSeg;
					}

					DMibAssertTrue(bFoundGap);
					DMibExpect(CMap::fsp_FindNextNonEmptySegment(pData, iPrevGapSegment), ==, iNextGapSegment);
					DMibExpect(CMap::fsp_FindPrevNonEmptySegment(pData, iNextGapSegment), ==, iPrevGapSegment);

					{
						DMibTestPath("FindSmallestGreaterThanEqual");
						auto *pValue = Map.f_FindSmallestGreaterThanEqual(ProbeKey);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, ExpectedGreaterValue);
					}
					{
						DMibTestPath("FindLargestLessThanEqual");
						auto *pValue = Map.f_FindLargestLessThanEqual(ProbeKey);
						DMibAssertTrue(pValue);
						DMibExpect(*pValue, ==, ExpectedLessValue);
					}
					{
						DMibTestPath("IteratorSmallestGreaterThanEqual");
						auto Iter = Map.f_GetIterator_SmallestGreaterThanEqual(ProbeKey);
						DMibExpectTrue((bool)Iter);
						DMibExpect(*Iter, ==, ExpectedGreaterValue);
					}
					{
						DMibTestPath("IteratorLargestLessThanEqual");
						auto Iter = Map.f_GetIterator_LargestLessThanEqual(ProbeKey);
						DMibExpectTrue((bool)Iter);
						DMibExpect(*Iter, ==, ExpectedLessValue);
					}

					fg_ExpectPackedMapValid(Map, "SparseBoundLookupsSkipEmptySegments");
				};
				DMibTestCategory("SingleLevelIndexLookupAcrossMultipleSegments")
				{
					using CMap = TCPackedMapTestAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = false, .m_MaxCapacityBits = 3}>>;

					CMap Map;
					Map.f_BulkLoad
						(
							5
							, [](auto &&_fInsert)
							{
								_fInsert(10, 100);
								_fInsert(20, 200);
								_fInsert(30, 300);
								_fInsert(40, 400);
								_fInsert(50, 500);
							}
						)
					;

					auto *pData = Map.mp_pData;
					DMibAssertTrue(pData);
					DMibExpect(pData->m_nSegments, >, 1);
					DMibExpect(pData->m_nStaticIndexLevels, ==, 1);

					umint iLookupSegment = 0;
					int32 LookupKey = 0;
					int32 LookupValue = 0;
					bool bFoundLaterSegment = false;

					for (umint iSeg = 1; iSeg < pData->m_nSegments; ++iSeg)
					{
						umint Count = pData->m_pSegmentMeta[iSeg].m_Count;
						if (Count == 0)
							continue;

						iLookupSegment = iSeg;
						umint iFirst = CMap::fsp_GetSegmentFirstSlot(iSeg, Count);
						LookupKey = pData->m_pKeys[iFirst];
						LookupValue = pData->m_pValues[iFirst];
						bFoundLaterSegment = true;
						break;
					}

					DMibAssertTrue(bFoundLaterSegment);
					DMibExpect(iLookupSegment, >, 0);

					{
						DMibTestPath("FindEqual");
						auto *pFoundValue = Map.f_FindEqual(LookupKey);
						DMibAssertTrue(pFoundValue);
						DMibExpect(*pFoundValue, ==, LookupValue);
					}
					{
						DMibTestPath("Exists");
						DMibExpectTrue(Map.f_Exists(LookupKey));
					}
					{
						DMibTestPath("Iterator");
						auto Iter = Map.f_Entries().f_GetIterator(LookupKey);
						DMibExpectTrue((bool)Iter);
						auto Ref = *Iter;
						DMibExpect(Ref.f_Key(), ==, LookupKey);
						DMibExpect(Ref.f_Value(), ==, LookupValue);
					}

					fg_ExpectPackedMapValid(Map, "SingleLevelIndexLookupAcrossMultipleSegments");
				};
				DMibTestCategory("BulkInsertProjectedWindow")
				{
					using CMap = TCPackedMapTestAccess<TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_MinSegments = 8, .m_MaxCapacityBits = 12}>>;

					CMap Map;
					auto Plan = fg_FindProjectedBulkInsertPlan(Map);

					DMibAssertTrue(Plan.m_bFound);

					Map.f_BulkInsert
						(
							2
							, [&](auto &&_fInsert)
							{
								_fInsert(Plan.m_LeftInsertKey, Plan.m_LeftInsertKey * 100);
								_fInsert(Plan.m_RightInsertKey, Plan.m_RightInsertKey * 100);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, Plan.m_nInitialCount + 2);
					DMibAssertTrue(Map.f_FindEqual(Plan.m_LeftInsertKey));
					DMibExpect(*Map.f_FindEqual(Plan.m_LeftInsertKey), ==, Plan.m_LeftInsertKey * 100);
					DMibAssertTrue(Map.f_FindEqual(Plan.m_RightInsertKey));
					DMibExpect(*Map.f_FindEqual(Plan.m_RightInsertKey), ==, Plan.m_RightInsertKey * 100);

					int32 PrevKey = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, PrevKey)(ETestFlag_Aggregated);
						PrevKey = *Iter;
					}

					fg_ExpectPackedMapValid(Map, "BulkInsert projected window");
				};
			};
			DMibTestSuite("CapacityOverflow")
			{
				// Use mc_MaxCapacityBits = 8 with segment size 4: max 2^8 = 256 elements across max 64 segments
				using CSmallMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_MaxCapacityBits = 8}>;

				DMibTestCategory("Insert")
				{
					CSmallMap Map;

					// Inserting enough elements to force a resize beyond mc_MaxCapacityBits should throw
					DMibExpectException
						(
							[&]
							{
								for (int32 iTest = 0; iTest < 512; ++iTest)
									Map.f_Insert(iTest, iTest * 10);
							}
							()
							, DMibErrorInstance("PackedMap capacity exceeded (mc_MaxCapacityBits = 8)")
						)
					;

					// Map should still contain the elements that were inserted before the overflow
					DMibExpect(Map.f_GetLen(), >, 0);
				};
				DMibTestCategory("BulkLoad")
				{
					CSmallMap Map;

					// BulkLoad requesting more capacity than mc_MaxCapacityBits allows should throw
					DMibExpectException
						(
							Map.f_BulkLoad
							(
								512
								, [](auto &&_fInsert)
								{
									for (int32 iTest = 0; iTest < 512; ++iTest)
										_fInsert(iTest, iTest * 10);
								}
							)
							, DMibErrorInstance("PackedMap capacity exceeded (mc_MaxCapacityBits = 8)")
						)
					;
				};
			};
			DMibTestSuite("ExceptionSafety")
			{
				this->f_ExceptionSafety<false>();
			};
			DMibTestSuite("ExceptionSafetyAdaptive")
			{
				this->f_ExceptionSafety<true>();
			};
		}

		struct CThrowingValue
		{
			static inline NAtomic::TCAtomic<umint> ms_nAlive{0};
			static inline umint ms_nThrowAfter = TCLimitsInt<umint>::mc_Max;
			static inline umint ms_nConstructions = 0;
			static inline umint ms_nDestructions = 0;
			static inline umint ms_nTotalConstructions = 0;
			int32 m_Value = 0;
			uint32 m_Magic = 0;

			static void fs_Reset(umint _nThrowAfter = TCLimitsInt<umint>::mc_Max)
			{
				ms_nAlive = 0;
				ms_nThrowAfter = _nThrowAfter;
				ms_nConstructions = 0;
				ms_nDestructions = 0;
				ms_nTotalConstructions = 0;
			}

			CThrowingValue()
			{
				m_Magic = 0xABCD1234;
				++ms_nAlive;
				++ms_nTotalConstructions;
			}

			explicit CThrowingValue(int32 _v)
				: m_Value(_v)
			{
				m_Magic = 0xABCD1234;
				++ms_nAlive;
				++ms_nTotalConstructions;
			}

			~CThrowingValue()
			{
				DMibFastCheck(m_Magic == 0xABCD1234);
				m_Magic = 0xDEADDEAD;
				--ms_nAlive;
				++ms_nDestructions;
			}

			CThrowingValue(CThrowingValue const &_o)
				: m_Value(_o.m_Value)
			{
				if (ms_nThrowAfter >= 0 && ++ms_nConstructions >= ms_nThrowAfter)
					DMibError("ThrowCopy");
				m_Magic = 0xABCD1234;
				++ms_nAlive;
				++ms_nTotalConstructions;
			}

			CThrowingValue(CThrowingValue &&_o)
				: m_Value(_o.m_Value)
			{
				if (ms_nThrowAfter >= 0 && ++ms_nConstructions >= ms_nThrowAfter)
					DMibError("ThrowMove");
				m_Magic = 0xABCD1234;
				++ms_nAlive;
				++ms_nTotalConstructions;
			}

			CThrowingValue &operator = (CThrowingValue const &) = default;
			CThrowingValue &operator = (CThrowingValue &&) = default;

			bool operator == (CThrowingValue const &_Other) const noexcept = default;
			auto operator <=> (CThrowingValue const &_Other) const noexcept = default;
		};

		template <typename t_CMap>
		auto fg_FindThrowAfterCompletedBulkLoadSegment(umint _nCount) -> umint
		{
			CThrowingValue::fs_Reset();

			TCPackedMapTestAccess<t_CMap> Map;
			Map.f_BulkLoad
				(
					_nCount
					, [&](auto &&_fInsert)
					{
						for (int32 i = 0; i < _nCount; ++i)
							_fInsert(CThrowingValue(i * 10), CThrowingValue(i * 100));
					}
				)
			;

			auto *pData = Map.mp_pData;
			umint FirstCompletedSegmentCount = 0;
			umint nNonEmptySegments = 0;
			for (umint iSegment = 0; iSegment < pData->m_nSegments; ++iSegment)
			{
				umint SegmentCount = pData->m_pSegmentMeta[iSegment].m_Count;
				if (SegmentCount <= 0)
					continue;

				++nNonEmptySegments;
				if (FirstCompletedSegmentCount == 0)
					FirstCompletedSegmentCount = SegmentCount;

				if (nNonEmptySegments >= 2)
					break;
			}

			DMibCheck(nNonEmptySegments >= 2);
			return FirstCompletedSegmentCount * 2 + 1;
		}

		// Nothrow move, throwing copy - tests c_bNothrowInsert vs mcp_bNothrowElementMove
		struct CNothrowMoveCopyThrow
		{
			static inline NAtomic::TCAtomic<umint> ms_nAlive{0};
			static inline umint ms_nConstructions = 0;
			static inline umint ms_nThrowAfter = TCLimitsInt<umint>::mc_Max;
			int32 m_Value = 0;
			uint32 m_Magic = 0;

			static void fs_Reset(umint _nThrowAfter = TCLimitsInt<umint>::mc_Max)
			{
				ms_nAlive = 0;
				ms_nConstructions = 0;
				ms_nThrowAfter = _nThrowAfter;
			}

			CNothrowMoveCopyThrow() noexcept
			{
				m_Magic = 0xABCD1234;
				++ms_nAlive;
			}

			explicit CNothrowMoveCopyThrow(int32 _v) noexcept
				: m_Value(_v)
			{
				m_Magic = 0xABCD1234;
				++ms_nAlive;
			}

			~CNothrowMoveCopyThrow()
			{
				DMibFastCheck(m_Magic == 0xABCD1234);
				m_Magic = 0xDEADDEAD;
				--ms_nAlive;
			}

			CNothrowMoveCopyThrow(CNothrowMoveCopyThrow const &_o)
				: m_Value(_o.m_Value)
			{
				if (ms_nThrowAfter >= 0 && ++ms_nConstructions >= ms_nThrowAfter)
					DMibError("ThrowCopy");
				m_Magic = 0xABCD1234;
				++ms_nAlive;
			}

			CNothrowMoveCopyThrow(CNothrowMoveCopyThrow &&_o) noexcept
				: m_Value(_o.m_Value)
			{
				m_Magic = 0xABCD1234;
				++ms_nAlive;
			}

			CNothrowMoveCopyThrow &operator = (CNothrowMoveCopyThrow const &) = default;
			CNothrowMoveCopyThrow &operator = (CNothrowMoveCopyThrow &&) = default;

			bool operator == (CNothrowMoveCopyThrow const &_Other) const noexcept = default;
			auto operator <=> (CNothrowMoveCopyThrow const &_Other) const noexcept = default;
		};

		// Throwing move, nothrow copy - mcp_bNothrowElementMove is false, but copy-insert is safe
		struct CThrowMoveNothrowCopy
		{
			static inline NAtomic::TCAtomic<umint> ms_nAlive{0};
			static inline umint ms_nConstructions = 0;
			static inline umint ms_nThrowAfter = TCLimitsInt<umint>::mc_Max;
			int32 m_Value = 0;
			uint32 m_Magic = 0;

			static void fs_Reset(umint _nThrowAfter = TCLimitsInt<umint>::mc_Max)
			{
				ms_nAlive = 0;
				ms_nConstructions = 0;
				ms_nThrowAfter = _nThrowAfter;
			}

			CThrowMoveNothrowCopy() noexcept
			{
				m_Magic = 0xABCD1234;
				++ms_nAlive;
			}

			explicit CThrowMoveNothrowCopy(int32 _v) noexcept
				: m_Value(_v)
			{
				m_Magic = 0xABCD1234;
				++ms_nAlive;
			}

			~CThrowMoveNothrowCopy()
			{
				DMibFastCheck(m_Magic == 0xABCD1234);
				m_Magic = 0xDEADDEAD;
				--ms_nAlive;
			}

			CThrowMoveNothrowCopy(CThrowMoveNothrowCopy const &_o) noexcept
				: m_Value(_o.m_Value)
			{
				m_Magic = 0xABCD1234;
				++ms_nAlive;
			}

			CThrowMoveNothrowCopy(CThrowMoveNothrowCopy &&_o)
				: m_Value(_o.m_Value)
			{
				if (ms_nThrowAfter >= 0 && ++ms_nConstructions >= ms_nThrowAfter)
					DMibError("ThrowMove");
				m_Magic = 0xABCD1234;
				++ms_nAlive;
			}

			CThrowMoveNothrowCopy &operator = (CThrowMoveNothrowCopy const &) = default;
			CThrowMoveNothrowCopy &operator = (CThrowMoveNothrowCopy &&) = default;

			bool operator == (CThrowMoveNothrowCopy const &_Other) const noexcept = default;
			auto operator <=> (CThrowMoveNothrowCopy const &_Other) const noexcept = default;
		};

		template <bool t_bAdaptive>
		void f_ExceptionSafety()
		{
			using CThrowMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

			// Verify our throwing type is indeed throwing-move
			static_assert(!TCPackedMapConstantsAccess<CThrowMap>::mc_bNothrowElementMove);

			// Verify int32 map is nothrow
			static_assert(TCPackedMapConstantsAccess<TCPackedMap<int32, int32>>::mc_bNothrowElementMove);

			DMibTestCategory("InsertSweep")
			{
				auto fInsertAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CThrowMap Map;
						for (int32 i = 0; i < 6; ++i)
							Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));
						Map.f_Insert(CThrowingValue(25), CThrowingValue(250));
					}
				;
				fInsertAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fInsertAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("RemoveSweep")
			{
				auto fRemoveAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CThrowMap Map;
						for (int32 i = 0; i < 6; ++i)
							Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));
						Map.f_Remove(CThrowingValue(20));
					}
				;
				fRemoveAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fRemoveAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("CopySweep")
			{
				auto fCopyAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CThrowMap Map;
						for (int32 i = 0; i < 8; ++i)
							Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));
						CThrowMap Copy(Map);
						(void)Copy;
					}
				;
				fCopyAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fCopyAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("ResizeSweep")
			{
				using CSmallCapMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
				auto fResizeAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CSmallCapMap Map;
						for (int32 i = 0; i < 3; ++i)
							Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));
						Map.f_Insert(CThrowingValue(15), CThrowingValue(150));
					}
				;
				fResizeAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fResizeAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("BulkLoadSweep")
			{
				auto fBulkLoadAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CThrowMap Map;
						Map.f_BulkLoad
							(
								10
								, [](auto &&_fInsert)
								{
									for (int32 i = 0; i < 10; ++i)
										_fInsert(CThrowingValue(i * 10), CThrowingValue(i * 100));
								}
							)
						;
					}
				;
				fBulkLoadAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fBulkLoadAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("BulkLoadCaughtFailureClearsState")
			{
				constexpr umint c_nCount = 10;
				using CThrowAccessMap = TCPackedMapTestAccess<CThrowMap>;

				umint nThrowAfter = fg_FindThrowAfterCompletedBulkLoadSegment<CThrowMap>(c_nCount);

				CThrowingValue::fs_Reset(nThrowAfter);
				CThrowAccessMap Map;

				DMibExpectExceptionType
					(
						Map.f_BulkLoad
						(
							c_nCount
							, [](auto &&_fInsert)
							{
								for (int32 i = 0; i < c_nCount; ++i)
									_fInsert(CThrowingValue(i * 10), CThrowingValue(i * 100));
							}
						)
						, CException
					)
				;

				DMibExpect(Map.f_IsEmpty(), ==, true);
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				fg_ExpectPackedMapValid(Map, "BulkLoadCaughtFailureClearsState");
			};
			DMibTestCategory("TemplatedCopyConstructorBulkLoadFailure")
			{
				constexpr umint c_nCount = 10;
				using CSourceMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
				using CDestinationMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

				umint nThrowAfter = fg_FindThrowAfterCompletedBulkLoadSegment<CDestinationMap>(c_nCount);

				CThrowingValue::fs_Reset();
				CSourceMap Source;
				Source.f_BulkLoad
					(
						c_nCount
						, [](auto &&_fInsert)
						{
							for (int32 i = 0; i < c_nCount; ++i)
								_fInsert(CThrowingValue(i * 10), CThrowingValue(i * 100));
						}
					)
				;

				umint nAliveBefore = (umint)CThrowingValue::ms_nAlive;
				CThrowingValue::ms_nConstructions = 0;
				CThrowingValue::ms_nThrowAfter = nThrowAfter;

				auto fCopyAttempt = [&]
					{
						CDestinationMap Copy(Source);
						(void)Copy;
					}
				;

				DMibExpectExceptionType(fCopyAttempt(), CException);
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, nAliveBefore);
			};
			DMibTestCategory("TemplatedCopyConstructorBulkLoadFailureFreesAllocation")
			{
				constexpr umint c_nCount = 10;
				using CSourceMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
				using CDestinationMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

				CFailingAllocator::fs_Reset();
				CThrowingValue::fs_Reset();
				CSourceMap Source;
				Source.f_BulkLoad
					(
						c_nCount
						, [](auto &&_fInsert)
						{
							for (int32 i = 0; i < c_nCount; ++i)
								_fInsert(CThrowingValue(i * 10), CThrowingValue(i * 100));
						}
					)
				;

				umint nAliveBefore = (umint)CThrowingValue::ms_nAlive;
				CFailingAllocator::fs_Reset();
				CThrowingValue::ms_nConstructions = 0;
				CThrowingValue::ms_nThrowAfter = 1;

				auto fCopyAttempt = [&]
					{
						CDestinationMap Copy(Source);
						(void)Copy;
					}
				;

				DMibExpectExceptionType(fCopyAttempt(), CException);
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, nAliveBefore);
				DMibExpect(CFailingAllocator::ms_nAllocations, >, 0);
				DMibExpect(CFailingAllocator::ms_nFrees, ==, CFailingAllocator::ms_nAllocations);
				CFailingAllocator::fs_Reset();
			};
			DMibTestCategory("TemplatedCopyConstructorInsertFailureFreesAllocation")
			{
				constexpr umint c_nCount = 10;
				using CSourceMap = TCPackedMap<int32, CThrowingValue, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;
				using CDestinationMap = TCPackedMap<int64, CThrowingValue, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

				CFailingAllocator::fs_Reset();
				CThrowingValue::fs_Reset();
				CSourceMap Source;
				for (int32 i = 0; i < c_nCount; ++i)
					Source.f_Insert(i * 10, CThrowingValue(i * 100));

				umint nAliveBefore = (umint)CThrowingValue::ms_nAlive;
				CFailingAllocator::fs_Reset();
				CThrowingValue::ms_nConstructions = 0;
				CThrowingValue::ms_nThrowAfter = 1;

				auto fCopyAttempt = [&]
					{
						CDestinationMap Copy(Source);
						(void)Copy;
					}
				;

				DMibExpectExceptionType(fCopyAttempt(), CException);
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, nAliveBefore);
				DMibExpect(CFailingAllocator::ms_nAllocations, >, 0);
				DMibExpect(CFailingAllocator::ms_nFrees, ==, CFailingAllocator::ms_nAllocations);
				CFailingAllocator::fs_Reset();
			};
			DMibTestCategory("BulkInsert")
			{
				// Normal operation (no exceptions)
				{
					DMibTestPath("NormalInt32");
					TCPackedMap<int32, int32> Map;

					for (int32 i = 0; i < 10; ++i)
						Map.f_Insert(i * 20, i * 200);

					DMibExpect(Map.f_GetLen(), ==, 10);

					Map.f_BulkInsert
						(
							5
							, [](auto &&_fInsert)
							{
								_fInsert(5, 50);
								_fInsert(15, 150);
								_fInsert(25, 250);
								_fInsert(35, 350);
								_fInsert(45, 450);
							}
						)
					;

					DMibExpect(Map.f_GetLen(), ==, 15);

					// Verify all elements present and sorted
					int32 Prev = -1;
					for (auto Iter = Map.f_Keys().f_GetIterator(); Iter; ++Iter)
					{
						DMibExpect(*Iter, >, Prev)(ETestFlag_Aggregated);
						Prev = *Iter;
					}
				}

				// Normal operation with throwing type (no exceptions triggered)
				{
					DMibTestPath("NormalThrowingType");
					CThrowingValue::fs_Reset();
					{
						CThrowMap Map;

						for (int32 i = 0; i < 4; ++i)
							Map.f_Insert(CThrowingValue(i * 20), CThrowingValue(i * 200));

						DMibExpect(Map.f_GetLen(), ==, 4);

						Map.f_BulkInsert
							(
								4
								, [](auto &&_fInsert)
								{
									_fInsert(CThrowingValue(5), CThrowingValue(50));
									_fInsert(CThrowingValue(15), CThrowingValue(150));
									_fInsert(CThrowingValue(25), CThrowingValue(250));
									_fInsert(CThrowingValue(35), CThrowingValue(350));
								}
							)
						;

						DMibExpect(Map.f_GetLen(), ==, 8);
					}
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}

				// Sweep through all possible throw points to cover populate, merge, and placement
				{
					DMibTestPath("ExceptionSweep");

					auto fBulkInsertAttempt = [&](umint _nThrowAfter)
						{
							CThrowingValue::fs_Reset(_nThrowAfter);
							CThrowMap Map;

							for (int32 i = 0; i < 4; ++i)
								Map.f_Insert(CThrowingValue(i * 20), CThrowingValue(i * 200));

							// Reset construction counter and set throw point after initial setup

							Map.f_BulkInsert
								(
									4
									, [](auto &&_fInsert)
									{
										_fInsert(CThrowingValue(5), CThrowingValue(50));
										_fInsert(CThrowingValue(15), CThrowingValue(150));
										_fInsert(CThrowingValue(25), CThrowingValue(250));
										_fInsert(CThrowingValue(35), CThrowingValue(350));
									}
								)
							;
						}
					;

					// Find the construction count needed for a successful bulk insert
					fBulkInsertAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotalConstructions = CThrowingValue::ms_nConstructions;
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);

					// Throw at every possible point
					for (umint iThrow = 1; iThrow <= nTotalConstructions; ++iThrow)
					{
						DMibTestPath("{}"_f << iThrow);
						CThrowingValue::fs_Reset();
						DMibExpectExceptionType(fBulkInsertAttempt(iThrow), CException);
						DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
					}
				}
			};
			DMibTestCategory("LinearInsertSweep")
			{
				// Insert sequentially to exercise adaptive paths over multiple resizes
				using CSmallThrowMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
				auto fLinearAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CSmallThrowMap Map;
						for (int32 i = 0; i < 24; ++i)
							Map.f_Insert(CThrowingValue(i), CThrowingValue(i * 100));
					}
				;
				fLinearAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fLinearAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("RandomInsertSweep")
			{
				// Insert in scrambled order to exercise non-adaptive rebalance paths over multiple resizes
				using CSmallThrowMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
				auto fRandomAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CSmallThrowMap Map;
						// Deterministic scramble: (i*7)%24 is a permutation since gcd(7,24)=1
						for (int32 i = 0; i < 24; ++i)
						{
							int32 Key = ((i * 7) % 24) * 10;
							Map.f_Insert(CThrowingValue(Key), CThrowingValue(Key * 10));
						}
					}
				;
				fRandomAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fRandomAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("LinearRemoveShrinkSweep")
			{
				// Insert many elements, then remove sequentially to trigger shrinks via adaptive path
				using CSmallThrowMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
				auto fLinearRemoveAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CSmallThrowMap Map;
						for (int32 i = 0; i < 24; ++i)
							Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));
						for (int32 i = 0; i < 20; ++i)
							Map.f_Remove(CThrowingValue(i * 10));
					}
				;
				fLinearRemoveAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fLinearRemoveAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("RandomRemoveShrinkSweep")
			{
				// Insert many elements, then remove in scrambled order to trigger non-adaptive rebalance paths
				using CSmallThrowMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
				auto fRandomRemoveAttempt = [&](umint _nThrowAfter)
					{
						CThrowingValue::fs_Reset(_nThrowAfter);
						CSmallThrowMap Map;
						for (int32 i = 0; i < 24; ++i)
							Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));
						// Remove in scrambled order: (i*7)%20 is a permutation since gcd(7,20)=1
						for (int32 i = 0; i < 20; ++i)
						{
							int32 Key = ((i * 7) % 20) * 10;
							Map.f_Remove(CThrowingValue(Key));
						}
					}
				;
				fRandomRemoveAttempt(TCLimitsInt<umint>::mc_Max);
				umint nTotal = CThrowingValue::ms_nConstructions;
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				for (umint i = 1; i <= nTotal; ++i)
				{
					DMibTestPath("{}"_f << i);
					CThrowingValue::fs_Reset();
					DMibExpectExceptionType(fRandomRemoveAttempt(i), CException);
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				}
			};
			DMibTestCategory("ResizeException")
			{
				CThrowingValue::fs_Reset();
				{
					using CSmallCapMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;

					CSmallCapMap Map;

					// Fill to force a resize on next insert
					for (int32 i = 0; i < 3; ++i)
						Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));

					// Set throw during resize element migration
					CThrowingValue::ms_nThrowAfter = 4;
					CThrowingValue::ms_nConstructions = 0;

					DMibExpectExceptionType(Map.f_Insert(CThrowingValue(15), CThrowingValue(150)), CException);
				}
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
			};
			DMibTestCategory("NothrowMoveThrowCopy")
			{
				// Test exception safety when move is nothrow but copy throws.
				// This exercises the c_bNothrowInsert path where mcp_bNothrowElementMove is true
				// but the actual construction (copy) can throw.
				using CNMMap = TCPackedMap<CNothrowMoveCopyThrow, CNothrowMoveCopyThrow, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

				// Verify move IS nothrow for this map type
				static_assert(TCPackedMapConstantsAccess<CNMMap>::mc_bNothrowElementMove);

				// InsertCopySweep: insert via const ref (copy path) with nothrow-move type
				DMibTestCategory("InsertCopySweep")
				{
					auto fInsertCopyAttempt = [&](umint _nThrowAfter)
						{
							CNothrowMoveCopyThrow::fs_Reset(_nThrowAfter);
							CNMMap Map;
							for (int32 i = 0; i < 6; ++i)
								Map.f_Insert(CNothrowMoveCopyThrow(i * 10), CNothrowMoveCopyThrow(i * 100));
							// Insert via const ref to force copy construction
							CNothrowMoveCopyThrow const Key(25);
							CNothrowMoveCopyThrow const Value(250);
							Map.f_Insert(Key, Value);
						}
					;
					fInsertCopyAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CNothrowMoveCopyThrow::ms_nConstructions;
					DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						DMibExpectExceptionType(fInsertCopyAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					}
				};

				// ResizeCopySweep: insert via copy when resize is triggered
				DMibTestCategory("ResizeCopySweep")
				{
					using CSmallNMMap = TCPackedMap<CNothrowMoveCopyThrow, CNothrowMoveCopyThrow, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CSmallNMMap>::mc_bNothrowElementMove);

					auto fResizeCopyAttempt = [&](umint _nThrowAfter)
						{
							CNothrowMoveCopyThrow::fs_Reset(_nThrowAfter);
							CSmallNMMap Map;
							for (int32 i = 0; i < 3; ++i)
								Map.f_Insert(CNothrowMoveCopyThrow(i * 10), CNothrowMoveCopyThrow(i * 100));
							CNothrowMoveCopyThrow const Key(15);
							CNothrowMoveCopyThrow const Value(150);
							Map.f_Insert(Key, Value);
						}
					;
					fResizeCopyAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CNothrowMoveCopyThrow::ms_nConstructions;
					DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						DMibExpectExceptionType(fResizeCopyAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					}
				};

				// CopySweep: copy construction of map with nothrow-move type
				DMibTestCategory("CopySweep")
				{
					auto fCopyAttempt = [&](umint _nThrowAfter)
						{
							CNothrowMoveCopyThrow::fs_Reset(_nThrowAfter);
							CNMMap Map;
							for (int32 i = 0; i < 8; ++i)
								Map.f_Insert(CNothrowMoveCopyThrow(i * 10), CNothrowMoveCopyThrow(i * 100));
							CNMMap Copy(Map);
							(void)Copy;
						}
					;
					fCopyAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CNothrowMoveCopyThrow::ms_nConstructions;
					DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						DMibExpectExceptionType(fCopyAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("LinearInsertCopySweep")
				{
					using CSmallNMMap = TCPackedMap<CNothrowMoveCopyThrow, CNothrowMoveCopyThrow, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					auto fLinearAttempt = [&](umint _nThrowAfter)
						{
							CNothrowMoveCopyThrow::fs_Reset(_nThrowAfter);
							CSmallNMMap Map;
							for (int32 i = 0; i < 24; ++i)
							{
								CNothrowMoveCopyThrow const Key(i);
								CNothrowMoveCopyThrow const Value(i * 100);
								Map.f_Insert(Key, Value);
							}
						}
					;
					fLinearAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CNothrowMoveCopyThrow::ms_nConstructions;
					DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						DMibExpectExceptionType(fLinearAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("RandomInsertCopySweep")
				{
					using CSmallNMMap = TCPackedMap<CNothrowMoveCopyThrow, CNothrowMoveCopyThrow, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					auto fRandomAttempt = [&](umint _nThrowAfter)
						{
							CNothrowMoveCopyThrow::fs_Reset(_nThrowAfter);
							CSmallNMMap Map;
							for (int32 i = 0; i < 24; ++i)
							{
								int32 v = ((i * 7) % 24) * 10;
								CNothrowMoveCopyThrow const Key(v);
								CNothrowMoveCopyThrow const Value(v * 10);
								Map.f_Insert(Key, Value);
							}
						}
					;
					fRandomAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CNothrowMoveCopyThrow::ms_nConstructions;
					DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						DMibExpectExceptionType(fRandomAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveCopyThrow::ms_nAlive, ==, 0);
					}
				};
			};
			DMibTestCategory("ThrowMoveNothrowCopy")
			{
				// Test exception safety when move throws but copy is nothrow.
				// mcp_bNothrowElementMove is false (internal ops use throwing move),
				// but insert via const ref uses nothrow copy so c_bNothrowInsert is true.
				using CTMMap = TCPackedMap<CThrowMoveNothrowCopy, CThrowMoveNothrowCopy, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

				// Verify move is NOT nothrow
				static_assert(!TCPackedMapConstantsAccess<CTMMap>::mc_bNothrowElementMove);

				// InsertMoveSweep: insert via rvalue (move path, throws)
				DMibTestCategory("InsertMoveSweep")
				{
					auto fInsertMoveAttempt = [&](umint _nThrowAfter)
						{
							CThrowMoveNothrowCopy::fs_Reset(_nThrowAfter);
							CTMMap Map;
							for (int32 i = 0; i < 6; ++i)
								Map.f_Insert(CThrowMoveNothrowCopy(i * 10), CThrowMoveNothrowCopy(i * 100));
							Map.f_Insert(CThrowMoveNothrowCopy(25), CThrowMoveNothrowCopy(250));
						}
					;
					fInsertMoveAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CThrowMoveNothrowCopy::ms_nConstructions;
					DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CThrowMoveNothrowCopy::fs_Reset();
						DMibExpectExceptionType(fInsertMoveAttempt(i), CException);
						DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
					}
				};

				// InsertCopyNoThrow: insert via const ref (copy path, nothrow) should succeed
				DMibTestCategory("InsertCopyNoThrow")
				{
					CThrowMoveNothrowCopy::fs_Reset();
					{
						CTMMap Map;
						for (int32 i = 0; i < 6; ++i)
							Map.f_Insert(CThrowMoveNothrowCopy(i * 10), CThrowMoveNothrowCopy(i * 100));
						// Set throw counter - only move will trigger it, copy won't
						CThrowMoveNothrowCopy::ms_nConstructions = 0;
						CThrowMoveNothrowCopy::ms_nThrowAfter = 1000;
						CThrowMoveNothrowCopy const Key(25);
						CThrowMoveNothrowCopy const Value(250);
						Map.f_Insert(Key, Value);
						DMibExpect(Map.f_GetLen(), ==, 7);
					}
					DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
				};

				// ResizeCopySweep: insert via const ref (nothrow copy) triggering resize where internal moves throw
				DMibTestCategory("ResizeCopySweep")
				{
					using CSmallTMMap = TCPackedMap<CThrowMoveNothrowCopy, CThrowMoveNothrowCopy, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(!TCPackedMapConstantsAccess<CSmallTMMap>::mc_bNothrowElementMove);

					auto fResizeCopyAttempt = [&](umint _nThrowAfter)
						{
							CThrowMoveNothrowCopy::fs_Reset(_nThrowAfter);
							CSmallTMMap Map;
							for (int32 i = 0; i < 3; ++i)
								Map.f_Insert(CThrowMoveNothrowCopy(i * 10), CThrowMoveNothrowCopy(i * 100));
							CThrowMoveNothrowCopy const Key(15);
							CThrowMoveNothrowCopy const Value(150);
							Map.f_Insert(Key, Value);
						}
					;
					fResizeCopyAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CThrowMoveNothrowCopy::ms_nConstructions;
					DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CThrowMoveNothrowCopy::fs_Reset();
						DMibExpectExceptionType(fResizeCopyAttempt(i), CException);
						DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("LinearInsertCopySweep")
				{
					using CSmallTMMap = TCPackedMap<CThrowMoveNothrowCopy, CThrowMoveNothrowCopy, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					auto fLinearAttempt = [&](umint _nThrowAfter)
						{
							CThrowMoveNothrowCopy::fs_Reset(_nThrowAfter);
							CSmallTMMap Map;
							for (int32 i = 0; i < 24; ++i)
							{
								CThrowMoveNothrowCopy const Key(i);
								CThrowMoveNothrowCopy const Value(i * 100);
								Map.f_Insert(Key, Value);
							}
						}
					;
					fLinearAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CThrowMoveNothrowCopy::ms_nConstructions;
					DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CThrowMoveNothrowCopy::fs_Reset();
						DMibExpectExceptionType(fLinearAttempt(i), CException);
						DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("RandomInsertCopySweep")
				{
					using CSmallTMMap = TCPackedMap<CThrowMoveNothrowCopy, CThrowMoveNothrowCopy, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					auto fRandomAttempt = [&](umint _nThrowAfter)
						{
							CThrowMoveNothrowCopy::fs_Reset(_nThrowAfter);
							CSmallTMMap Map;
							for (int32 i = 0; i < 24; ++i)
							{
								int32 v = ((i * 7) % 24) * 10;
								CThrowMoveNothrowCopy const Key(v);
								CThrowMoveNothrowCopy const Value(v * 10);
								Map.f_Insert(Key, Value);
							}
						}
					;
					fRandomAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CThrowMoveNothrowCopy::ms_nConstructions;
					DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CThrowMoveNothrowCopy::fs_Reset();
						DMibExpectExceptionType(fRandomAttempt(i), CException);
						DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);
					}
				};
			};
			DMibTestCategory("MoveMetadataExceptionSafety")
			{
				DMibTestCategory("SameTypeMoveConstructorPreservesSourceOnCompareMoveThrow")
				{
					using CMoveMap = TCPackedMap<int32, int32, CConfigurableThrowingMoveCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

					CConfigurableThrowingMoveCompare::fs_Reset();
					CMoveMap Source;
					for (int32 i = 0; i < 6; ++i)
						Source.f_Insert(i * 10, i * 100);

					CConfigurableThrowingMoveCompare::fs_Reset(true, false);
					DMibExpectExceptionType
						(
							[&]
							{
								CMoveMap Destination(fg_Move(Source));
								(void)Destination;
							}
							()
							, CException
						)
					;

					DMibExpect(Source.f_GetLen(), ==, 6);
					for (int32 i = 0; i < 6; ++i)
					{
						auto *pValue = Source.f_FindEqual(i * 10);
						DMibAssertTrue(pValue)(ETestFlag_Aggregated);
						DMibExpect(*pValue, ==, i * 100)(ETestFlag_Aggregated);
					}
					fg_ExpectPackedMapValid(Source, "SameTypeMoveConstructorPreservesSourceOnCompareMoveThrow");
					CConfigurableThrowingMoveCompare::fs_Reset();
				};
				DMibTestCategory("SameTypeMoveAssignmentPreservesSourceOnCompareMoveThrow")
				{
					using CMoveMap = TCPackedMap<int32, int32, CConfigurableThrowingMoveCompare, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

					CConfigurableThrowingMoveCompare::fs_Reset();
					CMoveMap Source;
					for (int32 i = 0; i < 6; ++i)
						Source.f_Insert(i * 10, i * 100);

					CMoveMap Destination;
					Destination.f_Insert(-10, -100);

					CConfigurableThrowingMoveCompare::fs_Reset(false, true);
					DMibExpectExceptionType(Destination = fg_Move(Source), CException);

					DMibExpect(Source.f_GetLen(), ==, 6);
					for (int32 i = 0; i < 6; ++i)
					{
						auto *pValue = Source.f_FindEqual(i * 10);
						DMibAssertTrue(pValue)(ETestFlag_Aggregated);
						DMibExpect(*pValue, ==, i * 100)(ETestFlag_Aggregated);
					}
					fg_ExpectPackedMapValid(Source, "SameTypeMoveAssignmentPreservesSourceOnCompareMoveThrow");
					CConfigurableThrowingMoveCompare::fs_Reset();
				};
				DMibTestCategory("TemplatedMoveConstructorBulkLoadAllocFailureClearsSource")
				{
					using CSourceMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;
					using CDestinationMap = TCPackedMap<int32, int32, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

					CSourceMap Source;
					for (int32 i = 0; i < 6; ++i)
						Source.f_Insert(i * 10, i * 100);

					CFailingAllocator::fs_Reset(1);
					DMibExpectExceptionType
						(
							[&]
							{
								CDestinationMap Destination(fg_Move(Source));
								(void)Destination;
							}
							()
							, CException
						)
					;

				DMibExpect(Source.f_IsEmpty(), ==, true);
				fg_ExpectPackedMapValid(Source, "TemplatedMoveConstructorBulkLoadAllocFailureClearsSource");
				CFailingAllocator::fs_Reset();
			};
			DMibTestCategory("TemplatedMoveConstructorInsertFailureClearsSourceAndFreesAllocation")
			{
				constexpr umint c_nCount = 10;
				using CSourceMap = TCPackedMap<int32, CThrowingValue, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;
				using CDestinationMap = TCPackedMap<int64, CThrowingValue, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

				CThrowingValue::fs_Reset();
				CSourceMap Source;
				for (int32 i = 0; i < c_nCount; ++i)
					Source.f_Insert(i * 10, CThrowingValue(i * 100));

				CFailingAllocator::fs_Reset();
				CThrowingValue::ms_nConstructions = 0;
				CThrowingValue::ms_nThrowAfter = 1;

				DMibExpectExceptionType
					(
						[&]
						{
							CDestinationMap Destination(fg_Move(Source));
							(void)Destination;
						}
						()
						, CException
					)
				;

				DMibExpect(Source.f_IsEmpty(), ==, true);
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
				DMibExpect(CFailingAllocator::ms_nAllocations, >, 0);
				DMibExpect(CFailingAllocator::ms_nFrees, ==, CFailingAllocator::ms_nAllocations);
				CFailingAllocator::fs_Reset();
			};
			DMibTestCategory("TemplatedMoveAssignmentBulkLoadAllocFailureClearsSource")
			{
					using CSourceMap = TCPackedMap<int32, int32, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;
					using CDestinationMap = TCPackedMap<int32, int32, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 8, .m_bAdaptive = t_bAdaptive}>;

					CSourceMap Source;
					for (int32 i = 0; i < 6; ++i)
						Source.f_Insert(i * 10, i * 100);

					CDestinationMap Destination;

					CFailingAllocator::fs_Reset(1);
					DMibExpectExceptionType(Destination = fg_Move(Source), CException);

					DMibExpect(Source.f_IsEmpty(), ==, true);
					fg_ExpectPackedMapValid(Source, "TemplatedMoveAssignmentBulkLoadAllocFailureClearsSource");
					CFailingAllocator::fs_Reset();
				};
			};
			DMibTestCategory("NoExceptionNormalOperation")
			{
				// Verify throwing types work correctly when not actually throwing
				CThrowingValue::fs_Reset();
				{
					CThrowMap Map;

					for (int32 i = 0; i < 20; ++i)
						Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));

					DMibExpect(Map.f_GetLen(), ==, 20);

					// Verify all values
					for (int32 i = 0; i < 20; ++i)
					{
						auto *pVal = Map.f_FindEqual(CThrowingValue(i * 10));
						DMibAssertTrue(pVal)(ETestFlag_Aggregated);
						DMibExpect(pVal->m_Value, ==, i * 100)(ETestFlag_Aggregated);
					}

					// Remove some
					for (int32 i = 0; i < 20; i += 2)
						Map.f_Remove(CThrowingValue(i * 10));

					DMibExpect(Map.f_GetLen(), ==, 10);

					// Copy
					CThrowMap Copy(Map);
					DMibExpect(Copy.f_GetLen(), ==, 10);
				}
				DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
			};
			DMibTestCategory("AllocFailSweep")
			{
				// Test allocation failure at every possible allocation point using CFailingAllocator.
				// Since CFailingAllocator is stateless, CScratchAllocator resolves to it,
				// so temp buffer allocations during resize also go through the failing allocator.

				DMibTestCategory("ResizeAllocSweep")
				{
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fResizeAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							CFailMap Map;
							// Fill segments to trigger resize on next insert
							for (int32 i = 0; i < 3; ++i)
								Map.f_Insert(CNothrowMoveTracked(i * 10), CNothrowMoveTracked(i * 100));

							// Reset counter before the insert that triggers resize
							Map.f_Insert(CNothrowMoveTracked(15), CNothrowMoveTracked(150));
						}
					;
					fResizeAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 0; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fResizeAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("BulkLoadAllocSweep")
				{
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fBulkLoadAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							CFailMap Map;
							Map.f_BulkLoad
								(
									20
									, [](auto &&_fInsert)
									{
										for (int32 i = 0; i < 20; ++i)
											_fInsert(CNothrowMoveTracked(i * 10), CNothrowMoveTracked(i * 100));
									}
								)
							;
						}
					;
					fBulkLoadAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fBulkLoadAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("LinearInsertAllocSweep")
				{
					// Sequential inserts trigger multiple resizes
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fLinearInsertAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							CFailMap Map;
							for (int32 i = 0; i < 30; ++i)
								Map.f_Insert(CNothrowMoveTracked(i), CNothrowMoveTracked(i * 100));
						}
					;
					fLinearInsertAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fLinearInsertAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("ResizeAllocSweepWithThrowingValue")
				{
					// Combine failing allocator with throwing values to verify no element leaks
					using CFailThrowMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;

					auto fResizeAttempt = [&](umint _nAllocAfterFail)
						{
							CThrowingValue::fs_Reset();
							CFailingAllocator::fs_Reset();
							CFailThrowMap Map;
							for (int32 i = 0; i < 3; ++i)
								Map.f_Insert(CThrowingValue(i * 10), CThrowingValue(i * 100));
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							Map.f_Insert(CThrowingValue(15), CThrowingValue(150));
						}
					;
					fResizeAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CThrowingValue::fs_Reset();
						DMibExpectExceptionType(fResizeAttempt(i), CException);
						DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("BulkInsertAllocSweep")
				{
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fBulkInsertAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset();
							CFailMap Map;
							for (int32 i = 0; i < 5; ++i)
								Map.f_Insert(CNothrowMoveTracked(i * 20), CNothrowMoveTracked(i * 200));
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							Map.f_BulkInsert
								(
									4
									, [](auto &&_fInsert)
									{
										_fInsert(CNothrowMoveTracked(5), CNothrowMoveTracked(50));
										_fInsert(CNothrowMoveTracked(15), CNothrowMoveTracked(150));
										_fInsert(CNothrowMoveTracked(25), CNothrowMoveTracked(250));
										_fInsert(CNothrowMoveTracked(35), CNothrowMoveTracked(350));
									}
								)
							;
						}
					;
					fBulkInsertAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fBulkInsertAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("BulkInsertAllocSweepWithThrowingValue")
				{
					// Combine failing allocator with throwing values to verify no element leaks
					using CFailThrowMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;

					auto fBulkInsertAttempt = [&](umint _nAllocAfterFail)
						{
							CThrowingValue::fs_Reset();
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							CFailThrowMap Map;
							for (int32 i = 0; i < 5; ++i)
								Map.f_Insert(CThrowingValue(i * 20), CThrowingValue(i * 200));
							Map.f_BulkInsert
								(
									4
									, [](auto &&_fInsert)
									{
										_fInsert(CThrowingValue(5), CThrowingValue(50));
										_fInsert(CThrowingValue(15), CThrowingValue(150));
										_fInsert(CThrowingValue(25), CThrowingValue(250));
										_fInsert(CThrowingValue(35), CThrowingValue(350));
									}
								)
							;
						}
					;
					fBulkInsertAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CThrowingValue::fs_Reset();
						DMibExpectExceptionType(fBulkInsertAttempt(i), CException);
						DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("BulkInsertAllocSweepTriggersResize")
				{
					// BulkInsert with enough new elements to trigger a resize inside f_BulkInsert
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fBulkInsertResizeAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							CFailMap Map;
							for (int32 i = 0; i < 3; ++i)
								Map.f_Insert(CNothrowMoveTracked(i * 100), CNothrowMoveTracked(i * 1000));
							// Insert enough elements to exceed root upper bound and force resize
							Map.f_BulkInsert
								(
									12
									, [](auto &&_fInsert)
									{
										for (int32 i = 0; i < 12; ++i)
											_fInsert(CNothrowMoveTracked(i * 10 + 1), CNothrowMoveTracked(i * 10 + 1));
									}
								)
							;
						}
					;
					fBulkInsertResizeAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fBulkInsertResizeAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("BulkInsertAllocSweepTriggersResizeWithThrowingValue")
				{
					// Resize inside f_BulkInsert with throwing values to check for element leaks
					using CFailThrowMap = TCPackedMap<CThrowingValue, CThrowingValue, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;

					auto fBulkInsertResizeAttempt = [&](umint _nAllocAfterFail)
						{
							CThrowingValue::fs_Reset();
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							CFailThrowMap Map;
							for (int32 i = 0; i < 3; ++i)
								Map.f_Insert(CThrowingValue(i * 100), CThrowingValue(i * 1000));
							Map.f_BulkInsert
								(
									12
									, [](auto &&_fInsert)
									{
										for (int32 i = 0; i < 12; ++i)
											_fInsert(CThrowingValue(i * 10 + 1), CThrowingValue(i * 10 + 1));
									}
								)
							;
						}
					;
					fBulkInsertResizeAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CThrowingValue::fs_Reset();
						DMibExpectExceptionType(fBulkInsertResizeAttempt(i), CException);
						DMibExpect((umint)CThrowingValue::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("BulkInsertAllocSweepMultipleWindows")
				{
					// BulkInsert into many segments to exercise multiple merged rebalance windows
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fBulkInsertMultiWindowAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							CFailMap Map;
							// Pre-populate with spread-out keys so inserts hit different segments
							for (int32 i = 0; i < 16; ++i)
								Map.f_Insert(CNothrowMoveTracked(i * 100), CNothrowMoveTracked(i * 1000));
							// Insert keys that interleave across multiple segments
							Map.f_BulkInsert
								(
									8
									, [](auto &&_fInsert)
									{
										_fInsert(CNothrowMoveTracked(50), CNothrowMoveTracked(500));
										_fInsert(CNothrowMoveTracked(250), CNothrowMoveTracked(2500));
										_fInsert(CNothrowMoveTracked(450), CNothrowMoveTracked(4500));
										_fInsert(CNothrowMoveTracked(650), CNothrowMoveTracked(6500));
										_fInsert(CNothrowMoveTracked(850), CNothrowMoveTracked(8500));
										_fInsert(CNothrowMoveTracked(1050), CNothrowMoveTracked(10500));
										_fInsert(CNothrowMoveTracked(1250), CNothrowMoveTracked(12500));
										_fInsert(CNothrowMoveTracked(1450), CNothrowMoveTracked(14500));
									}
								)
							;
						}
					;
					fBulkInsertMultiWindowAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fBulkInsertMultiWindowAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("RandomInsertAllocSweep")
				{
					// Scrambled insert order exercises rebalance allocation paths
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fRandomInsertAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							CFailMap Map;
							// Deterministic scramble: (i*7)%24 is a permutation since gcd(7,24)=1
							for (int32 i = 0; i < 24; ++i)
							{
								int32 Key = ((i * 7) % 24) * 10;
								Map.f_Insert(CNothrowMoveTracked(Key), CNothrowMoveTracked(Key * 10));
							}
						}
					;
					fRandomInsertAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fRandomInsertAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("LinearRemoveShrinkAllocSweep")
				{
					// Insert many elements, then remove sequentially to trigger shrink resizes
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fRemoveShrinkAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset();
							CFailMap Map;
							for (int32 i = 0; i < 24; ++i)
								Map.f_Insert(CNothrowMoveTracked(i * 10), CNothrowMoveTracked(i * 100));
							// Reset counter before removes that trigger shrinking
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							for (int32 i = 0; i < 20; ++i)
								Map.f_Remove(CNothrowMoveTracked(i * 10));
						}
					;
					fRemoveShrinkAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fRemoveShrinkAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("RandomRemoveShrinkAllocSweep")
				{
					// Remove in scrambled order to exercise rebalance + shrink allocation paths
					using CFailMap = TCPackedMap<CNothrowMoveTracked, CNothrowMoveTracked, CSort_Default, CFailingAllocator, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>;
					static_assert(TCPackedMapConstantsAccess<CFailMap>::mc_bNothrowElementMove);

					auto fRandomRemoveAttempt = [&](umint _nAllocAfterFail)
						{
							CNothrowMoveTracked::fs_Reset();
							CFailingAllocator::fs_Reset();
							CFailMap Map;
							for (int32 i = 0; i < 24; ++i)
								Map.f_Insert(CNothrowMoveTracked(i * 10), CNothrowMoveTracked(i * 100));
							CFailingAllocator::fs_Reset(_nAllocAfterFail);
							// Deterministic scramble: (i*11)%24 is a permutation since gcd(11,24)=1
							for (int32 i = 0; i < 20; ++i)
							{
								int32 Key = ((i * 11) % 24) * 10;
								Map.f_Remove(CNothrowMoveTracked(Key));
							}
						}
					;
					fRandomRemoveAttempt(TCLimitsInt<umint>::mc_Max);
					umint nTotal = CFailingAllocator::ms_nAllocations;
					DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					for (umint i = 1; i <= nTotal; ++i)
					{
						DMibTestPath("{}"_f << i);
						CNothrowMoveTracked::fs_Reset();
						DMibExpectExceptionType(fRandomRemoveAttempt(i), CException);
						DMibExpect((umint)CNothrowMoveTracked::ms_nAlive, ==, 0);
					}
				};
				DMibTestCategory("BulkInsertMultiWindowThrowingMoveElementCountConsistency")
				{
					// Verify that m_nElements is consistent with segment counts after a
					// throwing-move exception during multi-window BulkInsert.
					// When BulkInsert processes multiple windows sequentially, it defers the
					// m_nElements update until after all windows complete. If a later window
					// throws, PlaceCleanup adjusts m_nElements from the stale pre-BulkInsert
					// value, omitting new elements already merged into earlier windows.
					//
					// Setup: 128 elements in ~32 segments (SegmentSize=4), with new keys
					// concentrated in two widely-separated groups to ensure separate windows.
					using CMap = TCPackedMapTestAccess<TCPackedMap<CThrowMoveNothrowCopy, CThrowMoveNothrowCopy, CSort_Default, CAllocator_Heap, CPackedMapOptions{.m_SegmentSize = 4, .m_bAdaptive = t_bAdaptive}>>;
					static_assert(!TCPackedMapConstantsAccess<CMap>::mc_bNothrowElementMove);

					constexpr int32 c_nInitial = 128;
					constexpr int32 c_nNew = 4;

					auto fPopulateMap = [](CMap &_Map)
						{
							for (int32 i = 0; i < c_nInitial; ++i)
								_Map.f_Insert(CThrowMoveNothrowCopy(i * 100), CThrowMoveNothrowCopy(i * 1000));
						}
					;

					auto fDoBulkInsert = [](CMap &_Map)
						{
							// Two groups of new keys: near segment 0 and near the end
							// These should produce separate non-overlapping rebalance windows
							_Map.f_BulkInsert
								(
									c_nNew
									, [](auto &&_fInsert)
									{
										// Group 1: near the start (keys 5, 15)
										_fInsert(CThrowMoveNothrowCopy(5), CThrowMoveNothrowCopy(50));
										_fInsert(CThrowMoveNothrowCopy(15), CThrowMoveNothrowCopy(150));
										// Group 2: near the end (keys 12605, 12705)
										_fInsert(CThrowMoveNothrowCopy(12605), CThrowMoveNothrowCopy(126050));
										_fInsert(CThrowMoveNothrowCopy(12705), CThrowMoveNothrowCopy(127050));
									}
								)
							;
						}
					;

					// First: run without throwing to count total move constructions
					umint nTotalMoves = 0;
					{
						CThrowMoveNothrowCopy::fs_Reset();
						CMap Map;
						fPopulateMap(Map);
						CThrowMoveNothrowCopy::ms_nConstructions = 0;
						CThrowMoveNothrowCopy::ms_nThrowAfter = TCLimitsInt<umint>::mc_Max;
						fDoBulkInsert(Map);
						nTotalMoves = CThrowMoveNothrowCopy::ms_nConstructions;
					}
					DMibExpect((umint)CThrowMoveNothrowCopy::ms_nAlive, ==, 0);

					// Sweep through every throw point during BulkInsert and verify
					// element count consistency after exception
					for (umint iThrow = 1; iThrow <= nTotalMoves; ++iThrow)
					{
						DMibTestPath("{}"_f << iThrow);
						CThrowMoveNothrowCopy::fs_Reset();
						CMap Map;
						fPopulateMap(Map);

						CThrowMoveNothrowCopy::ms_nConstructions = 0;
						CThrowMoveNothrowCopy::ms_nThrowAfter = iThrow;

						try
						{
							fDoBulkInsert(Map);
						}
						catch (CException const &)
						{
						}

						if (Map.mp_pData)
						{
							// Sum actual element count from segment metadata
							umint nActualElements = 0;
							for (umint iSeg = 0; iSeg < Map.mp_pData->m_nSegments; ++iSeg)
								nActualElements += Map.mp_pData->m_pSegmentMeta[iSeg].m_Count;

							// f_GetLen() returns m_nElements which should match the actual count
							DMibExpect(Map.f_GetLen(), ==, nActualElements);
						}
					}
				};
			};
		}
	};

	DMibTestRegister(CPackedMap_Tests, Malterlib::Container);
}
