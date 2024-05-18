// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator[] (tf_CKey &&_Key) -> CUserData &
	{
		return mp_Tree.f_FindEqualOrInsert
			(
				_Key
				, [&]() -> CNode *
				{
					auto Memory = mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
					auto pData = (CNode *)Memory.m_pMemory;
					new ((void *)pData) CNode(fg_Forward<tf_CKey>(_Key));
					Memory.f_Claim();
					return pData;
				}
				, mp_Compare
			)->f_Value()
		;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Insert(CNodeHandle &&_Node) -> CUserData &
	{
		DMibFastCheck(!!_Node);
		return mp_Tree.f_FindEqualOrInsert
			(
				*_Node.mp_pNode
				, [&]() -> CNode *
				{
					return fg_Exchange(_Node.mp_pNode, nullptr);
				}
				, mp_Compare
			)->f_Value()
		;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Insert(tf_CKey &&_Key) -> CUserData &
	{
		return operator [](fg_Forward<tf_CKey>(_Key));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Map(tf_CKey &&_Key) -> CUserData &
	{
		return mp_Tree.f_FindEqualOrInsert
			(
				_Key
				, [&]() -> CNode *
				{
					auto Memory = mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
					auto pData = (CNode *)Memory.m_pMemory;
					new ((void *)pData) CNode(fg_Forward<tf_CKey>(_Key));
					Memory.f_Claim();
					return pData;
				}
				, mp_Compare
			)->f_Value()
		;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename... tfp_CArgs>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Map(tf_CKey &&_Key, bool &_bCreated, tfp_CArgs && ... p_Args) -> CUserData &
	{
		_bCreated = false;
		return mp_Tree.f_FindEqualOrInsert
			(
				_Key
				, [&]() -> CNode *
				{
					_bCreated = true;
					auto Memory = mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
					auto pData = (CNode *)Memory.m_pMemory;
					new ((void *)pData) CNode(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CArgs>(p_Args)...);
					Memory.f_Claim();
					return pData;
				}
				, mp_Compare
			)->f_Value()
		;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename... tfp_CParam>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator () (tf_CKey &&_Key, tfp_CParam && ... p_Params) -> TCMapResult<CUserData &>
	{
		bool bWasCreated = false;
		auto pData = mp_Tree.f_FindEqualOrInsert
			(
				_Key
				, [&]() -> CNode *
				{
					bWasCreated = true;
					auto Memory = mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
					auto pData = (CNode *)Memory.m_pMemory;
					new ((void *)pData) CNode(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CParam>(p_Params)...);
					Memory.f_Claim();
					return pData;
				}
				, mp_Compare
			)
		;
		return TCMapResult<CUserData &>(pData->f_Value(), bWasCreated);
	}
}
