// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	struct TCMap;

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	struct TCSet;
}

namespace NMib::NContainer::NPrivate
{
	template <typename t_CKey, typename t_CValue>
	struct TCMapUserData
	{
		using CType = t_CValue;
	};

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	struct TCMapIterator;

	template <typename t_CNode>
	struct TCMapMapper;

	template <typename t_CMap>
	struct TCMapConditionalMapper;

	template <typename t_CDestination, bool t_bIsRef>
	struct TCMapCopy
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
						auto *pReturn = new(Allocation.m_pMemory) CNode(_Other.m_Key, _Other.f_Value());
						Allocation.f_Claim();
						return pReturn;
					}
				)
			;
		}

		template <typename tf_CSource>
		static void fs_MoveAll(t_CDestination &_Map, tf_CSource && _Other)
		{
			if (_Other.f_IsEmpty())
				return;

			_Map.mp_Tree.f_MoveTree
				(
					_Other.mp_Tree
					, [&](auto *_pOther) -> CNode *
					{
						auto Allocation = _Map.mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
						auto *pReturn = new(Allocation.m_pMemory) CNode(fg_Move(_pOther->m_Key), fg_Move(_pOther->f_Value()));
						Allocation.f_Claim();

						fg_DeleteObjectDefiniteType(_Map.mp_Allocator, _pOther);

						return pReturn;
					}
				)
			;
		}

		template <typename tf_CSource>
		static void fs_CopyAddAll(t_CDestination &_Map, tf_CSource && _Other)
		{
			auto iSource = _Other.f_GetIterator();
			while (iSource)
			{
				if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey()))
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
						_Map.mp_Tree.f_Insert(pData);

						++iSource;
						while (iSource)
						{
							if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey()))
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
		static void fs_MoveAddAll(t_CDestination &_Map, tf_CSource && _Other)
		{
			auto iSource = _Other.f_GetIterator();
			while (iSource)
			{
				if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey()))
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
						new((void *)pData) CNode(iSource.f_GetKey(), fg_Move(*iSource));
						Cleanup.f_Claim();
						_Map.mp_Tree.f_Insert(pData);

						++iSource;
						while (iSource)
						{
							if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey()))
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

	};

	template <typename t_CDestination>
	struct TCMapCopy<t_CDestination, true>
	{
		using CNode = typename t_CDestination::CNode;

		template <typename tf_CSource>
		static void fs_CopyAll(t_CDestination &_Map, tf_CSource && _Other)
		{
			if (_Other.f_IsEmpty())
				return;

			_Map.mp_Tree.f_CopyTree
				(
					_Other.mp_Tree
					, [&](auto const &_Other) -> CNode *
					{
						auto Allocation = _Map.mp_Allocator.f_AllocSafe(sizeof(CNode), alignof(CNode));
						auto *pReturn = new(Allocation.m_pMemory) CNode(_Other.m_Key, fg_AutoConstCast(_Other.f_Value()));
						Allocation.f_Claim();
						return pReturn;
					}
				)
			;
		}

		template <typename tf_CSource>
		static void fs_CopyAddAll(t_CDestination &_Map, tf_CSource && _Other)
		{
			auto iSource = _Other.f_GetIterator();
			while (iSource)
			{
				if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey()))
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
						_Map.mp_Tree.f_Insert(pData);

						++iSource;
						while (iSource)
						{
							if (_Map.mp_Tree.f_FindEqual(iSource.f_GetKey()))
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
		static void fs_MoveAll(t_CDestination &_Map, tf_CSource && _Other)
		{
			return fs_CopyAll(_Map, fg_Forward<tf_CSource>(_Other));
		}

		template <typename tf_CSource>
		static void fs_MoveAddAll(t_CDestination &_Map, tf_CSource && _Other)
		{
			return fs_CopyAddAll(_Map, fg_Forward<tf_CSource>(_Other));
		}
	};
}
