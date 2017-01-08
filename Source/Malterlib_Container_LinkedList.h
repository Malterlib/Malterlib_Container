// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

namespace NMib
{
	namespace NContainer
	{
		template <typename t_CData>
		class TCLinkedListMember;
		
		template <typename t_CData, typename t_CAllocator = NMib::NMem::CAllocator_Heap >
		class TCLinkedList;

		struct CLinkedListMemberBase
		{
			DMibListLinkDS_Link(CLinkedListMemberBase, m_ListLink);
		};
		
		template <typename t_CData>
		class TCLinkedListMember : public CLinkedListMemberBase
		{
			template <typename t_CData2, typename t_CAllocator2> friend class TCLinkedList;

			t_CData m_Object;

			TCLinkedListMember()
			{
			}

			TCLinkedListMember(t_CData &_Data)
				: m_Object(_Data)
			{
			}
			
			TCLinkedListMember(t_CData &&_Data)
				: m_Object(fg_Move(_Data))
			{
			}
			
			TCLinkedListMember(const t_CData &_Data)
				: m_Object(_Data)
			{
			}
			
			inline_always static mint fs_GetOffset()
			{
				return DMibPOffsetOf(TCLinkedListMember, m_Object);
			}

			template <typename tf_CType, typename... tfp_CParams, mint... tfp_Indidies>
			TCLinkedListMember(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams, NMeta::TCIndices<tfp_Indidies...> const&)
				: m_Object
				(
					fg_Forward<tfp_CParams>(NContainer::fg_Get<tfp_Indidies>(_CreateParams.m_Params))...
				)
			{
			}
			
			template <typename tf_CType, typename... tfp_CParams>
			TCLinkedListMember(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
				: TCLinkedListMember(fg_Move(_CreateParams), typename NMeta::TCMakeConsecutiveIndices<TCConstruct<tf_CType, tfp_CParams...>::mc_nParams>::CType())
			{
			}

			t_CData &fp_GetObject()
			{
				return m_Object;
			}

			const t_CData &fp_GetObject() const
			{
				return m_Object;
			}

		public:
			~TCLinkedListMember()
			{
			}

		};

		template <typename t_CData, typename t_CAllocator>
		class TCLinkedList
		{
		private:

			typedef TCLinkedListMember<t_CData> CMember;
			

			class CAllocatorDisable : public NMem::CAllocator_Base
			{
			public:
				typedef typename t_CAllocator::CPtrHolder CPtrHolder;
			};

			
			typedef NMib::NIntrusive::TCDLinkList
				<
					CMember
					, CLinkedListMemberBase::CDLinkTranslatorm_ListLink
					, NMib::NIntrusive::TCDLinkAggregateListNoPrevPtr<NMib::NIntrusive::TCDLinkAggregateListNoPrevPtr_Data<typename t_CAllocator::CPtrHolder>>
					, NMib::NIntrusive::TCDLinkAggregateListNoPrevPtrList<CAllocatorDisable>
					, false
					, CAllocatorDisable
				> CList
			;
			
			typedef typename CList::CIterator CInternalIterator;
			typedef typename CList::CTranslator CTranslator;
			typedef typename CList::COffset COffset;
			typedef typename CList::CLink CLink;

			class CLocalData : public t_CAllocator
			{
			public:
				CList m_List;
				
				CLocalData()
				{
				}

				CLocalData(CLocalData &&_Other)
					: t_CAllocator((t_CAllocator &&)fg_Move(_Other))
					, m_List(fg_Move(_Other.m_List))
				{
				}
				
				template <typename... tfp_CParams>
				CLocalData(NMem::CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
					: t_CAllocator(fg_Forward<tfp_CParams>(p_Params)...)
				{
				}				
			};

			CLocalData m_Data;

