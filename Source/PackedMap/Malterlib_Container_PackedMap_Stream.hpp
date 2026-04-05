// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	// Stream serialization - feed (write)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CStream>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Feed(tf_CStream &_Stream) const
	{
		umint nItems = f_GetLen();
		fg_FeedLenToStream(_Stream, nItems);

		auto *pData = mp_pData;
		if (!pData || nItems == 0)
			return;

		f_Scan
			(
				[&](t_CKey const &_Key, t_CValue const &_Value)
				{
					_Stream.f_Feed(_Key);
					_Stream.f_Feed(_Value);
				}
			)
		;
	}

	// Stream deserialization - consume (read)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CStream>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Consume(tf_CStream &_Stream)
	{
		f_Clear(false);

		auto Rollback = g_OnScopeExit / [&]
			{
				f_Clear(false);
			}
		;

		uint64 nItems;
		fg_ConsumeLenFromStream(_Stream, nItems);
		fg_CheckLengthLimit(_Stream, nItems);

		if (nItems == 0)
			return;

		// Reserve capacity accounting for density upper bound
		fp_ReserveForElementCount((umint)nItems);

		while (nItems)
		{
			t_CKey Key;
			t_CValue Value;
			_Stream.f_Consume(Key);
			_Stream.f_Consume(Value);

			auto Result = f_Insert(fg_Move(Key), fg_Move(Value));
			if (!Result.m_bInserted)
				DMibError("TCPackedMap stream contained a duplicate key");
			--nItems;
		}

		Rollback.f_Clear();
	}
}
