// Copyright © 2025 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	// f_Every - Returns true if ALL elements satisfy predicate
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_FPredicate>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_Every(tf_FPredicate &&_fPredicate) const
	{
		if (!mp_StaticData.m_pData)
			return true;

		auto &Data = *mp_StaticData.m_pData;
		mint Len = Data.m_Length;
		t_CData const *pArray = Data.f_GetData();

		for (mint i = 0; i < Len; ++i)
		{
			if (!NPrivate::fg_InvokeVectorIteratorFunctor(_fPredicate, pArray[i], i, *this))
				return false;
		}
		return true;
	}

	// f_Some - Returns true if ANY element satisfies predicate
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_FPredicate>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_Some(tf_FPredicate &&_fPredicate) const
	{
		if (!mp_StaticData.m_pData)
			return false;

		auto &Data = *mp_StaticData.m_pData;
		mint Len = Data.m_Length;
		t_CData const *pArray = Data.f_GetData();

		for (mint i = 0; i < Len; ++i)
		{
			if (NPrivate::fg_InvokeVectorIteratorFunctor(_fPredicate, pArray[i], i, *this))
				return true;
		}
		return false;
	}

	// f_Reduce (with initial accumulator)
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CThis, typename tf_FReducer, typename tf_CAccumulator>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_Reduce(this tf_CThis &&_This, tf_FReducer &&_fReducer, tf_CAccumulator &&_Accumulator)
	{
		mint Len = 0;
		NTraits::TCCopyQualifiers<NTraits::TCRemoveReference<tf_CThis>, t_CData> *pArray = nullptr;

		if (_This.mp_StaticData.m_pData)
		{
			auto &Data = *_This.mp_StaticData.m_pData;
			Len = Data.m_Length;
			pArray = Data.f_GetData();
		}

		using CAccumulator = decltype(NPrivate::fg_InvokeVectorReduceFunctor(_fReducer, fg_Forward<tf_CAccumulator>(_Accumulator), fg_ForwardAs<tf_CThis>(pArray[0]), mint(0), fg_Forward<tf_CThis>(_This)));

		if constexpr (NTraits::cIsVoid<CAccumulator>)
		{
			for (mint i = 0; i < Len; ++i)
				NPrivate::fg_InvokeVectorReduceFunctor(_fReducer, fg_Forward<tf_CAccumulator>(_Accumulator), fg_ForwardAs<tf_CThis>(pArray[i]), i, fg_Forward<tf_CThis>(_This));
		}
		else
		{
			CAccumulator Accumulator = fg_Forward<tf_CAccumulator>(_Accumulator);

			for (mint i = 0; i < Len; ++i)
				Accumulator = NPrivate::fg_InvokeVectorReduceFunctor(_fReducer, fg_Move(Accumulator), fg_ForwardAs<tf_CThis>(pArray[i]), i, fg_Forward<tf_CThis>(_This));

			return Accumulator;
		}
	}

	// f_Reduce (without initial accumulator)
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CThis, typename tf_FReducer>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_Reduce(this tf_CThis &&_This, tf_FReducer &&_fReducer)
	{
		mint Len = 0;
		NTraits::TCCopyQualifiers<NTraits::TCRemoveReference<tf_CThis>, t_CData> *pArray = nullptr;

		if (_This.mp_StaticData.m_pData)
		{
			auto &Data = *_This.mp_StaticData.m_pData;
			Len = Data.m_Length;
			pArray = Data.f_GetData();
		}

		using CAccumulator = typename NTraits::TCMemberFunctionPointerTraits<decltype(&NTraits::TCRemoveReference<tf_FReducer>::operator())>::CReturn;

		CAccumulator Accumulator{};

		for (mint i = 0; i < Len; ++i)
			Accumulator = NPrivate::fg_InvokeVectorReduceFunctor(_fReducer, fg_Move(Accumulator), fg_ForwardAs<tf_CThis>(pArray[i]), i, fg_Forward<tf_CThis>(_This));

		return Accumulator;
	}

	// f_ReduceRight (with initial accumulator)
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CThis, typename tf_FReducer, typename tf_CAccumulator>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_ReduceRight(this tf_CThis &&_This, tf_FReducer &&_fReducer, tf_CAccumulator &&_Accumulator)
	{
		mint Len = 0;
		NTraits::TCCopyQualifiers<NTraits::TCRemoveReference<tf_CThis>, t_CData> *pArray = nullptr;

		if (_This.mp_StaticData.m_pData)
		{
			auto &Data = *_This.mp_StaticData.m_pData;
			Len = Data.m_Length;
			pArray = Data.f_GetData();
		}

		using CAccumulator = decltype(NPrivate::fg_InvokeVectorReduceFunctor(_fReducer, fg_Forward<tf_CAccumulator>(_Accumulator), fg_ForwardAs<tf_CThis>(pArray[0]), mint(0), fg_Forward<tf_CThis>(_This)));

		if constexpr (NTraits::cIsVoid<CAccumulator>)
		{
			for (mint i = Len; i > 0; --i)
				NPrivate::fg_InvokeVectorReduceFunctor(_fReducer, fg_Forward<tf_CAccumulator>(_Accumulator), fg_ForwardAs<tf_CThis>(pArray[i - 1]), i - 1, fg_Forward<tf_CThis>(_This));
		}
		else
		{
			CAccumulator Accumulator = fg_Forward<tf_CAccumulator>(_Accumulator);

			for (mint i = Len; i > 0; --i)
				Accumulator = NPrivate::fg_InvokeVectorReduceFunctor(_fReducer, fg_Move(Accumulator), fg_ForwardAs<tf_CThis>(pArray[i - 1]), i - 1, fg_Forward<tf_CThis>(_This));

			return Accumulator;
		}
	}

	// f_ReduceRight (without initial accumulator)
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CThis, typename tf_FReducer>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_ReduceRight(this tf_CThis &&_This, tf_FReducer &&_fReducer)
	{
		mint Len = 0;
		NTraits::TCCopyQualifiers<NTraits::TCRemoveReference<tf_CThis>, t_CData> *pArray = nullptr;

		if (_This.mp_StaticData.m_pData)
		{
			auto &Data = *_This.mp_StaticData.m_pData;
			Len = Data.m_Length;
			pArray = Data.f_GetData();
		}

		using CAccumulator = typename NTraits::TCMemberFunctionPointerTraits<decltype(&NTraits::TCRemoveReference<tf_FReducer>::operator())>::CReturn;

		CAccumulator Accumulator{};

		for (mint i = Len; i > 0; --i)
			Accumulator = NPrivate::fg_InvokeVectorReduceFunctor(_fReducer, fg_Move(Accumulator), fg_ForwardAs<tf_CThis>(pArray[i - 1]), i - 1, fg_Forward<tf_CThis>(_This));

		return Accumulator;
	}
}
