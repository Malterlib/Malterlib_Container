// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>

namespace NMib::NContainer
{
	template <typename t_CData>
	struct TCLinkedListMember;

	template <typename t_CData, typename t_CAllocator = NMib::NMemory::CAllocator_Heap>
	struct TCLinkedList;

	template <typename t_CNode, typename t_CAllocator>
	struct TCLinkedListNodeHandle;

	struct CLinkedListMemberBase
	{
		DMibListLinkDS_Link(CLinkedListMemberBase, m_ListLink);
	};

	template <typename t_CData>
	struct TCLinkedListMember final : public CLinkedListMemberBase
	{
		using CData = t_CData;

		~TCLinkedListMember();

	private:
		template <typename t_CData2, typename t_CAllocator2> friend struct TCLinkedList;
		template <typename t_CNode2, typename t_CAllocator2> friend struct TCLinkedListNodeHandle;

		TCLinkedListMember();
		TCLinkedListMember(t_CData &_Data);
		TCLinkedListMember(t_CData &&_Data);
		TCLinkedListMember(t_CData const &_Data);

		template <typename tf_CType, typename ...tfp_CParams, umint ...tfp_Indidies>
		TCLinkedListMember(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams, NMeta::TCIndices<tfp_Indidies...> const &_Indices);
		template <typename tf_CType, typename ...tfp_CParams>
		TCLinkedListMember(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams);

		inline_always static umint fsp_GetOffset();

		t_CData mp_Object;
	};

	template <typename t_CNode, typename t_CAllocator>
	struct TCLinkedListNodeHandle
	{
		using CNode = t_CNode;
		using CData = typename t_CNode::CData;
		using CAllocatorStore = TCConditional
			<
				!NTraits::cIsEmpty<t_CAllocator> && (!NTraits::cIsCopyConstructible<t_CAllocator> || t_CAllocator::mc_CanBeStatic)
				, t_CAllocator *
				, t_CAllocator
			>
		;

		TCLinkedListNodeHandle() = default;
		TCLinkedListNodeHandle(TCLinkedListNodeHandle &&_Other) noexcept;
		TCLinkedListNodeHandle(TCLinkedListNodeHandle const &_Other) = delete;
		~TCLinkedListNodeHandle();

		TCLinkedListNodeHandle &operator = (TCLinkedListNodeHandle &&_Other) noexcept;
		TCLinkedListNodeHandle &operator = (TCLinkedListNodeHandle const &_Other) = delete;

		[[nodiscard]] bool f_IsEmpty() const noexcept;
		[[nodiscard]] explicit operator bool () const noexcept;
		CData &f_Value() noexcept;
		CData const &f_Value() const noexcept;
		CData &operator * () noexcept;
		CData const &operator * () const noexcept;
		CData *operator -> () noexcept;
		CData const *operator -> () const noexcept;

	private:
		template <typename t_CData2, typename t_CAllocator2> friend struct TCLinkedList;

		TCLinkedListNodeHandle(t_CNode *_pNode, t_CAllocator &_Allocator);

		t_CAllocator &fp_Allocator() const;

		t_CNode *mp_pNode = nullptr;
		DMibNoUniqueAddress CAllocatorStore mp_Allocator;
	};

	template <typename t_CData, typename t_CAllocator>
	struct TCLinkedList
	{
	private:
		// Declared before the public section, which uses them.
		using CMember = TCLinkedListMember<t_CData>;

		struct CAllocatorDisable : public NMemory::CAllocator_Base
		{
		};

		using CList = NMib::NIntrusive::TCDLinkList
			<
				CMember
				, CLinkedListMemberBase::CDLinkTranslatorm_ListLink
				, NMib::NIntrusive::CDLinkAggregateListNoPrevPtr
				, NMib::NIntrusive::CDLinkAggregateListNoPrevPtrList
				, false
				, CAllocatorDisable
			>
		;

		using CInternalIterator = typename CList::CIterator;

	public: // NOLINT(malterlib-member-order): the public declarations use the private CMember, CAllocatorDisable and CInternalIterator
		using CNodeHandle = TCLinkedListNodeHandle<CMember, t_CAllocator>;

