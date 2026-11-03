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
		, mp_iSegment(0)
		, mp_iLocalPos(0)
		, mp_iCurrent(0)
		, mp_nElements(0)
		, mp_bValid(false)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapIterator(TCPackedMapIterator const &_iToCopy) noexcept
		: mp_pMap(_iToCopy.mp_pMap)
		, mp_iSegment(_iToCopy.mp_iSegment)
		, mp_iLocalPos(_iToCopy.mp_iLocalPos)
		, mp_iCurrent(_iToCopy.mp_iCurrent)
		, mp_nElements(_iToCopy.mp_nElements)
		, mp_bValid(_iToCopy.mp_bValid)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapIterator(CMapPointer _pMap, umint _nElements) noexcept
		: mp_pMap(_pMap)
		, mp_iSegment(0)
		, mp_iLocalPos(0)
		, mp_iCurrent(0)
		, mp_nElements(_nElements)
		, mp_bValid(false)
	{
		if (mp_pMap && mp_nElements > 0)
		{
			auto const *pData = mp_pMap->mp_pData;
			if (pData)
			{
				auto const *pMeta = pData->m_pSegmentMeta;
				mp_bValid = true;

				// Find the first element
				if constexpr (t_bReverse)
				{
					mp_iCurrent = mp_nElements - 1;
					// Start from the last element
					// Safe reverse loop for unsigned umint
					for (umint iSeg = pData->m_nSegments; iSeg-- > 0;)
					{
						if (pMeta[iSeg].m_Count > 0)
						{
							mp_iSegment = iSeg;
							mp_iLocalPos = pMeta[iSeg].m_Count - 1;
							break;
						}
					}
				}
				else
				{
					mp_iCurrent = 0;
					// Start from the first element
					for (mp_iSegment = 0; mp_iSegment < pData->m_nSegments; ++mp_iSegment)
					{
						if (pMeta[mp_iSegment].m_Count > 0)
						{
							mp_iLocalPos = 0;
							break;
						}
					}
				}
			}
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator bool () const noexcept
	{
		return mp_bValid;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small umint TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::f_GetLen() const noexcept
	{
		if (!mp_bValid)
			return 0;

		if constexpr (t_bReverse)
			return mp_iCurrent + 1;
		else
			return mp_nElements - mp_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::f_GetIterator() const noexcept -> TCPackedMapIterator
	{
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator = (TCPackedMapIterator const &_iToCopy) noexcept -> TCPackedMapIterator &
	{
		mp_pMap = _iToCopy.mp_pMap;
		mp_iSegment = _iToCopy.mp_iSegment;
		mp_iLocalPos = _iToCopy.mp_iLocalPos;
		mp_iCurrent = _iToCopy.mp_iCurrent;
		mp_nElements = _iToCopy.mp_nElements;
		mp_bValid = _iToCopy.mp_bValid;
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator * () const noexcept -> CValueRef
	{
		auto const *pData = mp_pMap->mp_pData;
		umint Count = pData->m_pSegmentMeta[mp_iSegment].m_Count;
		umint iFirst = mp_pMap->fsp_GetSegmentFirstSlot(mp_iSegment, Count);
		umint iSlot = iFirst + mp_iLocalPos;

		return pData->m_pValues[iSlot];
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator -> () const noexcept -> CValuePtr
	{
		return &operator * ();
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator ++ () noexcept -> TCPackedMapIterator &
	{
		if (!mp_bValid)
			return *this;

		if constexpr (t_bReverse)
		{
			if (mp_iCurrent == 0)
			{
				mp_bValid = false;
				return *this;
			}

			--mp_iCurrent;
			fp_RetreatToPrevElement();
		}
		else
		{
			++mp_iCurrent;
			if (mp_iCurrent >= mp_nElements)
			{
				mp_bValid = false;
				return *this;
			}

			fp_AdvanceToNextElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator ++ (int) noexcept -> TCPackedMapIterator
	{
		auto iCopy = *this;
		++(*this);
		return iCopy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator -- () noexcept -> TCPackedMapIterator &
	{
		if (!mp_pMap || mp_nElements == 0)
			return *this;

		auto const *pData = mp_pMap->mp_pData;
		if (!pData)
			return *this;

		auto const *pMeta = pData->m_pSegmentMeta;

		if (!mp_bValid)
		{
			mp_bValid = true;

			if constexpr (t_bReverse)
			{
				mp_iCurrent = 0;
				for (mp_iSegment = 0; mp_iSegment < pData->m_nSegments; ++mp_iSegment)
				{
					if (pMeta[mp_iSegment].m_Count > 0)
					{
						mp_iLocalPos = 0;
						break;
					}
				}
			}
			else
			{
				mp_iCurrent = mp_nElements - 1;
				for (umint iSeg = pData->m_nSegments; iSeg-- > 0;)
				{
					if (pMeta[iSeg].m_Count > 0)
					{
						mp_iSegment = iSeg;
						mp_iLocalPos = pMeta[iSeg].m_Count - 1;
						break;
					}
				}
			}

			return *this;
		}

		if constexpr (t_bReverse)
		{
			if (mp_iCurrent + 1 >= mp_nElements)
				return *this;

			++mp_iCurrent;
			fp_AdvanceToNextElement();
		}
		else
		{
			if (mp_iCurrent == 0)
				return *this;

			--mp_iCurrent;
			fp_RetreatToPrevElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator -- (int) noexcept -> TCPackedMapIterator
	{
		auto iCopy = *this;
		--(*this);
		return iCopy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr bool TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::operator == (TCPackedMapIterator const &_iOther) const noexcept
	{
		if (mp_pMap != _iOther.mp_pMap)
			return false;

		if (!mp_bValid || !_iOther.mp_bValid)
			return mp_bValid == _iOther.mp_bValid;

		return mp_iCurrent == _iOther.mp_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::fp_AdvanceToNextElement() noexcept
	{
		auto const *pData = mp_pMap->mp_pData;
		auto const *pMeta = pData->m_pSegmentMeta;

		++mp_iLocalPos;
		if (mp_iLocalPos >= pMeta[mp_iSegment].m_Count)
		{
			// Move to next segment
			++mp_iSegment;
			while (mp_iSegment < pData->m_nSegments && pMeta[mp_iSegment].m_Count == 0)
				++mp_iSegment;
			mp_iLocalPos = 0;
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapIterator<t_CMap, t_bConst, t_bReverse>::fp_RetreatToPrevElement() noexcept
	{
		auto const *pMeta = mp_pMap->mp_pData->m_pSegmentMeta;

		if (mp_iLocalPos > 0)
			--mp_iLocalPos;
		else
		{
			// Move to previous segment - safe check for unsigned umint
			if (mp_iSegment == 0)
				return; // Can't retreat further

			// Safe reverse search for unsigned umint
			for (umint iSeg = mp_iSegment; iSeg-- > 0;)
			{
				if (pMeta[iSeg].m_Count > 0)
				{
					mp_iSegment = iSeg;
					mp_iLocalPos = pMeta[iSeg].m_Count - 1;
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
		, mp_iSegment(0)
		, mp_iLocalPos(0)
		, mp_iCurrent(0)
		, mp_nElements(0)
		, mp_bValid(false)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyIterator(TCPackedMapKeyIterator const &_iToCopy) noexcept
		: mp_pMap(_iToCopy.mp_pMap)
		, mp_iSegment(_iToCopy.mp_iSegment)
		, mp_iLocalPos(_iToCopy.mp_iLocalPos)
		, mp_iCurrent(_iToCopy.mp_iCurrent)
		, mp_nElements(_iToCopy.mp_nElements)
		, mp_bValid(_iToCopy.mp_bValid)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyIterator(CMapPointer _pMap, umint _nElements) noexcept
		: mp_pMap(_pMap)
		, mp_iSegment(0)
		, mp_iLocalPos(0)
		, mp_iCurrent(0)
		, mp_nElements(_nElements)
		, mp_bValid(false)
	{
		if (mp_pMap && mp_nElements > 0)
		{
			auto const *pData = mp_pMap->mp_pData;
			if (pData)
			{
				auto const *pMeta = pData->m_pSegmentMeta;
				mp_bValid = true;

				if constexpr (t_bReverse)
				{
					mp_iCurrent = mp_nElements - 1;
					// Safe reverse loop for unsigned umint
					for (umint iSeg = pData->m_nSegments; iSeg-- > 0;)
					{
						if (pMeta[iSeg].m_Count > 0)
						{
							mp_iSegment = iSeg;
							mp_iLocalPos = pMeta[iSeg].m_Count - 1;
							break;
						}
					}
				}
				else
				{
					mp_iCurrent = 0;
					for (mp_iSegment = 0; mp_iSegment < pData->m_nSegments; ++mp_iSegment)
					{
						if (pMeta[mp_iSegment].m_Count > 0)
						{
							mp_iLocalPos = 0;
							break;
						}
					}
				}
			}
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator bool () const noexcept
	{
		return mp_bValid;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small umint TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::f_GetLen() const noexcept
	{
		if (!mp_bValid)
			return 0;

		if constexpr (t_bReverse)
			return mp_iCurrent + 1;
		else
			return mp_nElements - mp_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small auto TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::f_GetIterator() const noexcept -> TCPackedMapKeyIterator
	{
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator = (TCPackedMapKeyIterator const &_iToCopy) noexcept
	{
		mp_pMap = _iToCopy.mp_pMap;
		mp_iSegment = _iToCopy.mp_iSegment;
		mp_iLocalPos = _iToCopy.mp_iLocalPos;
		mp_iCurrent = _iToCopy.mp_iCurrent;
		mp_nElements = _iToCopy.mp_nElements;
		mp_bValid = _iToCopy.mp_bValid;
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator * () const noexcept -> CKeyRef
	{
		auto const *pData = mp_pMap->mp_pData;
		umint Count = pData->m_pSegmentMeta[mp_iSegment].m_Count;
		umint iFirst = mp_pMap->fsp_GetSegmentFirstSlot(mp_iSegment, Count);
		umint iSlot = iFirst + mp_iLocalPos;

		return pData->m_pKeys[iSlot];
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator -> () const noexcept -> CKeyPtr
	{
		return &operator * ();
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator ++ () noexcept
	{
		if (!mp_bValid)
			return *this;

		if constexpr (t_bReverse)
		{
			if (mp_iCurrent == 0)
			{
				mp_bValid = false;
				return *this;
			}

			--mp_iCurrent;
			fp_RetreatToPrevElement();
		}
		else
		{
			++mp_iCurrent;
			if (mp_iCurrent >= mp_nElements)
			{
				mp_bValid = false;
				return *this;
			}

			fp_AdvanceToNextElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator ++ (int) noexcept
	{
		auto iCopy = *this;
		++(*this);
		return iCopy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator -- () noexcept
	{
		if (!mp_pMap || mp_nElements == 0)
			return *this;

		auto const *pData = mp_pMap->mp_pData;
		if (!pData)
			return *this;

		auto const *pMeta = pData->m_pSegmentMeta;

		if (!mp_bValid)
		{
			mp_bValid = true;

			if constexpr (t_bReverse)
			{
				mp_iCurrent = 0;
				for (mp_iSegment = 0; mp_iSegment < pData->m_nSegments; ++mp_iSegment)
				{
					if (pMeta[mp_iSegment].m_Count > 0)
					{
						mp_iLocalPos = 0;
						break;
					}
				}
			}
			else
			{
				mp_iCurrent = mp_nElements - 1;
				for (umint iSeg = pData->m_nSegments; iSeg-- > 0;)
				{
					if (pMeta[iSeg].m_Count > 0)
					{
						mp_iSegment = iSeg;
						mp_iLocalPos = pMeta[iSeg].m_Count - 1;
						break;
					}
				}
			}

			return *this;
		}

		if constexpr (t_bReverse)
		{
			if (mp_iCurrent + 1 >= mp_nElements)
				return *this;

			++mp_iCurrent;
			fp_AdvanceToNextElement();
		}
		else
		{
			if (mp_iCurrent == 0)
				return *this;

			--mp_iCurrent;
			fp_RetreatToPrevElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse> TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator -- (int) noexcept
	{
		auto iCopy = *this;
		--(*this);
		return iCopy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr bool TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::operator == (TCPackedMapKeyIterator const &_iOther) const noexcept
	{
		if (mp_pMap != _iOther.mp_pMap)
			return false;

		if (!mp_bValid || !_iOther.mp_bValid)
			return mp_bValid == _iOther.mp_bValid;

		return mp_iCurrent == _iOther.mp_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::fp_AdvanceToNextElement() noexcept
	{
		auto const *pData = mp_pMap->mp_pData;
		auto const *pMeta = pData->m_pSegmentMeta;

		++mp_iLocalPos;
		if (mp_iLocalPos >= pMeta[mp_iSegment].m_Count)
		{
			++mp_iSegment;
			while (mp_iSegment < pData->m_nSegments && pMeta[mp_iSegment].m_Count == 0)
				++mp_iSegment;
			mp_iLocalPos = 0;
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapKeyIterator<t_CMap, t_bConst, t_bReverse>::fp_RetreatToPrevElement() noexcept
	{
		auto const *pMeta = mp_pMap->mp_pData->m_pSegmentMeta;

		if (mp_iLocalPos > 0)
			--mp_iLocalPos;
		else
		{
			// Move to previous segment - safe check for unsigned umint
			if (mp_iSegment == 0)
				return; // Can't retreat further

			// Safe reverse search for unsigned umint
			for (umint iSeg = mp_iSegment; iSeg-- > 0;)
			{
				if (pMeta[iSeg].m_Count > 0)
				{
					mp_iSegment = iSeg;
					mp_iLocalPos = pMeta[iSeg].m_Count - 1;
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
		, mp_iSegment(0)
		, mp_iLocalPos(0)
		, mp_iCurrent(0)
		, mp_nElements(0)
		, mp_bValid(false)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyValueIterator(TCPackedMapKeyValueIterator const &_iToCopy) noexcept
		: mp_pMap(_iToCopy.mp_pMap)
		, mp_iSegment(_iToCopy.mp_iSegment)
		, mp_iLocalPos(_iToCopy.mp_iLocalPos)
		, mp_iCurrent(_iToCopy.mp_iCurrent)
		, mp_nElements(_iToCopy.mp_nElements)
		, mp_bValid(_iToCopy.mp_bValid)
	{
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::TCPackedMapKeyValueIterator(CMapPointer _pMap, umint _nElements) noexcept
		: mp_pMap(_pMap)
		, mp_iSegment(0)
		, mp_iLocalPos(0)
		, mp_iCurrent(0)
		, mp_nElements(_nElements)
		, mp_bValid(false)
	{
		if (mp_pMap && mp_nElements > 0)
		{
			auto const *pData = mp_pMap->mp_pData;
			if (pData)
			{
				auto const *pMeta = pData->m_pSegmentMeta;
				mp_bValid = true;

				if constexpr (t_bReverse)
				{
					mp_iCurrent = mp_nElements - 1;
					// Safe reverse loop for unsigned umint
					for (umint iSeg = pData->m_nSegments; iSeg-- > 0;)
					{
						if (pMeta[iSeg].m_Count > 0)
						{
							mp_iSegment = iSeg;
							mp_iLocalPos = pMeta[iSeg].m_Count - 1;
							break;
						}
					}
				}
				else
				{
					mp_iCurrent = 0;
					for (mp_iSegment = 0; mp_iSegment < pData->m_nSegments; ++mp_iSegment)
					{
						if (pMeta[mp_iSegment].m_Count > 0)
						{
							mp_iLocalPos = 0;
							break;
						}
					}
				}
			}
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator bool () const noexcept
	{
		return mp_bValid;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small umint TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::f_GetLen() const noexcept
	{
		if (!mp_bValid)
			return 0;

		if constexpr (t_bReverse)
			return mp_iCurrent + 1;
		else
			return mp_nElements - mp_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr inline_small auto TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::f_GetIterator() const noexcept -> TCPackedMapKeyValueIterator
	{
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator = (TCPackedMapKeyValueIterator const &_iToCopy) noexcept
		-> TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> &
	{
		mp_pMap = _iToCopy.mp_pMap;
		mp_iSegment = _iToCopy.mp_iSegment;
		mp_iLocalPos = _iToCopy.mp_iLocalPos;
		mp_iCurrent = _iToCopy.mp_iCurrent;
		mp_nElements = _iToCopy.mp_nElements;
		mp_bValid = _iToCopy.mp_bValid;
		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr auto TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator * () const noexcept -> CRef
	{
		auto const *pData = mp_pMap->mp_pData;
		umint Count = pData->m_pSegmentMeta[mp_iSegment].m_Count;
		umint iFirst = mp_pMap->fsp_GetSegmentFirstSlot(mp_iSegment, Count);
		umint iSlot = iFirst + mp_iLocalPos;

		return CRef{pData->m_pKeys[iSlot], pData->m_pValues[iSlot]};
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator ++ () noexcept
	{
		if (!mp_bValid)
			return *this;

		if constexpr (t_bReverse)
		{
			if (mp_iCurrent == 0)
			{
				mp_bValid = false;
				return *this;
			}

			--mp_iCurrent;
			fp_RetreatToPrevElement();
		}
		else
		{
			++mp_iCurrent;
			if (mp_iCurrent >= mp_nElements)
			{
				mp_bValid = false;
				return *this;
			}

			fp_AdvanceToNextElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator ++ (int) noexcept
	{
		auto iCopy = *this;
		++(*this);
		return iCopy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> &TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator -- () noexcept
	{
		if (!mp_pMap || mp_nElements == 0)
			return *this;

		auto const *pData = mp_pMap->mp_pData;
		if (!pData)
			return *this;

		auto const *pMeta = pData->m_pSegmentMeta;

		if (!mp_bValid)
		{
			mp_bValid = true;

			if constexpr (t_bReverse)
			{
				mp_iCurrent = 0;
				for (mp_iSegment = 0; mp_iSegment < pData->m_nSegments; ++mp_iSegment)
				{
					if (pMeta[mp_iSegment].m_Count > 0)
					{
						mp_iLocalPos = 0;
						break;
					}
				}
			}
			else
			{
				mp_iCurrent = mp_nElements - 1;
				for (umint iSeg = pData->m_nSegments; iSeg-- > 0;)
				{
					if (pMeta[iSeg].m_Count > 0)
					{
						mp_iSegment = iSeg;
						mp_iLocalPos = pMeta[iSeg].m_Count - 1;
						break;
					}
				}
			}

			return *this;
		}

		if constexpr (t_bReverse)
		{
			if (mp_iCurrent + 1 >= mp_nElements)
				return *this;

			++mp_iCurrent;
			fp_AdvanceToNextElement();
		}
		else
		{
			if (mp_iCurrent == 0)
				return *this;

			--mp_iCurrent;
			fp_RetreatToPrevElement();
		}

		return *this;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse> TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator -- (int) noexcept
	{
		auto iCopy = *this;
		--(*this);
		return iCopy;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr bool TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::operator == (TCPackedMapKeyValueIterator const &_iOther) const noexcept
	{
		if (mp_pMap != _iOther.mp_pMap)
			return false;

		if (!mp_bValid || !_iOther.mp_bValid)
			return mp_bValid == _iOther.mp_bValid;

		return mp_iCurrent == _iOther.mp_iCurrent;
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::fp_AdvanceToNextElement() noexcept
	{
		auto const *pData = mp_pMap->mp_pData;
		auto const *pMeta = pData->m_pSegmentMeta;

		++mp_iLocalPos;
		if (mp_iLocalPos >= pMeta[mp_iSegment].m_Count)
		{
			++mp_iSegment;
			while (mp_iSegment < pData->m_nSegments && pMeta[mp_iSegment].m_Count == 0)
				++mp_iSegment;
			mp_iLocalPos = 0;
		}
	}

	template <typename t_CMap, bool t_bConst, bool t_bReverse>
	constexpr void TCPackedMapKeyValueIterator<t_CMap, t_bConst, t_bReverse>::fp_RetreatToPrevElement() noexcept
	{
		auto const *pMeta = mp_pMap->mp_pData->m_pSegmentMeta;

		if (mp_iLocalPos > 0)
			--mp_iLocalPos;
		else
		{
			// Move to previous segment - safe check for unsigned umint
			if (mp_iSegment == 0)
				return; // Can't retreat further

			// Safe reverse search for unsigned umint
			for (umint iSeg = mp_iSegment; iSeg-- > 0;)
			{
				if (pMeta[iSeg].m_Count > 0)
				{
					mp_iSegment = iSeg;
					mp_iLocalPos = pMeta[iSeg].m_Count - 1;
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
		CIterator iResult(this, f_GetLen());
		iResult.mp_iSegment = FindResult.m_iSegment;
		iResult.mp_iLocalPos = FindResult.m_iLocalPos;
		iResult.mp_iCurrent = Rank;
		return iResult;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetIterator(tf_CKey &&_Key) const noexcept -> CIteratorConst
	{
		auto FindResult = fsp_Find(mp_pData, mp_Compare, _Key);
		if (!FindResult.m_bExists)
			return CIteratorConst();

		umint Rank = fsp_ComputeRank(mp_pData, FindResult.m_iSegment, FindResult.m_iLocalPos);
		CIteratorConst iResult(this, f_GetLen());
		iResult.mp_iSegment = FindResult.m_iSegment;
		iResult.mp_iLocalPos = FindResult.m_iLocalPos;
		iResult.mp_iCurrent = Rank;
		return iResult;
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
		CIterator iResult(this, f_GetLen());
		iResult.mp_iSegment = iSegment;
		iResult.mp_iLocalPos = iLocalPos;
		iResult.mp_iCurrent = Rank;
		return iResult;
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

		CIteratorConst iResult(this, f_GetLen());
		iResult.mp_iSegment = iSegment;
		iResult.mp_iLocalPos = iLocalPos;
		iResult.mp_iCurrent = Rank;
		return iResult;
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
		CIterator iResult(this, f_GetLen());
		iResult.mp_iSegment = iSegment;
		iResult.mp_iLocalPos = iLocalPos;
		iResult.mp_iCurrent = Rank;
		return iResult;
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

		CIteratorConst iResult(this, f_GetLen());
		iResult.mp_iSegment = iSegment;
		iResult.mp_iLocalPos = iLocalPos;
		iResult.mp_iCurrent = Rank;
		return iResult;
	}
}
