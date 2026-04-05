// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_Copy(TCVector const &_Source)
	{
		umint nSource = _Source.f_GetLen();
		umint nDestination = f_GetLen();
		if (nSource && nSource <= nDestination)
		{
			NPrivate::fg_CopyOverArray(f_GetArray(), _Source.f_GetArray(), nSource);

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
			auto pSrc = _Source.f_GetArray();
			auto pDst = f_GetArray();
			if (nSource)
				NPrivate::fg_CopyArray(pDst, pSrc, nSource, mp_StaticData.m_pData->m_Length);
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(TCVector const &_Source)
		: mp_StaticData{_Source.fp_Allocator(), nullptr}
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		fp_Copy(_Source);

		Cleanup.f_Clear();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(TCVector &_Source)
		: TCVector(fg_Const(_Source))
	{
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Source)
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
		if (nSource)
			NPrivate::fg_CopyArray(pDst, pSrc, nSource, mp_StaticData.m_pData->m_Length);

		Cleanup.f_Clear();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Source)
		: TCVector(fg_Const(_Source))
	{
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::operator = (TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Source) -> TCVector &
	{
		umint nSource = _Source.f_GetLen();
		umint nDestination = f_GetLen();
		if (nSource && nSource <= nDestination)
		{
			NPrivate::fg_CopyOverArray(f_GetArray(), _Source.f_GetArray(), nSource);
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

			if (nSource)
				NPrivate::fg_CopyArray(pArray, pSrcArray, nSource, mp_StaticData.m_pData->m_Length);
		}
		return *this;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::operator = (TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Source) -> TCVector &
	{
		return operator = (fg_Const(_Source));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::operator = (TCVector const &_Source) -> TCVector &
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		fp_Copy(_Source);

		Cleanup.f_Clear();

		return *this;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::operator = (TCVector &_Source) -> TCVector &
	{
		return operator = (fg_Const(_Source));
	}
}
