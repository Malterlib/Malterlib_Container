// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData>
	TCLinkedListMember<t_CData>::TCLinkedListMember()
	{
	}

	template <typename t_CData>
	TCLinkedListMember<t_CData>::TCLinkedListMember(t_CData &_Data)
		: mp_Object(_Data)
	{
	}

	template <typename t_CData>
	TCLinkedListMember<t_CData>::TCLinkedListMember(t_CData &&_Data)
		: mp_Object(fg_Move(_Data))
	{
	}

	template <typename t_CData>
	TCLinkedListMember<t_CData>::TCLinkedListMember(t_CData const &_Data)
		: mp_Object(_Data)
	{
	}

	template <typename t_CData>
	inline_always umint TCLinkedListMember<t_CData>::fsp_GetOffset()
	{
		return DMibPOffsetOf(TCLinkedListMember, mp_Object);
	}

	template <typename t_CData>
	template <typename tf_CType, typename ...tfp_CParams, umint ...tfp_Indidies>
	TCLinkedListMember<t_CData>::TCLinkedListMember(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams, NMeta::TCIndices<tfp_Indidies...> const &_Indices)
		: mp_Object(fg_Forward<tfp_CParams>(fg_Get<tfp_Indidies>(_CreateParams.m_Params))...)
	{
	}

	template <typename t_CData>
	template <typename tf_CType, typename ...tfp_CParams>
	TCLinkedListMember<t_CData>::TCLinkedListMember(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
		: TCLinkedListMember(fg_Move(_CreateParams), NMeta::TCConsecutiveIndices<TCConstruct<tf_CType, tfp_CParams...>::mc_nParams>())
	{
	}

	template <typename t_CData>
	TCLinkedListMember<t_CData>::~TCLinkedListMember()
	{
	}

	template <typename t_CNode, typename t_CAllocator>
	TCLinkedListNodeHandle<t_CNode, t_CAllocator>::TCLinkedListNodeHandle(TCLinkedListNodeHandle &&_Other) noexcept
		: mp_pNode(fg_Exchange(_Other.mp_pNode, nullptr))
		, mp_Allocator(fg_Move(_Other.mp_Allocator))
	{
		if constexpr (NTraits::cIsPointer<CAllocatorStore>)
			_Other.mp_Allocator = nullptr;
	}

	template <typename t_CNode, typename t_CAllocator>
	TCLinkedListNodeHandle<t_CNode, t_CAllocator>::~TCLinkedListNodeHandle()
	{
		if (mp_pNode)
			fg_DeleteObjectDefiniteType(fp_Allocator(), mp_pNode);
	}

	template <typename t_CNode, typename t_CAllocator>
	auto TCLinkedListNodeHandle<t_CNode, t_CAllocator>::operator = (TCLinkedListNodeHandle &&_Other) noexcept -> TCLinkedListNodeHandle &
	{
		if (mp_pNode)
			fg_DeleteObjectDefiniteType(fp_Allocator(), mp_pNode);

		mp_Allocator = fg_Move(_Other.mp_Allocator);
		if constexpr (NTraits::cIsPointer<CAllocatorStore>)
			_Other.mp_Allocator = nullptr;

		mp_pNode = fg_Exchange(_Other.mp_pNode, nullptr);
		return *this;
	}

	template <typename t_CNode, typename t_CAllocator>
	bool TCLinkedListNodeHandle<t_CNode, t_CAllocator>::f_IsEmpty() const noexcept
	{
		return !mp_pNode;
	}

	template <typename t_CNode, typename t_CAllocator>
	TCLinkedListNodeHandle<t_CNode, t_CAllocator>::operator bool () const noexcept
	{
		return !!mp_pNode;
	}

	template <typename t_CNode, typename t_CAllocator>
	auto TCLinkedListNodeHandle<t_CNode, t_CAllocator>::f_Value() noexcept -> CData &
	{
		return mp_pNode->mp_Object;
	}

	template <typename t_CNode, typename t_CAllocator>
	auto TCLinkedListNodeHandle<t_CNode, t_CAllocator>::f_Value() const noexcept -> CData const &
	{
		return mp_pNode->mp_Object;
	}

	template <typename t_CNode, typename t_CAllocator>
	auto TCLinkedListNodeHandle<t_CNode, t_CAllocator>::operator * () noexcept -> CData &
	{
		return f_Value();
	}

	template <typename t_CNode, typename t_CAllocator>
	auto TCLinkedListNodeHandle<t_CNode, t_CAllocator>::operator * () const noexcept -> CData const &
	{
		return f_Value();
	}

	template <typename t_CNode, typename t_CAllocator>
	auto TCLinkedListNodeHandle<t_CNode, t_CAllocator>::operator -> () noexcept -> CData *
	{
		return &f_Value();
	}

	template <typename t_CNode, typename t_CAllocator>
	auto TCLinkedListNodeHandle<t_CNode, t_CAllocator>::operator -> () const noexcept -> CData const *
	{
		return &f_Value();
	}

	template <typename t_CNode, typename t_CAllocator>
	t_CAllocator &TCLinkedListNodeHandle<t_CNode, t_CAllocator>::fp_Allocator() const
	{
		if constexpr (NTraits::cIsPointer<CAllocatorStore>)
			return *mp_Allocator;
		else
			return (t_CAllocator &)mp_Allocator;
	}

	template <typename t_CNode, typename t_CAllocator>
	TCLinkedListNodeHandle<t_CNode, t_CAllocator>::TCLinkedListNodeHandle(t_CNode *_pNode, t_CAllocator &_Allocator)
		: mp_pNode(_pNode)
		, mp_Allocator
		(
			[&]
			{
				if constexpr (NTraits::cIsPointer<CAllocatorStore>)
					return &_Allocator;
				else
					return _Allocator;
			}
			()
		)
	{
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::CLocalData::CLocalData()
	{
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::CLocalData::CLocalData(CLocalData &&_Other)
		: t_CAllocator((t_CAllocator &&)fg_Move(_Other))
		, m_List(fg_Move(_Other.m_List))
	{
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename ...tfp_CParams>
	TCLinkedList<t_CData, t_CAllocator>::CLocalData::CLocalData(CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
		: t_CAllocator(fg_Forward<tfp_CParams>(p_Params)...)
	{
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	auto TCLinkedList<t_CData, t_CAllocator>::fp_Create(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams) -> CMember *
		requires (NTraits::cIsSame<tf_CType, t_CData> || NTraits::cIsVoid<tf_CType>) // Cannot override creation type
	{
		auto Memory = mp_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
		CMember *pData = (CMember *)Memory.m_pMemory;
		new((void *)pData) CMember(fg_Move(_CreateParams));
		Memory.f_Claim();
		return pData;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::fp_Create(t_CData &_ToInsert) -> CMember *
	{
		auto Memory = mp_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
		CMember *pData = (CMember *)Memory.m_pMemory;
		new((void *)pData) CMember(_ToInsert);
		Memory.f_Claim();
		return pData;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::fp_Create(t_CData &&_ToInsert) -> CMember *
	{
		auto Memory = mp_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
		CMember *pData = (CMember *)Memory.m_pMemory;
		new((void *)pData) CMember(fg_Move(_ToInsert));
		Memory.f_Claim();
		return pData;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::fp_Create(t_CData const &_ToInsert) -> CMember *
	{
		auto Memory = mp_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
		CMember *pData = (CMember *)Memory.m_pMemory;
		new((void *)pData) CMember(_ToInsert);
		Memory.f_Claim();
		return pData;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::fp_Create() -> CMember *
	{
		auto Memory = mp_Data.f_AllocSafe(sizeof(CMember), alignof(CMember));
		CMember *pData = (CMember *)Memory.m_pMemory;
		new((void *)pData) CMember();
		Memory.f_Claim();
		return pData;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_always auto TCLinkedList<t_CData, t_CAllocator>::fsp_MemberFromData(t_CData &_Data) -> CMember *
	{
		umint Offset = CMember::fsp_GetOffset();
		CMember *pMember = (CMember *)(((uint8 *)&_Data) - Offset);
		return pMember;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_always auto TCLinkedList<t_CData, t_CAllocator>::fsp_MemberFromData(t_CData const &_Data) -> CMember *
	{
		umint Offset = CMember::fsp_GetOffset();
		CMember *pMember = (CMember *)(((uint8 *)&_Data) - Offset);
		return pMember;
	}

	template <typename t_CData, typename t_CAllocator>
	void TCLinkedList<t_CData, t_CAllocator>::fp_CopyFrom(TCLinkedList const &_Container)
	{
		umint nAdded = 16;
		CIteratorConst iSource = _Container.f_GetIterator();
		while (iSource && nAdded--)
		{
			f_Insert(*iSource);
			++iSource;
		}
		if (!iSource)
			return;

		mp_Data.f_AllocBatch
			(
				sizeof(CMember)
				, alignof(CMember)
				, [&](void *_pData, umint _Size) -> bool
				{
					CMember *pData = new(_pData) CMember(*iSource);
					mp_Data.m_List.f_Insert(pData);
					++iSource;
					return iSource;
				}
			)
		;
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::CIterator::CIterator()
	{
		mp_pLinkedList = nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::CIterator::CIterator(TCLinkedList &_Map)
	{
		*this = _Map.f_GetIterator();
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::CIterator::f_GetIterator() const -> CIterator
	{
		CIterator Iterator;
		Iterator.mp_Iter = mp_Iter;
		Iterator.mp_pLinkedList = mp_pLinkedList;
		return Iterator;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::CIterator::operator = (TCLinkedList &_Map) -> CIterator &
	{
		*this = _Map.f_GetIterator();
		return *this;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_medium umint TCLinkedList<t_CData, t_CAllocator>::CIterator::f_GetLen() const
	{
		return mp_Iter.f_GetLen();
	}

	template <typename t_CData, typename t_CAllocator>
	inline_medium void TCLinkedList<t_CData, t_CAllocator>::CIterator::f_Next()
	{
		++mp_Iter;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_medium void TCLinkedList<t_CData, t_CAllocator>::CIterator::f_Prev()
	{
		--mp_Iter;
	}

	template <typename t_CData, typename t_CAllocator>
	void TCLinkedList<t_CData, t_CAllocator>::CIterator::f_Remove()
	{
		t_CData *pToRemove = f_GetCurrent();
		f_Next();
		mp_pLinkedList->f_Remove(*pToRemove);
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::CIterator::f_ExtractNode() -> CNodeHandle
	{
		DMibSafeCheck(f_GetCurrent(), "Cannot extract from exhausted iterator");

		CMember *pMember = fsp_MemberFromData(*f_GetCurrent());
		++*this;
		mp_pLinkedList->mp_Data.m_List.f_Remove(pMember);
		return CNodeHandle(pMember, mp_pLinkedList->mp_Data);
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small t_CData *TCLinkedList<t_CData, t_CAllocator>::CIterator::f_GetCurrent() const
	{
		CMember *pMember = mp_Iter;
		if (pMember)
			return &pMember->mp_Object;
		return nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small TCLinkedList<t_CData, t_CAllocator>::CIterator::operator t_CData * () const
	{
		return f_GetCurrent();
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small t_CData *TCLinkedList<t_CData, t_CAllocator>::CIterator::operator -> () const
	{
		return f_GetCurrent();
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small void TCLinkedList<t_CData, t_CAllocator>::CIterator::operator ++ ()
	{
		f_Next();
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small void TCLinkedList<t_CData, t_CAllocator>::CIterator::operator -- ()
	{
		f_Prev();
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::CIteratorConst(TCLinkedList const &_Map)
	{
		*this = _Map.f_GetIterator();
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::f_GetIterator() const -> CIteratorConst
	{
		CIteratorConst Iterator;
		Iterator.mp_Iter = mp_Iter;
		return Iterator;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::operator = (TCLinkedList const &_Map) -> CIteratorConst &
	{
		*this = _Map.f_GetIterator();
		return *this;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_medium umint TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::f_GetLen() const
	{
		return mp_Iter.f_GetLen();
	}

	template <typename t_CData, typename t_CAllocator>
	inline_medium void TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::f_Next()
	{
		++mp_Iter;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_medium void TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::f_Prev()
	{
		--mp_Iter;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small t_CData const *TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::f_GetCurrent() const
	{
		CMember const *pMember = mp_Iter;
		if (pMember)
			return &pMember->mp_Object;
		return nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::operator t_CData const * () const
	{
		return f_GetCurrent();
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small t_CData const *TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::operator -> () const
	{
		return f_GetCurrent();
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small void TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::operator ++ ()
	{
		f_Next();
	}

	template <typename t_CData, typename t_CAllocator>
	inline_small void TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::operator -- ()
	{
		f_Prev();
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::TCLinkedList()
	{
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::~TCLinkedList()
	{
		f_Clear();
	}

	template <typename t_CData, typename t_CAllocator>
	void TCLinkedList<t_CData, t_CAllocator>::f_Clear()
	{
		mp_Data.m_List.f_DeleteAllAllocatorDefiniteType((t_CAllocator &)mp_Data);
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::operator = (TCLinkedList const &_Other) -> TCLinkedList &
	{
		DMibSafeCheck(this != &_Other, "Must not copy same object to itself");
		f_Clear();

		fp_CopyFrom(_Other);

		return *this;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::operator = (TCLinkedList &&_Other) -> TCLinkedList &
	{
		DMibSafeCheck(this != &_Other, "Must not move same object to itself");

		f_Clear();
		mp_Data = fg_Move(_Other.mp_Data);

		return *this;
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename ...tfp_CParams>
	TCLinkedList<t_CData, t_CAllocator>::TCLinkedList(CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
		: mp_Data(_Allocator, fg_Forward<tfp_CParams>(p_Params)...)
	{
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::TCLinkedList(TCLinkedList &&_Other)
		: mp_Data(fg_Move(_Other.mp_Data))
	{
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::TCLinkedList(std::initializer_list<t_CData> const &_Values)
	{
		for (auto &Value : _Values)
			f_Insert(Value);
	}

	template <typename t_CData, typename t_CAllocator>
	TCLinkedList<t_CData, t_CAllocator>::TCLinkedList(TCLinkedList const &_Other)
	{
		fp_CopyFrom(_Other);
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::operator += (TCLinkedList const &_Other) -> TCLinkedList &
	{
		return f_Insert(_Other);
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::f_Insert(TCLinkedList const &_Other) -> TCLinkedList &
	{
		DMibSafeCheck(this != &_Other, "Must not add same object to itself");

		fp_CopyFrom(_Other);

		return *this;
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_FCompare>
	void TCLinkedList<t_CData, t_CAllocator>::f_Sort(tf_FCompare &&_fCompare)
	{
		mp_Data.m_List.f_Sort
			(
				[&](auto &&_Left, auto &&_Right)
				{
					return _fCompare(_Left.mp_Object, _Right.mp_Object);
				}
			)
		;
	}

	template <typename t_CData, typename t_CAllocator>
	void TCLinkedList<t_CData, t_CAllocator>::f_Sort()
	{
		f_Sort(CSort_Default{});
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::f_GetIterator() const -> CIteratorConst
	{
		CIteratorConst Iterator;
		Iterator.mp_Iter = mp_Data.m_List;
		return Iterator;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::f_GetIterator() -> CIterator
	{
		CIterator Iterator;
		Iterator.mp_pLinkedList = this;
		Iterator.mp_Iter = mp_Data.m_List;
		return Iterator;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::f_GetIteratorLast() const -> CIteratorConst
	{
		if (f_IsEmpty())
			return {};
		CIteratorConst Iterator;
		Iterator.mp_Iter = mp_Data.m_List.f_GetLast();
		return Iterator;
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::f_GetIteratorLast() -> CIterator
	{
		if (f_IsEmpty())
			return {};

		CIterator Iterator;
		Iterator.mp_pLinkedList = this;
		Iterator.mp_Iter = mp_Data.m_List.f_GetLast();
		return Iterator;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(CIterator const &_InsertAfter)
	{
		CMember *pData = fp_Create();
		mp_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData &_ToInsert, CIterator const &_InsertAfter)
	{
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData &&_ToInsert, CIterator const &_InsertAfter)
	{
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData const &_ToInsert, CIterator const &_InsertAfter)
	{
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData *_pToInsert, CIterator const &_InsertAfter)
	{
		CMember *pData = fsp_MemberFromData(*_pToInsert);
		mp_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData const *_pToInsert, CIterator const &_InsertAfter)
	{
		CMember *pData = fsp_MemberFromData(*_pToInsert);
		mp_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, CIterator const &_InsertAfter)
	{
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData const &_InsertAfter)
	{
		CMember *pMember = fsp_MemberFromData(_InsertAfter);
		CMember *pData = fp_Create();
		mp_Data.m_List.f_InsertAfter(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData &_ToInsert, t_CData const &_InsertAfter)
	{
		CMember *pMember = fsp_MemberFromData(_InsertAfter);
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertAfter(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData &&_ToInsert, t_CData const &_InsertAfter)
	{
		CMember *pMember = fsp_MemberFromData(_InsertAfter);
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertAfter(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData const &_ToInsert, t_CData const &_InsertAfter)
	{
		CMember *pMember = fsp_MemberFromData(_InsertAfter);
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertAfter(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData *_pToInsert, t_CData const &_InsertAfter)
	{
		CMember *pMember = fsp_MemberFromData(_InsertAfter);
		CMember *pData = fsp_MemberFromData(*_pToInsert);
		mp_Data.m_List.f_InsertAfter(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(t_CData const *_pToInsert, t_CData const &_InsertAfter)
	{
		CMember *pMember = fsp_MemberFromData(_InsertAfter);
		CMember *pData = fsp_MemberFromData(*_pToInsert);
		mp_Data.m_List.f_InsertAfter(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, t_CData const &_InsertAfter)
	{
		CMember *pMember = fsp_MemberFromData(_InsertAfter);
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertAfter(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(CIterator const &_InsertBefore)
	{
		CMember *pData = fp_Create();
		mp_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData &_ToInsert, CIterator const &_InsertBefore)
	{
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData &&_ToInsert, CIterator const &_InsertBefore)
	{
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData const &_ToInsert, CIterator const &_InsertBefore)
	{
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData *_pToInsert, CIterator const &_InsertBefore)
	{
		CMember *pData = fsp_MemberFromData(*_pToInsert);
		mp_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData const *_pToInsert, CIterator const &_InsertBefore)
	{
		CMember *pData = fsp_MemberFromData(*_pToInsert);
		mp_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, CIterator const &_InsertBefore)
	{
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData const &_InsertBefore)
	{
		CMember *pMember = fsp_MemberFromData(_InsertBefore);
		CMember *pData = fp_Create();
		mp_Data.m_List.f_InsertBefore(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData &_ToInsert, t_CData const &_InsertBefore)
	{
		CMember *pMember = fsp_MemberFromData(_InsertBefore);
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertBefore(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData &&_ToInsert, t_CData const &_InsertBefore)
	{
		CMember *pMember = fsp_MemberFromData(_InsertBefore);
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertBefore(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData const &_ToInsert, t_CData const &_InsertBefore)
	{
		CMember *pMember = fsp_MemberFromData(_InsertBefore);
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertBefore(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData *_pToInsert, t_CData const &_InsertBefore)
	{
		CMember *pMember = fsp_MemberFromData(_InsertBefore);
		CMember *pData = fsp_MemberFromData(*_pToInsert);
		mp_Data.m_List.f_InsertBefore(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(t_CData const *_pToInsert, t_CData const &_InsertBefore)
	{
		CMember *pMember = fsp_MemberFromData(_InsertBefore);
		CMember *pData = fsp_MemberFromData(*_pToInsert);
		mp_Data.m_List.f_InsertBefore(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertBefore(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, t_CData const &_InsertBefore)
	{
		CMember *pMember = fsp_MemberFromData(_InsertBefore);
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertBefore(pData, pMember);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Insert(t_CData &_ToInsert)
	{
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_Insert(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Insert(t_CData &&_ToInsert)
	{
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_Insert(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Insert(t_CData const &_ToInsert)
	{
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_Insert(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Insert()
	{
		CMember *pData = fp_Create();
		mp_Data.m_List.f_Insert(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
	{
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_Insert(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Push(t_CData &_ToInsert)
	{
		return f_InsertFirst(_ToInsert);
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Push(t_CData &&_ToInsert)
	{
		return f_InsertFirst(fg_Move(_ToInsert));
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Push(t_CData const &_ToInsert)
	{
		return f_InsertFirst(_ToInsert);
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Push(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
	{
		return f_InsertFirst(fg_Move(_ToInsert));
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertFirst(t_CData &_ToInsert)
	{
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertFirst(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertFirst(t_CData &&_ToInsert)
	{
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertFirst(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertFirst(t_CData const &_ToInsert)
	{
		CMember *pData = fp_Create(_ToInsert);
		mp_Data.m_List.f_InsertFirst(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertFirst()
	{
		CMember *pData = fp_Create();
		mp_Data.m_List.f_InsertFirst(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertFirst(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
	{
		CMember *pData = fp_Create(fg_Move(_ToInsert));
		mp_Data.m_List.f_InsertFirst(pData);
		return pData->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertLast(t_CData &_ToInsert)
	{
		return f_Insert(_ToInsert);
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertLast(t_CData &&_ToInsert)
	{
		return f_Insert(fg_Move(_ToInsert));
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertLast(t_CData const &_ToInsert)
	{
		return f_Insert(_ToInsert);
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertLast()
	{
		return f_Insert();
	}

	template <typename t_CData, typename t_CAllocator>
	template <typename tf_CType, typename ...tfp_CParams>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertLast(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
	{
		return f_Insert(fg_Move(_ToInsert));
	}

	template <typename t_CData, typename t_CAllocator>
	void TCLinkedList<t_CData, t_CAllocator>::f_Remove(t_CData &_Member)
	{
		CMember *pMember = fsp_MemberFromData(_Member);
		DMibSafeCheck(pMember->m_ListLink.f_IsInList(), "Object is not linked into the list");
		// This is too slow to check
		//DMibSafeCheck(mp_Data.m_List.f_Contains(pMember), "You must make sure that what you are removing is an actual object in the list");
		mp_Data.m_List.f_Remove(pMember);
		fg_DeleteObjectDefiniteType(mp_Data, pMember);
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData TCLinkedList<t_CData, t_CAllocator>::f_PopFirst()
	{
		CMember *pMember = mp_Data.m_List.f_GetFirst();
		DMibSafeCheck(pMember, "You cannot pop from an empty list");
		mp_Data.m_List.f_Remove(pMember);
		t_CData ToReturn = fg_Move(pMember->mp_Object);
		fg_DeleteObjectDefiniteType(mp_Data, pMember);
		return fg_Move(ToReturn);
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData TCLinkedList<t_CData, t_CAllocator>::f_PopLast()
	{
		CMember *pMember = mp_Data.m_List.f_GetLast();
		DMibSafeCheck(pMember, "You cannot pop from an empty list");
		mp_Data.m_List.f_Remove(pMember);
		t_CData ToReturn = fg_Move(pMember->mp_Object);
		fg_DeleteObjectDefiniteType(mp_Data, pMember);
		return fg_Move(ToReturn);
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData TCLinkedList<t_CData, t_CAllocator>::f_Pop()
	{
		return f_PopFirst();
	}

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::f_Extract(t_CData &_Member) -> CNodeHandle
	{
		CMember *pMember = fsp_MemberFromData(_Member);
		DMibSafeCheck(pMember->m_ListLink.f_IsInList(), "Object is not linked into the list");
		mp_Data.m_List.f_Remove(pMember);
		return CNodeHandle(pMember, mp_Data);
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_Insert(CNodeHandle &&_Handle)
	{
		DMibFastCheck(!!_Handle);
		CMember *pMember = fg_Exchange(_Handle.mp_pNode, nullptr);
		if constexpr (NTraits::cIsPointer<typename CNodeHandle::CAllocatorStore>)
			_Handle.mp_Allocator = nullptr;
		mp_Data.m_List.f_Insert(pMember);
		return pMember->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertAfter(CNodeHandle &&_Handle, CIterator const &_InsertAfter)
	{
		DMibFastCheck(!!_Handle);
		CMember *pMember = fg_Exchange(_Handle.mp_pNode, nullptr);
		if constexpr (NTraits::cIsPointer<typename CNodeHandle::CAllocatorStore>)
			_Handle.mp_Allocator = nullptr;
		mp_Data.m_List.f_InsertAfter(pMember, fsp_MemberFromData(*_InsertAfter));
		return pMember->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_InsertFirst(CNodeHandle &&_Handle)
	{
		DMibFastCheck(!!_Handle);
		CMember *pMember = fg_Exchange(_Handle.mp_pNode, nullptr);
		if constexpr (NTraits::cIsPointer<typename CNodeHandle::CAllocatorStore>)
			_Handle.mp_Allocator = nullptr;
		mp_Data.m_List.f_InsertFirst(pMember);
		return pMember->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_GetFirst()
	{
		return mp_Data.m_List.f_GetFirst()->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData const &TCLinkedList<t_CData, t_CAllocator>::f_GetFirst() const
	{
		return mp_Data.m_List.f_GetFirst()->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData &TCLinkedList<t_CData, t_CAllocator>::f_GetLast()
	{
		return mp_Data.m_List.f_GetLast()->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData const &TCLinkedList<t_CData, t_CAllocator>::f_GetLast() const
	{
		return mp_Data.m_List.f_GetLast()->mp_Object;
	}

	template <typename t_CData, typename t_CAllocator>
	bool TCLinkedList<t_CData, t_CAllocator>::f_IsEmpty() const
	{
		return mp_Data.m_List.f_IsEmpty();
	}

	template <typename t_CData, typename t_CAllocator>
	bool TCLinkedList<t_CData, t_CAllocator>::f_HasSingleEntry() const
	{
		if (mp_Data.m_List.f_IsEmpty())
			return false;
		return mp_Data.m_List.f_GetFirst()->m_ListLink.f_IsAloneInList();
	}

	template <typename t_CData, typename t_CAllocator>
	bool TCLinkedList<t_CData, t_CAllocator>::f_HasMoreThanOneEntry() const
	{
		return !f_IsEmpty() && !f_HasSingleEntry();
	}

	template <typename t_CData, typename t_CAllocator>
	umint TCLinkedList<t_CData, t_CAllocator>::f_GetLen() const
	{
		return mp_Data.m_List.f_GetLen();
	}

	template <typename t_CData, typename t_CAllocator>
	bool TCLinkedList<t_CData, t_CAllocator>::operator == (TCLinkedList const &_Other) const noexcept(noexcept(fg_GetType<t_CData const &>() == fg_GetType<t_CData const &>()))
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

	template <typename t_CData, typename t_CAllocator>
	auto TCLinkedList<t_CData, t_CAllocator>::operator <=> (TCLinkedList const &_Other) const noexcept(noexcept(fg_GetType<t_CData const &>() <=> fg_GetType<t_CData const &>()))
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

	template <typename t_CData, typename t_CAllocator>
	t_CData *TCLinkedList<t_CData, t_CAllocator>::fs_GetPrev(t_CData &_Current)
	{
		CMember *pMember = fsp_MemberFromData(_Current);
		CMember *pPrev = CInternalIterator::fs_GetPrev(pMember);
		if (pPrev)
			return &pPrev->mp_Object;
		return nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData *TCLinkedList<t_CData, t_CAllocator>::fs_GetNext(t_CData &_Current)
	{
		CMember *pMember = fsp_MemberFromData(_Current);
		CMember *pNext = CInternalIterator::fs_GetNext(pMember);
		if (pNext)
			return &pNext->mp_Object;
		return nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData const *TCLinkedList<t_CData, t_CAllocator>::fs_GetPrev(t_CData const &_Current)
	{
		CMember const *pMember = (CMember const *)&_Current;
		CMember const *pPrev = CInternalIterator::fs_GetPrev(pMember);
		if (pPrev)
			return &pPrev->mp_Object;
		return nullptr;
	}

	template <typename t_CData, typename t_CAllocator>
	t_CData const *TCLinkedList<t_CData, t_CAllocator>::fs_GetNext(t_CData const &_Current)
	{
		CMember const *pMember = (CMember const *)&_Current;
		CMember const *pNext = CInternalIterator::fs_GetNext(pMember);
		if (pNext)
			return &pNext->mp_Object;
		return nullptr;
	}
}
