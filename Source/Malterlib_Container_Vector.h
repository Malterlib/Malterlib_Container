// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Core/Core>

namespace NMib
{

	namespace NContainer
	{

		/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*\
		|	Class:				A List exception										|
		\*_____________________________________________________________________________*/

		DMibImpErrorClass(CExceptionList, NException::CException);

#		define DMibErrorList(_Description) DMibImpError(NMib::NContainer::CExceptionList, _Description)

#		ifndef DMibPNoShortCuts
#			define DErrorList(_Description) DMibErrorList(_Description)
#		endif

		/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*\
		|	Class:				A List exception										|
		\*_____________________________________________________________________________*/
		DMibImpErrorClass(CExceptionListBoundCheck, CExceptionList);


#		define DMibErrorListBoundCheck(_Description) DMibImpError(NMib::NContainer::CExceptionListBoundCheck, _Description)

#		ifndef DMibPNoShortCuts
#			define DErrorListBoundCheck(_Description) DMibErrorListBoundCheck(_Description)
#		endif


		class CVectorBoundsCheck
		{
		public:
			static inline_small void fs_CheckBounds(mint _Len, mint _Position)
			{
				if (_Position >= _Len)
					DMibErrorListBoundCheck("Tried to access element outside list");
			}
			static inline_small void fs_CheckOverlapping(mint _Start0, mint _Start1, mint _Len0, mint _Len1)
			{
				if (_Start0 <= _Start1)
				{
					if (_Start0 + _Len0 > _Start1)
						DMibErrorListBoundCheck("Overlapping range in move");
				}
				else
				{
					if (_Start1 + _Len1 > _Start0)
						DMibErrorListBoundCheck("Overlapping range in move");
				}
			}
		};

		class CVectorBoundsDontCheck
		{
		public:
			static inline_small void fs_CheckBounds(mint _Len, mint _Position)
			{
			}
			static inline_small void fs_CheckOverlapping(mint _Start0, mint _Start1, mint _Len0, mint _Len1)
			{
			}
		};

		class CVectorData
		{
			mint m_Length;
		public:
			inline_small void f_SetLen(mint _Len)
			{
				m_Length = _Len;
			}
			inline_small mint f_GetLen() const
			{
				return m_Length;
			}
		};

		// Always deault to boundschecking to guard against buffer overruns
//#		ifdef DMibDebug
		class CVectorBoundsCheckDefault : public CVectorBoundsCheck
		{
		};
//#		else
//		class CVectorBoundsCheckDefault : public CVectorBoundsDontCheck
//		{
//		};
//#		endif

		class CVectorStaticData_GrowFixed
		{
		public:
			CVectorStaticData_GrowFixed()
			{
				m_GrowSize = 8; // Default grow
			}

			class CExtraVectorData
			{
			public:
				mint m_AllocSize;
				inline_small void f_SetAllocSize(mint _AllocSize)
				{
					m_AllocSize = _AllocSize;
				}
				mint f_GetAllocSize(mint _AllocSize)
				{
					return m_AllocSize;
				}
			};

			mint m_GrowSize;

			inline_small void f_SetGrow(mint _Value) // Only allow power of two groth
			{
				m_GrowSize = mint(1) << NMib::fg_GetHighestBitSet(_Value);
			}

			inline_small mint f_GetAllocSize(mint _NeededSize)
			{
				return fg_AlignUp(_NeededSize, m_GrowSize);
			}
			inline_small bint f_NeedRealloc(mint _OldSize, mint _NeededSize, const CExtraVectorData *_pExtraData)
			{
				if (!_pExtraData)
					return true;
				mint Old = _pExtraData->m_AllocSize;
				mint New = f_GetAllocSize(_NeededSize);
				if (New > Old)
					return true;
				if (New < Old + m_GrowSize)
					return true;
				return false;
			}
			inline_small bint f_CanGrow(mint _OldSize, mint _NeededSize, const CExtraVectorData *_pExtraData)
			{
				if (!_pExtraData)
					return false;
				mint Old = _pExtraData->m_AllocSize;
				if (_NeededSize > Old)
					return false;
				return true;
			}
		};

		template <mint t_MinSize, bool t_bShrink>
		class TCVectorStaticData_GrowDouble
		{
		public:
			class CExtraVectorData
			{
			public:
				mint m_AllocSize;
				void f_SetAllocSize(mint _AllocSize)
				{
					m_AllocSize = _AllocSize;
				}
				mint f_GetAllocSize(mint _AllocSize)
				{
					return m_AllocSize;
				}
			};

			static inline_small void f_SetGrow(mint _Value)
			{
			}

			inline_small mint f_GetAllocSize(mint _NeededSize)
			{
				return fg_Max((mint(1) << NMib::fg_GetHighestBitSet(_NeededSize-1)) << 1, t_MinSize);
			}

			inline_small bint f_NeedRealloc(mint _OldSize, mint _NeededSize, const CExtraVectorData *_pExtraData)
			{
				if (!_pExtraData)
					return true;
				mint Old = _pExtraData->m_AllocSize;
				mint New = f_GetAllocSize(_NeededSize);
				if (New > Old)
					return true;

				if (t_bShrink)
				{
					if (Old > (New << 1))
						return true;
				}
				
				return false;
			}
			inline_small bint f_CanGrow(mint _OldSize, mint _NeededSize, const CExtraVectorData *_pExtraData)
			{
				if (!_pExtraData)
					return false;
				mint Old = _pExtraData->m_AllocSize;
				if (_NeededSize > Old)
					return false;
				return true;
			}
		};

		class CVectorStaticData_GrowOne
		{
		public:
			class CExtraVectorData
			{
			public:
				static inline_small void f_SetAllocSize(mint _AllocSize)
				{
				}
				static inline_small mint f_GetAllocSize(mint _AllocSize)
				{
					return _AllocSize;
				}
			};

			static inline_small void f_SetGrow(mint _Value)
			{
			}

			static inline_small mint f_GetAllocSize(mint _NeededSize)
			{
				return _NeededSize;
			}

			static inline_small bint f_NeedRealloc(mint _OldSize, mint _NeededSize, const CExtraVectorData *_pExtraData)
			{
				mint Old = f_GetAllocSize(_OldSize);
				mint New = f_GetAllocSize(_NeededSize);
				if (New != Old)
					return true;
				return false;
			}

			inline_small bint f_CanGrow(mint _OldSize, mint _NeededSize, const CExtraVectorData *_pExtraData)
			{
				return false;
			}

		};

		class CVectorStaticData_Bigger
		{
		public:
			class CExtraVectorData
			{
			public:
				mint m_AllocSize;
				inline_small void f_SetAllocSize(mint _AllocSize)
				{
					m_AllocSize = _AllocSize;
				}
				inline_small mint f_GetAllocSize(mint _AllocSize)
				{
					return m_AllocSize;
				}
			};

			static inline_small void f_SetGrow(mint _Value)
			{
			}

			static inline_small mint f_GetAllocSize(mint _NeededSize)
			{
				return _NeededSize;
			}

			static inline_small bint f_NeedRealloc(mint _OldSize, mint _NeededSize, const CExtraVectorData *_pExtraData)
			{
				mint Old = _pExtraData ? _pExtraData->m_AllocSize : 0u;
				mint New = f_GetAllocSize(_NeededSize);
				if (New > Old)
					return true;
				return false;
			}
			inline_small bint f_NeedReallocGrow(mint _OldSize, mint _NeededSize, const CExtraVectorData *_pExtraData)
			{
				if (!_pExtraData)
					return false;
				mint Old = _pExtraData->m_AllocSize;
				if (_NeededSize > Old)
					return false;
				return true;
			}
		};


