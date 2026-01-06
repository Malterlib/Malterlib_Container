// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	mint TCVector<t_CData, t_CAllocator, t_COptions>::f_GetArrayAllocSize() const
	{
		auto pData = mp_StaticData.m_pData;
		if (!pData)
			return 0;
		return pData->m_AllocSize;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_FreeData(CVectorData *_pData)
	{
		fp_Allocator().f_Free(_pData, _pData->m_AllocSize);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::fp_AllocData(mint _nObjects) -> CVectorData *
	{
		DMibFastCheck(_nObjects != 0);

#if defined(DCompiler_MSVC_Workaround)
		static constexpr mint mc_MaxObjects = fg_AlignDownConstExpr(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), fsp_Alignment()) / sizeof(t_CData);
#else
		static constexpr mint mc_MaxObjects = fg_AlignDownConstExpr(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), mcp_Alignment) / sizeof(t_CData);
#endif
		if (_nObjects >= mc_MaxObjects)
			DMibErrorMemory("Out of memory trying to allocate objects in vector");

		mint Size = _nObjects * sizeof(t_CData) + sizeof(CVectorData);
#if defined(DCompiler_MSVC_Workaround)
		CVectorData *pData = (CVectorData *)fp_Allocator().f_AllocAlignedWithSize(Size, fsp_Alignment(), EAllocationFlag_WillFreeWithSize);
#else
		CVectorData *pData = (CVectorData *)fp_Allocator().f_AllocAlignedWithSize(Size, mcp_Alignment, EAllocationFlag_WillFreeWithSize);
#endif
		pData->m_Length = 0;
		pData->m_AllocSize = Size;

		return pData;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::fp_AllocDataGrow(mint _nObjects) -> CVectorData *
	{
		return fp_AllocData(fsp_GetAllocSize(_nObjects));
	}


	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CAllocator &TCVector<t_CData, t_CAllocator, t_COptions>::fp_Allocator()
	{
		return mp_StaticData;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CAllocator const &TCVector<t_CData, t_CAllocator, t_COptions>::fp_Allocator() const
	{
		return mp_StaticData;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_never mint TCVector<t_CData, t_CAllocator, t_COptions>::fsp_GetAllocSize(mint _NeededSize)
	{
		return fg_Max(fg_Max((mint(1) << NMib::fg_GetHighestBitSetNoZero((_NeededSize-1) | 1)) << 1, _NeededSize), t_COptions::mc_MinSize);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small bool TCVector<t_CData, t_CAllocator, t_COptions>::fsp_NeedReallocGrow(mint _NeededSize, const CVectorData *_pExtraData)
	{
		if (!_pExtraData)
			return true;

		mint NeededSize = _NeededSize * sizeof(t_CData) + sizeof(CVectorData);
		mint CurrentSize = _pExtraData->m_AllocSize;
		if (NeededSize > CurrentSize)
			return true;

		return false;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small bool TCVector<t_CData, t_CAllocator, t_COptions>::fsp_NeedRealloc(mint _NeededSize, const CVectorData *_pExtraData)
	{
		if (!_pExtraData)
			return true;

		mint NeededSize = _NeededSize * sizeof(t_CData) + sizeof(CVectorData);
		mint CurrentSize = _pExtraData->m_AllocSize;
		if (NeededSize > CurrentSize)
			return true;

		if constexpr (t_COptions::mc_bShrink)
		{
			mint NeededSizeAligned = fsp_GetAllocSize(_NeededSize) * sizeof(t_CData) + sizeof(CVectorData);
			if (CurrentSize > (NeededSizeAligned << 1))
				return true;
		}

		return false;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small bool TCVector<t_CData, t_CAllocator, t_COptions>::fsp_CanGrow(mint _NeededSize, const CVectorData *_pExtraData)
	{
		if (!_pExtraData)
			return false;
		mint NeededSize = _NeededSize * sizeof(t_CData) + sizeof(CVectorData);
		mint CurrentSize = _pExtraData->m_AllocSize;
		if (NeededSize > CurrentSize)
			return false;
		return true;
	}
}
