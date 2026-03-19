// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(TCVector &&_Source)
		: mp_StaticData{fg_Move(_Source.fp_Allocator()), nullptr}
	{
		if constexpr (NTraits::cIsEmpty<t_CAllocator>)
		{
			mp_StaticData.m_pData = _Source.mp_StaticData.m_pData;
			_Source.mp_StaticData.m_pData = nullptr;
		}
		else
		{
			bool bCanSteal = false;
			if constexpr (t_CAllocator::mc_CanBeStatic)
			{
				if (!_Source.fp_Allocator().f_IsStatic(_Source.mp_StaticData.m_pData))
					bCanSteal = fp_Allocator() == _Source.fp_Allocator();
			}
			else
				bCanSteal = fp_Allocator() == _Source.fp_Allocator();

			if (bCanSteal)
			{
				mp_StaticData.m_pData = _Source.mp_StaticData.m_pData;
				_Source.mp_StaticData.m_pData = nullptr;
			}
			else
			{
				auto Cleanup = g_OnScopeExit / [&]
					{
						f_Clear();
					}
				;

				fp_Copy(_Source);

				Cleanup.f_Clear();
			}
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Source)
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		umint nSource = _Source.f_GetLen();
		fp_MakeNewRoom(nSource);

		auto pSrc = _Source.f_GetArray();
		auto pDst = f_GetArray();

		NPrivate::fg_MoveArray(pDst, pSrc, nSource);
		if (nSource)
			mp_StaticData.m_pData->m_Length += nSource;

		Cleanup.f_Clear();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::operator = (TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Source) -> TCVector &
	{
		umint nSource = _Source.f_GetLen();
		umint nDestination = f_GetLen();
		if (nSource && nSource <= nDestination)
		{
			NPrivate::fg_MoveOverArray(f_GetArray(), _Source.f_GetArray(), nSource);

			umint nToDestroy = nDestination - nSource;
			if (nToDestroy)
			{
				auto Cleanup = g_OnScopeExit / [&]
					{
						mp_StaticData.m_pData->m_Length = nSource + nToDestroy;
					}
				;
				NPrivate::fg_DestroyArray(f_GetArray() + nSource, nToDestroy, nToDestroy);
			}
		}
		else
		{
			fp_MakeNewRoom(nSource);

			auto pArray = f_GetArray();
			auto pSrcArray = _Source.f_GetArray();

			NPrivate::fg_MoveArray(pArray, pSrcArray, nSource);
			if (nSource)
				mp_StaticData.m_pData->m_Length += nSource;
		}
		return *this;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::operator = (TCVector &&_Source) -> TCVector &
	{
		f_Clear();

		if constexpr (NTraits::cIsEmpty<t_CAllocator>)
		{
			mp_StaticData.m_pData = _Source.mp_StaticData.m_pData;
			_Source.mp_StaticData.m_pData = nullptr;
		}
		else
		{
			bool bCanSteal = false;
			if constexpr (t_CAllocator::mc_CanBeStatic)
			{
				if (!_Source.fp_Allocator().f_IsStatic(_Source.mp_StaticData.m_pData))
					bCanSteal = fp_Allocator() == _Source.fp_Allocator();
			}
			else
				bCanSteal = fp_Allocator() == _Source.fp_Allocator();

			if (bCanSteal)
			{
				mp_StaticData.m_pData = _Source.mp_StaticData.m_pData;
				_Source.mp_StaticData.m_pData = nullptr;
			}
			else
			{
				auto Cleanup = g_OnScopeExit / [&]
					{
						f_Clear();
					}
				;

				fp_Copy(_Source);

				Cleanup.f_Clear();
			}
		}

		return *this;
	}
}
