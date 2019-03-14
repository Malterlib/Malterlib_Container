// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Core/Core>

#include "Malterlib_Container_Vector_Iterator.h"

namespace NMib::NContainer
{
	template <mint t_MinSize, bool t_bShrink, bool t_bCheckBounds>
	struct TCVectorOptions
	{
		static constexpr mint mc_MinSize = t_MinSize;
		static constexpr bool mc_bShrink = t_bShrink;
		static constexpr bool mc_bCheckBounds = t_bCheckBounds;
	};

	struct CVectorOptionsDefault
	{
		static constexpr mint mc_MinSize = 16;
		static constexpr bool mc_bShrink = true;
		static constexpr bool mc_bCheckBounds = true;
	};

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	class TCVector
	{
	public:
		typedef t_CData CData;

		typedef TCVectorIterator<t_CData> CIterator;
		typedef TCVectorIterator<t_CData const> CIteratorConst;

	public:
		TCVector() noexcept;
		~TCVector();

		TCVector(TCVector const &_Source);
		TCVector(TCVector &_Source);
		TCVector(TCVector &&_Source);

		TCVector(TCInitializerList<t_CData> const &_Values);
		TCVector(t_CData const *_pItems, mint _nItems);

		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		TCVector(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Source);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		TCVector(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Source);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		TCVector(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Source);

		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		TCVector &operator = (TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Source);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		TCVector &operator = (TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Source);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		TCVector &operator = (TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Source);

		TCVector &operator = (TCVector const &_Source);
		TCVector &operator = (TCVector &_Source);
		TCVector &operator = (TCVector &&_Source);

		CIterator f_GetIterator();
		CIteratorConst f_GetIterator() const;

		t_CData *f_GetArray();
		t_CData const *f_GetArray() const;
		t_CData *f_GetArray(mint _Len);

		t_CData &f_GetFirst();
		t_CData const &f_GetFirst() const;
		t_CData &f_GetLast();
		t_CData const &f_GetLast() const;

		mint f_GetLen() const;
		bool f_IsEmpty() const;

		mint f_GetArrayAllocSize() const;

		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		bool operator == (TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other) const;
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		bool operator < (TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other) const;

		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions, typename t_CFunctor>
		aint f_Compare(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other, t_CFunctor &&_Functor) const;
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		aint f_Compare(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other) const;
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		aint f_CompareLexicographical(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other) const;

		template
		<
			typename tf_CData0
			, typename tf_CAllocator0
			, typename tf_COptions0
			, typename tf_CData1
			, typename tf_CAllocator1
			, typename tf_COptions1
			, typename tf_CData2
			, typename tf_CAllocator2
			, typename tf_COptions2
		>
		void f_FindDifferences
			(
				const TCVector<tf_CData0, tf_CAllocator0, tf_COptions0> &_CompareTo
				, TCVector<tf_CData1, tf_CAllocator1, tf_COptions1> *_pAdded
				, TCVector<tf_CData2, tf_CAllocator2, tf_COptions2> *_pRemoved
			) const
		;

		/// Insert last
		
		template <typename tf_CData>
		TCVector &operator << (tf_CData &&_Data);

		t_CData &f_Insert();
		t_CData &f_Insert(const t_CData &_Data);
		inline_small t_CData &f_Insert(t_CData &_Data);
		t_CData &f_Insert(t_CData &&_Data);

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams);

		t_CData *f_Insert(TCInitializerList<t_CData> const &_Elements);