			template <typename tf_CType, typename... tfp_CParams>
			CMember *fp_Create(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
			{
				static_assert(NTraits::TCIsSame<tf_CType, t_CData>::mc_Value || NTraits::TCIsVoid<tf_CType>::mc_Value, "Cannot override creation type");
				CMember * pData = (CMember *)m_Data.f_AllocAligned(sizeof(CMember), NTraits::TCAlignmentOf<CMember>::mc_Value);
				new((void *)pData) CMember(fg_Move(_CreateParams));
				return pData;
			}

			CMember *fp_Create(t_CData &_ToInsert)
			{
				CMember * pData = (CMember *)m_Data.f_AllocAligned(sizeof(CMember), NTraits::TCAlignmentOf<CMember>::mc_Value);
				new((void *)pData) CMember(_ToInsert);
				return pData;
			}

			CMember *fp_Create(t_CData &&_ToInsert)
			{
				CMember * pData = (CMember *)m_Data.f_AllocAligned(sizeof(CMember), NTraits::TCAlignmentOf<CMember>::mc_Value);
				new((void *)pData) CMember(fg_Move(_ToInsert));
				return pData;
			}

			CMember *fp_Create(const t_CData &_ToInsert)
			{
				CMember * pData = (CMember *)m_Data.f_AllocAligned(sizeof(CMember), NTraits::TCAlignmentOf<CMember>::mc_Value);
				new((void *)pData) CMember(_ToInsert);
				return pData;
			}

			CMember *fp_Create()
			{
				CMember * pData = (CMember *)m_Data.f_AllocAligned(sizeof(CMember), NTraits::TCAlignmentOf<CMember>::mc_Value);
				new((void *)pData) CMember();
				return pData;
			}
			
			inline_always static CMember *fsp_MemberFromData(t_CData &_Data)
			{
				mint Offset = CMember::fs_GetOffset();
				CMember *pMember = (CMember *)(((uint8 *)&_Data) - Offset);
				return pMember;
			}
			
			inline_always static CMember *fsp_MemberFromData(t_CData const &_Data)
			{
				mint Offset = CMember::fs_GetOffset();
				CMember *pMember = (CMember *)(((uint8 *)&_Data) - Offset);
				return pMember;
			}
			
			void fp_CopyFrom(TCLinkedList const& _Container)
			{
				mint nAdded = 16;
				CIteratorConst iSource = _Container.f_GetIterator();
				while (iSource && nAdded--)
				{
					f_Insert(*iSource);
					++iSource;
				}
				if (!iSource)
					return;
				
				m_Data.f_AllocBatch
					(
						sizeof(CMember)
						, NTraits::TCAlignmentOf<CMember>::mc_Value
						, [&](void * _pData, mint _Size) -> bool
						{
							CMember * pData = new(_pData) CMember(*iSource);
							m_Data.m_List.f_Insert(pData);
							++iSource;							
							return iSource;
						}
					)
				;
			}

		public:


			class CIterator
			{
				friend class TCLinkedList;
				typedef CInternalIterator CIntrusiveIterator;
				CIntrusiveIterator m_Iter;
				TCLinkedList *m_pLinkedList;
			public:
#ifdef DMibDebuggerHelpers
				static TCLinkedList* ms_pList;
				static CList* ms_pIntrusiveList;
#endif

				CIterator()
				{
					m_pLinkedList = nullptr;					
#ifdef DMibDebuggerHelpers
					(void)&ms_pList;
					(void)&ms_pIntrusiveList;
#endif
				}

				CIterator(TCLinkedList &_Map)
				{
					*this = _Map.f_GetIterator();
#ifdef DMibDebuggerHelpers
					(void)&ms_pList;
					(void)&ms_pIntrusiveList;
#endif
				}

				CIterator &operator = (TCLinkedList &_Map)
				{
					*this = _Map.f_GetIterator();
					return *this;
				}

				inline_medium mint f_GetLen() const
				{
					return m_Iter.f_GetLen();
				}

				inline_medium void f_Next()
				{
					++m_Iter;
				}
				inline_medium void f_Prev()
				{
					--m_Iter;
				}
				void f_Remove()
				{
					t_CData *pToRemove = f_GetCurrent();
					f_Next();
					m_pLinkedList->f_Remove(*pToRemove);					
				}

				inline_small t_CData *f_GetCurrent() const
				{
					CMember *pMember = m_Iter;
					if (pMember)
						return &pMember->fp_GetObject();
					return nullptr;
				}

				inline_small operator t_CData *() const
				{
					return f_GetCurrent();
				}

				inline_small t_CData * operator ->() const
				{
					return f_GetCurrent();
				}

				inline_small void operator ++()
				{
					f_Next();
				}
				
				inline_small void operator --()
				{
					f_Prev();
				}
			};

			class CIteratorConst
			{
				friend class TCLinkedList;
				typedef typename DMibListLinkAllocatorDS_IterConst_FromTemplate(CMember, m_ListLink, CAllocatorDisable) CIntrusiveIterator;
				CIntrusiveIterator m_Iter;
			public:

#ifdef DMibDebuggerHelpers
				static TCLinkedList* ms_pList;
				static CList* ms_pIntrusiveList;
#endif
				CIteratorConst()
				{
#ifdef DMibDebuggerHelpers
					(void)&ms_pList;
					(void)&ms_pIntrusiveList;
#endif
				}

				CIteratorConst(const TCLinkedList &_Map)
				{
					*this = _Map.f_GetIterator();
#ifdef DMibDebuggerHelpers
					(void)&ms_pList;
					(void)&ms_pIntrusiveList;
#endif
				}

				CIteratorConst &operator = (const TCLinkedList &_Map)
				{
					*this = _Map.f_GetIterator();
					return *this;
				}

				inline_medium mint f_GetLen() const
				{
					return m_Iter.f_GetLen();
				}

				inline_medium void f_Next()
				{
					++m_Iter;
				}
				inline_medium void f_Prev()
				{
					--m_Iter;
				}

				inline_small const t_CData *f_GetCurrent() const
				{
					const CMember *pMember = m_Iter;
					if (pMember)
						return &pMember->fp_GetObject();
					return nullptr;
				}

				inline_small operator const t_CData *() const
				{
					return f_GetCurrent();
				}

				inline_small const t_CData * operator ->() const
				{
					return f_GetCurrent();
				}

				inline_small void operator ++()
				{
					f_Next();
				}
				
				inline_small void operator --()
				{
					f_Prev();
				}
			};

			TCLinkedList()
			{
			}

			~TCLinkedList()
			{
				f_Clear();
			}


			void f_Clear()
			{
				m_Data.m_List.f_DeleteAllAllocator((t_CAllocator &)m_Data);
			}			

			TCLinkedList &operator = (const TCLinkedList &_Other)
			{
				DMibSafeCheck(this != &_Other, "Must not copy same object to itself");
				f_Clear();
				
				fp_CopyFrom(_Other);

				return *this;
			}

			template <typename... tfp_CParams>
			TCLinkedList(NMem::CAllocatorConstructTag const &_Allocator, tfp_CParams && ...p_Params)
				: m_Data(_Allocator, fg_Forward<tfp_CParams>(p_Params)...)
			{
			}
			
			TCLinkedList(TCLinkedList &&_Other)
				: m_Data(fg_Move(_Other.m_Data))
			{
			}
			
			TCLinkedList(const TCLinkedList &_Other)
			{
				fp_CopyFrom(_Other);
			}

			TCLinkedList &operator += (const TCLinkedList &_Other)
			{
				return f_Insert(_Other);
			}

			TCLinkedList &f_Insert(const TCLinkedList &_Other)
			{
				DMibSafeCheck(this != &_Other, "Must not add same object to itself");
				
				fp_CopyFrom(_Other);
				
				return *this;
			}

	
			CIteratorConst f_GetIterator() const
			{
				CIteratorConst Iterator;
				Iterator.m_Iter = m_Data.m_List;
				return Iterator;
			}

			CIterator f_GetIterator()
			{
				CIterator Iterator;
				Iterator.m_pLinkedList = this;
				Iterator.m_Iter = m_Data.m_List;
				return Iterator;
			}

			t_CData &f_InsertAfter(const CIterator &_InsertAfter)
			{
				CMember * pData = fp_Create();
				m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(t_CData &_ToInsert, const CIterator &_InsertAfter)
			{
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(t_CData &&_ToInsert, const CIterator &_InsertAfter)
			{
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(const t_CData &_ToInsert, const CIterator &_InsertAfter)
			{
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(t_CData *_ToInsert, const CIterator &_InsertAfter)
			{
				CMember * pData = fsp_MemberFromData(*_ToInsert);
				m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(const t_CData *_ToInsert, const CIterator &_InsertAfter)
			{
				CMember * pData = fsp_MemberFromData(*_ToInsert);
				m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
				return pData->fp_GetObject();
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_InsertAfter(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, const CIterator &_InsertAfter)
			{
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertAfter(pData, fsp_MemberFromData(*_InsertAfter));
				return pData->fp_GetObject();
			}

			/////////////////////////////////////////////////////////////////////////////////////////////

			t_CData &f_InsertAfter(const t_CData &_InsertAfter)
			{
				CMember *pMember = fsp_MemberFromData(_InsertAfter);
				CMember *pData = fp_Create();
				m_Data.m_List.f_InsertAfter(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(t_CData &_ToInsert, const t_CData &_InsertAfter)
			{
				CMember *pMember = fsp_MemberFromData(_InsertAfter);
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertAfter(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(t_CData &&_ToInsert, const t_CData &_InsertAfter)
			{
				CMember *pMember = fsp_MemberFromData(_InsertAfter);
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertAfter(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(const t_CData &_ToInsert, const t_CData &_InsertAfter)
			{
				CMember *pMember = fsp_MemberFromData(_InsertAfter);
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertAfter(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(t_CData *_ToInsert, const t_CData &_InsertAfter)
			{
				CMember *pMember = fsp_MemberFromData(_InsertAfter);
				CMember * pData = fsp_MemberFromData(*_ToInsert);
				m_Data.m_List.f_InsertAfter(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertAfter(const t_CData *_ToInsert, const t_CData &_InsertAfter)
			{
				CMember *pMember = fsp_MemberFromData(_InsertAfter);
				CMember * pData = fsp_MemberFromData(*_ToInsert);
				m_Data.m_List.f_InsertAfter(pData, pMember);
				return pData->fp_GetObject();
			}


			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_InsertAfter(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, const t_CData &_InsertAfter)
			{
				CMember *pMember = fsp_MemberFromData(_InsertAfter);
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertAfter(pData, pMember);
				return pData->fp_GetObject();
			}

			/////////////////////////////////////////////////////////////////////////////////////////////

			t_CData &f_InsertBefore(const CIterator &_InsertBefore)
			{
				CMember * pData = fp_Create();
				m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(t_CData &_ToInsert, const CIterator &_InsertBefore)
			{
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(t_CData &&_ToInsert, const CIterator &_InsertBefore)
			{
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(const t_CData &_ToInsert, const CIterator &_InsertBefore)
			{
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(t_CData *_ToInsert, const CIterator &_InsertBefore)
			{
				CMember * pData = fsp_MemberFromData(*_ToInsert);
				m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(const t_CData *_ToInsert, const CIterator &_InsertBefore)
			{
				CMember * pData = fsp_MemberFromData(*_ToInsert);
				m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
				return pData->fp_GetObject();
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_InsertBefore(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, const CIterator &_InsertBefore)
			{
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertBefore(pData, fsp_MemberFromData(*_InsertBefore));
				return pData->fp_GetObject();
			}

			/////////////////////////////////////////////////////////////////////////////////////////////

			t_CData &f_InsertBefore(const t_CData &_InsertBefore)
			{
				CMember *pMember = fsp_MemberFromData(_InsertBefore);
				CMember *pData = fp_Create();
				m_Data.m_List.f_InsertBefore(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(t_CData &_ToInsert, const t_CData &_InsertBefore)
			{
				CMember *pMember = fsp_MemberFromData(_InsertBefore);
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertBefore(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(t_CData &&_ToInsert, const t_CData &_InsertBefore)
			{
				CMember *pMember = fsp_MemberFromData(_InsertBefore);
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertBefore(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(const t_CData &_ToInsert, const t_CData &_InsertBefore)
			{
				CMember *pMember = fsp_MemberFromData(_InsertBefore);
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertBefore(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(t_CData *_ToInsert, const t_CData &_InsertBefore)
			{
				CMember *pMember = fsp_MemberFromData(_InsertBefore);
				CMember * pData = fsp_MemberFromData(*_ToInsert);
				m_Data.m_List.f_InsertBefore(pData, pMember);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertBefore(const t_CData *_ToInsert, const t_CData &_InsertBefore)
			{
				CMember *pMember = fsp_MemberFromData(_InsertBefore);
				CMember * pData = fsp_MemberFromData(*_ToInsert);
				m_Data.m_List.f_InsertBefore(pData, pMember);
				return pData->fp_GetObject();
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_InsertBefore(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert, const t_CData &_InsertBefore)
			{
				CMember *pMember = fsp_MemberFromData(_InsertBefore);
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertBefore(pData, pMember);
				return pData->fp_GetObject();
			}


			/////////////////////////////////////////////////////////////////////////////////////////////

			t_CData &f_Insert(t_CData &_ToInsert)
			{
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_Insert(pData);
				return pData->fp_GetObject();
			}

			t_CData &f_Insert(t_CData &&_ToInsert)
			{
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_Insert(pData);
				return pData->fp_GetObject();
			}

			t_CData &f_Insert(const t_CData &_ToInsert)
			{
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_Insert(pData);
				return pData->fp_GetObject();
			}
			t_CData &f_Insert()
			{
				CMember * pData = fp_Create();
				m_Data.m_List.f_Insert(pData);
				return pData->fp_GetObject();
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
			{
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_Insert(pData);
				return pData->fp_GetObject();
			}

			/////////////////////////////////////////////////////////////////////////////////////////////

			t_CData &f_Push(t_CData &_ToInsert)
			{
				return f_InsertFirst(_ToInsert);
			}

			t_CData &f_Push(t_CData &&_ToInsert)
			{
				return f_InsertFirst(fg_Move(_ToInsert));
			}

			t_CData &f_Push(const t_CData &_ToInsert)
			{
				return f_InsertFirst(_ToInsert);
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_Push(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
			{
				return f_InsertFirst(fg_Move(_ToInsert));
			}

			/////////////////////////////////////////////////////////////////////////////////////////////

			t_CData &f_InsertFirst(t_CData &_ToInsert)
			{
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertFirst(pData);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertFirst(t_CData &&_ToInsert)
			{
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertFirst(pData);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertFirst(const t_CData &_ToInsert)
			{
				CMember * pData = fp_Create(_ToInsert);
				m_Data.m_List.f_InsertFirst(pData);
				return pData->fp_GetObject();
			}

			t_CData &f_InsertFirst()
			{
				CMember * pData = fp_Create();
				m_Data.m_List.f_InsertFirst(pData);
				return pData->fp_GetObject();
			}
			
			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_InsertFirst(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
			{
				CMember * pData = fp_Create(fg_Move(_ToInsert));
				m_Data.m_List.f_InsertFirst(pData);
				return pData->fp_GetObject();
			}

			/////////////////////////////////////////////////////////////////////////////////////////////

			t_CData &f_InsertLast(t_CData &_ToInsert)
			{
				return f_Insert(_ToInsert);
			}

			t_CData &f_InsertLast(t_CData &&_ToInsert)
			{
				return f_Insert(fg_Move(_ToInsert));
			}

			t_CData &f_InsertLast(const t_CData &_ToInsert)
			{
				return f_Insert(_ToInsert);
			}
			t_CData &f_InsertLast()
			{
				return f_Insert();
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_InsertLast(TCConstruct<tf_CType, tfp_CParams...> &&_ToInsert)
			{
				return f_Insert(fg_Move(_ToInsert));
			}

			/////////////////////////////////////////////////////////////////////////////////////////////

			void f_Remove(t_CData &_Member) // _Member must be a reference to a value currently IN the list!
			{
				CMember *pMember = fsp_MemberFromData(_Member);
				DMibSafeCheck(pMember->m_ListLink.f_IsInList(), "Object is not linked into the list");
				DMibSafeCheck(m_Data.m_List.f_Contains(pMember), "You must make sure that what you are removing is an actual object in the list");
				m_Data.m_List.f_Remove(pMember);
				fg_DeleteObject(m_Data, pMember);
			}

			t_CData f_Pop()
			{
				CMember *pMember = m_Data.m_List.f_Pop();
				DMibSafeCheck(pMember, "You cannot pop from an empty list");
				m_Data.m_List.f_Remove(pMember);
				t_CData ToReturn = fg_Move(pMember->fp_GetObject());
				fg_DeleteObject(m_Data, pMember);
				return fg_Move(ToReturn);
			}

			t_CData &f_GetFirst()
			{
				return m_Data.m_List.f_GetFirst()->fp_GetObject();
			}

			const t_CData &f_GetFirst() const
			{
				return m_Data.m_List.f_GetFirst()->fp_GetObject();
			}

			t_CData &f_GetLast()
			{
				return m_Data.m_List.f_GetLast()->fp_GetObject();
			}

			const t_CData &f_GetLast() const
			{
				return m_Data.m_List.f_GetLast()->fp_GetObject();
			}

			bint f_IsEmpty() const
			{
				return m_Data.m_List.f_IsEmpty();
			}

			mint f_GetLen() const
			{
				return m_Data.m_List.f_GetLen();
			}

			bint operator == (const TCLinkedList &_Other) const
			{
				CIteratorConst Iter0 = *this;
				CIteratorConst Iter1 = _Other;
				while (Iter0 && Iter1)
				{
					if (!(*Iter0 == *Iter1))
						return false;

					++Iter0;
					++Iter1;
				}

				if (Iter0 && !Iter1)
					return false;
				if (!Iter0 && Iter1)
					return false;

				return true;
			}

			bint operator < (const TCLinkedList &_Other) const
			{
				CIteratorConst Iter0 = *this;
				CIteratorConst Iter1 = _Other;
				while (Iter0 && Iter1)
				{
					if ((*Iter0 < *Iter1))
						return true;
					if ((*Iter1 < *Iter0))
						return false;

					++Iter0;
					++Iter1;
				}

				if (!Iter0 && Iter1)
					return true;

				return false;
			}

			static t_CData *fs_GetPrev(t_CData &_Current)
			{
				CMember *pMember = fsp_MemberFromData(_Current);
				CMember *pPrev = CInternalIterator::fs_GetPrev(pMember);
				if (pPrev)
					return &pPrev->fp_GetObject();
				return nullptr;
			}

			static t_CData *fs_GetNext(t_CData &_Current)
			{
				CMember *pMember = fsp_MemberFromData(_Current);
				CMember *pNext = CInternalIterator::fs_GetNext(pMember);
				if (pNext)
					return &pNext->fp_GetObject();
				return nullptr;
			}

			static t_CData const *fs_GetPrev(t_CData const &_Current)
			{
				CMember const *pMember = (CMember const *)&_Current;
				CMember const *pPrev = CInternalIterator::fs_GetPrev(pMember);
				if (pPrev)
					return &pPrev->fp_GetObject();
				return nullptr;
			}

			static t_CData const *fs_GetNext(t_CData const &_Current)
			{
				CMember const *pMember = (CMember const *)&_Current;
				CMember const *pNext = CInternalIterator::fs_GetNext(pMember);
				if (pNext)
					return &pNext->fp_GetObject();
				return nullptr;
			}


		};
#ifdef DMibDebuggerHelpers
		template <typename t_CData, typename t_CAllocator>
		assure_used TCLinkedList<t_CData, t_CAllocator>* TCLinkedList<t_CData, t_CAllocator>::CIterator::ms_pList = nullptr;
		template <typename t_CData, typename t_CAllocator>
		assure_used TCLinkedList<t_CData, t_CAllocator>* TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::ms_pList = nullptr;

		
		template <typename t_CData, typename t_CAllocator>
		assure_used typename TCLinkedList<t_CData, t_CAllocator>::CList* TCLinkedList<t_CData, t_CAllocator>::CIterator::ms_pIntrusiveList = nullptr;
		template <typename t_CData, typename t_CAllocator>
		assure_used typename TCLinkedList<t_CData, t_CAllocator>::CList* TCLinkedList<t_CData, t_CAllocator>::CIteratorConst::ms_pIntrusiveList = nullptr;
#endif
	}
	namespace NStream
	{
		template <typename t_CStream, typename t_CData, typename t_CAllocator>
		class TCBinaryStreamTypeReference<t_CStream, NContainer::TCLinkedList<t_CData, t_CAllocator> >
		{
		public:
			static void fs_Feed(t_CStream &_Stream, NContainer::TCLinkedList<t_CData, t_CAllocator> const &_Data)
			{
				mint nItems = _Data.f_GetLen();
				
				fg_FeedLenToStream(_Stream, nItems);

				for (auto iItem = _Data.f_GetIterator(); iItem; ++iItem)
					_Stream << *iItem;
			}

			static void fs_Feed(t_CStream &_Stream, NContainer::TCLinkedList<t_CData, t_CAllocator> &&_Data)
			{
				mint nItems = _Data.f_GetLen();
				
				fg_FeedLenToStream(_Stream, nItems);

				for (auto iItem = _Data.f_GetIterator(); iItem; ++iItem)
					_Stream << fg_Move(*iItem);
			}
	
			static void fs_Consume(t_CStream &_Stream, NContainer::TCLinkedList<t_CData, t_CAllocator> &_Data)
			{
				uint64 nItems;
				fg_ConsumeLenFromStream(_Stream, nItems);

				for (mint i = 0; i < nItems; ++i)
					_Stream >> _Data.f_Insert();
			}
		};
	}

}

