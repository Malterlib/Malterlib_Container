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
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::TCMap(NMemory::CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
		: mp_Allocator(fg_Forward<tfp_CParams>(p_Params)...)
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

		if constexpr(NTraits::TCIsSame<tf_CKey, t_CKey>::mc_Value && NTraits::TCIsSame<tf_CCompare, t_CCompare>::mc_Value)
			fp_CopyAll(_Other);
		else
			fp_CopyAddAll(_Other);

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

		if constexpr (NTraits::TCIsSame<tf_CKey, t_CKey>::mc_Value && NTraits::TCIsSame<tf_CCompare, t_CCompare>::mc_Value)
			fp_CopyAll(_Other);
		else
			fp_CopyAddAll(_Other);

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

		if constexpr (NTraits::TCIsSame<tf_CKey, t_CKey>::mc_Value && NTraits::TCIsSame<tf_CCompare, t_CCompare>::mc_Value)
			fp_MoveAll(_Other);
		else
			fp_MoveAddAll(_Other);

		Cleanup.f_Clear();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (TCMap const &_Other) -> TCMap &
	{
		DMibSafeCheck(this != &_Other, "Must not copy same object to itself");
		f_Clear();
		fp_CopyAll(_Other);

		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (const TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &_Other) -> TCMap &
	{
		f_Clear();
		if constexpr (NTraits::TCIsSame<tf_CKey, t_CKey>::mc_Value && NTraits::TCIsSame<tf_CCompare, t_CCompare>::mc_Value)
			fp_CopyAll(_Other);
		else
			fp_CopyAddAll(_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &_Other) -> TCMap &
	{
		f_Clear();
		if constexpr (NTraits::TCIsSame<tf_CKey, t_CKey>::mc_Value && NTraits::TCIsSame<tf_CCompare, t_CCompare>::mc_Value)
			fp_CopyAll(_Other);
		else
			fp_CopyAddAll(_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &&_Other) -> TCMap &
	{
		f_Clear();
		if constexpr (NTraits::TCIsSame<tf_CKey, t_CKey>::mc_Value && NTraits::TCIsSame<tf_CCompare, t_CCompare>::mc_Value)
			fp_MoveAll(_Other);
		else
			fp_MoveAddAll(_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator = (TCMap &&_Other) -> TCMap &
	{
		f_Clear();
		mp_Tree = fg_Move(_Other.mp_Tree);
		mp_Allocator = fg_Move(_Other.mp_Allocator);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CSource>
	inline_always void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_CopyAll(tf_CSource && _Other)
	{
		return NPrivate::TCMapCopy<TCMap, mcp_bIsReference && !NTraits::TCIsConst<CValueNoRef>::mc_Value>::fs_CopyAll(*this, fg_Forward<tf_CSource>(_Other));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CSource>
	inline_always void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_MoveAll(tf_CSource && _Other)
	{
		return NPrivate::TCMapCopy<TCMap, mcp_bIsReference && !NTraits::TCIsConst<CValueNoRef>::mc_Value>::fs_MoveAll(*this, fg_Forward<tf_CSource>(_Other));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CSource>
	inline_always void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_CopyAddAll(tf_CSource && _Other)
	{
		return NPrivate::TCMapCopy<TCMap, mcp_bIsReference && !NTraits::TCIsConst<CValueNoRef>::mc_Value>::fs_CopyAddAll(*this, fg_Forward<tf_CSource>(_Other));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CSource>
	inline_always void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_MoveAddAll(tf_CSource && _Other)
	{
		return NPrivate::TCMapCopy<TCMap, mcp_bIsReference && !NTraits::TCIsConst<CValueNoRef>::mc_Value>::fs_MoveAddAll(*this, fg_Forward<tf_CSource>(_Other));
	}
}