		t_CData *f_Insert(const t_CData *_pData, mint _Len = 1);
		t_CData *f_Insert(t_CData *_pData, mint _Len = 1);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_Insert(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_Insert(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_Insert(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector);

		t_CData &f_InsertLast();
		t_CData &f_InsertLast(const t_CData &_Data);
		t_CData &f_InsertLast(t_CData &_Data);
		t_CData &f_InsertLast(t_CData &&_Data);

		template <typename tf_CType, typename... tfp_CParams>
		t_CData &f_InsertLast(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams);

		t_CData *f_InsertLast(const t_CData *_pData, mint _Len = 1);
		t_CData *f_InsertLast(t_CData *_pData, mint _Len = 1);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertLast(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertLast(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertLast(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector);

		t_CData *f_AddArrayAtEnd(mint _Size);

		// Insert first

		t_CData &f_InsertFirst();
		t_CData &f_InsertFirst(const t_CData &_Data);
		t_CData &f_InsertFirst(t_CData &_Data);
		t_CData &f_InsertFirst(t_CData &&_Data);

		t_CData *f_InsertFirst(const t_CData *_pData, mint _Len = 1);
		t_CData *f_InsertFirst(t_CData *_pData, mint _Len = 1);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertFirst(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertFirst(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertFirst(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector);

		t_CData &f_Push();
		t_CData &f_Push(const t_CData &_Data);
		t_CData &f_Push(t_CData &_Data);
		t_CData &f_Push(t_CData &&_Data);

		t_CData *f_Push(const t_CData *_pData, mint _Len = 1);
		t_CData *f_Push(t_CData *_pData, mint _Len = 1);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_Push(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_Push(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_Push(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector);

		// Insert before

		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertBefore(mint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertBefore(mint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertBefore(mint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector);
		t_CData *f_InsertBefore(mint _Position, const t_CData *_pData, mint _Len = 1);
		t_CData *f_InsertBefore(mint _Position, t_CData *_pData, mint _Len = 1);
		t_CData &f_InsertBefore(mint _Position, const t_CData &_Data);
		t_CData &f_InsertBefore(mint _Position, t_CData &_Data);
		t_CData &f_InsertBefore(mint _Position, t_CData &&_Data);
		t_CData &f_InsertBefore(mint _Position);

		template <typename t_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector);
		template <typename t_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector);
		template <typename t_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector);
		template <typename t_CIterator>
		t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, const t_CData *_pData, mint _Len = 1);
		template <typename t_CIterator>
		t_CData *f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, t_CData *_pData, mint _Len = 1);
		template <typename t_CIterator>
		t_CData &f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, const t_CData &_Data);
		template <typename t_CIterator>
		t_CData &f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, t_CData &_Data);
		template <typename t_CIterator>
		t_CData &f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position, t_CData &&_Data);
		template <typename t_CIterator>
		t_CData &f_InsertBefore(TCVectorIterator<t_CIterator> const &_Position);

		// Insert after

		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertAfter(mint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertAfter(mint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector);
		template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertAfter(mint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector);
		t_CData *f_InsertAfter(mint _Position, const t_CData *_pData, mint _Len = 1);
		t_CData *f_InsertAfter(mint _Position, t_CData *_pData, mint _Len = 1);
		t_CData &f_InsertAfter(mint _Position, const t_CData &_Data);
		t_CData &f_InsertAfter(mint _Position, t_CData &_Data);
		t_CData &f_InsertAfter(mint _Position, t_CData &&_Data);
		t_CData &f_InsertAfter(mint _Position);

		template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector);
		template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector);
		template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
		t_CData *f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector);
		template <typename tf_CIterator>
		t_CData *f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, const t_CData *_pData, mint _Len = 1);
		template <typename tf_CIterator>
		t_CData *f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, t_CData *_pData, mint _Len = 1);
		template <typename tf_CIterator>
		t_CData &f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, const t_CData &_Data);
		template <typename tf_CIterator>
		t_CData &f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, t_CData &_Data);
		template <typename tf_CIterator>
		t_CData &f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, t_CData &&_Data);
		template <typename tf_CIterator>
		t_CData &f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position);

		// Other

		void f_Remove(mint _Start, mint _Len = 1);
		void f_Move(mint _FromPosition, mint _ToPosition, mint _Len = 1);
		void f_Replace(const TCVector &_Source, const mint _StartInSource = 0, const mint _Len = 0, const mint _StartInList = 0, const mint _MinLen = 0);

		t_CData f_Pop();
		t_CData f_PopBack();

		bool f_IsPosValid(aint _Index) const;
		inline_small t_CData &operator [](mint _Index);
		inline_small const t_CData &operator [](mint _Index) const;

		void f_Clear();
		void f_ClearNoTrim();

		void f_DeleteAll();
		void f_DeleteAllDefiniteType();
		template <typename tf_CAllocator2>
		void f_DeleteAllAllocator();
		template <typename tf_CAllocator2>
		void f_DeleteAllAllocatorDefiniteType();
		template <typename tf_CDeleter>
		void f_DeleteAllDeleter();
		void f_DeleteAllDeleter();

		template <typename tf_CSorter>
		void f_Sort(tf_CSorter &&_Sorter);
		void f_Sort();
		template <typename tf_CSorter>
		bool f_IsSorted(tf_CSorter &&_Sorter) const;
		bool f_IsAdjacentUnique() const;
		template <typename tf_CSorter>
		bool f_IsSortedNoDuplicates(tf_CSorter &&_Sorter) const;
		bool f_IsSortedNoDuplicates() const;

		template <typename tf_CSorter>
		void f_UniqueIfSorted(tf_CSorter &&_Sorter);
		void f_UniqueIfSorted();

		template <typename tf_CType>
		aint f_Contains(tf_CType const &_Data) const;
		template <typename tf_FUnaryPredicate>
		aint f_ContainsIf(tf_FUnaryPredicate &&_fPredicate) const;
		template <typename tf_CSorter, typename t_CFind>
		aint f_BinarySearch(tf_CSorter &&_Sorter, t_CFind const &_ToFind, aint _nMax = -1) const;
		template <typename tf_CFind>
		aint f_BinarySearch(tf_CFind const &_ToFind) const;
		template <typename tf_CSorter, typename tf_CFind>
		aint f_BinarySearchLowerBound(tf_CSorter &&_Sorter, tf_CFind const &_ToFind) const;

		mint f_Grow(mint _MinLen);
		void f_Reserve(mint _Space);
		void f_SetLen(mint _Len, bool _bTrim = true);
		void f_SetAtLeastLen(mint _Len, mint _Grow = 4096);

		TCVector f_Reverse() const;

		struct CFormatOptions
		{
			bool m_bSingleLine = false;
			bool m_bBrackets = true;
		};

		template <typename tf_COption>
		bool f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option) const;
		template <typename tf_CFormatInto, typename tf_CFormatOptions>
		void f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const;

	private:
