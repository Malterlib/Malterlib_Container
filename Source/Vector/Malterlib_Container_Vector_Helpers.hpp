// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename tf_CData>
	static void fg_ConstructArray(tf_CData *_pDest, umint _Len, umint &o_Len)
	{
		if constexpr (NTraits::cIsTrivialllyDefaultConstructible<tf_CData>)
		{
			o_Len += _Len;
			return;
		}
		else
		{
			for (umint iElement = 0; iElement < _Len; ++iElement)
			{
				new((void *)(_pDest + iElement)) tf_CData();
				++o_Len;
			}
		}
	}

	//////

	template <typename tf_CData>
	static void fg_DestroyArray(tf_CData *_pDest, umint _Len, umint &o_Len)
	{
		DMibFastCheck(_Len != 0);
		DMibFastCheck(_pDest);

		if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
		{
			o_Len -= _Len;
			return;
		}
		else
		{
#if DMibEnableSafeCheck > 0
			auto Cleanup = g_OnScopeExit / [&]
				{
					DMibFastCheck(false); // Destructor should not throw
				}
			;
#endif
			// Destroy all cells
			auto pEnd = _pDest - 1;
			for (auto pDest = _pDest + (_Len - 1); pDest != pEnd; --pDest, --o_Len)
				pDest->~tf_CData();

#if DMibEnableSafeCheck > 0
			Cleanup.f_Clear();
#endif
		}
	}

	//////

	template <typename tf_CData>
	static void fg_MoveArray(tf_CData *_pDest, tf_CData *_pSrc, umint _Len)
	{
		if constexpr (NTraits::cIsTriviallyMoveConstructible<tf_CData>)
		{
			if (_Len)
				NMemory::fg_MemCopy((void *)_pDest, _pSrc, _Len * sizeof(tf_CData));
		}
		else
		{
#if DMibEnableSafeCheck > 0
			auto Cleanup = g_OnScopeExit / [&]
				{
					DMibFastCheck(false); // Move should not throw
				}
			;
#endif

			for (umint iElement = 0; iElement < _Len; ++iElement)
				new((void *)(_pDest + iElement)) tf_CData(fg_Move(_pSrc[iElement]));

#if DMibEnableSafeCheck > 0
			Cleanup.f_Clear();
#endif
		}
	}

	template <typename tf_CData0, typename tf_CData1>
	static void fg_MoveArray(tf_CData0 *_pDest, tf_CData1 *_pSrc, umint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit / [&]
			{
				DMibFastCheck(false); // Move should not throw
			}
		;
#endif

		for (umint iElement = 0; iElement < _Len; ++iElement)
			new((void *)(_pDest + iElement)) tf_CData0(fg_Move(_pSrc[iElement]));

#if DMibEnableSafeCheck > 0
		Cleanup.f_Clear();
#endif
	}

	//////

	template <typename tf_CData>
	static void fg_CopyArray(tf_CData *_pDest, tf_CData const *_pSrc, umint _Len, umint &o_Len)
	{
		if constexpr (NTraits::cIsTriviallyCopyConstructible<tf_CData>)
		{
			if (_Len)
				NMemory::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(tf_CData));
			o_Len += _Len;
		}
		else if constexpr (NTraits::cIsNothrowConstructibleWith<tf_CData, tf_CData const &>)
		{
			for (umint iElement = 0; iElement < _Len; ++iElement)
				new((void *)(_pDest + iElement)) tf_CData(_pSrc[iElement]);
			o_Len += _Len;
		}
		else
		{
			for (umint iElement = 0; iElement < _Len; ++iElement)
			{
				new((void *)(_pDest + iElement)) tf_CData(_pSrc[iElement]);
				++o_Len;
			}
		}
	}

	template <typename tf_CData0, typename tf_CData1>
	static void fg_CopyArray(tf_CData0 *_pDest, tf_CData1 const *_pSrc, umint _Len, umint &o_Len)
	{
		if constexpr (NTraits::cIsNothrowConstructibleWith<tf_CData0, tf_CData1 const &>)
		{
			for (umint iElement = 0; iElement < _Len; ++iElement)
				new((void *)(_pDest + iElement)) tf_CData0(_pSrc[iElement]);
			o_Len += _Len;
		}
		else
		{
			for (umint iElement = 0; iElement < _Len; ++iElement)
			{
				new((void *)(_pDest + iElement)) tf_CData0(_pSrc[iElement]);
				++o_Len;
			}
		}
	}

	//////

	template <typename tf_CData>
	static void fg_MoveDestroyOverlappingArrayReverse(tf_CData *_pDest, tf_CData *_pSrc, umint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit / [&]
			{
				DMibFastCheck(false); // Move or destructor should not throw
			}
		;
#endif
		if constexpr (NTraits::cIsTriviallyMoveConstructible<tf_CData>)
		{
			if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(tf_CData));
			else
			{
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(tf_CData));
				for (smint iElement = smint(_Len) - 1; iElement >= 0; --iElement)
					_pSrc[iElement].~tf_CData();
			}
		}
		else
		{
			if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
			{
				for (smint iElement = smint(_Len) - 1; iElement >= 0; --iElement)
					new((void *)(_pDest + iElement)) tf_CData(fg_Move(_pSrc[iElement]));
			}
			else
			{
				for (smint iElement = smint(_Len) - 1; iElement >= 0; --iElement)
				{
					new((void *)(_pDest + iElement)) tf_CData(fg_Move(_pSrc[iElement]));
					_pSrc[iElement].~tf_CData();
				}
			}
		}

