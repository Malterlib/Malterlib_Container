// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Intrusive/AVLTree>

namespace NMib::NContainer
{
	class CMapNoData
	{
	public:
		bint operator == (CMapNoData const &_Other) const
		{
			return true;
		}

		bint operator < (CMapNoData const &_Other) const
		{
			return false;
		}

	};

	class CMapSet
	{
	public:
	};

	struct CMapTreeMemberBase
	{
		NIntrusive::TCAVLLink<> m_Link;
	};

	template <typename t_CKey, typename t_CData>
	class TCMapTreeMember : public CMapTreeMemberBase
	{
	public:

		inline_small TCMapTreeMember()
		{
		}
		template <typename tf_CKey, typename... tfp_CArg>
		inline_small TCMapTreeMember(tf_CKey &&_Key, tfp_CArg && ... p_Args)
			: m_Key(fg_Forward<tf_CKey>(_Key))
			, m_Data(fg_Forward<tfp_CArg>(p_Args)...)
		{
		}

		t_CKey m_Key;
		t_CData m_Data;

		inline_small static mint fs_GetOffset()
		{
			return DMibPOffsetOf(TCMapTreeMember, m_Data);
		}

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const
		{
			_Stream << m_Key;
			_Stream << m_Data;
		}

		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream)
		{
			_Stream >> m_Key;
			_Stream >> m_Data;
		}

		inline_small t_CData &f_GetData()
		{
			return m_Data;
		}
		inline_small const t_CData &f_GetData() const
		{
			return m_Data;
		}

