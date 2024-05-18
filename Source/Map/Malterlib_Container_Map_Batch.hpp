// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	namespace NPrivate
	{
		template <typename t_CNode>
		struct TCMapMapper
		{
			TCMapMapper(void * _pMemory)
				: m_pMemory(_pMemory)
			{
			}

			template <typename tf_CKey, typename... tfp_CArg>
			auto &operator ()(tf_CKey &&_Key, tfp_CArg && ... p_Args)
			{
				return (new(m_pMemory) t_CNode(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CArg>(p_Args)...))->f_Value();
			}

			void *m_pMemory;
		};

		template <typename t_CMap>
		struct TCMapConditionalMapper
		{
			using CNode = typename t_CMap::CNode;
			using CUserData = typename t_CMap::CUserData;

			TCMapConditionalMapper(void * _pMemory, t_CMap &_Map)
				: m_pMemory(_pMemory)
				, m_Map(_Map)
				, m_bAddFailed(false)
			{
			}

			template <typename tf_CKey, typename... tfp_CArg>
			TCMapResult<CUserData &> operator ()(tf_CKey &&_Key, tfp_CArg && ... p_Args)
			{
				m_bAddFailed = true;
				auto pData = m_Map.mp_Tree.f_FindEqualOrInsert
					(
						_Key
						, [&]() -> CNode *
						{
							m_bAddFailed = false;

							auto pData = (new(m_pMemory) CNode(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CArg>(p_Args)...));

							return pData;
						}
						, m_Map.mp_Compare
					)
				;

				return TCMapResult<CUserData &>(pData->f_Value(), !m_bAddFailed);
			}

			void *m_pMemory;
			t_CMap &m_Map;
			bool m_bAddFailed;
		};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_FDoInsert>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_BatchMap(tf_FDoInsert &&_fDoInsert)
	{
		mp_Allocator.f_AllocBatch
			(
				sizeof(CNode)
				, alignof(CNode)
				, [&](void * _pAlloc, mint _Size) -> bool
				{
					auto Cleanup = mp_Allocator.f_MakeSafe(_pAlloc, _Size);
					CMapper Mapper(_pAlloc);
					bool bRet = _fDoInsert(Mapper);
					Cleanup.f_Claim();
					auto pData = (CNode *)_pAlloc;
					mp_Tree.f_Insert(pData, mp_Compare);
					return bRet;
				}
			)
		;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_FDoInsert>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_BatchMapIfNotMapped(tf_FDoInsert &&_fDoInsert)
	{
		mp_Allocator.f_AllocBatch
			(
				sizeof(CNode)
				, alignof(CNode)
				, [&](void * _pAlloc, mint _Size) -> bool
				{
					auto Cleanup = mp_Allocator.f_MakeSafe(_pAlloc, _Size);

					CConditionalMapper Mapper(_pAlloc, *this);
					bool bRet = _fDoInsert(Mapper);
					while (Mapper.m_bAddFailed)
					{
						if (!bRet)
							return false;
						bRet = _fDoInsert(Mapper);
					}
					Cleanup.f_Claim();
					return bRet;
				}
			)
		;
	}
}
