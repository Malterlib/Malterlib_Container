// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Exists(tf_CKey &&_Key) const
	{
		return mp_Tree.f_FindEqual(_Key) != nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator[] (tf_CKey&& _Key) const -> CUserData const &
	{
		const CNode *pData = mp_Tree.f_FindEqual(_Key);
		// Will return a null ref if data does not exist (is this valid in C++?)
		if (pData)
			return pData->f_Value();
		else
			DMibError("Key not found");
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindEqual(tf_CKey &&_Key) const -> CUserData const *
	{
		const CNode *pData = mp_Tree.f_FindEqual(_Key);
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindEqual(tf_CKey &&_Key) -> CUserData *
	{
		CNode *pData = mp_Tree.f_FindEqual(_Key);
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindSmallestGreaterThanEqual(tf_CKey &&_Key) -> CUserData *
	{
		CNode *pData = mp_Tree.f_FindSmallestGreaterThanEqual(fg_Forward<tf_CKey>(_Key));
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindSmallestGreaterThanEqual(tf_CKey &&_Key) const -> CUserData const *
	{
		const CNode *pData = mp_Tree.f_FindSmallestGreaterThanEqual(fg_Forward<tf_CKey>(_Key));
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindLargestLessThanEqual(tf_CKey &&_Key) -> CUserData *
	{
		CNode *pData = mp_Tree.f_FindLargestLessThanEqual(fg_Forward<tf_CKey>(_Key));
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindLargestLessThanEqual(tf_CKey &&_Key) const -> CUserData const *
	{
		const CNode *pData = mp_Tree.f_FindLargestLessThanEqual(fg_Forward<tf_CKey>(_Key));
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindSmallest() -> CUserData *
	{
		CNode *pData = mp_Tree.f_FindSmallest();
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindSmallest() const -> CUserData const *
	{
		CNode *pData = mp_Tree.f_FindSmallest();
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindLargest() -> CUserData *
	{
		CNode *pData = mp_Tree.f_FindLargest();
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindLargest() const -> CUserData const *
	{
		CNode *pData = mp_Tree.f_FindLargest();
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindAny() -> CUserData *
	{
		CNode *pData = mp_Tree.f_GetRoot();
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindAny() const -> CUserData const *
	{
		CNode *pData = mp_Tree.f_GetRoot();
		if (pData)
			return &pData->f_Value();
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindSmallestKey() -> t_CKey *
	{
		CNode *pData = mp_Tree.f_FindSmallest();
		if (pData)
			return &pData->mp_Key;
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindSmallestKey() const -> t_CKey const *
	{
		CNode *pData = mp_Tree.f_FindSmallest();
		if (pData)
			return &pData->mp_Key;
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindLargestKey() -> t_CKey *
	{
		CNode *pData = mp_Tree.f_FindLargest();
		if (pData)
			return &pData->mp_Key;
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindAnyKey() -> t_CKey *
	{
		CNode *pData = mp_Tree.f_GetRoot();
		if (pData)
			return &pData->mp_Key;
		else
			return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FindLargestKey() const -> t_CKey const *
	{
		CNode *pData = mp_Tree.f_FindLargest();
		if (pData)
			return &pData->mp_Key;
		else
			return nullptr;
	}
}
