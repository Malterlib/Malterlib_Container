// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CStream>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Feed(tf_CStream &_Stream) const
	{
		CIteratorConst Iter = f_GetIterator();
		umint nItems = Iter.f_GetLen();

		fg_FeedLenToStream(_Stream, nItems);

		while (Iter)
		{
			_Stream.f_Feed(*Iter.mp_Iter);
			++Iter;
		};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CStream>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Consume(tf_CStream &_Stream)
	{
		f_Clear();

		uint64 nItems;
		fg_ConsumeLenFromStream(_Stream, nItems);
		fg_CheckLengthLimit(_Stream, nItems);

		while (nItems)
		{
			auto Memory = mp_Allocator.f_AllocSafe(sizeof(CNodeDestructive), alignof(CNodeDestructive));
			CNodeDestructive *pData = (CNodeDestructive *)Memory.m_pMemory;
			pData = new((void *)pData) CNodeDestructive();
			Memory.f_Claim();
			auto Cleanup = g_OnScopeExit / [&]
				{
					fg_DeleteObjectDefiniteType(mp_Allocator, pData);
				}
			;
			pData->f_Consume(_Stream);

			auto pFoundData = mp_Tree.f_FindEqualOrInsert
				(
					pData->mp_Key
					, [&]() -> CNodeDestructive *
					{
						return pData;
					}
					, mp_Compare
				)
			;

			if (pFoundData == pData)
				Cleanup.f_Clear();
			else
				DMibError("TCMap stream contained a duplicate key");

			--nItems;
		}
	}
}
