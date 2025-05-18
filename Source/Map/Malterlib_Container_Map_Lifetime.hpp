// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Clear()
	{
		mp_Tree.f_DeleteAllAllocatorDefiniteType(mp_Allocator);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap() noexcept
	{
		//DMibDTrace("sizeof(CNode) = {}" DMibNewLine, sizeof(CNode));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::~TCMap()
	{
		f_Clear();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap(TCMap const &_Other)
		: mp_Compare(_Other.mp_Compare)
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		fp_CopyAll(_Other);

		Cleanup.f_Clear();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap(std::initializer_list<NStorage::TCTuple<t_CKey, t_CValue>> const &_Values)
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		for (auto &Value : _Values)
			(*this)(fg_Get<0>(Value), fg_Get<1>(Value));

		Cleanup.f_Clear();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename... tfp_CParams>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap(CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
		: mp_Allocator(fg_Forward<tfp_CParams>(p_Params)...)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename... tfp_CParams>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap(CCompareConstructTag const &, tfp_CParams && ...p_Params)
		: mp_Compare(fg_Forward<tfp_CParams>(p_Params)...)
	{
	}


	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename... tfp_CAllocatorParams, typename... tfp_CCompareParams>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap
		(
			CAllocatorConstructTag &&_AlloctatorTag
			, CCompareConstructTag &&_CompareTag
			, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
			, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
		)
		: TCMap
		(
			NMeta::TCConsecutiveIndices<sizeof...(tfp_CAllocatorParams)>()
			, NMeta::TCConsecutiveIndices<sizeof...(tfp_CCompareParams)>()
			, fg_Move(_AlloctatorTag)
			, fg_Move(_CompareTag)
			, fg_Move(_ConstructAllocator)
			, fg_Move(_ConstructCompare)
		)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename... tfp_CAllocatorParams, typename... tfp_CCompareParams, mint... tp_IndicesAllocator, mint... tp_IndicesCompare>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap
		(
			NMeta::TCIndices<tp_IndicesAllocator...> const &_IndexSequenceAllocator
			, NMeta::TCIndices<tp_IndicesCompare...> const &_IndexSequuenceCompare
			, CAllocatorConstructTag &&
			, CCompareConstructTag &&
			, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
			, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
		)
		: mp_Allocator(fg_Forward<tfp_CAllocatorParams>(fg_Get<tp_IndicesAllocator>(_ConstructAllocator.m_Params))...)
		, mp_Compare(fg_Forward<tfp_CCompareParams>(fg_Get<tp_IndicesCompare>(_ConstructCompare.m_Params))...)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap(const TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &_Other)
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		if constexpr(NTraits::cIsSame<tf_CKey, t_CKey> && NTraits::cIsSame<tf_CCompare, t_CCompare>)
		{
			mp_Compare = _Other.mp_Compare;
			fp_CopyAll(_Other);
		}
		else
		{
			if constexpr (NTraits::cIsAssignableWith<t_CCompare, tf_CCompare const &>)
				mp_Compare = _Other.mp_Compare;
			fp_CopyAddAll(_Other);
		}

		Cleanup.f_Clear();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap(TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &_Other)
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		if constexpr (NTraits::cIsSame<tf_CKey, t_CKey> && NTraits::cIsSame<tf_CCompare, t_CCompare>)
		{
			mp_Compare = _Other.mp_Compare;
			fp_CopyAll(_Other);
		}
		else
		{
			if constexpr (NTraits::cIsAssignableWith<t_CCompare, tf_CCompare const &>)
				mp_Compare = _Other.mp_Compare;
			fp_CopyAddAll(_Other);
		}

		Cleanup.f_Clear();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap(TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &&_Other)
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		if constexpr (NTraits::cIsSame<tf_CKey, t_CKey> && NTraits::cIsSame<tf_CCompare, t_CCompare>)
		{
			mp_Compare = fg_Move(_Other.mp_Compare);
			fp_MoveAll(_Other);
		}
		else
		{
			if constexpr (NTraits::cIsAssignableWith<t_CCompare, tf_CCompare &&>)
				mp_Compare = fg_Move(_Other.mp_Compare);
			fp_MoveAddAll(_Other);
		}

		Cleanup.f_Clear();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (TCMap const &_Other) -> TCMap &
	{
		DMibSafeCheck(this != &_Other, "Must not copy same object to itself");
		f_Clear();

		mp_Compare = _Other.mp_Compare;
		fp_CopyAll(_Other);

		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (const TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &_Other) -> TCMap &
	{
		f_Clear();

		if constexpr (NTraits::cIsSame<tf_CKey, t_CKey> && NTraits::cIsSame<tf_CCompare, t_CCompare>)
		{
			mp_Compare = _Other.mp_Compare;
			fp_CopyAll(_Other);
		}
		else
		{
			if constexpr (NTraits::cIsAssignableWith<t_CCompare, tf_CCompare const &>)
				mp_Compare = _Other.mp_Compare;
			fp_CopyAddAll(_Other);
		}

		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &_Other) -> TCMap &
	{
		f_Clear();

		if constexpr (NTraits::cIsSame<tf_CKey, t_CKey> && NTraits::cIsSame<tf_CCompare, t_CCompare>)
		{
			mp_Compare = _Other.mp_Compare;
			fp_CopyAll(_Other);
		}
		else
		{
			if constexpr (NTraits::cIsAssignableWith<t_CCompare, tf_CCompare const &>)
				mp_Compare = _Other.mp_Compare;
			fp_CopyAddAll(_Other);
		}

		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &&_Other) -> TCMap &
	{
		f_Clear();

		if constexpr (NTraits::cIsSame<tf_CKey, t_CKey> && NTraits::cIsSame<tf_CCompare, t_CCompare>)
		{
			mp_Compare = fg_Move(_Other.mp_Compare);
			fp_MoveAll(_Other);
		}
		else
		{
			if constexpr (NTraits::cIsAssignableWith<t_CCompare, tf_CCompare &&>)
				mp_Compare = fg_Move(_Other.mp_Compare);
			fp_MoveAddAll(_Other);
		}
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (TCMap &&_Other) -> TCMap &
	{
		f_Clear();

		mp_Tree = fg_Move(_Other.mp_Tree);
		mp_Allocator = fg_Move(_Other.mp_Allocator);
		mp_Compare = fg_Move(_Other.mp_Compare);

		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CSource>
	inline_always void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_CopyAll(tf_CSource && _Other)
	{
		return NPrivate::TCMapCopy<TCMap, mcp_bIsReference && !NTraits::cIsConst<CValueNoRef>>::fs_CopyAll(*this, fg_Forward<tf_CSource>(_Other));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CSource>
	inline_always void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_MoveAll(tf_CSource && _Other)
	{
		return NPrivate::TCMapCopy<TCMap, mcp_bIsReference && !NTraits::cIsConst<CValueNoRef>>::fs_MoveAll(*this, fg_Forward<tf_CSource>(_Other));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CSource>
	inline_always void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_CopyAddAll(tf_CSource && _Other)
	{
		return NPrivate::TCMapCopy<TCMap, mcp_bIsReference && !NTraits::cIsConst<CValueNoRef>>::fs_CopyAddAll(*this, fg_Forward<tf_CSource>(_Other));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CSource>
	inline_always void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_MoveAddAll(tf_CSource && _Other)
	{
		return NPrivate::TCMapCopy<TCMap, mcp_bIsReference && !NTraits::cIsConst<CValueNoRef>>::fs_MoveAddAll(*this, fg_Forward<tf_CSource>(_Other));
	}
}
