// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	//
	// Value Iterator Implementation
	//

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapIterator() noexcept
		: mp_pMap(nullptr)
		, m_iSegment(0)
		, m_iLocalPos(0)
		, m_iCurrent(0)
		, m_nElements(0)
		, m_bValid(false)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapIterator(TCPackedMapIterator const &_ToCopy) noexcept
		: mp_pMap(_ToCopy.mp_pMap)
		, m_iSegment(_ToCopy.m_iSegment)
		, m_iLocalPos(_ToCopy.m_iLocalPos)
		, m_iCurrent(_ToCopy.m_iCurrent)
		, m_nElements(_ToCopy.m_nElements)
		, m_bValid(_ToCopy.m_bValid)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapIterator(CMapPointer _pMap, umint _nElements) noexcept
		: mp_pMap(_pMap)
		, m_iSegment(0)
		, m_iLocalPos(0)
		, m_iCurrent(0)
		, m_nElements(_nElements)
		, m_bValid(false)
	{
		if (mp_pMap && m_nElements > 0)
		{
			auto const *pData = mp_pMap->mp_pData;
			if (pData)
			{
				auto const *pMeta = pData->m_pSegmentMeta;
				m_bValid = true;

				// Find the first element
				if constexpr (t_bReverse)
				{
					m_iCurrent = m_nElements - 1;
					// Start from the last element
					// Safe reverse loop for unsigned umint
					for (umint iSeg = pData->m_nSegments; iSeg-- > 0; )
					{
						if (pMeta[iSeg].m_Count > 0)
						{
							m_iSegment = iSeg;
							m_iLocalPos = pMeta[iSeg].m_Count - 1;
							break;
						}
					}
				}
				else
				{
					m_iCurrent = 0;
					// Start from the first element
					for (m_iSegment = 0; m_iSegment < pData->m_nSegments; ++m_iSegment)
					{
						if (pMeta[m_iSegment].m_Count > 0)
						{
							m_iLocalPos = 0;
							break;
						}
					}
				}
			}
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator bool() const noexcept
	{
		return m_bValid;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small umint TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::f_GetLen() const noexcept
	{
		if (!m_bValid)
			return 0;

		if constexpr (t_bReverse)
			return m_iCurrent + 1;
		else
			return m_nElements - m_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::f_GetIterator() const noexcept -> TCPackedMapIterator
	{
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator = (TCPackedMapIterator const &_ToCopy) noexcept -> TCPackedMapIterator &
	{
		mp_pMap = _ToCopy.mp_pMap;
		m_iSegment = _ToCopy.m_iSegment;
		m_iLocalPos = _ToCopy.m_iLocalPos;
		m_iCurrent = _ToCopy.m_iCurrent;
		m_nElements = _ToCopy.m_nElements;
		m_bValid = _ToCopy.m_bValid;
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator * () const noexcept -> CValueRef
	{
		auto const *pData = mp_pMap->mp_pData;
		umint Count = pData->m_pSegmentMeta[m_iSegment].m_Count;
		umint iFirst = mp_pMap->fsp_GetSegmentFirstSlot(m_iSegment, Count);
		umint iSlot = iFirst + m_iLocalPos;

		return pData->m_pValues[iSlot];
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator -> () const noexcept -> CValuePtr
	{
		return &operator*();
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator ++ () noexcept -> TCPackedMapIterator &
	{
		if (!m_bValid)
			return *this;

		if constexpr (t_bReverse)
		{
			if (m_iCurrent == 0)
			{
				m_bValid = false;
				return *this;
			}

			--m_iCurrent;
			fp_RetreatToPrevElement();
		}
		else
		{
			++m_iCurrent;
			if (m_iCurrent >= m_nElements)
			{
				m_bValid = false;
				return *this;
			}

			fp_AdvanceToNextElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator ++ (int) noexcept -> TCPackedMapIterator
	{
		auto Copy = *this;
		++(*this);
		return Copy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator -- () noexcept -> TCPackedMapIterator &
	{
		if (!mp_pMap || m_nElements == 0)
			return *this;

		auto const *pData = mp_pMap->mp_pData;
		if (!pData)
			return *this;

		auto const *pMeta = pData->m_pSegmentMeta;

		if (!m_bValid)
		{
			m_bValid = true;

			if constexpr (t_bReverse)
			{
				m_iCurrent = 0;
				for (m_iSegment = 0; m_iSegment < pData->m_nSegments; ++m_iSegment)
				{
					if (pMeta[m_iSegment].m_Count > 0)
					{
						m_iLocalPos = 0;
						break;
					}
				}
			}
			else
			{
				m_iCurrent = m_nElements - 1;
				for (umint iSeg = pData->m_nSegments; iSeg-- > 0; )
				{
					if (pMeta[iSeg].m_Count > 0)
					{
						m_iSegment = iSeg;
						m_iLocalPos = pMeta[iSeg].m_Count - 1;
						break;
					}
				}
			}

			return *this;
		}

		if constexpr (t_bReverse)
		{
			if (m_iCurrent + 1 >= m_nElements)
				return *this;

			++m_iCurrent;
			fp_AdvanceToNextElement();
		}
		else
		{
			if (m_iCurrent == 0)
				return *this;

			--m_iCurrent;
			fp_RetreatToPrevElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator -- (int) noexcept -> TCPackedMapIterator
	{
		auto Copy = *this;
		--(*this);
		return Copy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr bool TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator == (TCPackedMapIterator const &_Other) const noexcept
	{
		if (mp_pMap != _Other.mp_pMap)
			return false;

		if (!m_bValid || !_Other.m_bValid)
			return m_bValid == _Other.m_bValid;

		return m_iCurrent == _Other.m_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::fp_AdvanceToNextElement() noexcept
	{
		auto const *pData = mp_pMap->mp_pData;
		auto const *pMeta = pData->m_pSegmentMeta;

		++m_iLocalPos;
		if (m_iLocalPos >= pMeta[m_iSegment].m_Count)
		{
			// Move to next segment
			++m_iSegment;
			while (m_iSegment < pData->m_nSegments && pMeta[m_iSegment].m_Count == 0)
				++m_iSegment;
			m_iLocalPos = 0;
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::fp_RetreatToPrevElement() noexcept
	{
		auto const *pMeta = mp_pMap->mp_pData->m_pSegmentMeta;

		if (m_iLocalPos > 0)
			--m_iLocalPos;
		else
		{
			// Move to previous segment - safe check for unsigned umint
			if (m_iSegment == 0)
				return;  // Can't retreat further

			// Safe reverse search for unsigned umint
			for (umint iSeg = m_iSegment; iSeg-- > 0; )
			{
				if (pMeta[iSeg].m_Count > 0)
				{
					m_iSegment = iSeg;
					m_iLocalPos = pMeta[iSeg].m_Count - 1;
					return;
				}
			}
		}
	}

	//
	// Key Iterator Implementation
	//

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyIterator() noexcept
		: mp_pMap(nullptr)
		, m_iSegment(0)
		, m_iLocalPos(0)
		, m_iCurrent(0)
		, m_nElements(0)
		, m_bValid(false)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyIterator(TCPackedMapKeyIterator const &_ToCopy) noexcept
		: mp_pMap(_ToCopy.mp_pMap)
		, m_iSegment(_ToCopy.m_iSegment)
		, m_iLocalPos(_ToCopy.m_iLocalPos)
		, m_iCurrent(_ToCopy.m_iCurrent)
		, m_nElements(_ToCopy.m_nElements)
		, m_bValid(_ToCopy.m_bValid)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyIterator(CMapPointer _pMap, umint _nElements) noexcept
		: mp_pMap(_pMap)
		, m_iSegment(0)
		, m_iLocalPos(0)
		, m_iCurrent(0)
		, m_nElements(_nElements)
		, m_bValid(false)
	{
		if (mp_pMap && m_nElements > 0)
		{
			auto const *pData = mp_pMap->mp_pData;
			if (pData)
			{
				auto const *pMeta = pData->m_pSegmentMeta;
				m_bValid = true;

				if constexpr (t_bReverse)
				{
					m_iCurrent = m_nElements - 1;
					// Safe reverse loop for unsigned umint
					for (umint iSeg = pData->m_nSegments; iSeg-- > 0; )
					{
						if (pMeta[iSeg].m_Count > 0)
						{
							m_iSegment = iSeg;
							m_iLocalPos = pMeta[iSeg].m_Count - 1;
							break;
						}
					}
				}
				else
				{
					m_iCurrent = 0;
					for (m_iSegment = 0; m_iSegment < pData->m_nSegments; ++m_iSegment)
					{
						if (pMeta[m_iSegment].m_Count > 0)
						{
							m_iLocalPos = 0;
							break;
						}
					}
				}
			}
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator bool() const noexcept
	{
		return m_bValid;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small umint TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::f_GetLen() const noexcept
	{
		if (!m_bValid)
			return 0;

		if constexpr (t_bReverse)
			return m_iCurrent + 1;
		else
			return m_nElements - m_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small auto TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::f_GetIterator() const noexcept -> TCPackedMapKeyIterator
	{
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator = (TCPackedMapKeyIterator const &_ToCopy) noexcept
	{
		mp_pMap = _ToCopy.mp_pMap;
		m_iSegment = _ToCopy.m_iSegment;
		m_iLocalPos = _ToCopy.m_iLocalPos;
		m_iCurrent = _ToCopy.m_iCurrent;
		m_nElements = _ToCopy.m_nElements;
		m_bValid = _ToCopy.m_bValid;
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator * () const noexcept -> CKeyRef
	{
		auto const *pData = mp_pMap->mp_pData;
		umint Count = pData->m_pSegmentMeta[m_iSegment].m_Count;
		umint iFirst = mp_pMap->fsp_GetSegmentFirstSlot(m_iSegment, Count);
		umint iSlot = iFirst + m_iLocalPos;

		return pData->m_pKeys[iSlot];
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator -> () const noexcept -> CKeyPtr
	{
		return &operator*();
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator ++ () noexcept
	{
		if (!m_bValid)
			return *this;

		if constexpr (t_bReverse)
		{
			if (m_iCurrent == 0)
			{
				m_bValid = false;
				return *this;
			}

			--m_iCurrent;
			fp_RetreatToPrevElement();
		}
		else
		{
			++m_iCurrent;
			if (m_iCurrent >= m_nElements)
			{
				m_bValid = false;
				return *this;
			}

			fp_AdvanceToNextElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator ++ (int) noexcept
	{
		auto Copy = *this;
		++(*this);
		return Copy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator -- () noexcept
	{
		if (!mp_pMap || m_nElements == 0)
			return *this;

		auto const *pData = mp_pMap->mp_pData;
		if (!pData)
			return *this;

		auto const *pMeta = pData->m_pSegmentMeta;

		if (!m_bValid)
		{
			m_bValid = true;

			if constexpr (t_bReverse)
			{
				m_iCurrent = 0;
				for (m_iSegment = 0; m_iSegment < pData->m_nSegments; ++m_iSegment)
				{
					if (pMeta[m_iSegment].m_Count > 0)
					{
						m_iLocalPos = 0;
						break;
					}
				}
			}
			else
			{
				m_iCurrent = m_nElements - 1;
				for (umint iSeg = pData->m_nSegments; iSeg-- > 0; )
				{
					if (pMeta[iSeg].m_Count > 0)
					{
						m_iSegment = iSeg;
						m_iLocalPos = pMeta[iSeg].m_Count - 1;
						break;
					}
				}
			}

			return *this;
		}

		if constexpr (t_bReverse)
		{
			if (m_iCurrent + 1 >= m_nElements)
				return *this;

			++m_iCurrent;
			fp_AdvanceToNextElement();
		}
		else
		{
			if (m_iCurrent == 0)
				return *this;

			--m_iCurrent;
			fp_RetreatToPrevElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator -- (int) noexcept
	{
		auto Copy = *this;
		--(*this);
		return Copy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr bool TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator == (TCPackedMapKeyIterator const &_Other) const noexcept
	{
		if (mp_pMap != _Other.mp_pMap)
			return false;

		if (!m_bValid || !_Other.m_bValid)
			return m_bValid == _Other.m_bValid;

		return m_iCurrent == _Other.m_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::fp_AdvanceToNextElement() noexcept
	{
		auto const *pData = mp_pMap->mp_pData;
		auto const *pMeta = pData->m_pSegmentMeta;

		++m_iLocalPos;
		if (m_iLocalPos >= pMeta[m_iSegment].m_Count)
		{
			++m_iSegment;
			while (m_iSegment < pData->m_nSegments && pMeta[m_iSegment].m_Count == 0)
				++m_iSegment;
			m_iLocalPos = 0;
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::fp_RetreatToPrevElement() noexcept
	{
		auto const *pMeta = mp_pMap->mp_pData->m_pSegmentMeta;

		if (m_iLocalPos > 0)
			--m_iLocalPos;
		else
		{
			// Move to previous segment - safe check for unsigned umint
			if (m_iSegment == 0)
				return;  // Can't retreat further

			// Safe reverse search for unsigned umint
			for (umint iSeg = m_iSegment; iSeg-- > 0; )
			{
				if (pMeta[iSeg].m_Count > 0)
				{
					m_iSegment = iSeg;
					m_iLocalPos = pMeta[iSeg].m_Count - 1;
					return;
				}
			}
		}
	}

	//
	// Key-Value Iterator Implementation
	//

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyValueIterator() noexcept
		: mp_pMap(nullptr)
		, m_iSegment(0)
		, m_iLocalPos(0)
		, m_iCurrent(0)
		, m_nElements(0)
		, m_bValid(false)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyValueIterator(TCPackedMapKeyValueIterator const &_ToCopy) noexcept
		: mp_pMap(_ToCopy.mp_pMap)
		, m_iSegment(_ToCopy.m_iSegment)
		, m_iLocalPos(_ToCopy.m_iLocalPos)
		, m_iCurrent(_ToCopy.m_iCurrent)
		, m_nElements(_ToCopy.m_nElements)
		, m_bValid(_ToCopy.m_bValid)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyValueIterator(CMapPointer _pMap, umint _nElements) noexcept
		: mp_pMap(_pMap)
		, m_iSegment(0)
		, m_iLocalPos(0)
		, m_iCurrent(0)
		, m_nElements(_nElements)
		, m_bValid(false)
	{
		if (mp_pMap && m_nElements > 0)
		{
			auto const *pData = mp_pMap->mp_pData;
			if (pData)
			{
				auto const *pMeta = pData->m_pSegmentMeta;
				m_bValid = true;

				if constexpr (t_bReverse)
				{
					m_iCurrent = m_nElements - 1;
					// Safe reverse loop for unsigned umint
					for (umint iSeg = pData->m_nSegments; iSeg-- > 0; )
					{
						if (pMeta[iSeg].m_Count > 0)
						{
							m_iSegment = iSeg;
							m_iLocalPos = pMeta[iSeg].m_Count - 1;
							break;
						}
					}
				}
				else
				{
					m_iCurrent = 0;
					for (m_iSegment = 0; m_iSegment < pData->m_nSegments; ++m_iSegment)
					{
						if (pMeta[m_iSegment].m_Count > 0)
						{
							m_iLocalPos = 0;
							break;
						}
					}
				}
			}
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator bool() const noexcept
	{
		return m_bValid;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small umint TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::f_GetLen() const noexcept
	{
		if (!m_bValid)
			return 0;

		if constexpr (t_bReverse)
			return m_iCurrent + 1;
		else
			return m_nElements - m_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small auto TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::f_GetIterator() const noexcept -> TCPackedMapKeyValueIterator
	{
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator =
		(
			TCPackedMapKeyValueIterator const &_ToCopy
		)
		noexcept
	{
		mp_pMap = _ToCopy.mp_pMap;
		m_iSegment = _ToCopy.m_iSegment;
		m_iLocalPos = _ToCopy.m_iLocalPos;
		m_iCurrent = _ToCopy.m_iCurrent;
		m_nElements = _ToCopy.m_nElements;
		m_bValid = _ToCopy.m_bValid;
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator * () const noexcept -> CRef
	{
		auto const *pData = mp_pMap->mp_pData;
		umint Count = pData->m_pSegmentMeta[m_iSegment].m_Count;
		umint iFirst = mp_pMap->fsp_GetSegmentFirstSlot(m_iSegment, Count);
		umint iSlot = iFirst + m_iLocalPos;

		return CRef{pData->m_pKeys[iSlot], pData->m_pValues[iSlot]};
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator ++ () noexcept
	{
		if (!m_bValid)
			return *this;

		if constexpr (t_bReverse)
		{
			if (m_iCurrent == 0)
			{
				m_bValid = false;
				return *this;
			}

			--m_iCurrent;
			fp_RetreatToPrevElement();
		}
		else
		{
			++m_iCurrent;
			if (m_iCurrent >= m_nElements)
			{
				m_bValid = false;
				return *this;
			}

			fp_AdvanceToNextElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator ++ (int) noexcept
	{
		auto Copy = *this;
		++(*this);
		return Copy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator -- () noexcept
	{
		if (!mp_pMap || m_nElements == 0)
			return *this;

		auto const *pData = mp_pMap->mp_pData;
		if (!pData)
			return *this;

		auto const *pMeta = pData->m_pSegmentMeta;

		if (!m_bValid)
		{
			m_bValid = true;

			if constexpr (t_bReverse)
			{
				m_iCurrent = 0;
				for (m_iSegment = 0; m_iSegment < pData->m_nSegments; ++m_iSegment)
				{
					if (pMeta[m_iSegment].m_Count > 0)
					{
						m_iLocalPos = 0;
						break;
					}
				}
			}
			else
			{
				m_iCurrent = m_nElements - 1;
				for (umint iSeg = pData->m_nSegments; iSeg-- > 0; )
				{
					if (pMeta[iSeg].m_Count > 0)
					{
						m_iSegment = iSeg;
						m_iLocalPos = pMeta[iSeg].m_Count - 1;
						break;
					}
				}
			}

			return *this;
		}

		if constexpr (t_bReverse)
		{
			if (m_iCurrent + 1 >= m_nElements)
				return *this;

			++m_iCurrent;
			fp_AdvanceToNextElement();
		}
		else
		{
			if (m_iCurrent == 0)
				return *this;

			--m_iCurrent;
			fp_RetreatToPrevElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator -- (int) noexcept
	{
		auto Copy = *this;
		--(*this);
		return Copy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr bool TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator == (TCPackedMapKeyValueIterator const &_Other) const noexcept
	{
		if (mp_pMap != _Other.mp_pMap)
			return false;

		if (!m_bValid || !_Other.m_bValid)
			return m_bValid == _Other.m_bValid;

		return m_iCurrent == _Other.m_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::fp_AdvanceToNextElement() noexcept
	{
		auto const *pData = mp_pMap->mp_pData;
		auto const *pMeta = pData->m_pSegmentMeta;

		++m_iLocalPos;
		if (m_iLocalPos >= pMeta[m_iSegment].m_Count)
		{
			++m_iSegment;
			while (m_iSegment < pData->m_nSegments && pMeta[m_iSegment].m_Count == 0)
				++m_iSegment;
			m_iLocalPos = 0;
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::fp_RetreatToPrevElement() noexcept
	{
		auto const *pMeta = mp_pMap->mp_pData->m_pSegmentMeta;

		if (m_iLocalPos > 0)
			--m_iLocalPos;
		else
		{
			// Move to previous segment - safe check for unsigned umint
			if (m_iSegment == 0)
				return;  // Can't retreat further

			// Safe reverse search for unsigned umint
			for (umint iSeg = m_iSegment; iSeg-- > 0; )
			{
				if (pMeta[iSeg].m_Count > 0)
				{
					m_iSegment = iSeg;
					m_iLocalPos = pMeta[iSeg].m_Count - 1;
					return;
				}
			}
		}
	}

	//
	// TCPackedMap Iterator Accessors
	//

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator() noexcept -> CIterator
	{
		return CIterator(this, f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator() const noexcept -> CIteratorConst
	{
		return CIteratorConst(this, f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIteratorReverse() noexcept -> CIteratorReverse
	{
		return CIteratorReverse(this, f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIteratorReverse() const noexcept -> CIteratorReverseConst
	{
		return CIteratorReverseConst(this, f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator(tf_CKey &&_Key) noexcept -> CIterator
	{
		auto FindResult = fsp_Find(mp_pData, mp_Compare, _Key);
		if (!FindResult.m_bExists)
			return CIterator();

		umint Rank = fsp_ComputeRank(mp_pData, FindResult.m_iSegment, FindResult.m_iLocalPos);
		CIterator Iter(this, f_GetLen());
		Iter.m_iSegment = FindResult.m_iSegment;
		Iter.m_iLocalPos = FindResult.m_iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator(tf_CKey &&_Key) const noexcept -> CIteratorConst
	{
		auto FindResult = fsp_Find(mp_pData, mp_Compare, _Key);
		if (!FindResult.m_bExists)
			return CIteratorConst();

		umint Rank = fsp_ComputeRank(mp_pData, FindResult.m_iSegment, FindResult.m_iLocalPos);
		CIteratorConst Iter(this, f_GetLen());
		Iter.m_iSegment = FindResult.m_iSegment;
		Iter.m_iLocalPos = FindResult.m_iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) noexcept -> CIterator
	{
		auto *pValue = f_FindSmallestGreaterThanEqual(fg_Forward<tf_CKey>(_Key));
		if (!pValue)
			return CIterator();

		// Calculate segment and local position from value pointer
		auto *pData = mp_pData;
		auto *pValues = pData->m_pValues;
		auto *pMeta = pData->m_pSegmentMeta;

		umint iSlot = pValue - pValues;
		umint iSegment = iSlot / mcp_SegmentSize;
		umint Count = pMeta[iSegment].m_Count;
		umint iFirst = fsp_GetSegmentFirstSlot(iSegment, Count);
		umint iLocalPos = iSlot - iFirst;

		umint Rank = fsp_ComputeRank(pData, iSegment, iLocalPos);
		CIterator Iter(this, f_GetLen());
		Iter.m_iSegment = iSegment;
		Iter.m_iLocalPos = iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const noexcept -> CIteratorConst
	{
		auto *pValue = f_FindSmallestGreaterThanEqual(fg_Forward<tf_CKey>(_Key));
		if (!pValue)
			return CIteratorConst();

		auto *pData = mp_pData;
		auto *pValues = pData->m_pValues;
		auto *pMeta = pData->m_pSegmentMeta;

		umint iSlot = pValue - pValues;
		umint iSegment = iSlot / mcp_SegmentSize;
		umint Count = pMeta[iSegment].m_Count;
		umint iFirst = fsp_GetSegmentFirstSlot(iSegment, Count);
		umint iLocalPos = iSlot - iFirst;
		umint Rank = fsp_ComputeRank(pData, iSegment, iLocalPos);

		CIteratorConst Iter(this, f_GetLen());
		Iter.m_iSegment = iSegment;
		Iter.m_iLocalPos = iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) noexcept -> CIterator
	{
		auto *pValue = f_FindLargestLessThanEqual(fg_Forward<tf_CKey>(_Key));
		if (!pValue)
			return CIterator();

		auto *pData = mp_pData;
		auto *pValues = pData->m_pValues;
		auto *pMeta = pData->m_pSegmentMeta;

		umint iSlot = pValue - pValues;
		umint iSegment = iSlot / mcp_SegmentSize;
		umint Count = pMeta[iSegment].m_Count;
		umint iFirst = fsp_GetSegmentFirstSlot(iSegment, Count);
		umint iLocalPos = iSlot - iFirst;

		umint Rank = fsp_ComputeRank(pData, iSegment, iLocalPos);
		CIterator Iter(this, f_GetLen());
		Iter.m_iSegment = iSegment;
		Iter.m_iLocalPos = iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const noexcept -> CIteratorConst
	{
		auto *pValue = f_FindLargestLessThanEqual(fg_Forward<tf_CKey>(_Key));
		if (!pValue)
			return CIteratorConst();

		auto *pData = mp_pData;
		auto *pValues = pData->m_pValues;
		auto *pMeta = pData->m_pSegmentMeta;

		umint iSlot = pValue - pValues;
		umint iSegment = iSlot / mcp_SegmentSize;
		umint Count = pMeta[iSegment].m_Count;
		umint iFirst = fsp_GetSegmentFirstSlot(iSegment, Count);
		umint iLocalPos = iSlot - iFirst;
		umint Rank = fsp_ComputeRank(pData, iSegment, iLocalPos);

		CIteratorConst Iter(this, f_GetLen());
		Iter.m_iSegment = iSegment;
		Iter.m_iLocalPos = iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}
}
