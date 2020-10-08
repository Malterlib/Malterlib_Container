// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename t_CData>
	static void fg_ConstructArray(t_CData *_pDest, mint _Len, mint &o_Len)
	{
		if constexpr (NTraits::TCHasTrivialDefaultConstructor<t_CData>::mc_Value)
		{
			o_Len += _Len;
			return;
		}
		else
		{
			for (mint i = 0; i < _Len; ++i)
			{
				new((void *)(_pDest + i)) t_CData();
				++o_Len;
			}
		}
	}

	//////

	template <typename t_CData>
	static void fg_DestroyArray(t_CData *_pDest, mint _Len, mint &o_Len)
	{
		if (!_pDest)
		{
			DMibFastCheck(_Len == 0);
			return;
		}
		
		DMibFastCheck(_Len != 0);

		if constexpr (NTraits::TCHasTrivialDestructor<t_CData>::mc_Value)
		{
			o_Len -= _Len;
			return;
		}
		else
		{
#if DMibEnableSafeCheck > 0
			auto Cleanup = g_OnScopeExit > [&]
				{
					DMibFastCheck(false); // Destructor should not throw
				}
			;
#endif
			// Destroy all cells
			auto pEnd = _pDest - 1;
			for (auto pDest = _pDest + (_Len - 1); pDest != pEnd; --pDest, --o_Len)
				pDest->~t_CData();

#if DMibEnableSafeCheck > 0
			Cleanup.f_Clear();
#endif
		}
	}

	//////

	template <typename t_CData>
	static void fg_MoveArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
	{
		if constexpr (NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value)
		{
			if (_Len)
				NMemory::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));
		}
		else
		{
#if DMibEnableSafeCheck > 0
			auto Cleanup = g_OnScopeExit > [&]
				{
					DMibFastCheck(false); // Move should not throw
				}
			;
#endif

			for (mint i = 0; i < _Len; ++i)
				new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));

#if DMibEnableSafeCheck > 0
			Cleanup.f_Clear();
#endif
		}
	}

	template <typename t_CData0, typename t_CData1>
	static void fg_MoveArray(t_CData0 *_pDest, t_CData1 *_pSrc, mint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit > [&]
			{
				DMibFastCheck(false); // Move should not throw
			}
		;
#endif

		for (mint i = 0; i < _Len; ++i)
			new((void *)(_pDest + i)) t_CData0(fg_Move(_pSrc[i]));

#if DMibEnableSafeCheck > 0
		Cleanup.f_Clear();
#endif
	}

	//////

	template <typename t_CData>
	static void fg_CopyArray(t_CData *_pDest, t_CData const *_pSrc, mint _Len, mint &o_Len)
	{
		if constexpr (NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value)
		{
			if (_Len)
				NMemory::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));
			o_Len += _Len;
		}
		else if constexpr (NTraits::TCIsConstructorNothrowCallableWith<t_CData, void (t_CData const &)>::mc_Value)
		{
			for (mint i = 0; i < _Len; ++i)
				new((void *)(_pDest + i)) t_CData(_pSrc[i]);
			o_Len += _Len;
		}
		else
		{
			for (mint i = 0; i < _Len; ++i)
			{
				new((void *)(_pDest + i)) t_CData(_pSrc[i]);
				++o_Len;
			}
		}
	}

	template <typename t_CData0, typename t_CData1>
	static void fg_CopyArray(t_CData0 *_pDest, t_CData1 const *_pSrc, mint _Len, mint &o_Len)
	{
		if constexpr (NTraits::TCIsConstructorNothrowCallableWith<t_CData0, void (t_CData1 const &)>::mc_Value)
		{
			for (mint i = 0; i < _Len; ++i)
				new((void *)(_pDest + i)) t_CData0(_pSrc[i]);
			o_Len += _Len;
		}
		else
		{
			for (mint i = 0; i < _Len; ++i)
			{
				new((void *)(_pDest + i)) t_CData0(_pSrc[i]);
				++o_Len;
			}
		}
	}

	//////

	template <typename t_CData>
	static void fg_MoveDestroyOverlappingArrayReverse(t_CData *_pDest, t_CData *_pSrc, mint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit > [&]
			{
				DMibFastCheck(false); // Move or destructor should not throw
			}
		;
#endif
		if constexpr (NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value)
		{
			if constexpr (NTraits::TCHasTrivialDestructor<t_CData>::mc_Value)
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
			if constexpr (NTraits::TCHasTrivialDestructor<t_CData>::mc_Value)
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
	static void fg_MoveDestroyOverlappingArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
	{
#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit > [&]
			{
				DMibFastCheck(false); // Move or destructor should not throw
			}
		;
#endif
		if constexpr (NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value)
		{
			if constexpr (NTraits::TCHasTrivialDestructor<t_CData>::mc_Value)
			{
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			else
			{
				NMemory::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
				for (mint i = 0; i < _Len; ++i)
					_pSrc[i].~t_CData();
			}
		}
		else
		{
			if constexpr (NTraits::TCHasTrivialDestructor<t_CData>::mc_Value)
			{
				for (mint i = 0; i < _Len; ++i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
				}
			}
			else
			{
				for (mint i = 0; i < _Len; ++i)
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
	static void fg_DestroyCopyArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
	{
		if constexpr (NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value)
		{
			if constexpr (NTraits::TCHasTrivialDestructor<t_CData>::mc_Value)
				NMemory::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));
			else
			{
#if DMibEnableSafeCheck > 0
				auto Cleanup = g_OnScopeExit > [&]
					{
						DMibFastCheck(false); // Destructor should not throw
					}
				;
#endif
				for (mint i = 0; i < _Len; ++i)
					_pDest[i].~t_CData();
				NMemory::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));

#if DMibEnableSafeCheck > 0
				Cleanup.f_Clear();
#endif
			}
		}
		else
		{
			if constexpr (NTraits::TCHasTrivialDestructor<t_CData>::mc_Value)
			{
				for (mint i = 0; i < _Len; ++i)
					new((void *)(_pDest + i)) t_CData(_pSrc[i]);
			}
			else
			{
#if DMibEnableSafeCheck > 0
				auto Cleanup = g_OnScopeExit > [&]
					{
						DMibFastCheck(false); // Destructor, or move should not throw
					}
				;
#endif
				for (mint i = 0; i < _Len; ++i)
				{
					auto Temp = fg_Move(_pDest[i]);
					_pDest[i].~t_CData();
					try
					{
						new((void *)(_pDest + i)) t_CData(_pSrc[i]);
					}
					catch (...)
					{
						new((void *)(_pDest + i)) t_CData(fg_Move(Temp));
						throw;
					}
				}
#if DMibEnableSafeCheck > 0
				Cleanup.f_Clear();
#endif
			}
		}
	}
}
