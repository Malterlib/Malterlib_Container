// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Core/Core>

#include "Malterlib_Container_PackedMap_Options.h"

namespace NMib::NContainer
{
	// Forward declarations
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	struct TCPackedMap;

	template <typename t_CKey, typename t_CValue>
	struct TCPackedMapRef;

	// Main value iterator
	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	struct TCPackedMapIterator
	{
		using CMap = t_CMap;
		using CValue = typename t_CMap::CValue;
		using CValueRef = TCConditional<t_bConst, CValue const &, CValue &>;
		using CValuePtr = TCConditional<t_bConst, CValue const *, CValue *>;
		using CMapPointer = TCConditional<t_bConst, t_CMap const *, t_CMap *>;

		constexpr TCPackedMapIterator() noexcept;
		constexpr TCPackedMapIterator(TCPackedMapIterator const &_ToCopy) noexcept;
		constexpr TCPackedMapIterator(CMapPointer _Map, umint _nElements) noexcept;

		constexpr TCPackedMapIterator &operator = (TCPackedMapIterator const &_ToCopy) noexcept;

		constexpr inline_small umint f_GetLen() const noexcept;
		constexpr inline_small TCPackedMapIterator f_GetIterator() const noexcept;

		constexpr inline_small operator bool() const noexcept;
		constexpr inline_small CValueRef operator * () const noexcept;
		constexpr inline_small CValuePtr operator -> () const noexcept;
		constexpr inline_small TCPackedMapIterator &operator ++ () noexcept;
		constexpr inline_small TCPackedMapIterator operator ++ (int) noexcept;
		constexpr inline_small TCPackedMapIterator &operator -- () noexcept;
		constexpr inline_small TCPackedMapIterator operator -- (int) noexcept;
		constexpr inline_small bool operator == (TCPackedMapIterator const &_Other) const noexcept;

	protected:
		template <typename, typename, typename, typename, CPackedMapOptions>
		friend struct TCPackedMap;

		constexpr void fp_AdvanceToNextElement() noexcept;
		constexpr void fp_RetreatToPrevElement() noexcept;

		CMapPointer mp_pMap = nullptr;
		umint m_iSegment = 0;           // Current segment
		umint m_iLocalPos = 0;          // Position within segment (relative to first element)
		umint m_iCurrent = 0;           // Absolute position in forward order
		umint m_nElements = 0;          // Total number of elements
		bool m_bValid = false;
	};

	// Key iterator - yields keys instead of values
	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	struct TCPackedMapKeyIterator
	{
		using CMap = t_CMap;
		using CKey = typename t_CMap::CKey;
		using CKeyRef = TCConditional<t_bConst, CKey const &, CKey const &>; // Keys are always const
		using CKeyPtr = TCConditional<t_bConst, CKey const *, CKey const *>;
		using CMapPointer = TCConditional<t_bConst, t_CMap const *, t_CMap *>;

		constexpr TCPackedMapKeyIterator() noexcept;
		constexpr TCPackedMapKeyIterator(TCPackedMapKeyIterator const &_ToCopy) noexcept;
		constexpr TCPackedMapKeyIterator(CMapPointer _Map, umint _nElements) noexcept;

		constexpr TCPackedMapKeyIterator &operator = (TCPackedMapKeyIterator const &_ToCopy) noexcept;

		constexpr inline_small umint f_GetLen() const noexcept;
		constexpr inline_small TCPackedMapKeyIterator f_GetIterator() const noexcept;

		constexpr inline_small operator bool() const noexcept;
		constexpr inline_small CKeyRef operator * () const noexcept;
		constexpr inline_small CKeyPtr operator -> () const noexcept;
		constexpr inline_small TCPackedMapKeyIterator &operator ++ () noexcept;
		constexpr inline_small TCPackedMapKeyIterator operator ++ (int) noexcept;
		constexpr inline_small TCPackedMapKeyIterator &operator -- () noexcept;
		constexpr inline_small TCPackedMapKeyIterator operator -- (int) noexcept;
		constexpr inline_small bool operator == (TCPackedMapKeyIterator const &_Other) const noexcept;

	protected:
		template <typename, typename, typename, typename, CPackedMapOptions>
		friend struct TCPackedMap;

		constexpr void fp_AdvanceToNextElement() noexcept;
		constexpr void fp_RetreatToPrevElement() noexcept;

		CMapPointer mp_pMap = nullptr;
		umint m_iSegment = 0;
		umint m_iLocalPos = 0;
		umint m_iCurrent = 0;
		umint m_nElements = 0;
		bool m_bValid = false;
	};

	// Key-Value iterator - yields TCPackedMapRef
	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	struct TCPackedMapKeyValueIterator
	{
		using CMap = t_CMap;
		using CKey = typename t_CMap::CKey;
		using CValue = typename t_CMap::CValue;
		using CRef = TCPackedMapRef<CKey, TCConditional<t_bConst, CValue const, CValue>>;
		using CMapPointer = TCConditional<t_bConst, t_CMap const *, t_CMap *>;

		constexpr TCPackedMapKeyValueIterator() noexcept;
		constexpr TCPackedMapKeyValueIterator(TCPackedMapKeyValueIterator const &_ToCopy) noexcept;
		constexpr TCPackedMapKeyValueIterator(CMapPointer _Map, umint _nElements) noexcept;

		constexpr TCPackedMapKeyValueIterator &operator = (TCPackedMapKeyValueIterator const &_ToCopy) noexcept;

		constexpr inline_small umint f_GetLen() const noexcept;
		constexpr inline_small TCPackedMapKeyValueIterator f_GetIterator() const noexcept;

		constexpr inline_small operator bool() const noexcept;
		constexpr inline_small CRef operator * () const noexcept;
		constexpr inline_small TCPackedMapKeyValueIterator &operator ++ () noexcept;
		constexpr inline_small TCPackedMapKeyValueIterator operator ++ (int) noexcept;
		constexpr inline_small TCPackedMapKeyValueIterator &operator -- () noexcept;
		constexpr inline_small TCPackedMapKeyValueIterator operator -- (int) noexcept;
		constexpr inline_small bool operator == (TCPackedMapKeyValueIterator const &_Other) const noexcept;

	protected:
		template <typename, typename, typename, typename, CPackedMapOptions>
		friend struct TCPackedMap;

		constexpr void fp_AdvanceToNextElement() noexcept;
		constexpr void fp_RetreatToPrevElement() noexcept;

		CMapPointer mp_pMap = nullptr;
		umint m_iSegment = 0;
		umint m_iLocalPos = 0;
		umint m_iCurrent = 0;
		umint m_nElements = 0;
		bool m_bValid = false;
	};
}