		namespace NPrivate
		{
			/***************************************************************************************************\
			|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
			| Simple																							|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/

			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value, void>::CType fg_MoveArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				NMem::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value, void>::CType fg_MoveArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
			}

			template <typename t_CData0, typename t_CData1>
			static void fg_MoveArray(t_CData0 *_pDest, t_CData1 *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
					new((void *)(_pDest + i)) t_CData0(fg_Move(_pSrc[i]));
			}

			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_DestroyArray(t_CData *_pDest, mint _Len)
			{
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_DestroyArray(t_CData *_pDest, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
				{
					_pDest[i].~t_CData();
				}
			}

			template <typename t_CData, typename t_CReportLen>
			static typename TCEnableIf<NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_DestroyArray(t_CData *_pDest, mint _Len, t_CReportLen *_pReport)
			{
			}
			template <typename t_CData, typename t_CReportLen>
			static typename TCEnableIf<!NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_DestroyArray(t_CData *_pDest, mint _Len, t_CReportLen *_pReport)
			{
				// Destroy all cells
				for (smint i = _Len - 1; i >= 0; --i)
				{
					_pReport->f_SetLen(i);
					_pDest[i].~t_CData();
				}
			}

			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialDefaultConstructor<t_CData>::mc_Value, void>::CType fg_ConstructArray(t_CData *_pDest, mint _Len)
			{
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialDefaultConstructor<t_CData>::mc_Value, void>::CType fg_ConstructArray(t_CData *_pDest, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
				{
					new((void *)(_pDest + i)) t_CData();
				}
			}

			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value, void>::CType fg_CopyArray(t_CData *_pDest, t_CData const *_pSrc, mint _Len)
			{
				NMem::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value, void>::CType fg_CopyArray(t_CData *_pDest, t_CData const *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
					new((void *)(_pDest + i)) t_CData(_pSrc[i]);
			}

			template <typename t_CData0, typename t_CData1>
			static void fg_CopyArray(t_CData0 *_pDest, t_CData1 const *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
					new((void *)(_pDest + i)) t_CData0(_pSrc[i]);
			}

			/***************************************************************************************************\
			|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
			| Combined																							|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/


			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_MoveDestroyOverlappingArrayReverse(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				NMem::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_MoveDestroyOverlappingArrayReverse(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				for (smint i = smint(_Len) - 1; i >= 0; --i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
				}
			}
			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && !NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_MoveDestroyOverlappingArrayReverse(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				NMem::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
				for (smint i = smint(_Len) - 1; i >= 0; --i)
					_pSrc[i].~t_CData();
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && !NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_MoveDestroyOverlappingArrayReverse(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				for (smint i = smint(_Len) - 1; i >= 0; --i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
					_pSrc[i].~t_CData();
				}
			}

			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_MoveDestroyOverlappingArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				NMem::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_MoveDestroyOverlappingArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
				}
			}
			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && !NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_MoveDestroyOverlappingArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				NMem::fg_MemMove(_pDest, _pSrc, _Len * sizeof(t_CData));
				for (mint i = 0; i < _Len; ++i)
					_pSrc[i].~t_CData();
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && !NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_MoveDestroyOverlappingArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
				{
					new((void *)(_pDest + i)) t_CData(fg_Move(_pSrc[i]));
					_pSrc[i].~t_CData();
				}
			}

			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_DestroyCopyArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				NMem::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_DestroyCopyArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
					new((void *)(_pDest + i)) t_CData(_pSrc[i]);
			}
			template <typename t_CData>
			static typename TCEnableIf<NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && !NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_DestroyCopyArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
					_pDest[i].~t_CData();
				NMem::fg_MemCopy(_pDest, _pSrc, _Len * sizeof(t_CData));
			}
			template <typename t_CData>
			static typename TCEnableIf<!NTraits::TCHasTrivialCopyConstructor<t_CData>::mc_Value && !NTraits::TCHasTrivialDestructor<t_CData>::mc_Value, void>::CType fg_DestroyCopyArray(t_CData *_pDest, t_CData *_pSrc, mint _Len)
			{
				for (mint i = 0; i < _Len; ++i)
				{
					_pDest[i].~t_CData();
					new((void *)(_pDest + i)) t_CData(_pSrc[i]);
				}
			}

		}

		/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*\
		|	Template Class:																					|
		|																									|
		|	Description:		Template for simple array management.										|
		|																									|
        |	Comments:			Use this class as a lightweight list implementation.						|
		|						If you are going to send lists around a lot (and							|
		|						save them) you might be better off using TVectorRef.						|
		|																									|
		|	See Also:			.																			|
		|																									|
		|	Location:			Template Classes															|
		|																									|
		|	Index:				!name																		|
		\*_________________________________________________________________________________________________*/

		template <typename t_CIteratorData>
		class TCVectorIterator
		{
			t_CIteratorData *m_Begin;
			t_CIteratorData *m_End;

			t_CIteratorData *fp_GetNext(t_CIteratorData *_pCurrent) const
			{
				if (m_Begin != m_End)
					return m_Begin+1;
				else
					return nullptr;
			}

			inline_small t_CIteratorData *fp_GetCurrent() const
			{
				if (m_Begin != m_End)
					return m_Begin;
				else
					return nullptr;
			}

		public:
			TCVectorIterator()
				: m_Begin(nullptr)
				, m_End(nullptr)
			{
			}

			TCVectorIterator(t_CIteratorData *_Begin,t_CIteratorData *_End)
				: m_Begin(_Begin)
				, m_End(_End)
			{
			}

			TCVectorIterator(TCVectorIterator const& _ToCopy)
				: m_Begin(_ToCopy.m_Begin)
				, m_End(_ToCopy.m_End)
			{
			}

			inline_small TCVectorIterator& operator=(TCVectorIterator const& _ToCopy)
			{
				m_Begin = _ToCopy.m_Begin;
				m_End = _ToCopy.m_End;
				return *this;
			}

			inline_small bint operator==(TCVectorIterator const& _Other) const
			{
				return 		(m_Begin == _Other.m_Begin)
						&&	(m_End == _Other.m_End);
			}

			inline_small operator t_CIteratorData *() const
			{
				return fp_GetCurrent();
			}

			inline_small t_CIteratorData * operator ->() const
			{
				return fp_GetCurrent();
			}

			inline_small TCVectorIterator& operator ++()
			{
				m_Begin = fp_GetNext(m_Begin);
				return *this;
			}

			inline_small TCVectorIterator& operator +=(smint _Offset)
			{
				m_Begin = m_Begin + _Offset;
				DMibFastCheck(m_Begin <= m_End);

				if (m_Begin == m_End)
					m_Begin = nullptr;
				
				return *this;
			}

		};
		template 
		<
			typename t_CData,			// The class you want in the array
			typename t_CAllocator,		// The class that checks the bounds on accesses
			typename t_CBoundsChecker,	// The allocator you want to allocate memory from
			typename t_CInternalData,	// This class has to be inherited for CVectorData or implement the same interface
			typename t_CStaticData		// Extra static data to same in the TCVector class
		>
		class TCVector
		{
		public:
			typedef t_CData CData;
			typedef typename t_CStaticData::CExtraVectorData CExtraVectorData;
		private:

			class CVectorData : public CExtraVectorData
			{
			public:
				t_CInternalData m_InternalData;
				inline_small void f_SetLen(mint _Len)
				{
					return m_InternalData.f_SetLen(_Len);
				}
				inline_small mint f_GetLen() const
				{
					return m_InternalData.f_GetLen();
				}
				inline_small t_CData *f_GetData()
				{
					return (t_CData *)(this + 1);
				}
			};


			class CStaticData : public t_CStaticData
			{
				t_CAllocator m_Allocator;
			public:
				TCDynamicPtr<typename t_CAllocator::CPtrHolder, CVectorData> m_pData;
				t_CAllocator &f_Allocator()
				{
					return m_Allocator;
				}
			};

			class CStaticDataEmptyAllocator : public t_CStaticData
			{
			public:
				TCDynamicPtr<typename t_CAllocator::CPtrHolder, CVectorData> m_pData;
				t_CAllocator &f_Allocator()
				{
					return *((t_CAllocator *)this);
				}
			};

			typename TCChooseType<NTraits::TCIsEmpty<t_CAllocator>::mc_Value, CStaticDataEmptyAllocator, CStaticData>::CType m_StaticData;

			template <typename t_CSorter>
			void fp_InsertSort(t_CData *_pArray, aint _Low, aint _High, t_CSorter &&_Sorter)
			{
				t_CData Temp;
				aint i, j;

				for (i = _Low + 1; i <= _High; i++) 
				{
					Temp = fg_Move(_pArray[i]);

					for (j = i-1; j >= _Low && fg_Forward<t_CSorter>(_Sorter)(Temp, _pArray[j]); j--)
						_pArray[j+1] = fg_Move(_pArray[j]);

					_pArray[j+1] = fg_Move(Temp);
				}
			}

			template <typename t_CSorter>
			aint fp_Partition(t_CData *_pArray, aint _Low, aint _High, t_CSorter &&_Sorter) 
			{
				t_CData Temp, Pivot;
				aint i, j, p;

				p = _Low + ((_High - _Low)>>1);
				Pivot = fg_Move(_pArray[p]);
				_pArray[p] = fg_Move(_pArray[_Low]);

				i = _Low+1;
				j = _High;
				while (1) 
				{
					while (i < j && fg_Forward<t_CSorter>(_Sorter)(_pArray[i], Pivot)) 
						i++;
					while (j >= i && fg_Forward<t_CSorter>(_Sorter)(Pivot, _pArray[j])) 
						j--;
					if (i >= j) 
						break;
					Temp = fg_Move(_pArray[i]);
					_pArray[i] = fg_Move(_pArray[j]);
					_pArray[j] = fg_Move(Temp);
					j--; i++;
				}

				if (_Low != j)
					_pArray[_Low] = fg_Move(_pArray[j]);
				_pArray[j] = fg_Move(Pivot);

				return j;
			}

			template <typename t_CSorter>
			void fpr_QuickSort(t_CData *_pArray, aint _Low, aint _High, t_CSorter &&_Sorter) 
			{
				aint m;

				while (_Low < _High) 
				{

					if (_High - _Low <= 12) 
					{
						fp_InsertSort(_pArray, _Low, _High, _Sorter);
						return;
					}

					m = fp_Partition(_pArray, _Low, _High, _Sorter);

					if (m - _Low <= _High - m) 
					{
						fpr_QuickSort(_pArray, _Low, m - 1, _Sorter);
						_Low = m + 1;
					}
					else
					{
						fpr_QuickSort(_pArray, m + 1, _High, _Sorter);
						_High = m - 1;
					}
				}
			}

			t_CData *fp_MakeRoomBegin(mint _Len)
			{
				if (_Len == 0)
					return nullptr;
				mint OldLen = f_GetLen();
				mint NewLen = OldLen + _Len;
				if (m_StaticData.f_NeedRealloc(OldLen, NewLen, m_StaticData.m_pData))
				{
					mint AllocSize = m_StaticData.f_GetAllocSize(NewLen);
					mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
					mint MaxObjects = fg_AlignDown(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), Granularity) / sizeof(t_CData);
					if (AllocSize >= MaxObjects)
						DMibErrorMemory("Out of memory trying to allocate objects in vector");
					mint Size = fg_AlignUp(AllocSize * sizeof(t_CData) + sizeof(CVectorData), Granularity);
					CVectorData *pNewData = fp_AllocData(Size);
					t_CData *pOldArray = f_GetArray();
					t_CData *pNewArray = pNewData->f_GetData();
					// Construct all new datas with copy constructor from old datas
					
					NPrivate::fg_MoveArray(pNewArray + _Len, pOldArray, OldLen);
					NPrivate::fg_DestroyArray(pOldArray, OldLen);

					// Destroy old array
					if (m_StaticData.m_pData)
						fp_FreeData(m_StaticData.m_pData);

					// Save new array
					pNewData->f_SetLen(NewLen);
					pNewData->f_SetAllocSize(AllocSize);
					m_StaticData.m_pData = pNewData;
					return pNewData->f_GetData();
				}
				else
				{
					t_CData *pOldArray = f_GetArray();

					// Move old data
					NPrivate::fg_MoveDestroyOverlappingArrayReverse(pOldArray + _Len, pOldArray, OldLen);

					m_StaticData.m_pData->f_SetLen(NewLen);
				}
				return m_StaticData.m_pData->f_GetData();
			}

