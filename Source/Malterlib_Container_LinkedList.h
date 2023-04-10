// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

namespace NMib::NContainer
{
	template <typename t_CData>
	class TCLinkedListMember;

	template <typename t_CData, typename t_CAllocator = NMib::NMemory::CAllocator_Heap >
	class TCLinkedList;

	struct CLinkedListMemberBase
	{
		DMibListLinkDS_Link(CLinkedListMemberBase, m_ListLink);
	};

	template <typename t_CData>
	class TCLinkedListMember final : public CLinkedListMemberBase
	{
		template <typename t_CData2, typename t_CAllocator2> friend class TCLinkedList;

		t_CData m_Object;

		TCLinkedListMember()
		{
		}

		TCLinkedListMember(t_CData &_Data)
			: m_Object(_Data)
		{
		}

		TCLinkedListMember(t_CData &&_Data)
			: m_Object(fg_Move(_Data))
		{
		}

		TCLinkedListMember(const t_CData &_Data)
			: m_Object(_Data)
		{
		}

		inline_always static mint fs_GetOffset()
		{
			return DMibPOffsetOf(TCLinkedListMember, m_Object);
		}

		template <typename tf_CType, typename... tfp_CParams, mint... tfp_Indidies>
		TCLinkedListMember(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams, NMeta::TCIndices<tfp_Indidies...> const&)
			: m_Object
			(
				fg_Forward<tfp_CParams>(fg_Get<tfp_Indidies>(_CreateParams.m_Params))...
			)
		{
		}

		template <typename tf_CType, typename... tfp_CParams>
		TCLinkedListMember(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
			: TCLinkedListMember(fg_Move(_CreateParams), typename NMeta::TCMakeConsecutiveIndices<TCConstruct<tf_CType, tfp_CParams...>::mc_nParams>::CType())
		{
		}

	public:
		~TCLinkedListMember()
		{
		}
	};

	template <typename t_CData, typename t_CAllocator>
	class TCLinkedList
	{
	private:

		typedef TCLinkedListMember<t_CData> CMember;

		class CAllocatorDisable : public NMemory::CAllocator_Base
		{
		public:
		};

		typedef NMib::NIntrusive::TCDLinkList
			<
				CMember
				, CLinkedListMemberBase::CDLinkTranslatorm_ListLink
				, NMib::NIntrusive::CDLinkAggregateListNoPrevPtr
				, NMib::NIntrusive::CDLinkAggregateListNoPrevPtrList
				, false
				, CAllocatorDisable
			> CList
		;

		typedef typename CList::CIterator CInternalIterator;
		typedef typename CList::CTranslator CTranslator;
		typedef typename CList::COffset COffset;
		typedef typename CList::CLink CLink;

		class CLocalData : public t_CAllocator
		{
		public:
			CList m_List;

			CLocalData()
			{
			}

			CLocalData(CLocalData &&_Other)
				: t_CAllocator((t_CAllocator &&)fg_Move(_Other))
				, m_List(fg_Move(_Other.m_List))
			{
			}

			template <typename... tfp_CParams>
			CLocalData(NMemory::CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
				: t_CAllocator(fg_Forward<tfp_CParams>(p_Params)...)
			{
			}

			CLocalData &operator = (CLocalData &&) = default;
		};

		CLocalData m_Data;

