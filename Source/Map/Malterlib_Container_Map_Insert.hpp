// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename... tfp_CParam>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator[] (tf_CKey &&_Key, tfp_CParam && ... p_Params) -> CUserData &
	{
		return mp_Tree.f_FindEqualOrInsert
			(
				_Key
				, [&]() -> CNodeDestructive *
				{
					auto Memory = mp_Allocator.f_AllocSafe(sizeof(CNodeDestructive), alignof(CNodeDestructive));
					auto pData = (CNodeDestructive *)Memory.m_pMemory;
					new ((void *)pData) CNodeDestructive(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CParam>(p_Params)...);
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

		if constexpr (NTraits::cIsEmpty<t_CAllocator>)
		{
			return mp_Tree.f_FindEqualOrInsert
				(
					_Node.f_Key()
					, [&]() -> CNodeDestructive *
					{
						return fg_Exchange(_Node.mp_pNode, nullptr);
					}
					, mp_Compare
				)->f_Value()
			;
		}
		else
		{
			return mp_Tree.f_FindEqualOrInsert
				(
					_Node.f_Key()
					, [&]() -> CNodeDestructive *
					{
						bool bCanSteal = false;
						if constexpr (t_CAllocator::mc_CanBeStatic)
						{
							if (_Node.f_GetAllocator().f_IsStatic(_Node.mp_pNode))
								bCanSteal = &mp_Allocator == &_Node.f_GetAllocator();
							else
								bCanSteal = mp_Allocator == _Node.f_GetAllocator();
						}
						else
							bCanSteal = mp_Allocator == _Node.f_GetAllocator();

						if (bCanSteal)
							return fg_Exchange(_Node.mp_pNode, nullptr);

						auto Allocation = mp_Allocator.f_AllocSafe(sizeof(CNodeDestructive), alignof(CNodeDestructive));
						auto *pData = new(Allocation.m_pMemory) CNodeDestructive(fg_Move(_Node.f_Key()), fg_Move(_Node.f_Value()));
						Allocation.f_Claim();

						_Node.f_Clear();

						return pData;
					}
					, mp_Compare
				)->f_Value()
			;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Insert(tf_CKey &&_Key) -> CUserData &
	{
		return operator [](fg_Forward<tf_CKey>(_Key));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_InsertOrAssign(CNodeHandle &&_Node) -> CUserData &
	{
		DMibFastCheck(!!_Node);

		bool bInserted = false;
		auto pResult = mp_Tree.f_FindEqualOrInsert
			(
				_Node.f_Key()
				, [&]() -> CNodeDestructive *
				{
					bInserted = true;
					if constexpr (NTraits::cIsEmpty<t_CAllocator>)
					{
						return fg_Exchange(_Node.mp_pNode, nullptr);
					}
					else
					{
						bool bCanSteal = false;
						if constexpr (t_CAllocator::mc_CanBeStatic)
						{
							if (_Node.f_GetAllocator().f_IsStatic(_Node.mp_pNode))
								bCanSteal = &mp_Allocator == &_Node.f_GetAllocator();
							else
								bCanSteal = mp_Allocator == _Node.f_GetAllocator();
						}
						else
							bCanSteal = mp_Allocator == _Node.f_GetAllocator();

						if (bCanSteal)
							return fg_Exchange(_Node.mp_pNode, nullptr);

						auto Allocation = mp_Allocator.f_AllocSafe(sizeof(CNodeDestructive), alignof(CNodeDestructive));
						auto *pData = new(Allocation.m_pMemory) CNodeDestructive(fg_Move(_Node.f_Key()), fg_Move(_Node.f_Value()));
						Allocation.f_Claim();

						_Node.f_Clear();

						return pData;
					}
				}
				, mp_Compare
			)
		;
		if (!bInserted)
			pResult->f_Value() = fg_Move(_Node.f_Value());
		return pResult->f_Value();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename... tfp_CParam>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator () (tf_CKey &&_Key, tfp_CParam && ... p_Params) -> TCMapResult<CUserData &>
	{
		bool bWasCreated = false;
		auto pData = mp_Tree.f_FindEqualOrInsert
			(
				_Key
				, [&]() -> CNodeDestructive *
				{
					bWasCreated = true;
					auto Memory = mp_Allocator.f_AllocSafe(sizeof(CNodeDestructive), alignof(CNodeDestructive));
					auto pData = (CNodeDestructive *)Memory.m_pMemory;
					new ((void *)pData) CNodeDestructive(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CParam>(p_Params)...);
					Memory.f_Claim();
					return pData;
				}
				, mp_Compare
			)
		;
		return TCMapResult<CUserData &>(pData->f_Value(), bWasCreated);
	}
}