			t_CData *fp_MakeRoomMiddle(mint _iStart, mint _Len)
			{
				if (_Len == 0)
					return nullptr;
				mint OldLen = f_GetLen();
				mint NewLen = OldLen + _Len;
				if (m_StaticData.f_NeedRealloc(OldLen, NewLen, m_StaticData.m_pData))
				{
					mint AllocSize = m_StaticData.f_GetAllocSize(NewLen);
					mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
					mint MaxObjects = fg_AlignDown(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), Granularity) / sizeof(t_CData);
					if (AllocSize >= MaxObjects)
						DMibErrorMemory("Out of memory trying to allocate objects in vector");
					mint Size = fg_AlignUp(AllocSize * sizeof(t_CData) + sizeof(CVectorData), Granularity);
					CVectorData *pNewData = fp_AllocData(Size);
					t_CData *pOldArray = f_GetArray();
					t_CData *pNewArray = pNewData->f_GetData();
					// Construct all new datas with copy constructor from old datas

					// Move old
					NPrivate::fg_MoveArray(pNewArray, pOldArray, _iStart);
					// Move old after hole
					if (OldLen > _iStart)
						NPrivate::fg_MoveArray(pNewArray + _Len + _iStart, pOldArray + _iStart, (OldLen - _iStart));
					NPrivate::fg_DestroyArray(pOldArray, OldLen);

					// Destroy old array
					if (m_StaticData.m_pData)
						fp_FreeData(m_StaticData.m_pData);

					// Save new array
					pNewData->f_SetLen(NewLen);
					pNewData->f_SetAllocSize(AllocSize);
					m_StaticData.m_pData = pNewData;
					return pNewData->f_GetData();
				}
				else
				{
					t_CData *pOldArray = f_GetArray();

					// Move old data
					if (OldLen > _iStart)
						NPrivate::fg_MoveDestroyOverlappingArrayReverse(pOldArray + _Len + _iStart, pOldArray + _iStart, (OldLen - _iStart));

					m_StaticData.m_pData->f_SetLen(NewLen);
				}
				return m_StaticData.m_pData->f_GetData();
			}

			t_CData *fp_MakeRoom(mint _Len)
			{
				mint NewLen = _Len;
				if (NewLen == 0)
				{
					f_Clear();
					return nullptr;
				}
				mint OldLen = f_GetLen();
				if (m_StaticData.f_NeedRealloc(OldLen, NewLen, m_StaticData.m_pData))
				{
					mint AllocSize = m_StaticData.f_GetAllocSize(NewLen);
					mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
					mint MaxObjects = fg_AlignDown(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), Granularity) / sizeof(t_CData);
					if (AllocSize >= MaxObjects)
						DMibErrorMemory("Out of memory trying to allocate objects in vector");
					mint Size = fg_AlignUp(AllocSize * sizeof(t_CData) + sizeof(CVectorData), Granularity);
					CVectorData *pNewData = fp_AllocData(Size);
					t_CData *pOldArray = f_GetArray();
					t_CData *pNewArray = pNewData->f_GetData();
					mint CurrentLength = OldLen;

					mint OldRemainingCells = fg_Min(NewLen, CurrentLength);

					// Construct all new datas with copy constructor from old datas
					NPrivate::fg_MoveArray(pNewArray, pOldArray, OldRemainingCells);

					NPrivate::fg_DestroyArray(pOldArray, CurrentLength);

					// Destroy old array
					if (m_StaticData.m_pData)
						fp_FreeData(m_StaticData.m_pData);

					// Save new array
					pNewData->f_SetLen(NewLen);
					pNewData->f_SetAllocSize(AllocSize);
					m_StaticData.m_pData = pNewData;
					return pNewData->f_GetData();
				}
				else
					m_StaticData.m_pData->f_SetLen(NewLen);
				return m_StaticData.m_pData->f_GetData();
			}

			t_CData *fp_MakeNewRoom(mint _Len)
			{
				mint NewLen = _Len;
				if (NewLen == 0)
				{
					f_Clear();
					return nullptr;
				}
				mint OldLen = f_GetLen();
				if (m_StaticData.f_NeedRealloc(OldLen, NewLen, m_StaticData.m_pData))
				{
					mint AllocSize = m_StaticData.f_GetAllocSize(NewLen);
					mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
					mint MaxObjects = fg_AlignDown(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), Granularity) / sizeof(t_CData);
					if (AllocSize >= MaxObjects)
						DMibErrorMemory("Out of memory trying to allocate objects in vector");
					mint Size = fg_AlignUp(AllocSize * sizeof(t_CData) + sizeof(CVectorData), Granularity);
					CVectorData *pNewData = fp_AllocData(Size);
					t_CData *pOldArray = f_GetArray();
					mint CurrentLength = OldLen;

					NPrivate::fg_DestroyArray(pOldArray, CurrentLength);
					
					// Destroy old array
					if (m_StaticData.m_pData)
						fp_FreeData(m_StaticData.m_pData);

					// Save new array
					pNewData->f_SetLen(NewLen);
					pNewData->f_SetAllocSize(AllocSize);
					m_StaticData.m_pData = pNewData;
				}
				else
				{
					t_CData *pOldArray = f_GetArray();
					NPrivate::fg_DestroyArray(pOldArray, OldLen);
					m_StaticData.m_pData->f_SetLen(NewLen);
				}
				return m_StaticData.m_pData->f_GetData();
			}

		public:

			typedef TCVectorIterator<t_CData> CIterator;
			typedef TCVectorIterator<t_CData const> CIteratorConst;
			CIterator f_GetIterator() 
			{ 
				return CIterator(f_GetArray(),f_GetArray()+f_GetLen()); 
			}
			CIteratorConst f_GetIterator() const 
			{ 
				return CIteratorConst(f_GetArray(),f_GetArray()+f_GetLen()); 
			}

		private:
			template <typename t_CIteratorData>
			mint fp_GetIteratorPos(TCVectorIterator<t_CIteratorData> const &_Iter) const
			{
				return _Iter - f_GetArray();
			}

			void fp_Copy(TCVector const &_Source)
			{
				mint nSource = _Source.f_GetLen();
				fp_MakeNewRoom(nSource);
				auto pSrc = _Source.f_GetArray();
				auto pDst = f_GetArray();

				NPrivate::fg_CopyArray(pDst, pSrc, nSource);
			}

			template <bool tf_bCanBeStatic>
			typename TCEnableIf<tf_bCanBeStatic, void>::CType fp_MoveConstruct(TCVector &&_Source)
			{
				if (_Source.m_StaticData.f_Allocator().f_IsStatic(_Source.m_StaticData.m_pData))
				{
					m_StaticData.m_pData = nullptr;
					fp_Copy(_Source);
				}
				else
				{
					m_StaticData.m_pData = _Source.m_StaticData.m_pData;
					_Source.m_StaticData.m_pData = nullptr;
				}
			}
			
			template <bool tf_bCanBeStatic>
			typename TCEnableIf<!tf_bCanBeStatic, void>::CType fp_MoveConstruct(TCVector &&_Source)
			{
				m_StaticData.m_pData = _Source.m_StaticData.m_pData;
				_Source.m_StaticData.m_pData = nullptr;
			}

			template <bool tf_bCanBeStatic>
			typename TCEnableIf<tf_bCanBeStatic, void>::CType fp_MoveCopy(TCVector &&_Source)
			{
				if (_Source.m_StaticData.f_Allocator().f_IsStatic(_Source.m_StaticData.m_pData))
					fp_Copy(_Source);
				else
				{
					m_StaticData.m_pData = _Source.m_StaticData.m_pData;
					_Source.m_StaticData.m_pData = nullptr;
				}
			}
			template <bool tf_bCanBeStatic>
			typename TCEnableIf<!tf_bCanBeStatic, void>::CType fp_MoveCopy(TCVector &&_Source)
			{
				m_StaticData.m_pData = _Source.m_StaticData.m_pData;
				_Source.m_StaticData.m_pData = nullptr;
			}
			
			void fp_FreeData(CVectorData *_pData)
			{
				mint nObjects = _pData->f_GetAllocSize(_pData->f_GetLen());

				mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
				mint Size = fg_AlignUp(nObjects * sizeof(t_CData) + sizeof(CVectorData), Granularity);
				
				m_StaticData.f_Allocator().f_Free(_pData, Size);
			}
			
			CVectorData *fp_AllocData(mint _Size)
			{
				return (CVectorData *)m_StaticData.f_Allocator().f_Alloc(_Size, EAllocationFlag_WillFreeWithSize);
			}
			
		public:

			TCVector() throw()
			{
				m_StaticData.m_pData = nullptr;

			}