#if defined(DCompiler_MSVC_Workaround)
		static constexpr mint fsp_Alignment()
		{
			return fg_MaxConstexpr
				(
					NTraits::TCAlignmentOf<t_CData>::mc_Value
					, NTraits::TCAlignmentOf<mint>::mc_Value
				)
			;
		}

		class alignas(fsp_Alignment()) CVectorData
		{
		public:
			mint m_Length;
			mint m_AllocSize;
			inline_small t_CData *f_GetData()
			{
				return (t_CData *)(this + 1);
			}
		};
#else
		static constexpr mint mcp_Alignment =
			fg_MaxConstexpr
			(
				NTraits::TCAlignmentOf<t_CData>::mc_Value
				, NTraits::TCAlignmentOf<mint>::mc_Value
			)
		;

		class alignas(mcp_Alignment) CVectorData
		{
		public:
			mint m_Length;
			mint m_AllocSize;
			inline_small t_CData *f_GetData()
			{
				return (t_CData *)(this + 1);
			}
		};
#endif

		struct CStaticData : public t_CAllocator
		{
			CVectorData *m_pData = nullptr;
		};

		t_CAllocator &fp_Allocator();
		t_CAllocator const &fp_Allocator() const;

		void fp_FreeData(CVectorData *_pData);
		CVectorData *fp_AllocData(mint _nObjects);
		CVectorData *fp_AllocDataGrow(mint _nObjects);

		static inline_small mint fsp_GetAllocSize(mint _NeededSize);
		static inline_small bool fsp_NeedRealloc(mint _NeededSize, const CVectorData *_pExtraData);
		static inline_small bool fsp_NeedReallocGrow(mint _NeededSize, const CVectorData *_pExtraData);
		static inline_small bool fsp_CanGrow(mint _NeededSize, const CVectorData *_pExtraData);

		template <typename tf_CSorter>
		void fp_InsertSort(t_CData *_pArray, aint _Low, aint _High, tf_CSorter &&_Sorter);
		template <typename tf_CSorter>
		aint fp_Partition(t_CData *_pArray, aint _Low, aint _High, tf_CSorter &&_Sorter);
		template <typename tf_CSorter>
		void fpr_QuickSort(t_CData *_pArray, aint _Low, aint _High, tf_CSorter &&_Sorter);


		t_CData *fp_MakeRoomBegin(mint _Len);
		void fp_MakeRoomBeginUndo(mint _Len);
		t_CData *fp_MakeRoomMiddle(mint _iStart, mint _Len);
		void fp_MakeRoomMiddleUndo(mint _iStart, mint _Len);
		t_CData *fp_MakeRoom(mint _Len);
		t_CData *fp_MakeNewRoom(mint _Len);

		static inline_small void fsp_CheckBounds(mint _Len, mint _Position);
		static inline_small void fsp_CheckOverlapping(mint _Start0, mint _Start1, mint _Len0, mint _Len1);

		void fp_Copy(TCVector const &_Source);

		template <typename tf_CIteratorData>
		mint fp_GetIteratorPos(TCVectorIterator<tf_CIteratorData> const &_Iter) const
		{
			return _Iter - f_GetArray();
		}

		inline_never t_CData &fp_Insert();
		template <typename tf_CType, typename... tfp_CParams>
		t_CData &fp_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams);
		inline_never t_CData &fp_Insert(t_CData const &_Data);
		inline_never t_CData &fp_Insert(t_CData &&_Data);

		template <typename tf_CAllocator, typename tf_CData>
		static void fsp_DeleteAllAllocatorHelper(tf_CData *_pData);
		template <typename tf_CAllocator, typename tf_CData>
		static void fsp_DeleteAllAllocatorHelperDefiniteType(tf_CData *_pData);
		template <typename tf_CData>
		void fp_DeleteAllDeleterHelper(tf_CData *_pData);

		CStaticData mp_StaticData;
	};

	template <typename t_CData, typename t_CAllocator = NMib::NMemory::CAllocator_Heap, typename t_COptions = TCVectorOptions<16, false> >
	using TCGrowingVector = TCVector<t_CData, t_CAllocator, t_COptions>;

	template <typename tf_CFirst, typename... tf_CParams>
	TCVector<typename NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>::CType> fg_CreateVector(tf_CFirst && _First, tf_CParams && ..._Params);
	template <typename tf_CReturn, typename... tf_CParams>
	TCVector<tf_CReturn> fg_CreateVector(tf_CParams && ..._Params);

	DMibImpErrorClassDefine(CExceptionList, NException::CException);
