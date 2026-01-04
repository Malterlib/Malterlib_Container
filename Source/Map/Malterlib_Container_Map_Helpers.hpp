// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	struct TCMap;

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	struct TCSet;

	enum EMapIteratorAccess : uint8
	{
		EMapIteratorAccess_Value = 0
		, EMapIteratorAccess_Key
		, EMapIteratorAccess_KeyValue
	};

	enum EMapIteratorFlags : uint8
	{
		EMapIteratorFlags_None = 0
		, EMapIteratorFlags_Reverse = DMibBit(0)
		, EMapIteratorFlags_Const = DMibBit(1)
		, EMapIteratorFlags_Bidirectional = DMibBit(2)
		, EMapIteratorFlags_Destructive = DMibBit(3)

		, EMapIteratorFlags_ReverseConst = EMapIteratorFlags_Reverse | EMapIteratorFlags_Const
		, EMapIteratorFlags_BidirectionalConst = EMapIteratorFlags_Bidirectional | EMapIteratorFlags_Const
		, EMapIteratorFlags_BidirectionalReverse = EMapIteratorFlags_Bidirectional | EMapIteratorFlags_Reverse
		, EMapIteratorFlags_BidirectionalReverseConst = EMapIteratorFlags_Bidirectional | EMapIteratorFlags_Reverse | EMapIteratorFlags_Const
		, EMapIteratorFlags_ReverseDestructive = EMapIteratorFlags_Reverse | EMapIteratorFlags_Destructive
		, EMapIteratorFlags_BidirectionalDestructive = EMapIteratorFlags_Bidirectional | EMapIteratorFlags_Destructive
		, EMapIteratorFlags_BidirectionalReverseDestructive = EMapIteratorFlags_Bidirectional | EMapIteratorFlags_Reverse | EMapIteratorFlags_Destructive
	};
}

namespace NMib::NContainer::NPrivate
{
	template <typename t_CType>
	struct TCIsMap
	{
		static constexpr bool mc_bValue = false;
	};

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	struct TCIsMap<TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>>
	{
		static constexpr bool mc_bValue = true;
	};

	template <typename t_CKey, typename t_CValue>
	struct TCMapUserData
	{
		using CType = t_CValue;
	};

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	struct TCMapIterator;

	template <typename t_CNode>
	struct TCMapMapper;

	template <typename t_CMap>
	struct TCMapConditionalMapper;

	template <typename t_CDestination, bool t_bIsRef>
	struct TCMapCopy
	{
		using CNode = typename t_CDestination::CNodeDestructive;

		template <typename tf_CSource>
		static void fs_CopyAll(t_CDestination &_Map, tf_CSource &&_Other)
		{
			if (_Other.f_IsEmpty())
				return;

			_Map.mp_Tree.f_CopyTree
				(
					_Other.mp_Tree
					, [&](auto const &_Other) -> CNode *
					{
						auto Allocation = _Map.mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
						auto *pReturn = new(Allocation.m_pMemory) CNode(_Other.mp_Key, _Other.f_Value());
						Allocation.f_Claim();
						return pReturn;
					}
				)
			;
		}

		template <typename tf_CSource>
		static void fs_MoveAll(t_CDestination &_Map, tf_CSource &&_Other)
		{
			if (_Other.f_IsEmpty())
				return;

			_Map.mp_Tree.f_MoveTree
				(
					_Other.mp_Tree
					, [&](auto *_pOther) -> CNode *
					{
						auto Allocation = _Map.mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
						auto *pReturn = new(Allocation.m_pMemory) CNode(fg_Move(_pOther->mp_Key), fg_Move(*_pOther).f_Value());
						Allocation.f_Claim();

						fg_DeleteObjectDefiniteType(_Other.mp_Allocator, _pOther);

						return pReturn;
					}
				)
			;
		}