#if DMibEnableSafeCheck > 0
		Cleanup.f_Clear();
#endif
	}

	//////

	template <typename tf_CData>
	static void fg_MoveDestroyOverlappingArray(tf_CData *_pDest, tf_CData *_pSrc, umint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit / [&]
			{
				DMibFastCheck(false); // Move or destructor should not throw
			}
		;
#endif
		if constexpr (NTraits::cIsTriviallyMoveConstructible<tf_CData>)
		{
			if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(tf_CData));
			else
			{
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(tf_CData));
				for (umint iElement = 0; iElement < _Len; ++iElement)
					_pSrc[iElement].~tf_CData();
			}
		}
		else
		{
			if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
			{
				for (umint iElement = 0; iElement < _Len; ++iElement)
					new((void *)(_pDest + iElement)) tf_CData(fg_Move(_pSrc[iElement]));
			}
			else
			{
				for (umint iElement = 0; iElement < _Len; ++iElement)
				{
					new((void *)(_pDest + iElement)) tf_CData(fg_Move(_pSrc[iElement]));
					_pSrc[iElement].~tf_CData();
				}
			}
		}
#if DMibEnableSafeCheck > 0
		Cleanup.f_Clear();
#endif
	}

	//////

	template <typename tf_CData, typename tf_CDataRight>
	static void fg_CopyOverArray(tf_CData *_pDest, tf_CDataRight const *_pSrc, umint _Len)
	{
		if constexpr (NTraits::cIsSame<tf_CData, tf_CDataRight> && NTraits::cIsTriviallyCopyConstructible<tf_CData>)
		{
			if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
				NMemory::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(tf_CData));
			else
			{
#if DMibEnableSafeCheck > 0
				auto Cleanup = g_OnScopeExit / [&]
					{
						DMibFastCheck(false); // Destructor should not throw
					}
				;
#endif
				for (umint iElement = 0; iElement < _Len; ++iElement)
					_pDest[iElement].~tf_CData();
				NMemory::fg_MemCopy((void *)_pDest, _pSrc, _Len * sizeof(tf_CData));

#if DMibEnableSafeCheck > 0
				Cleanup.f_Clear();
#endif
			}
		}
		else
		{
			if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
			{
				for (umint iElement = 0; iElement < _Len; ++iElement)
					new((void *)(_pDest + iElement)) tf_CData(_pSrc[iElement]);
			}
			else if constexpr (NTraits::cIsAssignableWith<tf_CData &, tf_CDataRight const &>)
			{
				for (umint iElement = 0; iElement < _Len; ++iElement)
					_pDest[iElement] = _pSrc[iElement];
			}
			else
			{
				for (umint iElement = 0; iElement < _Len; ++iElement)
				{
					auto *pDest = _pDest + iElement;
					auto Temp = fg_Move(*pDest);
					(*pDest).~tf_CData();

					auto Cleanup = g_OnScopeExit / [&]
						{
							new((void *)(pDest)) tf_CData(fg_Move(Temp));
						}
					;

					new((void *)(pDest)) tf_CData(_pSrc[iElement]);

					Cleanup.f_Clear();
				}
			}
		}
	}

	template <typename tf_CData, typename tf_CDataRight>
	static void fg_MoveOverArray(tf_CData *_pDest, tf_CDataRight *_pSrc, umint _Len)
	{
		if constexpr (NTraits::cIsSame<tf_CData, tf_CDataRight> && NTraits::cIsTriviallyMoveConstructible<tf_CData>)
		{
			if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
				NMemory::fg_MemCopy((void *)_pDest, _pSrc, _Len * sizeof(tf_CData));
			else
			{
#if DMibEnableSafeCheck > 0
				auto Cleanup = g_OnScopeExit / [&]
					{
						DMibFastCheck(false); // Destructor should not throw
					}
				;
#endif
				for (umint iElement = 0; iElement < _Len; ++iElement)
					_pDest[iElement].~tf_CData();
				NMemory::fg_MemCopy((void *)_pDest, _pSrc, _Len * sizeof(tf_CData));

#if DMibEnableSafeCheck > 0
				Cleanup.f_Clear();
#endif
			}
		}
		else
		{
			if constexpr (NTraits::cIsTriviallyDestructible<tf_CData>)
			{
				for (umint iElement = 0; iElement < _Len; ++iElement)
					new((void *)(_pDest + iElement)) tf_CData(fg_Move(_pSrc[iElement]));
			}
			else if constexpr (NTraits::cIsAssignableWith<tf_CData &, tf_CDataRight &&>)
			{
				for (umint iElement = 0; iElement < _Len; ++iElement)
					_pDest[iElement] = fg_Move(_pSrc[iElement]);
			}
			else
			{
				for (umint iElement = 0; iElement < _Len; ++iElement)
				{
					auto *pDest = _pDest + iElement;
					auto Temp = fg_Move(*pDest);
					(*pDest).~tf_CData();

					auto Cleanup = g_OnScopeExit / [&]
						{
							new((void *)(pDest)) tf_CData(fg_Move(Temp));
						}
					;

					new((void *)(pDest)) tf_CData(fg_Move(_pSrc[iElement]));

					Cleanup.f_Clear();
				}
			}
		}
	}
}
