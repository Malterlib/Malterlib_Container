// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "../Malterlib_Container_Shared.h"

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	struct TCIsContainer<TCVector<t_CData, t_CAllocator, t_COptions>>
	{
		static constexpr bool mc_Value = true;
	};

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector() noexcept
	{
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::~TCVector()
	{
		f_Clear();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(std::initializer_list<t_CData> const &_Values)
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		for (auto &Value : _Values)
			f_Insert(Value);

		Cleanup.f_Clear();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(t_CData const *_pItems, mint _nItems)
	{
		auto Cleanup = g_OnScopeExit / [&]
			{
				f_Clear();
			}
		;

		f_Insert(_pItems, _nItems);

		Cleanup.f_Clear();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_GetArray()
	{
		if (mp_StaticData.m_pData)
			return mp_StaticData.m_pData->f_GetData();
		else
			return nullptr;
	}
	
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData const *TCVector<t_CData, t_CAllocator, t_COptions>::f_GetArray() const
	{
		if (mp_StaticData.m_pData)
			return mp_StaticData.m_pData->f_GetData();
		else
			return nullptr;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_GetArray(mint _Len)
	{
		f_SetLen(_Len);
		return f_GetArray();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_GetFirst()
	{
		return (*this)[0];
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_GetLast()
	{
		return (*this)[f_GetLen()-1];
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData const &TCVector<t_CData, t_CAllocator, t_COptions>::f_GetFirst() const
	{
		return (*this)[0];
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData const &TCVector<t_CData, t_CAllocator, t_COptions>::f_GetLast() const
	{
		return (*this)[f_GetLen()-1];
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	mint TCVector<t_CData, t_CAllocator, t_COptions>::f_GetLen() const
	{
		if (mp_StaticData.m_pData)
			return mp_StaticData.m_pData->m_Length;
		else
			return 0;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsEmpty() const
	{
		if (mp_StaticData.m_pData)
			return mp_StaticData.m_pData->m_Length == 0;
		else
			return true;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_Reverse() const & -> TCVector
	{
		TCVector Ret;
		mint Len = f_GetLen();
		if (!Len)
			return Ret;

		Ret.fp_MakeNewRoom(Len);

		DMibFastCheck(Ret.mp_StaticData.m_pData);

		auto pDst = Ret.f_GetArray();
		auto pSrc = f_GetArray();

		auto &NewLen = Ret.mp_StaticData.m_pData->m_Length;
		for (mint i = 0; i < Len; ++i)
		{
			new((void *)(pDst + i)) t_CData(pSrc[Len - i - 1]);
			++NewLen;
		}

		return Ret;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_Reverse() && -> TCVector
	{
		mint Len = f_GetLen();
		if (Len <= 1)
			return fg_Move(*this);

		TCVector Ret;

		Ret.fp_MakeNewRoom(Len);

		DMibFastCheck(Ret.mp_StaticData.m_pData);

		auto pDst = Ret.f_GetArray();
		auto pSrc = f_GetArray();

		auto &NewLen = Ret.mp_StaticData.m_pData->m_Length;
		for (mint i = 0; i < Len; ++i)
		{
			new((void *)(pDst + i)) t_CData(fg_Move(pSrc[Len - i - 1]));
			++NewLen;
		}

		return Ret;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Clear()
	{
		if (mp_StaticData.m_pData)
		{
			auto Length = mp_StaticData.m_pData->m_Length;
			if (Length)
				NPrivate::fg_DestroyArray(mp_StaticData.m_pData->f_GetData(), Length, mp_StaticData.m_pData->m_Length);

			fp_FreeData(mp_StaticData.m_pData);
			mp_StaticData.m_pData = nullptr;
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_ClearNoTrim()
	{
		if (mp_StaticData.m_pData)
		{
			auto Length = mp_StaticData.m_pData->m_Length;
			if (Length)
				NPrivate::fg_DestroyArray(mp_StaticData.m_pData->f_GetData(), Length, mp_StaticData.m_pData->m_Length);

			mp_StaticData.m_pData->m_Length = 0;
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CContainer>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::fs_FromContainer(tf_CContainer &&_Container) -> TCVector
	{
		TCVector Return;

		if constexpr (cIsSet<tf_CContainer>)
		{
			if constexpr (NTraits::cIsRValueReference<tf_CContainer &&>)
			{
				_Container.f_ExtractAll
					(
						[&](auto &&_Handle)
						{
							Return.f_Insert(fg_Move(_Handle.f_Key()));
						}
					)
				;
			}
			else
			{
				for (auto &Value : _Container)
					Return.f_Insert(Value);
			}
		}
		else if constexpr (NTraits::cIsRValueReference<tf_CContainer &&>)
		{
			for (auto &Value : _Container)
				Return.f_Insert(fg_Move(Value));
		}
		else
		{
			for (auto &Value : _Container)
				Return.f_Insert(Value);
		}

		return Return;
	}

	namespace NPrivate
	{
		template <typename t_CType>
		struct TCIsVector
		{
			constexpr static bool mc_bValue = false;
		};

		template <typename t_CData, typename t_CAllocator, typename t_COptions>
		struct TCIsVector<TCVector<t_CData, t_CAllocator, t_COptions>>
		{
			constexpr static bool mc_bValue = true;
		};
	}
}
