// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Intrusive/AVLTree>

#include "Map/Malterlib_Container_Map_Helpers.hpp"

#include "Map/Malterlib_Container_Map_Node.h"
#include "Map/Malterlib_Container_Map_Result.h"
#include "Map/Malterlib_Container_Map_Iterator.h"

#include "Map/Malterlib_Container_Map_Helpers_Compare.hpp"

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare = NMib::CSort_Default, typename t_CAllocator = NMib::NMemory::CAllocator_Heap>
	struct TCMap
	{
		using CKey = t_CKey;
		using CValue = t_CValue;
		using CValueNoRef = typename NTraits::TCRemoveReference<t_CValue>::CType;
		using CNode = TCMapNode<t_CKey, t_CValue>;
		using CNodeHandle = TCMapNodeHandle<CNode, t_CAllocator>;

		using CMapper = NPrivate::TCMapMapper<CNode>;
		using CConditionalMapper = NPrivate::TCMapConditionalMapper<TCMap>;

		using CIterator = NPrivate::TCMapIterator<TCMap, false, false, false>;
		using CIteratorConst = NPrivate::TCMapIterator<TCMap, false, true, false>;
		using CIteratorReverse = NPrivate::TCMapIterator<TCMap, true, false, false>;
		using CIteratorReverseConst = NPrivate::TCMapIterator<TCMap, true, true, false>;
		using CIteratorBidirectional = NPrivate::TCMapIterator<TCMap, false, false, true>;
		using CIteratorBidirectionalConst = NPrivate::TCMapIterator<TCMap, false, true, true>;
		using CIteratorBidirectionalReverse = NPrivate::TCMapIterator<TCMap, true, false, true>;
		using CIteratorBidirectionalReverseConst = NPrivate::TCMapIterator<TCMap, true, true, true>;

		struct CFormatOptions
		{
			bool m_bSingleLine = false;
			bool m_bBrackets = true;
		};

	private:
		template <typename t_CDestination, bool t_bIsRef>
		friend struct NPrivate::TCMapCopy;
		friend CIterator;
		friend CIteratorConst;
		friend CIteratorReverse;
		friend CIteratorReverseConst;
		friend CIteratorBidirectional;
		friend CIteratorBidirectionalConst;
		friend CIteratorBidirectionalReverse;
		friend CIteratorBidirectionalReverseConst;
		friend CMapper;
		friend CConditionalMapper;

		using CNodeCompare = typename TCChooseType
			<
				NTraits::TCIsSame<t_CCompare, NMib::CSort_Default>::mc_Value
				, NPrivate::TCMapNodeCompare_Default<CNode, t_CKey>
				, NPrivate::TCMapNodeCompare_Custom<CNode, t_CCompare>
			>::CType
		;
		using CUserData = typename NTraits::TCRemoveReference<typename NPrivate::TCMapUserData<t_CKey, t_CValue>::CType>::CType;
		using CAVLTree = NIntrusive::TCAVLTree<&CMapNodeBase::m_Link, CNodeCompare, NMemory::CAllocator_Base, CNode>;

	public:
		~TCMap();
		TCMap() noexcept;

		TCMap(TCMap &&_Other) = default;
		TCMap(TCMap const &_Other);
		TCMap(TCInitializerList<NStorage::TCTuple<t_CKey, t_CValue>> const &_Values);

		template <typename... tfp_CParams>
		TCMap(NMemory::CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params);
		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
		TCMap(TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> const &_Other);
		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
		TCMap(TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &_Other);
		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
		TCMap(TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &&_Other);

		TCMap &operator = (TCMap const &_Other);
		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
		TCMap &operator = (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> const &_Other);
		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
		TCMap &operator = (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &_Other);
		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
		TCMap &operator = (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> &&_Other);
		TCMap &operator = (TCMap &&_Other);

		void f_Clear();

		static t_CKey const &fs_GetKey(CUserData const *_pData)
			requires (!NTraits::TCIsReference<t_CValue>::mc_Value) //This function is not supported when mapping reference types
		;
		static t_CKey const &fs_GetKey(const CUserData &_Data)
			requires (!NTraits::TCIsReference<t_CValue>::mc_Value) //This function is not supported when mapping reference types
		;

		template <typename tf_CKey>
		CUserData &operator[] (tf_CKey &&_Key);

		template <typename tf_CKey, typename... tfp_CParam>
		TCMapResult<CUserData &> operator () (tf_CKey &&_Key, tfp_CParam && ... p_Params);

		template <typename tf_CKey>
		CUserData &f_Insert(tf_CKey &&_Key);
		CUserData &f_Insert(CNodeHandle &&_Node);
		CUserData &f_Insert(CNodeHandle const &_Node) = delete;
		CUserData &f_Insert(CNodeHandle &_Node) = delete;

		template <typename tf_CKey>
		CUserData &f_Map(tf_CKey &&_Key);
		template <typename tf_CKey, typename... tfp_CArgs>
		CUserData &f_Map(tf_CKey &&_Key, bool &_bCreated, tfp_CArgs && ... p_Args);

		template <typename tf_FDoInsert>
		void f_BatchMap(tf_FDoInsert &&_fDoInsert);
		template <typename tf_FDoInsert>
		void f_BatchMapIfNotMapped(tf_FDoInsert &&_fDoInsert);

		template <typename tf_CKey>
		CUserData const &operator[] (tf_CKey&& _Key) const;

		template <typename tf_CKey>
		bool f_Exists(tf_CKey &&_Key) const;

		template <typename tf_CKey>
		CUserData const *f_FindEqual(tf_CKey &&_Key) const;
		template <typename tf_CKey>
		CUserData *f_FindEqual(tf_CKey &&_Key);

		template <typename tf_CKey>
		CUserData *f_FindSmallestGreaterThanEqual(tf_CKey &&_Key);
		template <typename tf_CKey>
		CUserData const *f_FindSmallestGreaterThanEqual(tf_CKey &&_Key) const;

		template <typename tf_CKey>
		CUserData *f_FindLargestLessThanEqual(tf_CKey &&_Key);
		template <typename tf_CKey>
		CUserData const *f_FindLargestLessThanEqual(tf_CKey &&_Key) const;

		CUserData *f_FindSmallest();
		CUserData const *f_FindSmallest() const;

		CUserData *f_FindLargest();
		CUserData const *f_FindLargest() const;

		CUserData *f_FindAny();
		CUserData const *f_FindAny() const;

		t_CKey *f_FindSmallestKey();
		t_CKey const *f_FindSmallestKey() const;
		t_CKey *f_FindLargestKey();
		t_CKey *f_FindAnyKey();
		t_CKey const *f_FindLargestKey() const;

		TCMap &operator += (TCMap const &_Other);
		TCMap &operator += (TCMap &&_Other);
		TCMap &operator -= (TCMap const &_Other);

		void f_Remove(CUserData *_pData)
			requires (!NTraits::TCIsReference<t_CValue>::mc_Value) //This function is not supported when mapping reference types
		;

		// This only makes sense when the actual pointer of the node is used for comparion
		bool f_TryRemovePointerBasedComparison(CUserData *_pData)
			requires (!NTraits::TCIsReference<t_CValue>::mc_Value) //This function is not supported when mapping reference types
		;

		template <typename tf_CKey>
		bool f_Remove(tf_CKey &&_Key);

		template <typename tf_CKey>
		CNodeHandle f_Extract(tf_CKey &&_Key);
		CNodeHandle f_Extract(CUserData *_pData);

		template <typename tf_FOnNode>
		void f_ExtractAll(tf_FOnNode &&_fOnNode);

		void f_ExtractAndInsert(TCMap &_Map, CUserData *_pData)
			requires (!NTraits::TCIsReference<t_CValue>::mc_Value) //This function is not supported when mapping reference types
		;

		bool f_IsEmpty() const;
		bool f_HasOneElement() const;
		bool f_HasMoreThanOneElement() const;
		mint f_GetLen() const;
		mint f_GetDepth() const;

		CIteratorConst f_GetIterator() const;
		CIterator f_GetIterator();
		CIteratorReverseConst f_GetIteratorReverse() const;
		CIteratorReverse f_GetIteratorReverse();
		CIteratorBidirectionalConst f_GetIteratorBidirectional() const;
		CIteratorBidirectional f_GetIteratorBidirectional();
		CIteratorBidirectionalReverseConst f_GetIteratorBidirectionalReverse() const;
		CIteratorBidirectionalReverse f_GetIteratorBidirectionalReverse();

		template <typename tf_CKey>
		CIterator f_GetIterator(tf_CKey &&_Key);
		template <typename tf_CKey>
		CIteratorConst f_GetIterator(tf_CKey &&_Key) const;
		template <typename tf_CKey>
		CIterator f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key);
		template <typename tf_CKey>
		CIteratorConst f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const;
		template <typename tf_CKey>
		CIterator f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key);
		template <typename tf_CKey>
		CIteratorConst f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const;

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
		bool operator == (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> const &_Other) const;
		bool operator == (TCMap const &_Other) const;

		template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
		auto operator <=> (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> const &_Other) const;
		auto operator <=> (TCMap const &_Other) const;

		template <typename tf_COption>
		bool f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option) const;
		template <typename tf_CFormatInto, typename tf_CFormatOptions>
		void f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const;

		TCSet<t_CKey, t_CCompare, t_CAllocator> f_KeySet() const;

	private:
		template <typename tf_CSource>
		inline_always void fp_CopyAll(tf_CSource && _Other);
		template <typename tf_CSource>
		inline_always void fp_MoveAll(tf_CSource && _Other);
		template <typename tf_CSource>
		inline_always void fp_CopyAddAll(tf_CSource && _Other);
		template <typename tf_CSource>
		inline_always void fp_MoveAddAll(tf_CSource && _Other);

		constexpr static bool mcp_bIsReference = NTraits::TCIsReference<t_CValue>::mc_Value;

		DMibNoUniqueAddress t_CAllocator mp_Allocator;
		DMibNoUniqueAddress CAVLTree mp_Tree;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif

#include "Map/Malterlib_Container_Map.hpp"
#include "Map/Malterlib_Container_Map_Lifetime.hpp"
#include "Map/Malterlib_Container_Map_Batch.hpp"
#include "Map/Malterlib_Container_Map_Compare.hpp"
#include "Map/Malterlib_Container_Map_Find.hpp"
#include "Map/Malterlib_Container_Map_Insert.hpp"
#include "Map/Malterlib_Container_Map_Iterator.hpp"
#include "Map/Malterlib_Container_Map_Node.hpp"
#include "Map/Malterlib_Container_Map_Remove.hpp"
#include "Map/Malterlib_Container_Map_Result.hpp"
#include "Map/Malterlib_Container_Map_Stream.hpp"
#include "Map/Malterlib_Container_Map_Operations.hpp"
#include "Map/Malterlib_Container_Map_Occupancy.hpp"
#include "Map/Malterlib_Container_Map_Extract.hpp"