		inline_small t_CKey &f_GetKey()
		{
			return m_Key;
		}
		inline_small const t_CKey &f_GetKey() const
		{
			return m_Key;
		}
	};

	template <typename t_CKey>
	class TCMapTreeMember<t_CKey, CMapNoData> : public CMapTreeMemberBase
	{
	public:

		inline_small TCMapTreeMember()
		{
		}
		template <typename tf_CKey>
		inline_small TCMapTreeMember(tf_CKey &&_Key)
			: m_Key(fg_Forward<tf_CKey>(_Key))
		{
		}

		template <typename tf_CKey, typename tf_CArg0>
		inline_small TCMapTreeMember(tf_CKey &&_Key, tf_CArg0 &&_Arg0)
			: m_Key(fg_Forward<tf_CKey>(_Key))
		{
		}

		inline_small static mint fs_GetOffset()
		{
			return 0;
		}

		t_CKey m_Key;

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const
		{
			_Stream << m_Key;
		}

		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream)
		{
			_Stream >> m_Key;
		}

		inline_small CMapNoData &f_GetData()
		{
			return *((CMapNoData *)this);
		}
		inline_small const CMapNoData &f_GetData() const
		{
			return *((const CMapNoData *)this);
		}

		inline_small t_CKey &f_GetKey()
		{
			return m_Key;
		}
		inline_small const t_CKey &f_GetKey() const
		{
			return m_Key;
		}

	};

	template <typename t_CKey>
	class TCMapTreeMember<t_CKey, CMapSet> : public CMapTreeMemberBase
	{
	public:

		inline_small TCMapTreeMember()
		{
		}
		template <typename tf_CKey>
		inline_small TCMapTreeMember(tf_CKey &&_Key)
			: m_Key(fg_Forward<tf_CKey>(_Key))
		{
		}

		template <typename tf_CKey, typename tf_CArg0>
		inline_small TCMapTreeMember(tf_CKey &&_Key, tf_CArg0 &&_Arg0)
			: m_Key(fg_Forward<tf_CKey>(_Key))
		{
		}

		inline_small static mint fs_GetOffset()
		{
			return DMibPOffsetOf(TCMapTreeMember, m_Key);
		}

		t_CKey m_Key;

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const
		{
			_Stream << m_Key;
		}

		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream)
		{
			_Stream >> m_Key;
		}

		inline_small t_CKey &f_GetData()
		{
			return m_Key;
		}
		inline_small const t_CKey &f_GetData() const
		{
			return m_Key;
		}

		inline_small t_CKey &f_GetKey()
		{
			return m_Key;
		}
		inline_small const t_CKey &f_GetKey() const
		{
			return m_Key;
		}

	};

	template <typename t_CKey, typename t_CData = CMapNoData, typename t_CCompare = NMib::CSort_Default, typename t_CAllocator = NMib::NMemory::CAllocator_Heap >
	class TCMap;

	class CMapWasCreated
	{
		template <typename t_CKey, typename t_CData, typename t_CCompare, typename t_CAllocator> friend class TCMap;
		bint mp_bWasCreated;
	private:
		operator bool () const
		{
			return mp_bWasCreated;
		}
	};

	template <typename t_CReturnType>
	class TCMapResult
	{
		template <typename t_CKey, typename t_CData, typename t_CCompare, typename t_CAllocator> friend class TCMap;

		t_CReturnType mp_Return;
		bint mp_bWasCreated;
		inline_small TCMapResult(t_CReturnType _Return, bint _bWasCreated)
			: mp_Return(_Return)
			, mp_bWasCreated(_bWasCreated)
		{
		}
	public:

		inline_small bint f_WasCreated() const
		{
			return mp_bWasCreated;
		}

		inline_small operator t_CReturnType ()
		{
			return mp_Return;
		}

		inline_small t_CReturnType f_GetResult()
		{
			return mp_Return;
		}

		inline_small t_CReturnType operator *()
		{
			return mp_Return;
		}

	};

	template <typename t_CKey, typename t_CData, typename t_CCompare, typename t_CAllocator>
	class TCMap
	{
	public:
		typedef typename NTraits::TCRemoveReference<t_CData>::CType CData;
		typedef TCMapTreeMember<t_CKey, t_CData> CMapTreeMember; // This needs to be public until we have C++14 usable for generic lambdas
	private:


		class CMapTreeMemberCompare
		{
		public:

			bint operator () (CMapTreeMember const &_Left, CMapTreeMember const &_Right) const
			{
				return t_CCompare()(_Left.m_Key, _Right.m_Key);
			}

			bint operator () (CMapTreeMember &_Left, CMapTreeMember &_Right) const
			{
				return t_CCompare()(_Left.m_Key, _Right.m_Key);
			}

			template <typename tf_CKey>
			bint operator () (CMapTreeMember const &_Left, tf_CKey const &_Right) const
			{
				return t_CCompare()(_Left.m_Key, _Right);
			}

			template <typename tf_CKey>
			bint operator () (tf_CKey const &_Left, CMapTreeMember const &_Right) const
			{
				return t_CCompare()(_Left, _Right.m_Key);
			}
		};

		typedef typename NTraits::TCRemoveReference<typename TCChooseType<NTraits::TCIsSame<CMapSet, t_CData>::mc_Value, t_CKey, t_CData>::CType>::CType CUserData;

		enum
		{
			mcp_bIsReference = NTraits::TCIsReference<t_CData>::mc_Value
		};

		class CAllocatorDisable : public NMemory::CAllocator_Base
		{
		public:
		};

		typedef NIntrusive::TCAVLTree<&CMapTreeMemberBase::m_Link, CMapTreeMemberCompare, CAllocatorDisable, CMapTreeMember> CAVLTree;

		class CLocalData : public t_CAllocator
		{
		public:
			CLocalData()
			{
			}
			template <typename... tfp_CParams>
			CLocalData(NMemory::CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
				: t_CAllocator(fg_Forward<tfp_CParams>(p_Params)...)
			{
			}
			CLocalData(CLocalData &&_Other)
				: t_CAllocator(fg_Move(_Other))
				, m_Tree(fg_Move(_Other.m_Tree))
			{
			}
			CLocalData & operator = (CLocalData &&_Other)
			{
				*((t_CAllocator *)this) = fg_Move(*((t_CAllocator *)(&_Other)));
				m_Tree = fg_Move(_Other.m_Tree);
				return *this;
			}
			CAVLTree m_Tree;
		};

		CLocalData mp_Data;

		template <bint t_bIsRef, typename t_CDummy = int>
		class TCMapCopy
		{
		public:
			template <typename tf_CSource>
			static void fs_CopyAll(TCMap &_Map, tf_CSource && _Other)
			{
				if (_Other.f_IsEmpty())
					return;

				auto iSource = _Other.f_GetIterator();

				_Map.mp_Data.f_AllocBatch
					(
						sizeof(CMapTreeMember)
						, NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value
						, [&](void * _pAlloc, mint _Size) -> bool
						{
							auto Cleanup = _Map.mp_Data.f_MakeSafe(_pAlloc, _Size);
							auto pData = (CMapTreeMember *)_pAlloc;
							new((void *)pData) CMapTreeMember(iSource.f_GetKey(), *iSource);
							Cleanup.f_Claim();
							_Map.mp_Data.m_Tree.f_Insert(pData);
							++iSource;
							return iSource;
						}
					)
				;
			}

			template <typename tf_CSource>
			static void fs_MoveAll(TCMap &_Map, tf_CSource && _Other)
			{
				if (_Other.f_IsEmpty())
					return;

				auto iSource = _Other.f_GetIterator();

				_Map.mp_Data.f_AllocBatch
					(
						sizeof(CMapTreeMember)
						, NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value
						, [&](void * _pAlloc, mint _Size) -> bool
						{
							auto Cleanup = _Map.mp_Data.f_MakeSafe(_pAlloc, _Size);
							auto pData = (CMapTreeMember *)_pAlloc;
							new((void *)pData) CMapTreeMember(iSource.f_GetKey(), fg_Move(*iSource));
							Cleanup.f_Claim();
							_Map.mp_Data.m_Tree.f_Insert(pData);
							++iSource;
							return iSource;
						}
					)
				;
			}

			template <typename tf_CSource>
			static void fs_CopyAddAll(TCMap &_Map, tf_CSource && _Other)
			{
				auto iSource = _Other.f_GetIterator();
				while (iSource)
				{
					if (_Map.mp_Data.m_Tree.f_FindEqual(iSource.f_GetKey()))
					{
						++iSource;
						continue;
					}
					break;
				}
				if (!iSource)
					return;

				_Map.mp_Data.f_AllocBatch
					(
						sizeof(CMapTreeMember)
						, NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value
						, [&](void * _pAlloc, mint _Size) -> bool
						{
							auto Cleanup = _Map.mp_Data.f_MakeSafe(_pAlloc, _Size);
							auto pData = (CMapTreeMember *)_pAlloc;
							new((void *)pData) CMapTreeMember(iSource.f_GetKey(), *iSource);
							Cleanup.f_Claim();
							_Map.mp_Data.m_Tree.f_Insert(pData);

							++iSource;
							while (iSource)
							{
								if (_Map.mp_Data.m_Tree.f_FindEqual(iSource.f_GetKey()))
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
			static void fs_MoveAddAll(TCMap &_Map, tf_CSource && _Other)
			{
				auto iSource = _Other.f_GetIterator();
				while (iSource)
				{
					if (_Map.mp_Data.m_Tree.f_FindEqual(iSource.f_GetKey()))
					{
						++iSource;
						continue;
					}
					break;
				}
				if (!iSource)
					return;

				_Map.mp_Data.f_AllocBatch
					(
						sizeof(CMapTreeMember)
						, NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value
						, [&](void * _pAlloc, mint _Size) -> bool
						{
							auto Cleanup = _Map.mp_Data.f_MakeSafe(_pAlloc, _Size);
							auto pData = (CMapTreeMember *)_pAlloc;
							new((void *)pData) CMapTreeMember(iSource.f_GetKey(), fg_Move(*iSource));
							Cleanup.f_Claim();
							_Map.mp_Data.m_Tree.f_Insert(pData);

							++iSource;
							while (iSource)
							{
								if (_Map.mp_Data.m_Tree.f_FindEqual(iSource.f_GetKey()))
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

		template <typename t_CDummy>
		class TCMapCopy<true, t_CDummy>
		{
		public:
			template <typename tf_CSource>
			static void fs_CopyAll(TCMap &_Map, tf_CSource && _Other)
			{
				if (_Other.f_IsEmpty())
					return;

				auto iSource = _Other.f_GetIterator();

				_Map.mp_Data.f_AllocBatch
					(
						sizeof(CMapTreeMember)
						, NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value
						, [&](void * _pAlloc, mint _Size) -> bool
						{
							auto Cleanup = _Map.mp_Data.f_MakeSafe(_pAlloc, _Size);
							auto pData = (CMapTreeMember *)_pAlloc;
							new((void *)pData) CMapTreeMember(iSource.f_GetKey(), *const_cast<CData *>(&*iSource));
							Cleanup.f_Claim();
							_Map.mp_Data.m_Tree.f_Insert(pData);
							++iSource;
							return iSource;
						}
					)
				;
			}

			template <typename tf_CSource>
			static void fs_CopyAddAll(TCMap &_Map, tf_CSource && _Other)
			{
				auto iSource = _Other.f_GetIterator();
				while (iSource)
				{
					if (_Map.mp_Data.m_Tree.f_FindEqual(iSource.f_GetKey()))
					{
						++iSource;
						continue;
					}
					break;
				}
				if (!iSource)
					return;

				_Map.mp_Data.f_AllocBatch
					(
						sizeof(CMapTreeMember)
						, NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value
						, [&](void * _pAlloc, mint _Size) -> bool
						{
							auto Cleanup = _Map.mp_Data.f_MakeSafe(_pAlloc, _Size);
							auto pData = (CMapTreeMember *)_pAlloc;
							new((void *)pData) CMapTreeMember(iSource.f_GetKey(), *const_cast<CData *>(&*iSource));
							Cleanup.f_Claim();
							_Map.mp_Data.m_Tree.f_Insert(pData);

							++iSource;
							while (iSource)
							{
								if (_Map.mp_Data.m_Tree.f_FindEqual(iSource.f_GetKey()))
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
			static void fs_MoveAll(TCMap &_Map, tf_CSource && _Other)
			{
				return fs_CopyAll(_Map, fg_Forward<tf_CSource>(_Other));
			}

			template <typename tf_CSource>
			static void fs_MoveAddAll(TCMap &_Map, tf_CSource && _Other)
			{
				return fs_CopyAddAll(_Map, fg_Forward<tf_CSource>(_Other));
			}
		};

		template <typename tf_CSource>
		inline_always void fp_CopyAll(tf_CSource && _Other)
		{
			return TCMapCopy<mcp_bIsReference && !NTraits::TCIsConst<CData>::mc_Value>::fs_CopyAll(*this, fg_Forward<tf_CSource>(_Other));
		}

		template <typename tf_CSource>
		inline_always void fp_MoveAll(tf_CSource && _Other)
		{
			return TCMapCopy<mcp_bIsReference && !NTraits::TCIsConst<CData>::mc_Value>::fs_MoveAll(*this, fg_Forward<tf_CSource>(_Other));
		}

		template <typename tf_CSource>
		inline_always void fp_CopyAddAll(tf_CSource && _Other)
		{
			return TCMapCopy<mcp_bIsReference && !NTraits::TCIsConst<CData>::mc_Value>::fs_CopyAddAll(*this, fg_Forward<tf_CSource>(_Other));
		}
		template <typename tf_CSource>
		inline_always void fp_MoveAddAll(tf_CSource && _Other)
		{
			return TCMapCopy<mcp_bIsReference && !NTraits::TCIsConst<CData>::mc_Value>::fs_MoveAddAll(*this, fg_Forward<tf_CSource>(_Other));
		}

	public:

		struct CMapper
		{
			void *m_pMemory;
			CMapper(void * _pMemory)
				: m_pMemory(_pMemory)
			{
			}
			template <typename tf_CKey, typename... tfp_CArg>
			CUserData &operator ()(tf_CKey &&_Key, tfp_CArg && ... p_Args)
			{
				return (new(m_pMemory) CMapTreeMember(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CArg>(p_Args)...))->f_GetData();
			}
		};

		struct CConditionalMapper
		{
			void *m_pMemory;
			TCMap & m_Map;
			bool m_bAddFailed;
			CConditionalMapper(void * _pMemory, TCMap & _Map)
				: m_pMemory(_pMemory)
				, m_Map(_Map)
				, m_bAddFailed(false)
			{
			}
			template <typename tf_CKey, typename... tfp_CArg>
			TCMapResult<CUserData &> operator ()(tf_CKey &&_Key, tfp_CArg && ... p_Args)
			{
				if (auto pOld = m_Map.f_FindEqual(fg_Forward<tf_CKey>(_Key)))
				{
					m_bAddFailed = true;
					return TCMapResult<CUserData &>(*pOld, false);
				}
				m_bAddFailed = false;
				return TCMapResult<CUserData &>((new(m_pMemory) CMapTreeMember(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CArg>(p_Args)...))->f_GetData(), true);
			}
		};

		template <bint t_bReverse, bint t_bConst, bint t_bBidirectional>
		friend class TCIterator;

		template <bint t_bReverse, bint t_bConst, bint t_bBidirectional>
		class TCIterator
		{
			friend class TCMap;

			typedef typename NTraits::TCSetConst<TCMap, t_bConst>::CType CMapQualified;
			typedef typename TCChooseType<t_bConst, typename NTraits::TCAddConst<CUserData>::CType, CUserData>::CType CUserDataQualified;

			typename CAVLTree::CIterator mp_Iter;
			CMapQualified *mp_pMap;

		public:

			TCIterator()
			{
				mp_pMap = nullptr;
			}

			TCIterator(CMapQualified &_Map)
			{
				mp_pMap = &_Map;
				if (t_bReverse)
					mp_Iter.f_StartBackward(_Map.mp_Data.m_Tree);
				else
					mp_Iter.f_StartForward(_Map.mp_Data.m_Tree);
			}

			template <typename tf_CKey>
			TCIterator(CMapQualified &_Map, const tf_CKey &_Key)
			{
				static_assert(!t_bReverse, "Not supported for reverse iterators");
				mp_pMap = &_Map;
				mp_Iter.f_InitForSearch(_Map.mp_Data.m_Tree);
				mp_Iter.f_FindEqualForward(fg_Forward<tf_CKey>(_Key));
			}

			TCIterator &operator = (CMapQualified &_Map)
			{
				mp_pMap = &_Map;
				if (t_bReverse)
					mp_Iter.f_StartBackward(_Map.mp_Data.m_Tree);
				else
					mp_Iter.f_StartForward(_Map.mp_Data.m_Tree);
				return *this;
			}


			inline_medium mint f_GetLen() const
			{
				return mp_Iter.f_GetLen();
			}

			inline_medium void f_Next()
			{
				if (t_bBidirectional)
				{
					if (t_bReverse)
						mp_Iter.f_PrevBidirectional();
					else
						mp_Iter.f_NextBidirectional();
				}
				else
				{
					if (t_bReverse)
						mp_Iter.f_Prev();
					else
						mp_Iter.f_Next();
				}
			}
			inline_medium void f_Prev()
			{
				static_assert(t_bBidirectional, "Only available when bidirectional");

				if (t_bReverse)
					mp_Iter.f_NextBidirectional();
				else
					mp_Iter.f_PrevBidirectional();

			}
			void f_Remove()
			{
#ifdef DCompiler_MSVC_Workaround
				auto *pToDelete = mp_Iter.f_GetCurrent();
				mp_Iter.f_Next();
				auto *pToFind = mp_Iter.f_GetCurrent();
				mp_pMap->mp_Data.m_Tree.f_Remove(pToDelete, CMapTreeMemberCompare());
				fg_DeleteObjectDefiniteType((t_CAllocator &)mp_pMap->mp_Data, pToDelete);
				if (pToFind)
				{
					mp_Iter.f_InitForSearch(&mp_pMap->mp_Data.m_Tree);
					mp_Iter.f_FindEqualForward(*pToFind);
				}
#else
				mp_Iter.f_DeleteAllocatorDefiniteType(mp_pMap->mp_Data.m_Tree, CMapTreeMemberCompare(), (t_CAllocator &)mp_pMap->mp_Data);
#endif
			}

			inline_small CUserDataQualified *f_GetCurrent() const
			{
				CMapTreeMember *pMember = mp_Iter;
				if (pMember)
					return &pMember->f_GetData();
				return nullptr;
			}

			const t_CKey &f_GetKey() const
			{
				CMapTreeMember *pMember = mp_Iter;
				return pMember->f_GetKey();
			}

			inline_small operator CUserDataQualified *() const
			{
				return f_GetCurrent();
			}

			inline_small CUserDataQualified * operator ->() const
			{
				return f_GetCurrent();
			}

			inline_small void operator ++()
			{
				f_Next();
			}

			inline_small void operator --()
			{
				static_assert(t_bBidirectional, "Only available when bidirectional");
				f_Prev();
			}

			explicit operator bool () const
			{
				return f_GetCurrent() != nullptr;
			}
		};

		typedef TCIterator<false, false, false> CIterator;
		typedef TCIterator<false, true, false> CIteratorConst;
		typedef TCIterator<true, false, false> CIteratorReverse;
		typedef TCIterator<true, true, false> CIteratorReverseConst;
		typedef TCIterator<false, false, true> CIteratorBidirectional;
		typedef TCIterator<false, true, true> CIteratorBidirectionalConst;
		typedef TCIterator<true, false, true> CIteratorBidirectionalReverse;
		typedef TCIterator<true, true, true> CIteratorBidirectionalReverseConst;


		TCMap() noexcept
		{
			//DMibDTrace("sizeof(CMapTreeMember) = {}" DMibNewLine, sizeof(CMapTreeMember));
		}

		~TCMap()
		{
			f_Clear();
		}
		void f_Clear()
		{
			mp_Data.m_Tree.f_DeleteAllAllocatorDefiniteType((t_CAllocator &)mp_Data);
		}

		TCMap &operator = (const TCMap &_Other)
		{
			DMibSafeCheck(this != &_Other, "Must not copy same object to itself");
			f_Clear();
			fp_CopyAll(_Other);

			return *this;
		}

		TCMap(const TCMap &_Other)
		{
			auto Cleanup = g_OnScopeExit > [&]
				{
					f_Clear();
				}
			;

			fp_CopyAll(_Other);

			Cleanup.f_Clear();
		}

		TCMap(TCMap &_Other)
		{
			auto Cleanup = g_OnScopeExit > [&]
				{
					f_Clear();
				}
			;

			fp_CopyAll(_Other);

			Cleanup.f_Clear();
		}

		TCMap(TCInitializerList<CMapTreeMember> const &_Values)
		{
			auto Cleanup = g_OnScopeExit > [&]
				{
					f_Clear();
				}
			;

			for (auto &Value : _Values)
				(*this)(Value.m_Key, Value.m_Data);

			Cleanup.f_Clear();
		}

		template <typename... tfp_CParams>
		TCMap(NMemory::CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
			: mp_Data(_Allocator, fg_Forward<tfp_CParams>(p_Params)...)
		{
		}


		template <typename tf_CKey, typename tf_CData, typename tf_CCompare, typename tf_CAllocator>
		TCMap(const TCMap<tf_CKey, tf_CData, tf_CCompare, tf_CAllocator> &_Other)
		{
			auto Cleanup = g_OnScopeExit > [&]
				{
					f_Clear();
				}
			;

			fp_CopyAll(_Other);

			Cleanup.f_Clear();
		}

		template <typename tf_CKey, typename tf_CData, typename tf_CCompare, typename tf_CAllocator>
		TCMap &operator = (const TCMap<tf_CKey, tf_CData, tf_CCompare, tf_CAllocator> &_Other)
		{
			f_Clear();
			fp_CopyAll(_Other);
			return *this;
		}

		template <typename tf_CKey, typename tf_CData, typename tf_CCompare, typename tf_CAllocator>
		TCMap(TCMap<tf_CKey, tf_CData, tf_CCompare, tf_CAllocator> &_Other)
		{
			auto Cleanup = g_OnScopeExit > [&]
				{
					f_Clear();
				}
			;

			fp_CopyAll(_Other);

			Cleanup.f_Clear();
		}

		template <typename tf_CKey, typename tf_CData, typename tf_CCompare, typename tf_CAllocator>
		TCMap &operator = (TCMap<tf_CKey, tf_CData, tf_CCompare, tf_CAllocator> &_Other)
		{
			f_Clear();
			fp_CopyAll(_Other);
			return *this;
		}

		template <typename tf_CKey, typename tf_CData, typename tf_CCompare, typename tf_CAllocator>
		TCMap(TCMap<tf_CKey, tf_CData, tf_CCompare, tf_CAllocator> &&_Other)
		{
			auto Cleanup = g_OnScopeExit > [&]
				{
					f_Clear();
				}
			;

			fp_MoveAll(_Other);

			Cleanup.f_Clear();
		}

		template <typename tf_CKey, typename tf_CData, typename tf_CCompare, typename tf_CAllocator>
		TCMap &operator = (TCMap<tf_CKey, tf_CData, tf_CCompare, tf_CAllocator> &&_Other)
		{
			f_Clear();
			fp_MoveAll(_Other);
			return *this;
		}


		TCMap(TCMap &&_Other)
			: mp_Data(fg_Move(_Other.mp_Data))
		{

		}

		TCMap &operator = (TCMap &&_Other)
		{
			f_Clear();
			mp_Data = fg_Move(_Other.mp_Data);
			return *this;
		}

		TCMap &operator += (const TCMap &_Other)
		{
			DMibSafeCheck(this != &_Other, "Must not add same object to itself");
			fp_CopyAddAll(_Other);
			return *this;
		}

		TCMap &operator += (TCMap &&_Other)
		{
			DMibSafeCheck(this != &_Other, "Must not add same object to itself");
			fp_MoveAddAll(_Other);
			return *this;
		}

		TCMap &operator -= (const TCMap &_Other)
		{
			DMibSafeCheck(this != &_Other, "Must not remove same object from itself");
			CIteratorConst Iter = _Other.f_GetIterator();
			while (Iter)
			{
				f_Remove(Iter.f_GetKey());
				++Iter;
			}

			return *this;
		}

		template <typename tf_CKey>
		CUserData& operator[] (tf_CKey &&_Key)
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindEqual(_Key);
			if (!pData)
			{
				auto Memory = mp_Data.f_AllocSafe(sizeof(CMapTreeMember), NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value);
				pData = (CMapTreeMember *)Memory.m_pMemory;
				new((void *)pData) CMapTreeMember(fg_Forward<tf_CKey>(_Key));
				Memory.f_Claim();
				mp_Data.m_Tree.f_Insert(pData);
			}
			return pData->f_GetData();
		}

		template <typename tf_CKey>
		const CUserData& operator[] (tf_CKey&& _Key) const
		{
			const CMapTreeMember *pData = mp_Data.m_Tree.f_FindEqual(_Key);
			// Will return a null ref if data does not exist (is this valid in C++?)
			if (pData)
				return pData->f_GetData();
			else
				DMibError("Key not found");
		}

		template <typename tf_CKey>
		CUserData& f_Map (tf_CKey &&_Key)
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindEqual(_Key);
			if (!pData)
			{
				auto Memory = mp_Data.f_AllocSafe(sizeof(CMapTreeMember), NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value);
				pData = (CMapTreeMember *)Memory.m_pMemory;
				new((void *)pData) CMapTreeMember(fg_Forward<tf_CKey>(_Key));
				Memory.f_Claim();
				mp_Data.m_Tree.f_Insert(pData);
			}
			return pData->f_GetData();
		}

		template <typename tf_CKey, typename... tfp_CArgs>
		CUserData& f_Map (tf_CKey &&_Key, bint &_bCreated, tfp_CArgs && ... p_Args)
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindEqual(_Key);
			if (!pData)
			{
				_bCreated = true;
				auto Memory = mp_Data.f_AllocSafe(sizeof(CMapTreeMember), NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value);
				pData = (CMapTreeMember *)Memory.m_pMemory;
				new((void *)pData) CMapTreeMember(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CArgs>(p_Args)...);
				Memory.f_Claim();
				mp_Data.m_Tree.f_Insert(pData);
			}
			else
				_bCreated = false;
			return pData->f_GetData();
		}

		template <typename tf_CKey, typename... tfp_CParam>
		TCMapResult<CUserData &> operator () (tf_CKey &&_Key, tfp_CParam && ... p_Params)
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindEqual(_Key);
			bint bWasCreated = false;
			if (!pData)
			{
				bWasCreated = true;
				auto Memory = mp_Data.f_AllocSafe(sizeof(CMapTreeMember), NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value);
				pData = (CMapTreeMember *)Memory.m_pMemory;
				new((void *)pData) CMapTreeMember(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CParam>(p_Params)...);
				Memory.f_Claim();
				mp_Data.m_Tree.f_Insert(pData);
			}
			return TCMapResult<CUserData &>(pData->f_GetData(), bWasCreated);
		}

		template <typename tf_CFunctor>
		void f_BatchMap(tf_CFunctor && _Functor)
		{
			mp_Data.f_AllocBatch
				(
					sizeof(CMapTreeMember)
					, NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value
					, [&](void * _pAlloc, mint _Size) -> bool
					{
						auto Cleanup = mp_Data.f_MakeSafe(_pAlloc, _Size);
						CMapper Mapper(_pAlloc);
						bool bRet = _Functor(Mapper);
						Cleanup.f_Claim();
						auto pData = (CMapTreeMember *)_pAlloc;
						mp_Data.m_Tree.f_Insert(pData);
						return bRet;
					}
				)
			;
		}

		template <typename tf_CFunctor>
		void f_BatchMapIfNotMapped(tf_CFunctor && _Functor)
		{
			mp_Data.f_AllocBatch
				(
					sizeof(CMapTreeMember)
					, NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value
					, [&](void * _pAlloc, mint _Size) -> bool
					{
						auto Cleanup = mp_Data.f_MakeSafe(_pAlloc, _Size);

						CConditionalMapper Mapper(_pAlloc, *this);
						bool bRet = _Functor(Mapper);
						auto pData = (CMapTreeMember *)_pAlloc;
						while (Mapper.m_bAddFailed)
						{
							if (!bRet)
								return false;
							bRet = _Functor(Mapper);
						}
						Cleanup.f_Claim();
						mp_Data.m_Tree.f_Insert(pData);
						return bRet;
					}
				)
			;
		}

		template <typename tf_CKey>
		const CUserData * f_FindEqual(tf_CKey &&_Key) const
		{
			const CMapTreeMember *pData = mp_Data.m_Tree.f_FindEqual(_Key);
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		template <typename tf_CKey>
		CUserData * f_FindEqual(tf_CKey &&_Key)
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindEqual(_Key);
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		template <typename tf_CKey>
		CUserData * f_FindSmallestGreaterThanEqual(tf_CKey &&_Key)
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindSmallestGreaterThanEqual(fg_Forward<tf_CKey>(_Key));
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		template <typename tf_CKey>
		const CUserData * f_FindSmallestGreaterThanEqual(tf_CKey &&_Key) const
		{
			const CMapTreeMember *pData = mp_Data.m_Tree.f_FindSmallestGreaterThanEqual(fg_Forward<tf_CKey>(_Key));
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		template <typename tf_CKey>
		CUserData * f_FindLargestLessThanEqual(tf_CKey &&_Key)
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindLargestLessThanEqual(fg_Forward<tf_CKey>(_Key));
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		template <typename tf_CKey>
		const CUserData * f_FindLargestLessThanEqual(tf_CKey &&_Key) const
		{
			const CMapTreeMember *pData = mp_Data.m_Tree.f_FindLargestLessThanEqual(fg_Forward<tf_CKey>(_Key));
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		CUserData * f_FindSmallest()
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindSmallest();
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		const CUserData * f_FindSmallest() const
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindSmallest();
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		CUserData * f_FindLargest()
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindLargest();
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		CUserData * f_FindAny()
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_GetRoot();
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}

		const CUserData * f_FindLargest() const
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindLargest();
			if (pData)
				return &pData->f_GetData();
			else
				return nullptr;
		}


		t_CKey * f_FindSmallestKey()
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindSmallest();
			if (pData)
				return &pData->f_GetKey();
			else
				return nullptr;
		}

		const t_CKey * f_FindSmallestKey() const
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindSmallest();
			if (pData)
				return &pData->f_GetKey();
			else
				return nullptr;
		}

		t_CKey * f_FindLargestKey()
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindLargest();
			if (pData)
				return &pData->f_GetKey();
			else
				return nullptr;
		}

		t_CKey * f_FindAnyKey()
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_GetRoot();
			if (pData)
				return &pData->f_GetKey();
			else
				return nullptr;
		}

		const t_CKey * f_FindLargestKey() const
		{
			CMapTreeMember *pData = mp_Data.m_Tree.f_FindLargest();
			if (pData)
				return &pData->f_GetKey();
			else
				return nullptr;
		}


		static const t_CKey &fs_GetKey(const CUserData * _pData)
		{
			static_assert(!NTraits::TCIsReference<t_CData>::mc_Value, "This function is not supported when mapping reference types");
			mint Offset = CMapTreeMember::fs_GetOffset();
			const CMapTreeMember *pMember = (const CMapTreeMember *)(((uint8 *)_pData) - Offset);
			return pMember->f_GetKey();
		}

		static const t_CKey &fs_GetKey(const CUserData &_Data)
		{
			static_assert(!NTraits::TCIsReference<t_CData>::mc_Value, "This function is not supported when mapping reference types");
			mint Offset = CMapTreeMember::fs_GetOffset();
			const CMapTreeMember *pMember = (const CMapTreeMember *)(((uint8 *)(&_Data)) - Offset);;
			return pMember->f_GetKey();
		}

		void f_Remove(CUserData * _pData)
		{
			static_assert(!NTraits::TCIsReference<t_CData>::mc_Value, "This function is not supported when mapping reference types");
			mint Offset = CMapTreeMember::fs_GetOffset();
			CMapTreeMember *pMember = (CMapTreeMember *)(((uint8 *)_pData) - Offset);
			mp_Data.m_Tree.f_Remove(pMember);
			fg_DeleteObjectDefiniteType(mp_Data, pMember);
		}

		// This only makes sense when the actual pointer of the node is used for comparion
		bool f_TryRemovePointerBasedComparison(CUserData * _pData)
		{
			static_assert(!NTraits::TCIsReference<t_CData>::mc_Value, "This function is not supported when mapping reference types");
			mint Offset = CMapTreeMember::fs_GetOffset();
			CMapTreeMember *pMember = (CMapTreeMember *)(((uint8 *)_pData) - Offset);
			if (!mp_Data.m_Tree.f_FindEqual(pMember->f_GetData()))
				return false;
			mp_Data.m_Tree.f_Remove(pMember);
			DMibFastCheck(!mp_Data.m_Tree.f_FindEqual(pMember->f_GetData()));
			fg_DeleteObjectDefiniteType(mp_Data, pMember);
			return true;
		}

		void f_ExtractAndInsert(TCMap &_Map, CUserData *_pData)
		{
			static_assert(!NTraits::TCIsReference<t_CData>::mc_Value, "This function is not supported when mapping reference types");
			mint Offset = CMapTreeMember::fs_GetOffset();
			CMapTreeMember *pMember = (CMapTreeMember *)(((uint8 *)_pData) - Offset);
			_Map.mp_Data.m_Tree.f_Remove(pMember);
			mp_Data.m_Tree.f_Insert(pMember);
		}

		template <typename tf_CKey>
		bint f_Exists(tf_CKey &&_Key) const
		{
			return mp_Data.m_Tree.f_FindEqual(_Key) != nullptr;
		}

		template <typename tf_CKey>
		bint f_Lookup(tf_CKey &&_Key, CUserData& _Data) const
		{
			CMapTreeMember *pMember = mp_Data.m_Tree.f_FindEqual(_Key);
			if (pMember)
			{
				_Data = pMember->f_GetData();
				return true;
			}
			else
				return false;
		}

		template <typename tf_CKey>
		bint f_Remove(tf_CKey &&_Key)
		{
			CMapTreeMember *pMember = mp_Data.m_Tree.f_FindEqual(_Key);
			if (pMember)
			{
				mp_Data.m_Tree.f_Remove(pMember);
				fg_DeleteObjectDefiniteType(mp_Data, pMember);
				return true;
			}
			return false;
		}

		bint f_IsEmpty() const
		{
			return mp_Data.m_Tree.f_IsEmpty();
		}

		bool f_HasOneMember() const
		{
			return mp_Data.m_Tree.f_HasOneMember();
		}

		mint f_GetLen() const
		{
			CIteratorConst Iter = *this;
			return Iter.f_GetLen();
		}

		CIteratorConst f_GetIterator() const
		{
			CIteratorConst Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter = mp_Data.m_Tree;
			return Iterator;
		}

		CIterator f_GetIterator()
		{
			CIterator Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter = mp_Data.m_Tree;
			return Iterator;
		}

		CIteratorReverseConst f_GetIteratorReverse() const
		{
			CIteratorReverseConst Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_StartBackward(mp_Data.m_Tree);
			return Iterator;
		}

		CIteratorReverse f_GetIteratorReverse()
		{
			CIteratorReverse Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_StartBackward(mp_Data.m_Tree);
			return Iterator;
		}

		CIteratorBidirectionalConst f_GetIteratorBidirectional() const
		{
			CIteratorBidirectionalConst Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter = mp_Data.m_Tree;
			return Iterator;
		}

		CIteratorBidirectional f_GetIteratorBidirectional()
		{
			CIteratorBidirectional Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter = mp_Data.m_Tree;
			return Iterator;
		}

		CIteratorBidirectionalReverseConst f_GetIteratorBidirectionalReverse() const
		{
			CIteratorBidirectionalReverseConst Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_StartBackward(mp_Data.m_Tree);
			return Iterator;
		}

		CIteratorBidirectionalReverse f_GetIteratorBidirectionalReverse()
		{
			CIteratorBidirectionalReverse Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_StartBackward(mp_Data.m_Tree);
			return Iterator;
		}


		template <typename tf_CKey>
		CIterator f_GetIterator(tf_CKey &&_Key)
		{
			CIterator Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_InitForSearch(mp_Data.m_Tree);
			Iterator.mp_Iter.f_FindEqualForward(fg_Forward<tf_CKey>(_Key));
			return Iterator;
		}

		template <typename tf_CKey>
		CIteratorConst f_GetIterator(tf_CKey &&_Key) const
		{
			CIteratorConst Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_InitForSearch(mp_Data.m_Tree);
			Iterator.mp_Iter.f_FindEqualForward(fg_Forward<tf_CKey>(_Key));
			return Iterator;
		}

		template <typename tf_CKey>
		CIterator f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key)
		{
			CIterator Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_InitForSearch(mp_Data.m_Tree);
			Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key));
			return Iterator;
		}

		template <typename tf_CKey>
		CIteratorConst f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const
		{
			CIteratorConst Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_InitForSearch(mp_Data.m_Tree);
			Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key));
			return Iterator;
		}

		template <typename tf_CKey>
		CIterator f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key)
		{
			CIterator Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_InitForSearch(mp_Data.m_Tree);
			Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key));
			return Iterator;
		}

		template <typename tf_CKey>
		CIteratorConst f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const
		{
			CIteratorConst Iterator;
			Iterator.mp_pMap = this;
			Iterator.mp_Iter.f_InitForSearch(mp_Data.m_Tree);
			Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key));
			return Iterator;
		}

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const
		{
			CIteratorConst Iter = f_GetIterator();
			mint nItems = Iter.f_GetLen();

			fg_FeedLenToStream(_Stream, nItems);

			while (Iter)
			{
				_Stream.f_Feed(*Iter.mp_Iter);
				++Iter;
			};
		}

		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream)
		{
			f_Clear();

			uint64 nItems;
			fg_ConsumeLenFromStream(_Stream, nItems);
			fg_CheckLengthLimit(_Stream, nItems);

			while (nItems)
			{
				auto Memory = mp_Data.f_AllocSafe(sizeof(CMapTreeMember), NTraits::TCAlignmentOf<CMapTreeMember>::mc_Value);
				CMapTreeMember *pData = (CMapTreeMember *)Memory.m_pMemory;
				pData = new((void *)pData) CMapTreeMember();
				Memory.f_Claim();
				auto Cleanup = g_OnScopeExit > [&]
					{
						fg_DeleteObjectDefiniteType(mp_Data, pData);
					}
				;
				pData->f_Consume(_Stream);
				if (mp_Data.m_Tree.f_FindEqual(pData->f_GetKey()))
				{
					DMibError("TCMap stream contained a duplicate key");
				}
				else
				{
					Cleanup.f_Clear();
					mp_Data.m_Tree.f_Insert(pData);
				}
				--nItems;
			}
		}

		bint operator == (const TCMap &_Other) const
		{
			CIteratorConst Iter0 = *this;
			CIteratorConst Iter1 = _Other;
			while (Iter0 && Iter1)
			{
				if (!(Iter0.f_GetKey() == Iter1.f_GetKey()))
					return false;
				if (!(*Iter0 == *Iter1))
					return false;

				++Iter0;
				++Iter1;
			}

			if (Iter0 && !Iter1)
				return false;
			if (!Iter0 && Iter1)
				return false;

			return true;
		}

		bint operator < (const TCMap &_Other) const
		{
			CIteratorConst Iter0 = *this;
			CIteratorConst Iter1 = _Other;
			while (Iter0 && Iter1)
			{
				if (Iter0.f_GetKey() < Iter1.f_GetKey())
					return true;
				if (Iter1.f_GetKey() < Iter0.f_GetKey())
					return false;
				if (*Iter0 < *Iter1)
					return true;
				if (*Iter1 < *Iter0)
					return false;

				++Iter0;
				++Iter1;
			}

			if (!Iter0 && Iter1)
				return true;

			return false;
		}

		struct CFormatOptions
		{
			bool m_bSingleLine = false;
			bool m_bBrackets = true;
		};

		template <typename tf_COption>
		bool f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option) const;
		template <typename tf_CFormatInto, typename tf_CFormatOptions>
		void f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const;
	};

	template
	<
		typename t_CKey
		, typename t_CData = CMapNoData
		, typename t_CCompare = NMib::CSort_Default
		, typename t_CAllocator = NMib::NMemory::CAllocator_Virtual
		, mint t_GrowSize = 128
		, typename t_CPoolType = NMib::NMemory::CPoolType_FreeableSmall
		, typename t_CLockType = NMib::NThread::CNoLock
	>
	class TCMapWithPool : public TCMap<t_CKey, t_CData, t_CCompare, NMemory::TCPoolAllocator<TCMapTreeMember<t_CKey, t_CData>, t_GrowSize, t_CAllocator>>
	{
		typedef TCMap<t_CKey, t_CData, t_CCompare, NMemory::TCPoolAllocator<TCMapTreeMember<t_CKey, t_CData>, t_GrowSize, t_CAllocator, t_CPoolType, t_CLockType>> CSuper;
	public:
		TCMapWithPool()
		{
		}
		TCMapWithPool(TCMapWithPool &&_Other)
			: CSuper((CSuper &&)fg_Move(_Other))
		{
		}
		TCMapWithPool(TCMapWithPool const &_Other)
			: CSuper((CSuper const &)_Other)
		{
		}
		template <typename tf_COther>
		TCMapWithPool(tf_COther &&_Other)
			: CSuper(fg_Forward<tf_COther>(_Other))
		{
		}

		TCMapWithPool &operator = (TCMapWithPool &&_Other)
		{
			*((CSuper *)this) = ((CSuper &&)fg_Move(_Other));
			return *this;
		}
		TCMapWithPool &operator = (TCMapWithPool const &_Other)
		{
			*((CSuper *)this) = ((CSuper const &)_Other);
			return *this;
		}
		template <typename tf_COther>
		TCMapWithPool &operator = (tf_COther &&_Other)
		{
			*((CSuper *)this) = fg_Forward<tf_COther>(_Other);
			return *this;
		}
	};


	template <typename t_CKey, typename t_CCompare = NMib::CSort_Default, typename t_CAllocator = NMib::NMemory::CAllocator_Heap>
	class TCSet : public TCMap<t_CKey, CMapSet, t_CCompare, t_CAllocator>
	{
		typedef TCMap<t_CKey, CMapSet, t_CCompare, t_CAllocator> CSuper;
	public:
		TCSet()
		{
		}
		TCSet(TCSet &&_Other)
			: CSuper((CSuper &&)fg_Move(_Other))
		{
		}
		TCSet(TCSet const &_Other)
			: CSuper((CSuper const &)_Other)
		{
		}

		TCSet(TCInitializerList<t_CKey> const &_Values)
		{
			for (auto &Value : _Values)
				(*this)[Value];
		}

		template <typename tf_CContainer, TCEnableIfType<!NTraits::TCIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>::mc_Value> * = nullptr>
		TCSet &f_AddContainer(tf_CContainer &&_Container)
		{
			for (auto &Value : _Container)
				(*this)[Value];
			return *this;
		}

		TCSet f_Or(TCSet const &_Other) const
		{
			TCSet Return = *this;
			Return += _Other;
			return Return;
		}

		TCSet operator | (TCSet const &_Right) const
		{
			return f_Or(_Right);
		}

		TCSet f_And(TCSet const &_Other) const
		{
			TCSet Return;
			auto iLeft = this->f_GetIterator();
			auto iRight = _Other.f_GetIterator();

			if (!iRight)
				return {};

			while (true)
			{
				while (iLeft && *iLeft < *iRight)
					++iLeft;
				if (!iLeft)
					break;
				while (iRight && *iRight < *iLeft)
					++iRight;
				if (!iRight)
					break;
				Return[*iLeft];
				++iLeft;
				++iRight;
			}

			return Return;
		}

		TCSet operator & (TCSet const &_Right) const
		{
			return f_And(_Right);
		}

		TCSet f_Xor(TCSet const &_Right) const
		{
			TCSet Return;
			auto iLeft = this->f_GetIterator();
			auto iRight = _Right.f_GetIterator();
			if (!iRight)
			{
				for (; iLeft; ++iLeft)
					Return[*iLeft];
				return Return;
			}

			while (true)
			{
				for (; iLeft && *iLeft < *iRight; ++iLeft)
					Return[*iLeft];

				if (!iLeft)
				{
					for (; iRight; ++iRight)
						Return[*iRight];
					break;
				}
				for (; iRight && *iRight < *iLeft; ++iRight)
					Return[*iRight];
				if (!iRight)
				{
					for (; iLeft; ++iLeft)
						Return[*iLeft];
					break;
				}
				Return[*iLeft];
				++iLeft;
				++iRight;
			}

			return Return;
		}

		TCSet operator ^ (TCSet const &_Right)
		{
			return f_Xor(_Right);
		}

		TCSet f_Difference(TCSet const &_Right) const
		{
			TCSet Return = *this;

			DMibFastCheck(&Return != this);

			Return -= _Right;

			return Return;
		}

		TCSet operator - (TCSet const &_Right)
		{
			return f_Difference(_Right);
		}

		template <typename tf_COther>
		TCSet(tf_COther &&_Other)
			: CSuper(fg_Forward<tf_COther>(_Other))
		{
		}

		TCSet &operator = (TCSet &&_Other)
		{
			*((CSuper *)this) = ((CSuper &&)fg_Move(_Other));
			return *this;
		}
		TCSet &operator = (TCSet const &_Other)
		{
			*((CSuper *)this) = ((CSuper const &)_Other);
			return *this;
		}
		template <typename tf_COther>
		TCSet &operator = (tf_COther &&_Other)
		{
			*((CSuper *)this) = fg_Forward<tf_COther>(_Other);
			return *this;
		}

		template <typename tf_CFormatInto, typename tf_CFormatOptions>
		void f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const;
	};

	template <typename tf_CReturn, typename... tf_CParams>
	void fg_CreateSetHelper(TCSet<tf_CReturn> &_Return)
	{
	}

	template <typename tf_CReturn, typename tf_CFirst, typename... tf_CParams>
	void fg_CreateSetHelper(TCSet<tf_CReturn> &_Return, tf_CFirst &&_First, tf_CParams && ...p_Params)
	{
		_Return[fg_Forward<tf_CFirst>(_First)];
		fg_CreateSetHelper<tf_CReturn>(_Return, fg_Forward<tf_CParams>(p_Params)...);
	}

	template <typename tf_CFirst, typename... tf_CParams>
	TCSet<typename NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>::CType> fg_CreateSet(tf_CFirst && _First, tf_CParams && ...p_Params)
	{
		TCSet<typename NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>::CType> Return;
		fg_CreateSetHelper<typename NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>::CType>(Return, fg_Forward<tf_CFirst>(_First), fg_Forward<tf_CParams>(p_Params)...);
		return Return;
	}

	template <typename tf_CReturn, typename... tf_CParams>
	TCSet<tf_CReturn> fg_CreateSet(tf_CParams && ...p_Params)
	{
		TCSet<tf_CReturn> Return;
		fg_CreateSetHelper<tf_CReturn>(Return, fg_Forward<tf_CParams>(p_Params)...);
		return Return;
	}


	template <typename t_CKey, typename t_CCompare = NMib::CSort_Default>
	class TCSetWithPool : public TCSet<t_CKey, t_CCompare, NMemory::TCPoolAllocator<TCMapTreeMember<t_CKey, CMapSet>>>
	{
		typedef TCSet<t_CKey, t_CCompare, NMemory::TCPoolAllocator<TCMapTreeMember<t_CKey, CMapSet>>> CSuper;
	public:
		TCSetWithPool()
		{
		}
		TCSetWithPool(TCSetWithPool &&_Other)
			: CSuper((CSuper &&)fg_Move(_Other))
		{
		}
		TCSetWithPool(TCSetWithPool const &_Other)
			: CSuper((CSuper const &)_Other)
		{
		}
		template <typename tf_COther>
		TCSetWithPool(tf_COther &&_Other)
			: CSuper(fg_Forward<tf_COther>(_Other))
		{
		}

		TCSetWithPool &operator = (TCSetWithPool &&_Other)
		{
			*((CSuper *)this) = ((CSuper &&)fg_Move(_Other));
			return *this;
		}
		TCSetWithPool &operator = (TCSetWithPool const &_Other)
		{
			*((CSuper *)this) = ((CSuper const &)_Other);
			return *this;
		}
		template <typename tf_COther>
		TCSetWithPool &operator = (tf_COther &&_Other)
		{
			*((CSuper *)this) = fg_Forward<tf_COther>(_Other);
			return *this;
		}
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
