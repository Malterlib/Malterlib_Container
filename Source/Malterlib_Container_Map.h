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
	struct TCMap;

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	struct TCMap
	{
		using CKey = t_CKey;
		using CValue = t_CValue;
		using CValueNoRef = NTraits::TCRemoveReference<t_CValue>;
		using CNode = TCMapNode<t_CKey, t_CValue>;
		using CNodeHandle = TCMapNodeHandle<CNode, t_CAllocator>;

		using CMapper = NPrivate::TCMapMapper<CNode>;
		using CConditionalMapper = NPrivate::TCMapConditionalMapper<TCMap>;

		using CIterator = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Value, false, false, false>;
		using CIteratorConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Value, false, true, false>;
		using CIteratorReverse = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Value, true, false, false>;
		using CIteratorReverseConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Value, true, true, false>;
		using CIteratorBidirectional = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Value, false, false, true>;
		using CIteratorBidirectionalConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Value, false, true, true>;
		using CIteratorBidirectionalReverse = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Value, true, false, true>;
		using CIteratorBidirectionalReverseConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Value, true, true, true>;

		using CKeyIterator = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Key, false, false, false>;
		using CKeyIteratorConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Key, false, true, false>;
		using CKeyIteratorReverse = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Key, true, false, false>;
		using CKeyIteratorReverseConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Key, true, true, false>;
		using CKeyIteratorBidirectional = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Key, false, false, true>;
		using CKeyIteratorBidirectionalConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Key, false, true, true>;
		using CKeyIteratorBidirectionalReverse = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Key, true, false, true>;
		using CKeyIteratorBidirectionalReverseConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_Key, true, true, true>;

		using CKeyValueIterator = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_KeyValue, false, false, false>;
		using CKeyValueIteratorConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_KeyValue, false, true, false>;
		using CKeyValueIteratorReverse = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_KeyValue, true, false, false>;
		using CKeyValueIteratorReverseConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_KeyValue, true, true, false>;
		using CKeyValueIteratorBidirectional = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_KeyValue, false, false, true>;
		using CKeyValueIteratorBidirectionalConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_KeyValue, false, true, true>;
		using CKeyValueIteratorBidirectionalReverse = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_KeyValue, true, false, true>;
		using CKeyValueIteratorBidirectionalReverseConst = NPrivate::TCMapIterator<TCMap, EMapIteratorAccess_KeyValue, true, true, true>;

		struct CKeys
		{
			CKeys(TCMap &_Map);

			CKeyIterator f_GetIterator();
			CKeyIteratorReverse f_GetIteratorReverse();
			CKeyIteratorBidirectional f_GetIteratorBidirectional();
			CKeyIteratorBidirectionalReverse f_GetIteratorBidirectionalReverse();

			template <typename tf_CKey>
			CKeyIterator f_GetIterator(tf_CKey &&_Key);
			template <typename tf_CKey>
			CKeyIterator f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key);
			template <typename tf_CKey>
			CKeyIterator f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key);

			template <typename tf_CKey>
			CKeyIteratorBidirectional f_GetIteratorBidirectional(tf_CKey &&_Key);
			template <typename tf_CKey>
			CKeyIteratorBidirectional f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key);
			template <typename tf_CKey>
			CKeyIteratorBidirectional f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key);

		private:
			TCMap &mp_Map;
		};

		struct CKeysConst
		{
			CKeysConst(TCMap const &_Map);

			CKeyIteratorConst f_GetIterator() const;
			CKeyIteratorReverseConst f_GetIteratorReverse() const;
			CKeyIteratorBidirectionalConst f_GetIteratorBidirectional() const;
			CKeyIteratorBidirectionalReverseConst f_GetIteratorBidirectionalReverse() const;

			template <typename tf_CKey>
			CKeyIteratorConst f_GetIterator(tf_CKey &&_Key) const;
			template <typename tf_CKey>
			CKeyIteratorConst f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const;
			template <typename tf_CKey>
			CKeyIteratorConst f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const;

			template <typename tf_CKey>
			CKeyIteratorBidirectionalConst f_GetIteratorBidirectional(tf_CKey &&_Key) const;
			template <typename tf_CKey>
			CKeyIteratorBidirectionalConst f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) const;
			template <typename tf_CKey>
			CKeyIteratorBidirectionalConst f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) const;

		private:
			TCMap const &mp_Map;
		};

		struct CEntries
		{
			CEntries(TCMap &_Map);

			CKeyValueIterator f_GetIterator();
			CKeyValueIteratorReverse f_GetIteratorReverse();
			CKeyValueIteratorBidirectional f_GetIteratorBidirectional();
			CKeyValueIteratorBidirectionalReverse f_GetIteratorBidirectionalReverse();

			template <typename tf_CKey>
			CKeyValueIterator f_GetIterator(tf_CKey &&_Key);
			template <typename tf_CKey>
			CKeyValueIterator f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key);
			template <typename tf_CKey>
			CKeyValueIterator f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key);

			template <typename tf_CKey>
			CKeyValueIteratorBidirectional f_GetIteratorBidirectional(tf_CKey &&_Key);
			template <typename tf_CKey>
			CKeyValueIteratorBidirectional f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key);
			template <typename tf_CKey>
			CKeyValueIteratorBidirectional f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key);

		private:
			TCMap &mp_Map;
		};

		struct CEntriesConst
		{
			CEntriesConst(TCMap const &_Map);

			CKeyValueIteratorConst f_GetIterator() const;
			CKeyValueIteratorReverseConst f_GetIteratorReverse() const;
			CKeyValueIteratorBidirectionalConst f_GetIteratorBidirectional() const;
			CKeyValueIteratorBidirectionalReverseConst f_GetIteratorBidirectionalReverse() const;

			template <typename tf_CKey>
			CKeyValueIteratorConst f_GetIterator(tf_CKey &&_Key) const;
			template <typename tf_CKey>
			CKeyValueIteratorConst f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const;
			template <typename tf_CKey>
			CKeyValueIteratorConst f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const;

			template <typename tf_CKey>
			CKeyValueIteratorBidirectionalConst f_GetIteratorBidirectional(tf_CKey &&_Key) const;
			template <typename tf_CKey>
			CKeyValueIteratorBidirectionalConst f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) const;
			template <typename tf_CKey>
			CKeyValueIteratorBidirectionalConst f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) const;

		private:
			TCMap const &mp_Map;
		};

		struct CFormatOptions
		{
			bool m_bSingleLine = false;
			bool m_bBrackets = true;
		};

	private:
		template <typename t_CKey2, typename t_CValue2, typename t_CCompare2, typename t_CAllocator2>
		friend struct TCMap;
		
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

		template <typename t_CMap2, EMapIteratorAccess t_Access2, bool t_bReverse2, bool t_bConst2, bool t_bBidirectional2>
		friend struct NPrivate::TCMapIterator;
		
		using CNodeCompare = TCConditional
			<
				NTraits::cIsSame<t_CCompare, NMib::CSort_Default>
				, NPrivate::TCMapNodeCompare_Default<CNode, t_CKey>
				, NPrivate::TCMapNodeCompare_Custom<CNode, t_CCompare>
			>
		;
		using CUserData = NTraits::TCRemoveReference<typename NPrivate::TCMapUserData<t_CKey, t_CValue>::CType>;
		using CAVLTree = NIntrusive::TCAVLTree<&CMapNodeBase::m_Link, void, NMemory::CAllocator_Base, CNode>;

	public:
		~TCMap();
		TCMap() noexcept;

		TCMap(TCMap &&_Other) = default;
		TCMap(TCMap const &_Other);
		TCMap(std::initializer_list<NStorage::TCTuple<t_CKey, t_CValue>> const &_Values);

		template <typename... tfp_CParams>
		TCMap(CAllocatorConstructTag const &, tfp_CParams && ...p_Params);
		
		template <typename... tfp_CParams>
		TCMap(CCompareConstructTag const &, tfp_CParams && ...p_Params);

		template <typename... tfp_CAllocatorParams, typename... tfp_CCompareParams>
		TCMap
			(
				CAllocatorConstructTag &&
				, CCompareConstructTag &&
				, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
				, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
			)
		;

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
			requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
		;
		static t_CKey const &fs_GetKey(const CUserData &_Data)
			requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
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
			requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
		;

		// This only makes sense when the actual pointer of the node is used for comparion
		bool f_TryRemovePointerBasedComparison(CUserData *_pData)
			requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
		;

		template <typename tf_CKey>
		bool f_Remove(tf_CKey &&_Key);

		template <typename tf_CKey>
		CNodeHandle f_Extract(tf_CKey &&_Key);
		CNodeHandle f_Extract(CUserData *_pData);

		template <typename tf_FOnNode>
		void f_ExtractAll(tf_FOnNode &&_fOnNode);

		void f_ExtractAndInsert(TCMap &_Map, CUserData *_pData)
			requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
		;

		bool f_IsEmpty() const;
		bool f_HasOneElement() const;
		bool f_HasMoreThanOneElement() const;
		mint f_GetLen() const;
		mint f_GetDepth() const;

		CEntries f_Entries();
		CEntriesConst f_Entries() const;

		CKeys f_Keys();
		CKeysConst f_Keys() const;

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

		template <typename tf_CKey>
		CIteratorBidirectional f_GetIteratorBidirectional(tf_CKey &&_Key);
		template <typename tf_CKey>
		CIteratorBidirectionalConst f_GetIteratorBidirectional(tf_CKey &&_Key) const;
		template <typename tf_CKey>
		CIteratorBidirectional f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key);
		template <typename tf_CKey>
		CIteratorBidirectionalConst f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) const;
		template <typename tf_CKey>
		CIteratorBidirectional f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key);
		template <typename tf_CKey>
		CIteratorBidirectionalConst f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) const;

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

		template <typename... tfp_CAllocatorParams, typename... tfp_CCompareParams, mint... tp_IndicesAllocator, mint... tp_IndicesCompare>
		TCMap
			(
				NMeta::TCIndices<tp_IndicesAllocator...> const &_IndexSequenceAllocator
				, NMeta::TCIndices<tp_IndicesCompare...> const &_IndexSequuenceCompare
				, CAllocatorConstructTag &&
				, CCompareConstructTag &&
				, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
				, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
			)
		;

		constexpr static bool mcp_bIsReference = NTraits::cIsReference<t_CValue>;

		DMibNoUniqueAddress t_CAllocator mp_Allocator;
		DMibNoUniqueAddress CNodeCompare mp_Compare;
		CAVLTree mp_Tree;
	};

	template <typename t_CType>
	concept cIsMap = NPrivate::TCIsMap<NTraits::TCRemoveReferenceAndQualifiers<t_CType>>::mc_bValue;
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
#include "Map/Malterlib_Container_Map_KeyValues.hpp"