		template <typename tf_CType, typename... tfp_CParams>
		CMember *fp_Create(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
			requires (NTraits::TCIsSame<tf_CType, t_CData>::mc_Value || NTraits::TCIsVoid<tf_CType>::mc_Value) // Cannot override creation type
		{
			auto Memory = m_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
			CMember * pData = (CMember *)Memory.m_pMemory;
			new((void *)pData) CMember(fg_Move(_CreateParams));
			Memory.f_Claim();
			return pData;
		}

		CMember *fp_Create(t_CData &_ToInsert)
		{
			auto Memory = m_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
			CMember * pData = (CMember *)Memory.m_pMemory;
			new((void *)pData) CMember(_ToInsert);
			Memory.f_Claim();
			return pData;
		}

		CMember *fp_Create(t_CData &&_ToInsert)
		{
			auto Memory = m_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
			CMember * pData = (CMember *)Memory.m_pMemory;
			new((void *)pData) CMember(fg_Move(_ToInsert));
			Memory.f_Claim();
			return pData;
		}

		CMember *fp_Create(const t_CData &_ToInsert)
		{
			auto Memory = m_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
			CMember * pData = (CMember *)Memory.m_pMemory;
			new((void *)pData) CMember(_ToInsert);
			Memory.f_Claim();
			return pData;
		}

		CMember *fp_Create()
		{
			auto Memory = m_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
			CMember * pData = (CMember *)Memory.m_pMemory;
			new((void *)pData) CMember();
			Memory.f_Claim();
			return pData;
		}

		inline_always static CMember *fsp_MemberFromData(t_CData &_Data)
		{
			mint Offset = CMember::fs_GetOffset();
			CMember *pMember = (CMember *)(((uint8 *)&_Data) - Offset);
			return pMember;
		}

		inline_always static CMember *fsp_MemberFromData(t_CData const &_Data)
		{
			mint Offset = CMember::fs_GetOffset();
			CMember *pMember = (CMember *)(((uint8 *)&_Data) - Offset);
			return pMember;
		}

		void fp_CopyFrom(TCLinkedList const& _Container)
		{
			mint nAdded = 16;
			CIteratorConst iSource = _Container.f_GetIterator();
			while (iSource && nAdded--)
			{
				f_Insert(*iSource);
				++iSource;
			}
			if (!iSource)
				return;

			m_Data.f_AllocBatch
				(
					sizeof(CMember)
					, alignof(CMember)
					, [&](void * _pData, mint _Size) -> bool
					{
						CMember * pData = new(_pData) CMember(*iSource);
						m_Data.m_List.f_Insert(pData);
						++iSource;
						return iSource;
					}
				)
			;
		}

	public:


		class CIterator
		{
			friend class TCLinkedList;
			typedef CInternalIterator CIntrusiveIterator;
			CIntrusiveIterator m_Iter;
			TCLinkedList *m_pLinkedList;
		public:
#ifdef DMibDebuggerHelpers
			static TCLinkedList *fs_Debug_List();
			static CList *fs_Debug_IntrusiveList();
#endif

			CIterator()
			{
				m_pLinkedList = nullptr;
#ifdef DMibDebuggerHelpers
				static_assert(TCInstantiateValue<&fs_Debug_List>::mc_Value);
				static_assert(TCInstantiateValue<&fs_Debug_IntrusiveList>::mc_Value);
#endif
			}

			CIterator(TCLinkedList &_Map)
			{
				*this = _Map.f_GetIterator();
#ifdef DMibDebuggerHelpers
				static_assert(TCInstantiateValue<&fs_Debug_List>::mc_Value);
				static_assert(TCInstantiateValue<&fs_Debug_IntrusiveList>::mc_Value);
#endif
			}

			CIterator &operator = (TCLinkedList &_Map)
			{
				*this = _Map.f_GetIterator();
				return *this;
			}

			inline_medium mint f_GetLen() const
			{
				return m_Iter.f_GetLen();
			}

			inline_medium void f_Next()
			{
				++m_Iter;
			}
			inline_medium void f_Prev()
			{
				--m_Iter;
			}
			void f_Remove()
			{
				t_CData *pToRemove = f_GetCurrent();
				f_Next();
				m_pLinkedList->f_Remove(*pToRemove);
			}

			inline_small t_CData *f_GetCurrent() const
			{
				CMember *pMember = m_Iter;
				if (pMember)
					return &pMember->m_Object;
				return nullptr;
			}

			inline_small operator t_CData *() const
			{
				return f_GetCurrent();
			}

			inline_small t_CData * operator ->() const
			{
				return f_GetCurrent();
			}

			inline_small void operator ++()
			{
				f_Next();
			}

			inline_small void operator --()
			{
				f_Prev();
			}
		};

		class CIteratorConst
		{
			friend class TCLinkedList;
			typedef typename DMibListLinkAllocatorDS_IterConst_FromTemplate(CMember, m_ListLink, CAllocatorDisable) CIntrusiveIterator;
			CIntrusiveIterator m_Iter;
		public:

#ifdef DMibDebuggerHelpers
			static TCLinkedList *fs_Debug_List();
			static CList *fs_Debug_IntrusiveList();
#endif
			CIteratorConst()
			{
#ifdef DMibDebuggerHelpers
				static_assert(TCInstantiateValue<&fs_Debug_List>::mc_Value);
				static_assert(TCInstantiateValue<&fs_Debug_IntrusiveList>::mc_Value);
#endif
			}

			CIteratorConst(const TCLinkedList &_Map)
			{
				*this = _Map.f_GetIterator();
#ifdef DMibDebuggerHelpers
				static_assert(TCInstantiateValue<&fs_Debug_List>::mc_Value);
				static_assert(TCInstantiateValue<&fs_Debug_IntrusiveList>::mc_Value);
#endif
			}

			CIteratorConst &operator = (const TCLinkedList &_Map)
			{
				*this = _Map.f_GetIterator();
				return *this;
			}

			inline_medium mint f_GetLen() const
			{
				return m_Iter.f_GetLen();
			}

			inline_medium void f_Next()
			{
				++m_Iter;
			}
			inline_medium void f_Prev()
			{
				--m_Iter;
			}

			inline_small const t_CData *f_GetCurrent() const
			{
				const CMember *pMember = m_Iter;
				if (pMember)
					return &pMember->m_Object;
				return nullptr;
			}

			inline_small operator const t_CData *() const
			{
				return f_GetCurrent();
			}

			inline_small const t_CData * operator ->() const
			{
				return f_GetCurrent();
			}

			inline_small void operator ++()
			{
				f_Next();
			}

			inline_small void operator --()
			{
				f_Prev();
			}
		};

		TCLinkedList()
		{
		}

		~TCLinkedList()
		{
			f_Clear();
		}

		void f_Clear()
		{
			m_Data.m_List.f_DeleteAllAllocatorDefiniteType((t_CAllocator &)m_Data);
		}

		TCLinkedList &operator = (const TCLinkedList &_Other)
		{
			DMibSafeCheck(this != &_Other, "Must not copy same object to itself");
			f_Clear();

			fp_CopyFrom(_Other);

			return *this;
		}

		TCLinkedList &operator = (TCLinkedList &&_Other)
		{
			DMibSafeCheck(this != &_Other, "Must not move same object to itself");

			f_Clear();
			m_Data = fg_Move(_Other.m_Data);

			return *this;
		}

		template <typename... tfp_CParams>
		TCLinkedList(NMemory::CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
			: m_Data(_Allocator, fg_Forward<tfp_CParams>(p_Params)...)
		{
		}

		TCLinkedList(TCLinkedList &&_Other)
			: m_Data(fg_Move(_Other.m_Data))
		{
		}

		TCLinkedList(std::initializer_list<t_CData> const &_Values)
		{
			for (auto &Value : _Values)
				f_Insert(Value);
		}

		TCLinkedList(const TCLinkedList &_Other)
		{
			fp_CopyFrom(_Other);
		}

		TCLinkedList &operator += (const TCLinkedList &_Other)
		{
			return f_Insert(_Other);
		}

		TCLinkedList &f_Insert(const TCLinkedList &_Other)
		{
			DMibSafeCheck(this != &_Other, "Must not add same object to itself");

			fp_CopyFrom(_Other);

			return *this;
		}

		template <typename tf_CCompare>
		void f_Sort(tf_CCompare &&_fCompare)
		{
			m_Data.m_List.f_Sort
				(
					[&](auto &&_Left, auto &&_Right)
					{
						return _fCompare(_Left.m_Object, _Right.m_Object);
					}
				)
			;
		}

		void f_Sort()
		{
			f_Sort(CSort_Default{});
		}

		CIteratorConst f_GetIterator() const
		{
			CIteratorConst Iterator;
			Iterator.m_Iter = m_Data.m_List;
			return Iterator;
		}

		CIterator f_GetIterator()
		{
			CIterator Iterator;
			Iterator.m_pLinkedList = this;
			Iterator.m_Iter = m_Data.m_List;
			return Iterator;
		}

		CIteratorConst f_GetIteratorLast() const
		{
			if (f_IsEmpty())
				return {};
			CIteratorConst Iterator;
			Iterator.m_Iter = m_Data.m_List.f_GetLast();
			return Iterator;
		}

		CIterator f_GetIteratorLast()
		{
			if (f_IsEmpty())
				return {};

			CIterator Iterator;
			Iterator.m_pLinkedList = this;
			Iterator.m_Iter = m_Data.m_List.f_GetLast();
			return Iterator;
		}

		t_CData &f_InsertAfter(const CIterator &_InsertAfter)
		{
			CMember * pData = fp_Create();
			m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(t_CData &_ToInsert, const CIterator &_InsertAfter)
		{
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(t_CData &&_ToInsert, const CIterator &_InsertAfter)
		{
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(const t_CData &_ToInsert, const CIterator &_InsertAfter)
		{
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(t_CData *_ToInsert, const CIterator &_InsertAfter)
		{
			CMember * pData = fsp_MemberFromData(*_ToInsert);
			m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(const t_CData *_ToInsert, const CIterator &_InsertAfter)
		{
			CMember * pData = fsp_MemberFromData(*_ToInsert);
			m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
			return pData->m_Object;
		}

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_InsertAfter(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, const CIterator &_InsertAfter)
		{
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
			return pData->m_Object;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertAfter(const t_CData &_InsertAfter)
		{
			CMember *pMember = fsp_MemberFromData(_InsertAfter);
			CMember *pData = fp_Create();
			m_Data.m_List.f_InsertAfter(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(t_CData &_ToInsert, const t_CData &_InsertAfter)
		{
			CMember *pMember = fsp_MemberFromData(_InsertAfter);
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertAfter(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(t_CData &&_ToInsert, const t_CData &_InsertAfter)
		{
			CMember *pMember = fsp_MemberFromData(_InsertAfter);
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertAfter(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(const t_CData &_ToInsert, const t_CData &_InsertAfter)
		{
			CMember *pMember = fsp_MemberFromData(_InsertAfter);
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertAfter(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(t_CData *_ToInsert, const t_CData &_InsertAfter)
		{
			CMember *pMember = fsp_MemberFromData(_InsertAfter);
			CMember * pData = fsp_MemberFromData(*_ToInsert);
			m_Data.m_List.f_InsertAfter(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertAfter(const t_CData *_ToInsert, const t_CData &_InsertAfter)
		{
			CMember *pMember = fsp_MemberFromData(_InsertAfter);
			CMember * pData = fsp_MemberFromData(*_ToInsert);
			m_Data.m_List.f_InsertAfter(pData, pMember);
			return pData->m_Object;
		}


		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_InsertAfter(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, const t_CData &_InsertAfter)
		{
			CMember *pMember = fsp_MemberFromData(_InsertAfter);
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertAfter(pData, pMember);
			return pData->m_Object;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertBefore(const CIterator &_InsertBefore)
		{
			CMember * pData = fp_Create();
			m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(t_CData &_ToInsert, const CIterator &_InsertBefore)
		{
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(t_CData &&_ToInsert, const CIterator &_InsertBefore)
		{
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(const t_CData &_ToInsert, const CIterator &_InsertBefore)
		{
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(t_CData *_ToInsert, const CIterator &_InsertBefore)
		{
			CMember * pData = fsp_MemberFromData(*_ToInsert);
			m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(const t_CData *_ToInsert, const CIterator &_InsertBefore)
		{
			CMember * pData = fsp_MemberFromData(*_ToInsert);
			m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
			return pData->m_Object;
		}

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_InsertBefore(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, const CIterator &_InsertBefore)
		{
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
			return pData->m_Object;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertBefore(const t_CData &_InsertBefore)
		{
			CMember *pMember = fsp_MemberFromData(_InsertBefore);
			CMember *pData = fp_Create();
			m_Data.m_List.f_InsertBefore(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(t_CData &_ToInsert, const t_CData &_InsertBefore)
		{
			CMember *pMember = fsp_MemberFromData(_InsertBefore);
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertBefore(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(t_CData &&_ToInsert, const t_CData &_InsertBefore)
		{
			CMember *pMember = fsp_MemberFromData(_InsertBefore);
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertBefore(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(const t_CData &_ToInsert, const t_CData &_InsertBefore)
		{
			CMember *pMember = fsp_MemberFromData(_InsertBefore);
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertBefore(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(t_CData *_ToInsert, const t_CData &_InsertBefore)
		{
			CMember *pMember = fsp_MemberFromData(_InsertBefore);
			CMember * pData = fsp_MemberFromData(*_ToInsert);
			m_Data.m_List.f_InsertBefore(pData, pMember);
			return pData->m_Object;
		}

		t_CData &f_InsertBefore(const t_CData *_ToInsert, const t_CData &_InsertBefore)
		{
			CMember *pMember = fsp_MemberFromData(_InsertBefore);
			CMember * pData = fsp_MemberFromData(*_ToInsert);
			m_Data.m_List.f_InsertBefore(pData, pMember);
			return pData->m_Object;
		}

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_InsertBefore(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, const t_CData &_InsertBefore)
		{
			CMember *pMember = fsp_MemberFromData(_InsertBefore);
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertBefore(pData, pMember);
			return pData->m_Object;
		}


		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_Insert(t_CData &_ToInsert)
		{
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_Insert(pData);
			return pData->m_Object;
		}

		t_CData &f_Insert(t_CData &&_ToInsert)
		{
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_Insert(pData);
			return pData->m_Object;
		}

		t_CData &f_Insert(const t_CData &_ToInsert)
		{
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_Insert(pData);
			return pData->m_Object;
		}
		t_CData &f_Insert()
		{
			CMember * pData = fp_Create();
			m_Data.m_List.f_Insert(pData);
			return pData->m_Object;
		}

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
		{
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_Insert(pData);
			return pData->m_Object;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_Push(t_CData &_ToInsert)
		{
			return f_InsertFirst(_ToInsert);
		}

		t_CData &f_Push(t_CData &&_ToInsert)
		{
			return f_InsertFirst(fg_Move(_ToInsert));
		}

		t_CData &f_Push(const t_CData &_ToInsert)
		{
			return f_InsertFirst(_ToInsert);
		}

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_Push(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
		{
			return f_InsertFirst(fg_Move(_ToInsert));
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertFirst(t_CData &_ToInsert)
		{
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertFirst(pData);
			return pData->m_Object;
		}

		t_CData &f_InsertFirst(t_CData &&_ToInsert)
		{
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertFirst(pData);
			return pData->m_Object;
		}

		t_CData &f_InsertFirst(const t_CData &_ToInsert)
		{
			CMember * pData = fp_Create(_ToInsert);
			m_Data.m_List.f_InsertFirst(pData);
			return pData->m_Object;
		}

		t_CData &f_InsertFirst()
		{
			CMember * pData = fp_Create();
			m_Data.m_List.f_InsertFirst(pData);
			return pData->m_Object;
		}

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_InsertFirst(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
		{
			CMember * pData = fp_Create(fg_Move(_ToInsert));
			m_Data.m_List.f_InsertFirst(pData);
			return pData->m_Object;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertLast(t_CData &_ToInsert)
		{
			return f_Insert(_ToInsert);
		}

		t_CData &f_InsertLast(t_CData &&_ToInsert)
		{
			return f_Insert(fg_Move(_ToInsert));
		}

		t_CData &f_InsertLast(const t_CData &_ToInsert)
		{
			return f_Insert(_ToInsert);
		}
		t_CData &f_InsertLast()
		{
			return f_Insert();
		}

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_InsertLast(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
		{
			return f_Insert(fg_Move(_ToInsert));
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		void f_Remove(t_CData &_Member) // _Member must be a reference to a value currently IN the list!
		{
			CMember *pMember = fsp_MemberFromData(_Member);
			DMibSafeCheck(pMember->m_ListLink.f_IsInList(), "Object is not linked into the list");
			// This is too slow to check
			//DMibSafeCheck(m_Data.m_List.f_Contains(pMember), "You must make sure that what you are removing is an actual object in the list");
			m_Data.m_List.f_Remove(pMember);
			fg_DeleteObjectDefiniteType(m_Data, pMember);
		}

		t_CData f_Pop()
		{
			CMember *pMember = m_Data.m_List.f_Pop();
			DMibSafeCheck(pMember, "You cannot pop from an empty list");
			m_Data.m_List.f_Remove(pMember);
			t_CData ToReturn = fg_Move(pMember->m_Object);
			fg_DeleteObjectDefiniteType(m_Data, pMember);
			return fg_Move(ToReturn);
		}

		t_CData &f_GetFirst()
		{
			return m_Data.m_List.f_GetFirst()->m_Object;
		}

		const t_CData &f_GetFirst() const
		{
			return m_Data.m_List.f_GetFirst()->m_Object;
		}

		t_CData &f_GetLast()
		{
			return m_Data.m_List.f_GetLast()->m_Object;
		}

		const t_CData &f_GetLast() const
		{
			return m_Data.m_List.f_GetLast()->m_Object;
		}

		bool f_IsEmpty() const
		{
			return m_Data.m_List.f_IsEmpty();
		}

		bool f_HasSingleEntry() const
		{
			if (m_Data.m_List.f_IsEmpty())
				return false;
			return m_Data.m_List.f_GetFirst()->m_ListLink.f_IsAloneInList();
		}

		bool f_HasMoreThanOneEntry() const
		{
			return !f_IsEmpty() && !f_HasSingleEntry();
		}

		mint f_GetLen() const
		{
			return m_Data.m_List.f_GetLen();
		}

		bool operator == (const TCLinkedList &_Other) const
		{
			CIteratorConst Iter0 = *this;
			CIteratorConst Iter1 = _Other;
			while (Iter0 && Iter1)
			{
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

		auto operator <=> (const TCLinkedList &_Other) const
		{
			using COrdering = decltype(fg_GetType<t_CData const &>() <=> fg_GetType<t_CData const &>());

			CIteratorConst Iter0 = *this;
			CIteratorConst Iter1 = _Other;
			while (Iter0 && Iter1)
			{
				if (auto Result = (*Iter0 <=> *Iter1); Result != 0)
					return Result;

				++Iter0;
				++Iter1;
			}

			if (!Iter0 && Iter1)
				return COrdering::less;
			if (Iter0 && !Iter1)
				return COrdering::greater;

			return COrdering::equivalent;
		}

		static t_CData *fs_GetPrev(t_CData &_Current)
		{
			CMember *pMember = fsp_MemberFromData(_Current);
			CMember *pPrev = CInternalIterator::fs_GetPrev(pMember);
			if (pPrev)
				return &pPrev->m_Object;
			return nullptr;
		}

		static t_CData *fs_GetNext(t_CData &_Current)
		{
			CMember *pMember = fsp_MemberFromData(_Current);
			CMember *pNext = CInternalIterator::fs_GetNext(pMember);
			if (pNext)
				return &pNext->m_Object;
			return nullptr;
		}

		static t_CData const *fs_GetPrev(t_CData const &_Current)
		{
			CMember const *pMember = (CMember const *)&_Current;
			CMember const *pPrev = CInternalIterator::fs_GetPrev(pMember);
			if (pPrev)
				return &pPrev->m_Object;
			return nullptr;
		}

		static t_CData const *fs_GetNext(t_CData const &_Current)
		{
			CMember const *pMember = (CMember const *)&_Current;
			CMember const *pNext = CInternalIterator::fs_GetNext(pMember);
			if (pNext)
				return &pNext->m_Object;
			return nullptr;
		}


	};
#ifdef DMibDebuggerHelpers
	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator> *TCLinkedList<t_CData, t_CAllocator>::CIterator::fs_Debug_List()
	{
		return nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>* TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::fs_Debug_List()
	{
		return nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	typename TCLinkedList<t_CData, t_CAllocator>::CList *TCLinkedList<t_CData, t_CAllocator>::CIterator::fs_Debug_IntrusiveList()
	{
		return nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	typename TCLinkedList<t_CData, t_CAllocator>::CList* TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::fs_Debug_IntrusiveList()
	{
		return nullptr;
	}
#endif
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