		struct CIterator
		{
			CIterator();
			CIterator(TCLinkedList &_Map);
			CIterator f_GetIterator() const;
			CIterator &operator = (TCLinkedList &_Map);
			inline_medium umint f_GetLen() const;
			inline_medium void f_Next();
			inline_medium void f_Prev();
			void f_Remove();
			CNodeHandle f_ExtractNode();
			inline_small t_CData *f_GetCurrent() const;
			inline_small operator t_CData * () const;
			inline_small t_CData *operator -> () const;
			inline_small void operator ++ ();
			inline_small void operator -- ();

		private:
			friend struct TCLinkedList;
			using CIntrusiveIterator = CInternalIterator;
			CIntrusiveIterator mp_Iter;
			TCLinkedList *mp_pLinkedList;
		};

		struct CIteratorConst
		{
			CIteratorConst() = default;
			CIteratorConst(TCLinkedList const &_Map);
			CIteratorConst f_GetIterator() const;
			CIteratorConst &operator = (TCLinkedList const &_Map);
			inline_medium umint f_GetLen() const;
			inline_medium void f_Next();
			inline_medium void f_Prev();
			inline_small t_CData const *f_GetCurrent() const;
			inline_small operator t_CData const * () const;
			inline_small t_CData const *operator -> () const;
			inline_small void operator ++ ();
			inline_small void operator -- ();

		private:
			friend struct TCLinkedList;
			using CIntrusiveIterator = typename DMibListLinkAllocatorDS_IterConst_FromTemplate(CMember, m_ListLink, CAllocatorDisable);
			CIntrusiveIterator mp_Iter;
		};

		TCLinkedList();

		template <typename ...tfp_CParams>
		TCLinkedList(CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params);

		TCLinkedList(TCLinkedList &&_Other);
		TCLinkedList(std::initializer_list<t_CData> const &_Values);
		TCLinkedList(TCLinkedList const &_Other);
		~TCLinkedList();

		void f_Clear();
		TCLinkedList &operator = (TCLinkedList const &_Other);
		TCLinkedList &operator = (TCLinkedList &&_Other);
		TCLinkedList &operator += (TCLinkedList const &_Other);
		TCLinkedList &f_Insert(TCLinkedList const &_Other);

		template <typename tf_FCompare>
		void f_Sort(tf_FCompare &&_fCompare);
		void f_Sort();

		CIteratorConst f_GetIterator() const;
		CIterator f_GetIterator();
		CIteratorConst f_GetIteratorLast() const;
		CIterator f_GetIteratorLast();

		t_CData &f_InsertAfter(CIterator const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData &_ToInsert, CIterator const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData &&_ToInsert, CIterator const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData const &_ToInsert, CIterator const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData *_pToInsert, CIterator const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData const *_pToInsert, CIterator const &_InsertAfter);

		template <typename tf_CType, typename ...tfp_CParams>
		t_CData &f_InsertAfter(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, CIterator const &_InsertAfter);

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertAfter(t_CData const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData &_ToInsert, t_CData const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData &&_ToInsert, t_CData const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData const &_ToInsert, t_CData const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData *_pToInsert, t_CData const &_InsertAfter);
		t_CData &f_InsertAfter(t_CData const *_pToInsert, t_CData const &_InsertAfter);

		template <typename tf_CType, typename ...tfp_CParams>
		t_CData &f_InsertAfter(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, t_CData const &_InsertAfter);

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertBefore(CIterator const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData &_ToInsert, CIterator const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData &&_ToInsert, CIterator const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData const &_ToInsert, CIterator const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData *_pToInsert, CIterator const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData const *_pToInsert, CIterator const &_InsertBefore);

		template <typename tf_CType, typename ...tfp_CParams>
		t_CData &f_InsertBefore(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, CIterator const &_InsertBefore);

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertBefore(t_CData const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData &_ToInsert, t_CData const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData &&_ToInsert, t_CData const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData const &_ToInsert, t_CData const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData *_pToInsert, t_CData const &_InsertBefore);
		t_CData &f_InsertBefore(t_CData const *_pToInsert, t_CData const &_InsertBefore);

