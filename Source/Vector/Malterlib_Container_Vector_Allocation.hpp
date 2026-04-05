// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	umint TCVector<t_CData, t_CAllocator, t_COptions>::f_GetArrayAllocSize() const
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
	auto TCVector<t_CData, t_CAllocator, t_COptions>::fp_AllocData(umint _nObjects) -> CVectorData *
	{
		DMibFastCheck(_nObjects != 0);

#if defined(DCompiler_MSVC_Workaround)
		static constexpr umint mc_MaxObjects = fg_AlignDownConstExpr(TCLimitsInt<umint>::mc_Max - sizeof(CVectorData), fsp_Alignment()) / sizeof(t_CData);
#else
		static constexpr umint mc_MaxObjects = fg_AlignDownConstExpr(TCLimitsInt<umint>::mc_Max - sizeof(CVectorData), mcp_Alignment) / sizeof(t_CData);
#endif
		if (_nObjects >= mc_MaxObjects)
			DMibErrorMemory("Out of memory trying to allocate objects in vector");

		umint Size = _nObjects * sizeof(t_CData) + sizeof(CVectorData);
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
	auto TCVector<t_CData, t_CAllocator, t_COptions>::fp_AllocDataGrow(umint _nObjects) -> CVectorData *
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
	inline_never umint TCVector<t_CData, t_CAllocator, t_COptions>::fsp_GetAllocSize(umint _NeededSize)
	{
		return fg_Max(fg_Max((umint(1) << NMib::fg_GetHighestBitSetNoZero((_NeededSize-1) | 1)) << 1, _NeededSize), t_COptions::mc_MinSize);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small bool TCVector<t_CData, t_CAllocator, t_COptions>::fsp_NeedReallocGrow(umint _NeededSize, const CVectorData *_pExtraData)
	{
		if (!_pExtraData)
			return true;

		umint NeededSize = _NeededSize * sizeof(t_CData) + sizeof(CVectorData);
		umint CurrentSize = _pExtraData->m_AllocSize;
		if (NeededSize > CurrentSize)
			return true;

		return false;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small bool TCVector<t_CData, t_CAllocator, t_COptions>::fsp_NeedRealloc(umint _NeededSize, const CVectorData *_pExtraData)
	{
		if (!_pExtraData)
			return true;

		umint NeededSize = _NeededSize * sizeof(t_CData) + sizeof(CVectorData);
		umint CurrentSize = _pExtraData->m_AllocSize;
		if (NeededSize > CurrentSize)
			return true;

		if constexpr (t_COptions::mc_bShrink)
		{
			umint NeededSizeAligned = fsp_GetAllocSize(_NeededSize) * sizeof(t_CData) + sizeof(CVectorData);
			if (CurrentSize > (NeededSizeAligned << 1))
				return true;
		}

		return false;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small bool TCVector<t_CData, t_CAllocator, t_COptions>::fsp_CanGrow(umint _NeededSize, const CVectorData *_pExtraData)
	{
		if (!_pExtraData)
			return false;
		umint NeededSize = _NeededSize * sizeof(t_CData) + sizeof(CVectorData);
		umint CurrentSize = _pExtraData->m_AllocSize;
		if (NeededSize > CurrentSize)
			return false;
		return true;
	}
}
