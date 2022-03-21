// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	//
	// CAVLCompare_TCRegistry
	//

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	inline_small auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CAVLCompare_TCRegistry::operator () (TCRegistry const &_Left) const -> CRegistryKey const &
	{
		return _Left.mp_Key;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	inline_small auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CAVLCompare_TCRegistry::operator () (TCRegistry &_Left) const -> CRegistryKey &
	{
		return _Left.mp_Key;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	inline_small bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CAVLCompare_TCRegistry::operator () (CRegistryKey const &_Left, CRegistryKey const &_Right) const
	{
		return _Left.f_CompareKey(_Right) < 0;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	inline_small bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CAVLCompare_TCRegistry::operator () (CRegistryKey &_Left, CRegistryKey const &_Right) const
	{
		return _Left.f_CompareKey(_Right) < 0;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	inline_small bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CAVLCompare_TCRegistry::operator () (CRegistryKey const &_Left, CRegistryKey &_Right) const
	{
		return _Left.f_CompareKey(_Right) < 0;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	inline_small bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CAVLCompare_TCRegistry::operator () (CRegistryKey &_Left, CRegistryKey &_Right) const
	{
		return _Left.f_CompareKey(_Right) < 0;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CKey>
	inline_small bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CAVLCompare_TCRegistry::operator ()(CRegistryKey const &_Left, const tf_CKey &_Right) const
	{
		return _Left.f_Compare(_Right) < 0;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CKey>
	inline_small bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CAVLCompare_TCRegistry::operator ()(tf_CKey const &_Left, CRegistryKey const &_Right) const
	{
		return _Right.f_Compare(_Left) > 0;
	}

	//
	// CChildren_Sorted
	//

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::CChildren_Sorted(CChildren_Sorted &&_Other)
		: m_Tree(fg_Move(_Other.m_Tree))
	{
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::operator =(CChildren_Sorted &&_Other) -> CChildren_Sorted &
	{
		m_Tree = fg_Move(_Other.m_Tree);
		return *this;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::CChildren_Sorted()
	{
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::f_GetIterator() const -> CIterator
	{
		return m_Tree.f_GetIterator();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::f_Insert(TCRegistry *_pReg)
	{
		m_Tree.f_Insert(_pReg);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::f_Insert(TCRegistry *_pReg, TCRegistry *_pAfter)
	{
		m_Tree.f_Insert(_pReg);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::f_Remove(TCRegistry *_pReg)
	{
		m_Tree.f_Remove(_pReg);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CStream>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Tree;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CStream>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_Sorted::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Tree;
	}

	//
	// CChildren_PreserveOrder
	//

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::CChildren_PreserveOrder()
	{
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::CChildren_PreserveOrder(CChildren_PreserveOrder &&_Other)
		: m_Tree(fg_Move(_Other.m_Tree))
		, m_ChildrenOrder(fg_Move(_Other.m_ChildrenOrder))
	{
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::operator =(CChildren_PreserveOrder &&_Other) -> CChildren_PreserveOrder &
	{
		m_Tree = fg_Move(_Other.m_Tree);
		m_ChildrenOrder = fg_Move(_Other.m_ChildrenOrder);
		return *this;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::f_GetIterator() const -> CIterator
	{
		return fg_RemoveQualifiers(m_ChildrenOrder).f_GetIterator();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::f_Insert(TCRegistry *_pReg)
	{
		m_Tree.f_Insert(_pReg);
		m_ChildrenOrder.f_Insert(_pReg);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::f_Insert(TCRegistry *_pReg, TCRegistry *_pAfter)
	{
		m_Tree.f_Insert(_pReg);
		if (_pAfter)
			m_ChildrenOrder.f_InsertAfter(_pReg, _pAfter);
		else
			m_ChildrenOrder.f_Insert(_pReg);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::f_Remove(TCRegistry *_pReg)
	{
		m_Tree.f_Remove(_pReg);
		m_ChildrenOrder.f_Remove(_pReg);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CStream>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_ChildrenOrder;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CStream>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::CChildren_PreserveOrder::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_ChildrenOrder;
		auto Iter = m_ChildrenOrder.f_GetIterator();
		while (Iter)
		{
			m_Tree.f_Insert(Iter);
			++Iter;
		}
	}
}