		template <typename tf_CType, typename ...tfp_CParams>
		t_CData &f_InsertBefore(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, t_CData const &_InsertBefore);

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_Insert(t_CData &_ToInsert);
		t_CData &f_Insert(t_CData &&_ToInsert);
		t_CData &f_Insert(t_CData const &_ToInsert);
		t_CData &f_Insert();

		template <typename tf_CType, typename ...tfp_CParams>
		t_CData &f_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert);

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_Push(t_CData &_ToInsert);
		t_CData &f_Push(t_CData &&_ToInsert);
		t_CData &f_Push(t_CData const &_ToInsert);

		template <typename tf_CType, typename ...tfp_CParams>
		t_CData &f_Push(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert);

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertFirst(t_CData &_ToInsert);
		t_CData &f_InsertFirst(t_CData &&_ToInsert);
		t_CData &f_InsertFirst(t_CData const &_ToInsert);
		t_CData &f_InsertFirst();

		template <typename tf_CType, typename ...tfp_CParams>
		t_CData &f_InsertFirst(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert);

		/////////////////////////////////////////////////////////////////////////////////////////////

		t_CData &f_InsertLast(t_CData &_ToInsert);
		t_CData &f_InsertLast(t_CData &&_ToInsert);
		t_CData &f_InsertLast(t_CData const &_ToInsert);
		t_CData &f_InsertLast();

		template <typename tf_CType, typename ...tfp_CParams>
		t_CData &f_InsertLast(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert);

		/////////////////////////////////////////////////////////////////////////////////////////////

		void f_Remove(t_CData &_Member); // _Member must be a reference to a value currently IN the list!
		t_CData f_PopFirst();
		t_CData f_PopLast();
		t_CData f_Pop();

		CNodeHandle f_Extract(t_CData &_Member);
		t_CData &f_Insert(CNodeHandle &&_Handle);
		t_CData &f_InsertAfter(CNodeHandle &&_Handle, CIterator const &_InsertAfter);
		t_CData &f_InsertFirst(CNodeHandle &&_Handle);

		t_CData &f_GetFirst();
		t_CData const &f_GetFirst() const;
		t_CData &f_GetLast();
		t_CData const &f_GetLast() const;

		bool f_IsEmpty() const;
		bool f_HasSingleEntry() const;
		bool f_HasMoreThanOneEntry() const;
		umint f_GetLen() const;

		bool operator == (TCLinkedList const &_Other) const noexcept(noexcept(fg_GetType<t_CData const &>() == fg_GetType<t_CData const &>()));
		auto operator <=> (TCLinkedList const &_Other) const noexcept(noexcept(fg_GetType<t_CData const &>() <=> fg_GetType<t_CData const &>()));

		static t_CData *fs_GetPrev(t_CData &_Current);
		static t_CData *fs_GetNext(t_CData &_Current);
		static t_CData const *fs_GetPrev(t_CData const &_Current);
		static t_CData const *fs_GetNext(t_CData const &_Current);

	private:
		using CTranslator = typename CList::CTranslator;
		using COffset = typename CList::COffset;
		using CLink = typename CList::CLink;

		struct CLocalData : public t_CAllocator
		{
			CLocalData();
			CLocalData(CLocalData &&_Other);
			template <typename ...tfp_CParams>
			CLocalData(CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params);
			CLocalData &operator = (CLocalData &&_Other) = default;

			CList m_List;
		};

		template <typename tf_CType, typename ...tfp_CParams>
		CMember *fp_Create(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
			requires (NTraits::cIsSame<tf_CType, t_CData> || NTraits::cIsVoid<tf_CType>) // Cannot override creation type
		;

		CMember *fp_Create(t_CData &_ToInsert);
		CMember *fp_Create(t_CData &&_ToInsert);
		CMember *fp_Create(t_CData const &_ToInsert);
		CMember *fp_Create();

		inline_always static CMember *fsp_MemberFromData(t_CData &_Data);
		inline_always static CMember *fsp_MemberFromData(t_CData const &_Data);

		void fp_CopyFrom(TCLinkedList const &_Container);

		CLocalData mp_Data;
	};
}

#ifndef DMibPNoShortCuts
using namespace NMib::NContainer;
#endif

#include "Malterlib_Container_LinkedList.hpp"