/*			TCVector(const TCVector&_Source)
			{
				m_StaticData.m_pData = nullptr;
				f_Replace(_Source);
			}*/
			
			TCVector(TCVector &&_Source) throw()
			{
				fp_MoveConstruct<t_CAllocator::mc_CanBeStatic>(fg_Move(_Source));
			}

			TCVector(TCVector const &_Source)
			{
				m_StaticData.m_pData = nullptr;
				fp_Copy(_Source);
			}

			TCVector(TCVector &_Source)
			{
				m_StaticData.m_pData = nullptr;
				fp_Copy(_Source);
			}


			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			TCVector(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Source)
			{
				m_StaticData.m_pData = nullptr;
				mint nSource = _Source.f_GetLen();
				fp_MakeNewRoom(nSource);
				auto pSrc = _Source.f_GetArray();
				auto pDst = f_GetArray();

				NPrivate::fg_CopyArray(pDst, pSrc, nSource);
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			TCVector(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Source)
			{
				m_StaticData.m_pData = nullptr;
				mint nSource = _Source.f_GetLen();
				fp_MakeNewRoom(nSource);
				auto pSrc = _Source.f_GetArray();
				auto pDst = f_GetArray();
				NPrivate::fg_CopyArray(pDst, pSrc, nSource);
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			TCVector(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Source)
			{
				m_StaticData.m_pData = nullptr;
				mint nSource = _Source.f_GetLen();
				fp_MakeNewRoom(nSource);
				auto pSrc = _Source.f_GetArray();
				auto pDst = f_GetArray();
				NPrivate::fg_MoveArray(pDst, pSrc, nSource);
			}



			~TCVector()
			{
				f_Clear();
			}

			void f_SetGrow(mint _Value)
			{
				m_StaticData.f_SetGrow(_Value);
			}

			t_CData *f_GetArray()
			{
				if (m_StaticData.m_pData)
					return m_StaticData.m_pData->f_GetData();
				else
					return nullptr;
			}

			
			t_CData &f_GetFirst()
			{
				return (*this)[0];
			}

			t_CData &f_GetLast()
			{
				return (*this)[f_GetLen()-1];
			}

			t_CData const &f_GetFirst() const
			{
				return (*this)[0];
			}

			t_CData const &f_GetLast() const
			{
				return (*this)[f_GetLen()-1];
			}

			t_CData *f_GetArray(mint _Len)
			{
				f_SetLen(_Len);
				return f_GetArray();
			}

			mint f_GetLen() const
			{
				if (m_StaticData.m_pData)
					return m_StaticData.m_pData->f_GetLen();
				else
					return 0;
			}
			mint f_IsEmpty() const
			{
				if (m_StaticData.m_pData)
					return m_StaticData.m_pData->f_GetLen() == 0;
				else
					return true;
			}

			const t_CData *f_GetArray() const
			{
				if (m_StaticData.m_pData)
					return m_StaticData.m_pData->f_GetData();
				else
					return nullptr;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther, typename t_CFunctor>
			aint f_Compare(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Other, t_CFunctor &&_Functor) const
			{
				mint Len0 = f_GetLen();
				mint Len1 = _Other.f_GetLen();
				if (Len0 > Len1)
					return 1;
				else if (Len0 < Len1)
					return -1;

				const t_CData *pData0 = f_GetArray();
				const t_CDataOther *pData1 = _Other.f_GetArray();

				for (mint i = 0; i < Len0; ++i)
				{
					aint iCompare = _Functor(pData0[i], pData1[i]);
					if (iCompare)
						return iCompare;
				}
				return 0;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			aint f_Compare(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Other) const
			{
				mint Len0 = f_GetLen();
				mint Len1 = _Other.f_GetLen();
				if (Len0 > Len1)
					return 1;
				else if (Len0 < Len1)
					return -1;

				const t_CData *pData0 = f_GetArray();
				const t_CDataOther *pData1 = _Other.f_GetArray();

				for (mint i = 0; i < Len0; ++i)
				{
					if (pData0[i] > pData1[i])
						return 1;
					else if (pData0[i] < pData1[i])
						return -1;
				}
				return 0;
			}

			TCVector &operator << (const t_CData &_Data)
			{
				f_Insert(_Data);
				return *this;
			}

			/***************************************************************************************************\
			|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
			| Insert last																						|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/


			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_Insert(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Vector)
			{
				mint PrevLen = f_GetLen();
				mint AddLen = _Vector.f_GetLen();
				mint NewLen = PrevLen + AddLen;
				fp_MakeRoom(NewLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther const *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_CopyArray(pArray + PrevLen, pSrcArray, AddLen);

				return pArray + PrevLen;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_Insert(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Vector)
			{
				mint PrevLen = f_GetLen();
				mint AddLen = _Vector.f_GetLen();
				mint NewLen = PrevLen + AddLen;
				fp_MakeRoom(NewLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_CopyArray(pArray + PrevLen, pSrcArray, AddLen);

				return pArray + PrevLen;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_Insert(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Vector)
			{
				mint PrevLen = f_GetLen();
				mint AddLen = _Vector.f_GetLen();
				mint NewLen = PrevLen + AddLen;
				fp_MakeRoom(NewLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_MoveArray(pArray + PrevLen, pSrcArray, AddLen);

				return pArray + PrevLen;
			}

			t_CData *f_Insert(const t_CData *_pData, mint _Len = 1)
			{
				aint PrevLen = f_GetLen();
				aint NewLen = PrevLen + _Len;
				fp_MakeRoom(NewLen);
				t_CData *pArray = f_GetArray();

				NPrivate::fg_CopyArray(pArray + PrevLen, _pData, _Len);

				return pArray + PrevLen;
			}

			t_CData *f_Insert(t_CData *_pData, mint _Len = 1)
			{
				aint PrevLen = f_GetLen();
				aint NewLen = PrevLen + _Len;
				fp_MakeRoom(NewLen);
				t_CData *pArray = f_GetArray();

				NPrivate::fg_CopyArray(pArray + PrevLen, _pData, _Len);

				return pArray + PrevLen;
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &fp_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
			{
				aint PrevLen = f_GetLen();
				t_CData *pArray = fp_MakeRoom(PrevLen + 1) + PrevLen;
				_CreateParams.template f_Create<t_CData>(NMem::TCAllocator_Placement<sizeof(t_CData)>((void *)(pArray)));
				return *pArray;
				
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
			{
				mint PrevLen = f_GetLen();
				auto pData = m_StaticData.m_pData;
				if (m_StaticData.f_CanGrow(PrevLen, PrevLen + 1, pData))
				{
					pData->f_SetLen(PrevLen + 1);
					t_CData *pArray = pData->f_GetData() + PrevLen;
					_CreateParams.template f_Create<t_CData>(NMem::TCAllocator_Placement<sizeof(t_CData)>((void *)(pArray)));
					return *pArray;
				}
				return fp_Insert(fg_Move(_CreateParams));
			}

			t_CData &f_Insert(const t_CData &_Data)
			{
				aint PrevLen = f_GetLen();
				t_CData *pArray = fp_MakeRoom(PrevLen + 1);
				new((void *)(pArray + PrevLen)) t_CData(_Data);
				return pArray[PrevLen];
			}

			inline_never t_CData &fp_Insert(t_CData &_Data)
			{
				mint PrevLen = f_GetLen();
				t_CData *pArray = fp_MakeRoom(PrevLen + 1) + PrevLen;
				new((void *)(pArray)) t_CData(_Data);
				return *pArray;
			}

			inline_small t_CData &f_Insert(t_CData &_Data)
			{
				mint PrevLen = f_GetLen();
				auto pData = m_StaticData.m_pData;
				if (m_StaticData.f_CanGrow(PrevLen, PrevLen + 1, pData))
				{
					pData->f_SetLen(PrevLen + 1);
					t_CData *pArray = pData->f_GetData() + PrevLen;
					new((void *)(pArray)) t_CData(_Data);
					return *pArray;
				}
				return fp_Insert(_Data);
			}

			t_CData &f_Insert(t_CData &&_Data)
			{
				aint PrevLen = f_GetLen();
				fp_MakeRoom(PrevLen + 1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray + PrevLen)) t_CData(fg_Move(_Data));
				return pArray[PrevLen];
			}

			t_CData &f_Insert()
			{
				aint PrevLen = f_GetLen();
				fp_MakeRoom(PrevLen + 1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray + PrevLen)) t_CData();
				return pArray[PrevLen];
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertLast(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Vector)
			{
				return f_Insert(_Vector);
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertLast(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Vector)
			{
				return f_Insert(_Vector);
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertLast(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Vector)
			{
				return f_Insert(_Vector);
			}

			t_CData *f_InsertLast(const t_CData *_pData, mint _Len = 1)
			{
				return f_Insert(_pData, _Len);
			}

			t_CData *f_InsertLast(t_CData *_pData, mint _Len = 1)
			{
				return f_Insert(_pData, _Len);
			}

			template <typename tf_CType, typename... tfp_CParams>
			t_CData &f_InsertLast(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
			{
				return f_Insert(fg_Move(_CreateParams));
			}

			t_CData &f_InsertLast(const t_CData &_Data)
			{
				return f_Insert(_Data);
			}

			t_CData &f_InsertLast(t_CData &_Data)
			{
				return f_Insert(_Data);
			}

			t_CData &f_InsertLast(t_CData &&_Data)
			{
				return f_Insert(_Data);
			}

			t_CData &f_InsertLast()
			{
				return f_Insert();
			}

			/***************************************************************************************************\
			|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
			| Insert first																						|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/


			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertFirst(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Vector)
			{
				mint AddLen = _Vector.f_GetLen();
				fp_MakeRoomBegin(AddLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther const *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_CopyArray(pArray, pSrcArray, AddLen);

				return pArray;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertFirst(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Vector)
			{
				mint AddLen = _Vector.f_GetLen();
				fp_MakeRoomBegin(AddLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_CopyArray(pArray, pSrcArray, AddLen);

				return pArray;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertFirst(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Vector)
			{
				mint AddLen = _Vector.f_GetLen();
				fp_MakeRoomBegin(AddLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_MoveArray(pArray, pSrcArray, AddLen);

				return pArray;
			}

			t_CData *f_InsertFirst(const t_CData *_pData, mint _Len = 1)
			{
				fp_MakeRoomBegin(_Len);
				t_CData *pArray = f_GetArray();

				NPrivate::fg_CopyArray(pArray, _pData, _Len);

				return pArray;
			}

			t_CData *f_InsertFirst(t_CData *_pData, mint _Len = 1)
			{
				fp_MakeRoomBegin(_Len);
				t_CData *pArray = f_GetArray();

				NPrivate::fg_CopyArray(pArray, _pData, _Len);

				return pArray;
			}

			t_CData &f_InsertFirst(const t_CData &_Data)
			{
				fp_MakeRoomBegin(1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray)) t_CData(_Data);
				return pArray[0];
			}

			t_CData &f_InsertFirst(t_CData &_Data)
			{
				fp_MakeRoomBegin(1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray)) t_CData(_Data);
				return pArray[0];
			}

			t_CData &f_InsertFirst(t_CData &&_Data)
			{
				fp_MakeRoomBegin(1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray)) t_CData(fg_Move(_Data));
				return pArray[0];
			}

			t_CData &f_InsertFirst()
			{
				fp_MakeRoomBegin(1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray)) t_CData();
				return pArray[0];
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_Push(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Vector)
			{
				return f_InsertFirst(_Vector);
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_Push(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Vector)
			{
				return f_InsertFirst(_Vector);
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_Push(TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Vector)
			{
				return f_InsertFirst(_Vector);
			}

			t_CData *f_Push(const t_CData *_pData, mint _Len = 1)
			{
				return f_InsertFirst(_pData, _Len);
			}

			t_CData *f_Push(t_CData *_pData, mint _Len = 1)
			{
				return f_InsertFirst(_pData, _Len);
			}

			t_CData &f_Push(const t_CData &_Data)
			{
				return f_InsertFirst(_Data);
			}

			t_CData &f_Push(t_CData &_Data)
			{
				return f_InsertFirst(_Data);
			}

			t_CData &f_Push(t_CData &&_Data)
			{
				return f_InsertFirst(_Data);
			}

			t_CData &f_Push()
			{
				return f_InsertFirst();
			}

			/***************************************************************************************************\
			|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
			| Insert before																						|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/


			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertBefore(mint _Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Vector)
			{
				mint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				mint AddLen = _Vector.f_GetLen();
				fp_MakeRoomMiddle(_Position, AddLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther const *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_CopyArray(pArray + _Position, pSrcArray, AddLen);

				return pArray + _Position;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertBefore(mint _Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Vector)
			{
				mint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				mint AddLen = _Vector.f_GetLen();
				fp_MakeRoomMiddle(_Position, AddLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_CopyArray(pArray + _Position, pSrcArray, AddLen);

				return pArray + _Position;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertBefore(mint _Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Vector)
			{
				mint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				mint AddLen = _Vector.f_GetLen();
				fp_MakeRoomMiddle(_Position, AddLen);
				t_CData *pArray = f_GetArray();
				t_CDataOther *pSrcArray = _Vector.f_GetArray();
				
				NPrivate::fg_MoveArray(pArray + _Position, pSrcArray, AddLen);

				return pArray + _Position;
			}

			t_CData *f_InsertBefore(mint _Position, const t_CData *_pData, mint _Len = 1)
			{
				aint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				fp_MakeRoomMiddle(_Position, _Len);
				t_CData *pArray = f_GetArray();

				NPrivate::fg_CopyArray(pArray + _Position, _pData, _Len);

				return pArray + _Position;
			}

			t_CData *f_InsertBefore(mint _Position, t_CData *_pData, mint _Len = 1)
			{
				aint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				fp_MakeRoomMiddle(_Position, _Len);
				t_CData *pArray = f_GetArray();

				NPrivate::fg_CopyArray(pArray + _Position, _pData, _Len);

				return pArray + _Position;
			}

			t_CData &f_InsertBefore(mint _Position, const t_CData &_Data)
			{
				aint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				fp_MakeRoomMiddle(_Position, 1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray + _Position)) t_CData(_Data);
				return pArray[_Position];
			}

			t_CData &f_InsertBefore(mint _Position, t_CData &_Data)
			{
				aint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				fp_MakeRoomMiddle(_Position, 1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray + _Position)) t_CData(_Data);
				return pArray[_Position];
			}

			t_CData &f_InsertBefore(mint _Position, t_CData &&_Data)
			{
				aint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				fp_MakeRoomMiddle(_Position, 1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray + _Position)) t_CData(fg_Move(_Data));
				return pArray[_Position];
			}

			t_CData &f_InsertBefore(mint _Position)
			{
				aint PrevLen = f_GetLen();
				t_CBoundsChecker::fs_CheckBounds(PrevLen+1, _Position);
				fp_MakeRoomMiddle(_Position, 1);
				t_CData *pArray = f_GetArray();
				new((void *)(pArray + _Position)) t_CData();
				return pArray[_Position];
			}

			template <typename t_CIterator, typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Vector)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position), _Vector);
			}

			template <typename t_CIterator, typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Vector)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position), _Vector);
			}

			template <typename t_CIterator, typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Vector)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position), _Vector);
			}

			template <typename t_CIterator>
			t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, const t_CData *_pData, mint _Len = 1)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position), _pData, _Len);
			}

			template <typename t_CIterator>
			t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, t_CData *_pData, mint _Len = 1)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position), _pData, _Len);
			}

			template <typename t_CIterator>
			t_CData &f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, const t_CData &_Data)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position), _Data);
			}

			template <typename t_CIterator>
			t_CData &f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, t_CData &_Data)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position), _Data);
			}

			template <typename t_CIterator>
			t_CData &f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, t_CData &&_Data)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position), _Data);
			}

			template <typename t_CIterator>
			t_CData &f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position)
			{
				return f_InsertBefore(fp_GetIteratorPos(_Position));
			}

			/***************************************************************************************************\
			|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
			| Insert after																						|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/



			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertAfter(mint _Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Vector)
			{
				return f_InsertBefore(_Position + 1, _Vector);
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertAfter(mint _Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Vector)
			{
				return f_InsertBefore(_Position + 1, _Vector);
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertAfter(mint _Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Vector)
			{
				return f_InsertBefore(_Position + 1, _Vector);
			}

			t_CData *f_InsertAfter(mint _Position, const t_CData *_pData, mint _Len = 1)
			{
				return f_InsertBefore(_Position + 1, _pData, _Len);
			}

			t_CData *f_InsertAfter(mint _Position, t_CData *_pData, mint _Len = 1)
			{
				return f_InsertBefore(_Position + 1, _pData, _Len);
			}

			t_CData &f_InsertAfter(mint _Position, const t_CData &_Data)
			{
				return f_InsertBefore(_Position + 1, _Data);
			}

			t_CData &f_InsertAfter(mint _Position, t_CData &_Data)
			{
				return f_InsertBefore(_Position + 1, _Data);
			}

			t_CData &f_InsertAfter(mint _Position, t_CData &&_Data)
			{
				return f_InsertBefore(_Position + 1, _Data);
			}

			t_CData &f_InsertAfter(mint _Position)
			{
				return f_InsertBefore(_Position + 1);
			}


			template <typename t_CIterator, typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Vector)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position), _Vector);
			}

			template <typename t_CIterator, typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Vector)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position), _Vector);
			}

			template <typename t_CIterator, typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			t_CData *f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position, TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Vector)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position), _Vector);
			}

			template <typename t_CIterator>
			t_CData *f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position, const t_CData *_pData, mint _Len = 1)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position), _pData, _Len);
			}

			template <typename t_CIterator>
			t_CData *f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position, t_CData *_pData, mint _Len = 1)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position), _pData, _Len);
			}

			template <typename t_CIterator>
			t_CData &f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position, const t_CData &_Data)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position), _Data);
			}

			template <typename t_CIterator>
			t_CData &f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position, t_CData &_Data)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position), _Data);
			}

			template <typename t_CIterator>
			t_CData &f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position, t_CData &&_Data)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position), _Data);
			}

			template <typename t_CIterator>
			t_CData &f_InsertAfter(TCVectorIterator<t_CIterator> const &_Position)
			{
				return f_InsertAfter(fp_GetIteratorPos(_Position));
			}

			/***************************************************************************************************\
			|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
			| Move																								|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/


			void f_Move(mint _FromPosition, mint _ToPosition, mint _Len = 1)
			{
				if (_ToPosition == _FromPosition || _Len == 0)
					return;

				mint OldLen = f_GetLen();

				t_CBoundsChecker::fs_CheckOverlapping(_FromPosition, _ToPosition, _Len, _Len);
				t_CBoundsChecker::fs_CheckBounds(OldLen+1, _FromPosition + _Len);
				t_CBoundsChecker::fs_CheckBounds(OldLen+1, _ToPosition);

				// TODO: Make inplace

				mint AllocSize = m_StaticData.f_GetAllocSize(OldLen);
				mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
				mint MaxObjects = fg_AlignDown(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), Granularity) / sizeof(t_CData);
				if (AllocSize >= MaxObjects)
					DMibErrorMemory("Out of memory trying to allocate objects in vector");
				mint Size = fg_AlignUp(AllocSize * sizeof(t_CData) + sizeof(CVectorData), Granularity);
				CVectorData *pNewData = fp_AllocData(Size);
				t_CData *pOldArray = f_GetArray();
				t_CData *pNewArray = pNewData->f_GetData();

				if (_FromPosition < _ToPosition)
				{
					NPrivate::fg_MoveArray(pNewArray, pOldArray, _FromPosition);

					mint MiddleCopyLen = _ToPosition - (_FromPosition + _Len);
					mint End = _FromPosition + MiddleCopyLen;
					if (End > _FromPosition)
						NPrivate::fg_MoveArray(pNewArray + _FromPosition, pOldArray + _FromPosition + _Len, (End - _FromPosition));

					mint End2 = End + _Len;
					if (End2 > End)
						NPrivate::fg_MoveArray(pNewArray + End, pOldArray + End - MiddleCopyLen, (End2 - End));

					if (OldLen > End2)
						NPrivate::fg_MoveArray(pNewArray + End2, pOldArray + End2, (OldLen - End2));
				}
				else
				{
					NPrivate::fg_MoveArray(pNewArray, pOldArray, (_ToPosition));

					mint Diff = _FromPosition - _ToPosition;
					mint End = _ToPosition + _Len;
					if (End > _ToPosition)
						NPrivate::fg_MoveArray(pNewArray + _ToPosition, pOldArray + Diff + _ToPosition, (End - _ToPosition));

					mint End2 = End + _FromPosition - _ToPosition;

					if (End2 > End)
						NPrivate::fg_MoveArray(pNewArray + End, pOldArray + End - _Len, (End2 - End));
					if (OldLen > End2)
						NPrivate::fg_MoveArray(pNewArray + End2, pOldArray + End2, (OldLen - End2));
				}

				NPrivate::fg_DestroyArray(pOldArray, OldLen);

				// Destroy old array
				if (m_StaticData.m_pData)
					fp_FreeData(m_StaticData.m_pData);

				// Save new array
				pNewData->f_SetLen(OldLen);
				pNewData->f_SetAllocSize(AllocSize);
				m_StaticData.m_pData = pNewData;
			}


			/***************************************************************************************************\
			|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
			| Pop																								|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/

			t_CData f_Pop()
			{
				t_CData Temp = fg_Move((*this)[0]);
				f_Remove(0);
				return Temp;
			}


			t_CData *f_AddArrayAtEnd(mint _Size)
			{
				aint PrevLen = f_GetLen();
				fp_MakeRoom(PrevLen + _Size);
				t_CData *pArray = f_GetArray();
				for (mint i = 0; i < _Size; ++i)
					new((void *)(pArray + PrevLen + i)) t_CData();
				return pArray + PrevLen;
			}

			template <typename t_CType2>
			aint f_Contains(const t_CType2 &_Data) const
			{
				aint Len = f_GetLen();
				const t_CData *pArray = f_GetArray();

				for (int i = 0; i < Len; ++i)
				{
					if (pArray[i] == _Data)
						return i;
				}
				return -1;
			}

			void f_GetAt(aint nIndex, t_CData &_Data)
			{
				aint Len = f_GetLen();
				t_CData *pArray = f_GetArray();

				for (int i = 0; i < Len; ++i)
				{
					if (i == nIndex)
					{
						_Data = pArray[i]; 
						return;
					}
				}
			}

		/*	operator t_CData *()
			{
				return f_GetArray();
			}*/


			bint f_IsPosValid(aint _Index) const
			{
				if (_Index < 0)
					return false;
				if (_Index >= (aint)f_GetLen())
					return false;
				return true;
			}

			inline_small t_CData &operator [](mint _Index)
			{
				t_CBoundsChecker::fs_CheckBounds(f_GetLen(), _Index);
				return (m_StaticData.m_pData->f_GetData())[_Index];
			}

			inline_small const t_CData &operator [](mint _Index) const
			{
				t_CBoundsChecker::fs_CheckBounds(f_GetLen(), _Index);
				return (m_StaticData.m_pData->f_GetData())[_Index];
			}
			
			void f_DeleteAll()
			{
				mint Len = f_GetLen();
				t_CData *pArray = f_GetArray();
				for (mint i = 0; i < Len; ++i)
				{
					delete pArray[i];
				}
				f_Clear();
			}

			template <typename t_CAllocator2, typename t_CData2>
			static void fsp_DeleteAllAllocatorHelper(t_CData2 *_pData)
			{
				fg_DeleteObject(t_CAllocator2(), _pData);
			}
				
			template <typename t_CAllocator2>
			void f_DeleteAllAllocator()
			{
				mint Len = f_GetLen();
				t_CData *pArray = f_GetArray();
				for (mint i = 0; i < Len; ++i)
				{
					fsp_DeleteAllAllocatorHelper<t_CAllocator2>(pArray[i]);
				}
				f_Clear();
			}

			template <typename t_CDeleter>
			void f_DeleteAllDeleter()
			{
				mint Len = f_GetLen();
				t_CData *pArray = f_GetArray();
				for (mint i = 0; i < Len; ++i)
				{
					t_CDeleter::fs_Delete(pArray[i]);
				}
				f_Clear();
			}

			template <typename t_CData2>
			void fp_DeleteAllDeleterHelper(t_CData2 *_pData)
			{
				f_DeleteAllDeleter<t_CData2>();
			}

			void f_DeleteAllDeleter()
			{
				fp_DeleteAllDeleterHelper((t_CData)0);
			}

			void f_Clear()
			{	
				if (m_StaticData.m_pData)
				{
					NPrivate::fg_DestroyArray(m_StaticData.m_pData->f_GetData(), f_GetLen(), (CVectorData *)m_StaticData.m_pData);
					fp_FreeData(m_StaticData.m_pData);
					m_StaticData.m_pData = nullptr;
				}
			}

			void f_ClearNoTrim()
			{	
				if (m_StaticData.m_pData)
				{
					NPrivate::fg_DestroyArray(m_StaticData.m_pData->f_GetData(), f_GetLen(), (CVectorData *)m_StaticData.m_pData);
					
					m_StaticData.m_pData->f_SetLen(0);
				}
			}

			void f_SetAtLeastLen(mint _Len, mint _Grow = 4096)
			{
				if (f_GetLen() < _Len)
					f_SetLen(_Len + _Grow, false);
			}

			template <typename t_CSorter>
			void f_Sort(t_CSorter &&_Sorter)
			{
				fpr_QuickSort(f_GetArray(), 0, f_GetLen()-1, fg_Forward<t_CSorter>(_Sorter));
			}

			void f_Sort()
			{
				f_Sort(CSort_Default());
			}

			template <typename t_CSorter>
			bint f_IsSorted(t_CSorter &&_Sorter) const
			{
				aint len = f_GetLen();
				if(len<2) return 1;
				t_CData const*pArray = f_GetArray();
				for(aint i=0,j=1;j!=len;++i,++j)
					if(fg_Forward<t_CSorter>(_Sorter)(pArray[j], pArray[i])) 
						return 0;
				return 1;
			}
			bint f_IsAdjacentUnique() const
			{
				return f_IsSorted(CSort_Default());
			}
			template <typename t_CSorter>
			bint f_IsSortedNoDuplicates(t_CSorter &&_Sorter) const
			{
				aint len = f_GetLen();
				if(len<2) return 1;
				t_CData const*pArray = f_GetArray();
				for(aint i=0,j=1;j!=len;++i,++j)
					if(!fg_Forward<t_CSorter>(_Sorter)(pArray[i],pArray[j])) 
						return 0;
				return 1;
			}
			bint f_IsSortedNoDuplicates() const
			{
				return f_IsSortedNoDuplicates(CSort_Default());
			}

			template <typename t_CSorter, typename t_CFind>
			aint f_BinarySearch(t_CSorter &&_Sorter, const t_CFind &_ToFind, aint _nMax = -1) const
			{
				mint Len = f_GetLen();
				if (_nMax >= 0)
					Len = fg_Min(mint(_nMax), Len);
				mint Low = 0;
				mint High = Len;
				t_CData const*pArray = f_GetArray();

				while(Low < High)
				{
					mint Mid = (Low + High) >> 1;
					if(fg_Forward<t_CSorter>(_Sorter)(pArray[Mid], _ToFind))
						Low = Mid + 1;
					else
						High = Mid;
				}
				if(Low < Len && !fg_Forward<t_CSorter>(_Sorter)(pArray[Low], _ToFind) && !fg_Forward<t_CSorter>(_Sorter)(_ToFind, pArray[Low]))
					return Low;
				else
					return -1;
			}

			template <typename t_CFind>
			aint f_BinarySearch(const t_CFind &_ToFind) const
			{
				return f_BinarySearch(CSort_Default(), _ToFind, -1);
			}

			template <typename t_CSorter, typename t_CFind>
			aint f_BinarySearchLowerBound(t_CSorter &&_Sorter, const t_CFind &_ToFind) const
			{						
				mint Low = 0;
				mint High = f_GetLen();
				
				if (High == 0)
					return -1;

				while(Low < High)
				{
					mint Mid = (Low + High) >> 1;
					if(fg_Forward<t_CSorter>(_Sorter)( operator[](Mid) , _ToFind))
						Low = Mid + 1;
					else
						High = Mid;
				}

				return Low;
			}			

			mint f_Grow(mint _MinLen)
			{
				mint NewLen = _MinLen;
				mint OldLen = f_GetLen();
				if (m_StaticData.f_NeedRealloc(OldLen, NewLen, m_StaticData.m_pData))
				{
					mint AllocSize = m_StaticData.f_GetAllocSize(NewLen);
					NewLen = AllocSize;
					mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
					mint MaxObjects = fg_AlignDown(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), Granularity) / sizeof(t_CData);
					if (AllocSize >= MaxObjects)
						DMibErrorMemory("Out of memory trying to allocate objects in vector");
					mint Size = fg_AlignUp(AllocSize * sizeof(t_CData) + sizeof(CVectorData), Granularity);
					CVectorData *pNewData = fp_AllocData(Size);
					t_CData *pOldArray = f_GetArray();
					t_CData *pNewArray = pNewData->f_GetData();
					mint CurrentLength = OldLen;

					mint OldRemainingCells = fg_Min(NewLen, CurrentLength);

					// Construct all new datas with copy constructor from old datas
					NPrivate::fg_MoveArray(pNewArray, pOldArray, OldRemainingCells);

					NPrivate::fg_DestroyArray(pOldArray, CurrentLength);

					// Destroy old array
					if (m_StaticData.m_pData)
						fp_FreeData(m_StaticData.m_pData);

					// Save new array
					pNewData->f_SetLen(NewLen);
					pNewData->f_SetAllocSize(AllocSize);
					m_StaticData.m_pData = pNewData;
				}
				else
				{
					NewLen = m_StaticData.m_pData->f_GetAllocSize(OldLen);
					m_StaticData.m_pData->f_SetLen(NewLen);
				}

				if (NewLen > OldLen)
					NPrivate::fg_ConstructArray(f_GetArray() + OldLen, NewLen - OldLen);

				return NewLen;
			}

			void f_SetLen(mint _Len, bint _bTrim = true)
			{
				mint OldLen = f_GetLen();

				if (OldLen == _Len)
					return;

				mint NewLen = _Len;

				if (NewLen == 0)
					return f_Clear();

				if (_bTrim || m_StaticData.f_NeedRealloc(OldLen, NewLen, m_StaticData.m_pData))
				{
					mint AllocSize = _bTrim ? NewLen : m_StaticData.f_GetAllocSize(NewLen);
					mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
					mint MaxObjects = fg_AlignDown(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), Granularity) / sizeof(t_CData);
					if (AllocSize >= MaxObjects)
						DMibErrorMemory("Out of memory trying to allocate objects in vector");
					mint Size = fg_AlignUp(AllocSize * sizeof(t_CData) + sizeof(CVectorData), Granularity);
					CVectorData *pNewData = fp_AllocData(Size);
					t_CData *pOldArray = f_GetArray();
					t_CData *pNewArray = pNewData->f_GetData();
					mint CurrentLength = OldLen;

					mint OldRemainingCells = fg_Min(NewLen, CurrentLength);

					// Construct all new datas with copy constructor from old datas
					
					NPrivate::fg_MoveArray(pNewArray, pOldArray, (OldRemainingCells));

					NPrivate::fg_DestroyArray(pOldArray, CurrentLength);

					// Construct all new datas
					if (NewLen > OldRemainingCells)
						NPrivate::fg_ConstructArray(pNewArray + OldRemainingCells, NewLen - OldRemainingCells);

					// Destroy old array
					if (m_StaticData.m_pData)
					{
						fp_FreeData(m_StaticData.m_pData);
					}

					// Save new array
					pNewData->f_SetLen(NewLen);
					pNewData->f_SetAllocSize(AllocSize);
					m_StaticData.m_pData = pNewData;
				}
				else
				{
					t_CData *pOldArray = f_GetArray();

					if (OldLen > NewLen)
						NPrivate::fg_DestroyArray(pOldArray + NewLen, OldLen - NewLen);

					// Construct new data
					if (NewLen > OldLen)
						NPrivate::fg_ConstructArray(pOldArray + OldLen, NewLen - OldLen);

					m_StaticData.m_pData->f_SetLen(NewLen);
				}

			}

			// If you send 0 as _Len the remaining part of the list will be copied
			void f_Replace(const TCVector &_Source, const mint _StartInSource = 0, const mint _Len = 0, const mint _StartInList = 0, const mint _MinLen = 0)
			{
				mint Len;
				if (!_Len)
				{
					Len = _Source.f_GetLen() - _StartInSource;
				}
				else
					Len = _Len;

				DMibSafeCheck(_StartInSource + Len <= _Source.f_GetLen(), "Attempting to copy to much from source array");

				mint NewLen = fg_Max(fg_Max(_StartInList + Len, f_GetLen()), _MinLen);

				CVectorData *pNewData;
				if (NewLen != f_GetLen())
				{
					// New list
					mint Granularity = m_StaticData.f_Allocator().f_GranularityAlloc();
					mint MaxObjects = fg_AlignDown(TCLimitsInt<mint>::mc_Max - sizeof(CVectorData), Granularity) / sizeof(t_CData);
					if (NewLen >= MaxObjects)
						DMibErrorMemory("Out of memory trying to allocate objects in vector");

					pNewData = fp_AllocData(fg_AlignUp(NewLen * sizeof(t_CData) + sizeof(CVectorData), NewLen));
					t_CData *pOldArray = f_GetArray();
					const t_CData *pSourceArray = _Source.f_GetArray();
					t_CData *pNewArray = pNewData->f_GetData();
					mint CurrentLength = f_GetLen();

					mint OldRemainingCells = fg_Min(CurrentLength, _StartInList);

					// Construct all new datas with copy constructor from old datas
					NPrivate::fg_MoveArray(pNewArray, pOldArray, (OldRemainingCells));

					if (_StartInList > OldRemainingCells)
						NPrivate::fg_ConstructArray(pNewArray + OldRemainingCells, _StartInList - OldRemainingCells);

					// Construct all new datas with copy constructor from source datas
					NPrivate::fg_CopyArray(pNewArray + _StartInList, pSourceArray + _StartInSource, Len);

					// Copy the rest of the datas
					if (CurrentLength > _StartInList + Len)
						NPrivate::fg_MoveArray(pNewArray + _StartInList + Len, pOldArray + _StartInList + Len, (CurrentLength - (_StartInList + Len)));

					mint MaxLen = fg_Max(_StartInList + Len, CurrentLength);

					if (NewLen > MaxLen)
						NPrivate::fg_ConstructArray(pNewArray + MaxLen, NewLen - MaxLen);

					// Destroy old array
					if (m_StaticData.m_pData)
					{
						NPrivate::fg_DestroyArray(pOldArray, CurrentLength);

						fp_FreeData(m_StaticData.m_pData);
					}

					// Save new array
					m_StaticData.m_pData = pNewData;
					m_StaticData.m_pData->f_SetLen(NewLen);
					m_StaticData.m_pData->f_SetAllocSize(NewLen);
				}
				else
				{
					// We can fit everything in the old array

					t_CData *pOldArray = f_GetArray();
					const t_CData *pSourceArray = _Source.f_GetArray();

					DMibSafeCheck((_StartInList + NewLen) == f_GetLen(), "Must align");

					fg_DestroyCopyArray(pOldArray + _StartInList, pSourceArray + _StartInSource - _StartInList, Len);
				}
			}

			void f_Remove(mint _Start, mint _Len = 1)
			{
				if (_Len == 0)
					return;
				mint CurrentLen = f_GetLen();

				t_CBoundsChecker::fs_CheckBounds(CurrentLen, _Start);
				t_CBoundsChecker::fs_CheckBounds(CurrentLen, _Start+_Len-1);

				_Start = fg_Min(_Start, CurrentLen-1);
				_Len = fg_Min(CurrentLen - _Start, _Len);

				mint NewLen = CurrentLen-_Len;
				if (NewLen == 0)
					return f_Clear();

				CVectorData *pNewData;
				if (NewLen != CurrentLen)
				{
					if (m_StaticData.f_NeedRealloc(CurrentLen, NewLen, m_StaticData.m_pData))
					{
						// New list
						mint AllocSize = m_StaticData.f_GetAllocSize(NewLen);
						pNewData = fp_AllocData(fg_AlignUp(AllocSize * sizeof(t_CData) + sizeof(CVectorData), m_StaticData.f_Allocator().f_GranularityAlloc()));

						t_CData *pOldArray = f_GetArray();
						t_CData *pNewArray = pNewData->f_GetData();

						// Construct all new datas with copy constructor from old datas
						NPrivate::fg_MoveArray(pNewArray, pOldArray, _Start);
						if (NewLen > _Start)
							NPrivate::fg_MoveArray(pNewArray + _Start, pOldArray + _Start + _Len, (NewLen - _Start));

						NPrivate::fg_DestroyArray(pOldArray, CurrentLen);

						// Destroy old array
						if (m_StaticData.m_pData)
							fp_FreeData(m_StaticData.m_pData);

						// Save new array
						pNewData->f_SetLen(NewLen);
						pNewData->f_SetAllocSize(AllocSize);
						m_StaticData.m_pData = pNewData;
					}
					else
					{
						t_CData *pOldArray = f_GetArray();

						// Destroy All datas that are to be destroyed
						NPrivate::fg_DestroyArray(pOldArray + _Start, _Len);

						if (NewLen > _Start)
							NPrivate::fg_MoveDestroyOverlappingArray(pOldArray + _Start, pOldArray + _Start + _Len, (NewLen - _Start));

						m_StaticData.m_pData->f_SetLen(NewLen);
					}
				}
			}

		/*	void f_Insert(const TCVector&_Source, mint _StartInSource = 0, mint _Len = 0, mint _StartInList = 0, mint _MinLen = 0)
			{

			}*/

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			TCVector &operator = (TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Source)
			{
				mint nSource = _Source.f_GetLen();
				fp_MakeNewRoom(nSource);
				auto pArray = f_GetArray();
				auto pSrcArray = _Source.f_GetArray();

				NPrivate::fg_CopyArray(pArray, pSrcArray, nSource);
				return *this;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			TCVector &operator = (TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &_Source)
			{
				mint nSource = _Source.f_GetLen();
				fp_MakeNewRoom(nSource);
				auto pArray = f_GetArray();
				auto pSrcArray = _Source.f_GetArray();

				NPrivate::fg_CopyArray(pArray, pSrcArray, nSource);

				return *this;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			TCVector &operator = (TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> &&_Source)
			{
				mint nSource = _Source.f_GetLen();
				fp_MakeNewRoom(nSource);
				auto pArray = f_GetArray();
				auto pSrcArray = _Source.f_GetArray();

				NPrivate::fg_MoveArray(pArray, pSrcArray, nSource);

				return *this;
			}

			TCVector &operator = (TCVector const &_Source)
			{
				fp_Copy(_Source);
				return *this;
			}

			TCVector &operator = (TCVector &_Source)
			{
				fp_Copy(_Source);
				return *this;
			}

			TCVector &operator = (TCVector &&_Source) throw()
			{
				f_Clear();
				fp_MoveCopy<t_CAllocator::mc_CanBeStatic>(fg_Move(_Source));
				return *this;
			}

			template <
				typename t_CDataOther0, typename t_CAllocatorOther0, typename t_CBoundsCheckerOther0, typename t_CInternalDataOther0, typename t_CStaticDataOther0
				,typename t_CDataOther1, typename t_CAllocatorOther1, typename t_CBoundsCheckerOther1, typename t_CInternalDataOther1, typename t_CStaticDataOther1
				,typename t_CDataOther2, typename t_CAllocatorOther2, typename t_CBoundsCheckerOther2, typename t_CInternalDataOther2, typename t_CStaticDataOther2
			>
			void f_FindDifferences
			(
				const TCVector<t_CDataOther0, t_CAllocatorOther0, t_CBoundsCheckerOther0, t_CInternalDataOther0, t_CStaticDataOther0> &_CompareTo
				, TCVector<t_CDataOther1, t_CAllocatorOther1, t_CBoundsCheckerOther1, t_CInternalDataOther1, t_CStaticDataOther1> *_pAdded
				, TCVector<t_CDataOther2, t_CAllocatorOther2, t_CBoundsCheckerOther2, t_CInternalDataOther2, t_CStaticDataOther2> *_pRemoved) const
			{
				// Find added
				if (_pAdded)
				{
					TCVector Sorted = *this;
					Sorted.f_Sort();
					mint nItems = _CompareTo.f_GetLen();
					for (mint i = 0; i < nItems; ++i)
					{
						const t_CDataOther0 &CompareTo = _CompareTo[i];
						if (Sorted.f_BinarySearch(CompareTo) < 0)
							_pAdded->f_Insert(CompareTo);
					}
				}

				// Find removed
				if (_pRemoved)
				{
					TCVector<t_CDataOther0> Sorted = _CompareTo;
					Sorted.f_Sort();
					mint nItems = f_GetLen();
					for (mint i = 0; i < nItems; ++i)
					{
						const t_CData &CompareTo = (*this)[i];
						if (Sorted.f_BinarySearch(CompareTo) < 0)
							_pRemoved->f_Insert(CompareTo);
					}
				}
			}

			TCVector f_Reverse() const
			{
				TCVector Ret;
				mint Len = f_GetLen();

				Ret.fp_MakeNewRoom(Len);
				auto pDst = Ret.f_GetArray();
				auto pSrc = f_GetArray();

				for (mint i = 0; i < Len; ++i)
				{
					new((void *)(pDst + i)) t_CData(pSrc[Len - i - 1]);
				}

				return Ret;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			bint operator == (TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Other) const
			{
				mint Len = f_GetLen();
				if (Len != _Other.f_GetLen())
					return false;

				auto pThisArray = f_GetArray();
				auto pOtherArray = _Other.f_GetArray();

				for (mint i = 0; i < Len; ++i)
				{
					if (pThisArray[i] != pOtherArray[i])
						return false;
				}

				return true;
			}

			template <typename t_CDataOther, typename t_CAllocatorOther, typename t_CBoundsCheckerOther, typename t_CInternalDataOther, typename t_CStaticDataOther>
			bint operator < (TCVector<t_CDataOther, t_CAllocatorOther, t_CBoundsCheckerOther, t_CInternalDataOther, t_CStaticDataOther> const &_Other) const
			{
				mint Len = f_GetLen();
				mint OtherLen = _Other.f_GetLen();

				if (Len < OtherLen)
					return true;
				else if (Len > OtherLen)
					return false;

				auto pThisArray = f_GetArray();
				auto pOtherArray = _Other.f_GetArray();

				for (mint i = 0; i < Len; ++i)
				{
					auto &Left = pThisArray[i];
					auto &Right = pOtherArray[i];

					if (Left < Right)
						return true;
					else if (Left > Right)
						return false;
				}

				return false;
			}

			struct CFormatOptions
			{
				zbool m_bSingleLine;
			};
			
			template <typename tf_COption>
			bool f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option) const;
			template <typename tf_CFormatInto, typename tf_CFormatOptions>
			void f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const;
		};

		template <typename tf_CReturn, typename... tf_CParams>
		void fg_CreateVectorHelper(TCVector<tf_CReturn> &_Return)
		{
		}

		template <typename tf_CReturn, typename tf_CFirst, typename... tf_CParams>
		void fg_CreateVectorHelper(TCVector<tf_CReturn> &_Return, tf_CFirst &&_First, tf_CParams && ..._Params)
		{
			_Return.f_Insert(fg_Construct(fg_Forward<tf_CFirst>(_First)));
			fg_CreateVectorHelper<tf_CReturn>(_Return, fg_Forward<tf_CParams>(_Params)...);
		}

		template <typename tf_CFirst, typename... tf_CParams>
		TCVector<typename NTraits::TCRemoveReference<tf_CFirst>::CType> fg_CreateVector(tf_CFirst && _First, tf_CParams && ..._Params)
		{
			TCVector<typename NTraits::TCRemoveReference<tf_CFirst>::CType> Return;
			fg_CreateVectorHelper<typename NTraits::TCRemoveReference<tf_CFirst>::CType>(Return, fg_Forward<tf_CParams>(_Params)...);
			return Return;
		}

		template <typename tf_CReturn, typename... tf_CParams>
		TCVector<tf_CReturn> fg_CreateVector(tf_CParams && ..._Params)
		{
			TCVector<tf_CReturn> Return;
			fg_CreateVectorHelper<tf_CReturn>(Return, fg_Forward<tf_CParams>(_Params)...);
			return Return;
		}
	}

	namespace NStream
	{
		template <typename t_CStream, typename t_CData, typename t_CAllocator, typename t_CBoundsChecker, typename t_CInternalData, typename t_CStaticData>
		class TCBinaryStreamTypeReference<t_CStream, NContainer::TCVector<t_CData, t_CAllocator, t_CBoundsChecker, t_CInternalData, t_CStaticData> >
		{
		public:
			static void fs_Feed(t_CStream &_Stream, NContainer::TCVector<t_CData, t_CAllocator, t_CBoundsChecker, t_CInternalData, t_CStaticData> const &_Data)
			{
				mint nItems = _Data.f_GetLen();
				
				fg_FeedLenToStream(_Stream, nItems);

				for (mint i = 0; i < nItems; ++i)
				{
					_Stream << _Data[i];
				};
			}
	
			static void fs_Consume(t_CStream &_Stream, NContainer::TCVector<t_CData, t_CAllocator, t_CBoundsChecker, t_CInternalData, t_CStaticData> &_Data)
			{
				uint64 nItems;
				fg_ConsumeLenFromStream(_Stream, nItems);
				_Data.f_SetLen(nItems);

				for (mint i = 0; i < nItems; ++i)
				{
					_Stream >> _Data[i];
				};
			}
		};

		// Optimize for simple data cases

#define DMibListVectorStreamSimpleImplement(_DataType) \
		template <typename t_CStream, typename t_CAllocator, typename t_CBoundsChecker, typename t_CInternalData, typename t_CStaticData>\
		class TCBinaryStreamTypeReference<t_CStream, NContainer::TCVector<_DataType, t_CAllocator, t_CBoundsChecker, t_CInternalData, t_CStaticData> >\
		{\
		public:\
			static void fs_Feed(t_CStream &_Stream, NContainer::TCVector<_DataType, t_CAllocator, t_CBoundsChecker, t_CInternalData, t_CStaticData> const &_Data)\
			{\
				mint nItems = _Data.f_GetLen();\
				fg_FeedLenToStream(_Stream, nItems);\
				if (sizeof(_DataType) != 1)\
					fg_FeedEndianArrayToStream(_Stream, _Data.f_GetArray(), nItems, _Stream.f_Endian());\
				else\
					_Stream.f_FeedBytes(_Data.f_GetArray(), nItems * sizeof(_DataType));\
			}\
			static void fs_Consume(t_CStream &_Stream, NContainer::TCVector<_DataType, t_CAllocator, t_CBoundsChecker, t_CInternalData, t_CStaticData> &_Data)\
			{\
				uint64 nItems;\
				fg_ConsumeLenFromStream(_Stream, nItems);\
				_Data.f_SetLen(nItems);\
				_Stream.f_ConsumeBytes(_Data.f_GetArray(), nItems * sizeof(_DataType));\
				if (sizeof(_DataType) != 1)\
					fg_ByteSwapArray(_Stream, _Data.f_GetArray(), nItems, _Stream.f_Endian());\
			}\
		};

		DMibListVectorStreamSimpleImplement(int8);
		DMibListVectorStreamSimpleImplement(uint8);
		DMibListVectorStreamSimpleImplement(int16);
		DMibListVectorStreamSimpleImplement(uint16);
		DMibListVectorStreamSimpleImplement(int32);
		DMibListVectorStreamSimpleImplement(uint32);
		DMibListVectorStreamSimpleImplement(int64);
		DMibListVectorStreamSimpleImplement(uint64);
		DMibListVectorStreamSimpleImplement(fp32);
		DMibListVectorStreamSimpleImplement(fp64);

#ifdef DMibPUniqueType_ch8
		DMibListVectorStreamSimpleImplement(ch8);
#endif
#ifdef DMibPUniqueType_ch16
		DMibListVectorStreamSimpleImplement(ch16);
#endif
#ifdef DMibPUniqueType_ch32
		DMibListVectorStreamSimpleImplement(ch32);
#endif
	}

};
