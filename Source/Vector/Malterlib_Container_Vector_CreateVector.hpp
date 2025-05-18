// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	namespace NPrivate
	{
		template <typename tf_CReturn, typename... tf_CParams>
		void fg_CreateVectorHelper(tf_CReturn &_Return)
		{
		}

		template <typename tf_CReturn, typename tf_CFirst, typename... tf_CParams>
		void fg_CreateVectorHelper(tf_CReturn &_Return, tf_CFirst &&_First, tf_CParams &&...p_Params)
		{
			_Return.f_Insert(fg_Construct(fg_Forward<tf_CFirst>(_First)));
			fg_CreateVectorHelper<tf_CReturn>(_Return, fg_Forward<tf_CParams>(p_Params)...);
		}
	}

	template <typename tf_CFirst, typename... tf_CParams>
	auto fg_CreateVector(tf_CFirst &&_First, tf_CParams &&...p_Params)
	{
		if constexpr (NTraits::cIsSame<tf_CFirst, uint8>)
		{
			using CReturn = CByteVector;
			CReturn Return;
			NPrivate::fg_CreateVectorHelper<CReturn>(Return, fg_Forward<tf_CFirst>(_First), fg_Forward<tf_CParams>(p_Params)...);
			return Return;
		}
		else
		{
			using CReturn = TCVector<NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>>;
			CReturn Return;
			NPrivate::fg_CreateVectorHelper<CReturn>(Return, fg_Forward<tf_CFirst>(_First), fg_Forward<tf_CParams>(p_Params)...);
			return Return;
		}
	}

	template <typename tf_CReturn, typename... tf_CParams>
	auto fg_CreateVector(tf_CParams && ...p_Params)
	{
		if constexpr (NTraits::cIsSame<tf_CReturn, uint8>)
		{
			CByteVector Return;
			NPrivate::fg_CreateVectorHelper<CByteVector>(Return, fg_Forward<tf_CParams>(p_Params)...);
			return Return;
		}
		else
		{
			TCVector<tf_CReturn> Return;
			NPrivate::fg_CreateVectorHelper<TCVector<tf_CReturn>>(Return, fg_Forward<tf_CParams>(p_Params)...);
			return Return;
		}
	}
}
