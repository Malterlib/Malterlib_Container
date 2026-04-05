// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename t_CData>
	static void fg_ConstructArray(t_CData *_pDest, umint _Len, umint &o_Len)
	{
		if constexpr (NTraits::cIsTrivialllyDefaultConstructible<t_CData>)
		{
			o_Len += _Len;
			return;
		}
		else
		{
			for (umint i = 0; i < _Len; ++i)
			{
				new((void *)(_pDest + i)) t_CData();
				++o_Len;
			}
		}
	}

	//////

	template <typename t_CData>
	static void fg_DestroyArray(t_CData *_pDest, umint _Len, umint &o_Len)
	{
		DMibFastCheck(_Len != 0);
		DMibFastCheck(_pDest);

		if constexpr (NTraits::cIsTriviallyDestructible<t_CData>)
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
#ifndef DMibClangAnalyzerWorkaround
			// Destroy all cells
			auto pEnd = _pDest - 1;
			for (auto pDest = _pDest + (_Len - 1); pDest != pEnd; --pDest, --o_Len)
				pDest->~t_CData();
#endif

#if DMibEnableSafeCheck > 0
			Cleanup.f_Clear();
#endif
		}
	}

	//////

	template <typename t_CData>
	static void fg_MoveArray(t_CData *_pDest, t_CData *_pSrc, umint _Len)
	{
		if constexpr (NTraits::cIsTriviallyMoveConstructible<t_CData>)
		{
			if (_Len)
				NMemory::fg_MemCopy((void *)_pDest, _pSrc, _Len * sizeof(t_CData));
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

			for (umint i = 0; i < _Len; ++i)
				new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));

#if DMibEnableSafeCheck > 0
			Cleanup.f_Clear();
#endif
		}
	}

	template <typename t_CData0, typename t_CData1>
	static void fg_MoveArray(t_CData0 *_pDest, t_CData1 *_pSrc, umint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit / [&]
			{
				DMibFastCheck(false); // Move should not throw
			}
		;
#endif

		for (umint i = 0; i < _Len; ++i)
			new((void *)(_pDest + i)) t_CData0(fg_Move(_pSrc[i]));

#if DMibEnableSafeCheck > 0
		Cleanup.f_Clear();
#endif
	}

	//////

	template <typename t_CData>
	static void fg_CopyArray(t_CData *_pDest, t_CData const *_pSrc, umint _Len, umint &o_Len)
	{
		if constexpr (NTraits::cIsTriviallyCopyConstructible<t_CData>)
		{
			if (_Len)
				NMemory::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));
			o_Len += _Len;
		}
		else if constexpr (NTraits::cIsNothrowConstructibleWith<t_CData, t_CData const &>)
		{
			for (umint i = 0; i < _Len; ++i)
				new((void *)(_pDest + i)) t_CData(_pSrc[i]);
			o_Len += _Len;
		}
		else
		{
			for (umint i = 0; i < _Len; ++i)
			{
				new((void *)(_pDest + i)) t_CData(_pSrc[i]);
				++o_Len;
			}
		}
	}

	template <typename t_CData0, typename t_CData1>
	static void fg_CopyArray(t_CData0 *_pDest, t_CData1 const *_pSrc, umint _Len, umint &o_Len)
	{
		if constexpr (NTraits::cIsNothrowConstructibleWith<t_CData0, t_CData1 const &>)
		{
			for (umint i = 0; i < _Len; ++i)
				new((void *)(_pDest + i)) t_CData0(_pSrc[i]);
			o_Len += _Len;
		}
		else
		{
			for (umint i = 0; i < _Len; ++i)
			{
				new((void *)(_pDest + i)) t_CData0(_pSrc[i]);
				++o_Len;
			}
		}
	}

	//////

	template <typename t_CData>
	static void fg_MoveDestroyOverlappingArrayReverse(t_CData *_pDest, t_CData *_pSrc, umint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit / [&]
			{
				DMibFastCheck(false); // Move or destructor should not throw
			}
		;
#endif
		if constexpr (NTraits::cIsTriviallyMoveConstructible<t_CData>)
		{
			if constexpr (NTraits::cIsTriviallyDestructible<t_CData>)
			{
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			else
			{
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
				for (smint i = smint(_Len) - 1; i >= 0; --i)
				{
					_pSrc[i].~t_CData();
				}
			}
		}
		else
		{
			if constexpr (NTraits::cIsTriviallyDestructible<t_CData>)
			{
				for (smint i = smint(_Len) - 1; i >= 0; --i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
				}
			}
			else
			{
				for (smint i = smint(_Len) - 1; i >= 0; --i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
					_pSrc[i].~t_CData();
				}
			}
		}

#if DMibEnableSafeCheck > 0
		Cleanup.f_Clear();
#endif
	}

	//////

	template <typename t_CData>
	static void fg_MoveDestroyOverlappingArray(t_CData *_pDest, t_CData *_pSrc, umint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit / [&]
			{
				DMibFastCheck(false); // Move or destructor should not throw
			}
		;
#endif
		if constexpr (NTraits::cIsTriviallyMoveConstructible<t_CData>)
		{
			if constexpr (NTraits::cIsTriviallyDestructible<t_CData>)
			{
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			else
			{
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
				for (umint i = 0; i < _Len; ++i)
					_pSrc[i].~t_CData();
			}
		}
		else
		{
			if constexpr (NTraits::cIsTriviallyDestructible<t_CData>)
			{
				for (umint i = 0; i < _Len; ++i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
				}
			}
			else
			{
				for (umint i = 0; i < _Len; ++i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
					_pSrc[i].~t_CData();
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
				for (umint i = 0; i < _Len; ++i)
					_pDest[i].~tf_CData();
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
				for (umint i = 0; i < _Len; ++i)
					new((void *)(_pDest + i)) tf_CData(_pSrc[i]);
			}
			else if constexpr (NTraits::cIsAssignableWith<tf_CData &, tf_CDataRight const &>)
			{
				for (umint i = 0; i < _Len; ++i)
					_pDest[i] = _pSrc[i];
			}
			else
			{
				for (umint i = 0; i < _Len; ++i)
				{
					auto *pDest = _pDest + i;
					auto Temp = fg_Move(*pDest);
					(*pDest).~tf_CData();

					auto Cleanup = g_OnScopeExit / [&]
						{
							new((void *)(pDest)) tf_CData(fg_Move(Temp));
						}
					;

					new((void *)(pDest)) tf_CData(_pSrc[i]);

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
				for (umint i = 0; i < _Len; ++i)
					_pDest[i].~tf_CData();
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
				for (umint i = 0; i < _Len; ++i)
					new((void *)(_pDest + i)) tf_CData(fg_Move(_pSrc[i]));
			}
			else if constexpr (NTraits::cIsAssignableWith<tf_CData &, tf_CDataRight &&>)
			{
				for (umint i = 0; i < _Len; ++i)
					_pDest[i] = fg_Move(_pSrc[i]);
			}
			else
			{
				for (umint i = 0; i < _Len; ++i)
				{
					auto *pDest = _pDest + i;
					auto Temp = fg_Move(*pDest);
					(*pDest).~tf_CData();

					auto Cleanup = g_OnScopeExit / [&]
						{
							new((void *)(pDest)) tf_CData(fg_Move(Temp));
						}
					;

					new((void *)(pDest)) tf_CData(fg_Move(_pSrc[i]));

					Cleanup.f_Clear();
				}
			}
		}
	}
}