#	define DMibErrorList(_Description) DMibImpError(NMib::NContainer::CExceptionList, _Description)
#	ifndef DMibPNoShortCuts
#		define DErrorList(_Description) DMibErrorList(_Description)
#	endif

	DMibImpErrorClassDefine(CExceptionListBoundCheck, CExceptionList);
#	define DMibErrorListBoundCheck(_Description) DMibImpError(NMib::NContainer::CExceptionListBoundCheck, _Description)
#	ifndef DMibPNoShortCuts
#		define DErrorListBoundCheck(_Description) DMibErrorListBoundCheck(_Description)
#	endif
}

#include "Vector/Malterlib_Container_Vector_Helpers.hpp"
#include "Vector/Malterlib_Container_Vector.hpp"
		
#include "Vector/Malterlib_Container_Vector_Allocation.hpp"
#include "Vector/Malterlib_Container_Vector_BoundsCheck.hpp"
#include "Vector/Malterlib_Container_Vector_Compare.hpp"
#include "Vector/Malterlib_Container_Vector_ConstructCopy.hpp"
#include "Vector/Malterlib_Container_Vector_ConstructMove.hpp"
#include "Vector/Malterlib_Container_Vector_CreateVector.hpp"
#include "Vector/Malterlib_Container_Vector_Delete.hpp"
#include "Vector/Malterlib_Container_Vector_Find.hpp"
#include "Vector/Malterlib_Container_Vector_Format.hpp"
#include "Vector/Malterlib_Container_Vector_Index.hpp"
#include "Vector/Malterlib_Container_Vector_InsertAfter.hpp"
#include "Vector/Malterlib_Container_Vector_InsertBefore.hpp"
#include "Vector/Malterlib_Container_Vector_InsertBeforeCopy.hpp"
#include "Vector/Malterlib_Container_Vector_InsertBeforeForwards.hpp"
#include "Vector/Malterlib_Container_Vector_InsertBeforeMove.hpp"
#include "Vector/Malterlib_Container_Vector_InsertFirst.hpp"
#include "Vector/Malterlib_Container_Vector_InsertFirstCopy.hpp"
#include "Vector/Malterlib_Container_Vector_InsertFirstForwards.hpp"
#include "Vector/Malterlib_Container_Vector_InsertFirstMove.hpp"
#include "Vector/Malterlib_Container_Vector_InsertLast.hpp"
#include "Vector/Malterlib_Container_Vector_InsertLastCopy.hpp"
#include "Vector/Malterlib_Container_Vector_InsertLastForwards.hpp"
#include "Vector/Malterlib_Container_Vector_InsertLastMove.hpp"
#include "Vector/Malterlib_Container_Vector_Iterator.hpp"
#include "Vector/Malterlib_Container_Vector_Iterator.hpp"
#include "Vector/Malterlib_Container_Vector_MakeRoom.hpp"
#include "Vector/Malterlib_Container_Vector_Move.hpp"
#include "Vector/Malterlib_Container_Vector_Remove.hpp"
#include "Vector/Malterlib_Container_Vector_Replace.hpp"
#include "Vector/Malterlib_Container_Vector_SetLen.hpp"
#include "Vector/Malterlib_Container_Vector_Sort.hpp"
#include "Vector/Malterlib_Container_Vector_Stream.hpp"

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