		template <typename tf_CSource>
		static void fs_CopyAddAll(t_CDestination &_Map, tf_CSource &&_Other)
		{
			auto iSource = _Other.f_GetIterator();
			while (iSource)
			{
				if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey(), _Map.mp_Compare))
				{
					++iSource;
					continue;
				}
				break;
			}
			if (!iSource)
				return;

			_Map.mp_Allocator.f_AllocBatch
				(
					sizeof(CNode)
					, alignof(CNode)
					, [&](void * _pAlloc, mint _Size) -> bool
					{
						auto Cleanup = _Map.mp_Allocator.f_MakeSafe(_pAlloc, _Size);
						auto pData = (CNode *)_pAlloc;
						new((void *)pData) CNode(iSource.f_GetKey(), *iSource);
						Cleanup.f_Claim();
						_Map.mp_Tree.f_Insert(pData, _Map.mp_Compare);

						++iSource;
						while (iSource)
						{
							if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey(), _Map.mp_Compare))
							{
								++iSource;
								continue;
							}
							break;
						}
						return iSource;
					}
				)
			;
		}

		template <typename tf_CSource>
		static void fs_MoveAddAll(t_CDestination &_Map, tf_CSource &&_Other)
		{
			_Other.f_ExtractAll
				(
					[&](auto &&_Handle)
					{
						if (_Map.mp_Tree.f_FindEqual(_Handle.f_Key(), _Map.mp_Compare))
							return;

						auto Allocation = _Map.mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
						auto *pData = new(Allocation.m_pMemory) CNode(fg_Move(_Handle.f_Key()), fg_Move(_Handle.f_Value()));
						Allocation.f_Claim();
						_Map.mp_Tree.f_Insert(pData, _Map.mp_Compare);
					}
				)
			;
		}
	};

	template <typename t_CDestination>
	struct TCMapCopy<t_CDestination, true>
	{
		using CNode = typename t_CDestination::CNode;

		template <typename tf_CSource>
		static void fs_CopyAll(t_CDestination &_Map, tf_CSource &&_Other)
		{
			if (_Other.f_IsEmpty())
				return;

			_Map.mp_Tree.f_CopyTree
				(
					_Other.mp_Tree
					, [&](auto const &_Other) -> CNode *
					{
						auto Allocation = _Map.mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
						auto *pReturn = new(Allocation.m_pMemory) CNode(_Other.mp_Key, fg_AutoConstCast(_Other.f_Value()));
						Allocation.f_Claim();
						return pReturn;
					}
				)
			;
		}

		template <typename tf_CSource>
		static void fs_CopyAddAll(t_CDestination &_Map, tf_CSource &&_Other)
		{
			auto iSource = _Other.f_GetIterator();
			while (iSource)
			{
				if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey(), _Map.mp_Compare))
				{
					++iSource;
					continue;
				}
				break;
			}
			if (!iSource)
				return;

			_Map.mp_Allocator.f_AllocBatch
				(
					sizeof(CNode)
					, alignof(CNode)
					, [&](void * _pAlloc, mint _Size) -> bool
					{
						auto Cleanup = _Map.mp_Allocator.f_MakeSafe(_pAlloc, _Size);
						auto pData = (CNode *)_pAlloc;
						new((void *)pData) CNode(iSource.f_GetKey(), fg_AutoConstCast(*iSource));
						Cleanup.f_Claim();
						_Map.mp_Tree.f_Insert(pData, _Map.mp_Compare);

						++iSource;
						while (iSource)
						{
							if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey(), _Map.mp_Compare))
							{
								++iSource;
								continue;
							}
							break;
						}
						return iSource;
					}
				)
			;
		}

		template <typename tf_CSource>
		static void fs_MoveAll(t_CDestination &_Map, tf_CSource &&_Other)
		{
			return fs_CopyAll(_Map, fg_Forward<tf_CSource>(_Other));
		}

		template <typename tf_CSource>
		static void fs_MoveAddAll(t_CDestination &_Map, tf_CSource &&_Other)
		{
			return fs_CopyAddAll(_Map, fg_Forward<tf_CSource>(_Other));
		}
	};
}
