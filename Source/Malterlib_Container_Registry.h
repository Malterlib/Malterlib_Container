// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Container_BitArray.h"

#include "../../Core/Source/Malterlib_Core_PlatformInterface.h"
#include "Malterlib_Container_Registry_StringTable.h"

namespace NMib::NContainer
{
	DMibImpErrorClassDefine(CExceptionRegistry, NMib::NException::CException);

#	define DMibErrorRegistry(d_Description) DMibImpError(NMib::NContainer::CExceptionRegistry, d_Description)

#	ifndef DMibPNoShortCuts
#		define DErrorRegistry(d_Description) DMibErrorRegistry(d_Description)
#	endif

	enum ERegistryFlag : uint32
	{
		ERegistryFlag_None = 0
		, ERegistryFlag_PreserveOrder = DMibBit(0)
		, ERegistryFlag_DuplicateKeys = DMibBit(1)
		, ERegistryFlag_PreserveWhitspace = DMibBit(2)
		, ERegistryFlag_FullLocation = DMibBit(3)
	};

	template <typename t_CStr, ERegistryFlag t_Flags>
	concept cCompatibleRegistryFlags =
		(sizeof(typename t_CStr::CChar) == 1) // Only 8 bit string types supported
		&&
		( // Preserve whitespace can only be used together with duplicate keys
			(t_Flags & (ERegistryFlag_PreserveWhitspace | ERegistryFlag_DuplicateKeys)) == ERegistryFlag_None
			|| (t_Flags & (ERegistryFlag_PreserveWhitspace | ERegistryFlag_DuplicateKeys)) == ERegistryFlag_DuplicateKeys
			|| (t_Flags & (ERegistryFlag_PreserveWhitspace | ERegistryFlag_DuplicateKeys)) == (ERegistryFlag_PreserveWhitspace | ERegistryFlag_DuplicateKeys)
		)
	;

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags = ERegistryFlag_None, typename t_CStr = t_CKey>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	struct TCRegistry;

	enum ERegistryWhiteSpaceLocation
	{
		ERegistryWhiteSpaceLocation_BeforeKey = 0,
		ERegistryWhiteSpaceLocation_Between,
		ERegistryWhiteSpaceLocation_After,
		ERegistryWhiteSpaceLocation_BeforeChildScopeStart,
		ERegistryWhiteSpaceLocation_AfterChildScopeStart,
		ERegistryWhiteSpaceLocation_BeforeChildScopeEnd,
		ERegistryWhiteSpaceLocation_AfterChildScopeEnd,
		ERegistryWhiteSpaceLocation_Max
	};
}

#include "Malterlib_Container_Registry_Key.h"
#include "Malterlib_Container_Registry_CustomValue.h"

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	struct TCRegistry
	{
		using CLocation = NStr::TCParseLocation<t_CStr, (t_Flags & ERegistryFlag_FullLocation) != 0>;

	private:
		using CChar = typename t_CStr::CChar;

		using CRegistryKey = typename TCChooseType
			<
				(t_Flags & ERegistryFlag_DuplicateKeys) != 0
				, typename TCChooseType
				<
					(t_Flags & ERegistryFlag_PreserveWhitspace) != 0
					, TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>
					, TCRegistryKeyStrMulti<t_CKey, t_CStr>
				>::CType
				, TCRegistryKeyStr<t_CKey, t_CStr>
			>::CType
		;

		template <typename t_CKey2, typename t_CStr2>
		friend struct TCRegistryKeyStr;

		template <typename t_CKey2, typename t_CStr2>
		friend struct TCRegistryKeyStrMulti;

		template <typename t_CKey2, typename t_CStr2, ERegistryFlag t_Flags2>
		friend struct TCRegistryKeyStrPreserve;

		template <typename t_CKey2, typename t_CData2, ERegistryFlag t_Flags2, typename t_CStr2>
			requires cCompatibleRegistryFlags<t_CStr2, t_Flags2>
		friend struct TCRegistry;

		template <typename t_CStream2, typename t_CData2>
		friend class NMib::NStream::TCBinaryStreamTypeReference;

		struct CAVLCompare_TCRegistry
		{
			inline_small CRegistryKey const &operator () (TCRegistry const &_Left) const;
			inline_small CRegistryKey &operator () (TCRegistry &_Left) const;
			inline_small COrdering_Partial operator () (CRegistryKey const &_Left, CRegistryKey const &_Right) const;
			template <typename tf_CKey>
			inline_small COrdering_Partial operator ()(CRegistryKey const &_Left, tf_CKey const &_Right) const;
			template <typename tf_CKey>
			inline_small COrdering_Partial operator ()(tf_CKey const &_Left, CRegistryKey const &_Right) const;
		};

		NIntrusive::TCAVLLink<> mp_ChildLink;

		using CTree = NIntrusive::TCAVLTree<&TCRegistry::mp_ChildLink, CAVLCompare_TCRegistry>;

		struct CChildren_Sorted
		{
			using CIterator = typename CTree::CIterator;

			CChildren_Sorted(CChildren_Sorted &&_Other);
			CChildren_Sorted &operator =(CChildren_Sorted &&_Other);
			CChildren_Sorted();

			CIterator f_GetIterator() const;
			void f_Insert(TCRegistry *_pReg);
			void f_Insert(TCRegistry *_pReg, TCRegistry *_pAfter);
			void f_Remove(TCRegistry *_pReg);

			template <typename tf_CStream>
			void f_Feed(tf_CStream &_Stream) const;
			template <typename tf_CStream>
			void f_Consume(tf_CStream &_Stream);

			CTree m_Tree;
		};

		struct CChildren_PreserveOrder
		{
			DMibListLinkDS_Member(m_Link);

			struct CDLinkTranslatorm_Link
			{
				template <typename t_CClass, mint t_Offset = sizeof(CRegistryKey) + sizeof(t_CData) + sizeof(NIntrusive::TCAVLLink<>)>
				struct TCOffset
				{
					enum
					{
						mc_Offset = DMibPOffsetOf(t_CClass, mp_Children.m_Link)
					};
				};
			};

			typedef NMib::NIntrusive::TCDLinkList
				<
					TCRegistry
					, CDLinkTranslatorm_Link
					, NMib::NIntrusive::CDLinkAggregateListNoPrevPtr
					, NMib::NIntrusive::CDLinkAggregateListNoPrevPtrList
					, false
					, NMib::NMemory::CDefaultAllocator
				>
				CLinkedList
			;

			typedef typename CLinkedList::CIterator CIterator;

			CChildren_PreserveOrder();
			CChildren_PreserveOrder(CChildren_PreserveOrder &&_Other);
			CChildren_PreserveOrder &operator =(CChildren_PreserveOrder &&_Other);

			CIterator f_GetIterator() const;

			void f_Insert(TCRegistry *_pReg);
			void f_Insert(TCRegistry *_pReg, TCRegistry *_pAfter);
			void f_Remove(TCRegistry *_pReg);

			template <typename tf_CStream>
			void f_Feed(tf_CStream &_Stream) const;
			template <typename tf_CStream>
			void f_Consume(tf_CStream &_Stream);

			CLinkedList m_ChildrenOrder;
			CTree m_Tree;
		};

		struct CPreserveParseContext
		{
			CPreserveParseContext();
			void f_SetFile(t_CStr const &_File);
			CLocation f_GetLocation(ch8 const *_pParse) const;
			void f_AddLine(ch8 const *_pParse);
			void f_SetStartWhiteSpace(ch8 const *_pParse);
			ch8 const *f_GetStartWhiteSpace() const;
			t_CStr f_GetNextWhiteSpace(ch8 const *_pParse);
			void f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str);
			t_CStr const &f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const;
			void f_SetLastAdded(TCRegistry *_pReg, bool _bHadChildren);
			TCRegistry *f_GetLastAdded(bool &_bLastHadChildren) const;
			void f_SetStartParse(CChar const *_pStartParse);
			CChar const *f_GetStartParse() const;
			t_CStr f_FormatLocation(CLocation const &_Location) const;
			t_CStr f_FormatLocation(ch8 const *_pParse) const;

			t_CStr m_File;
			uint32 m_Line = 1;
			ch8 const *m_pLastStartWhitespace = nullptr;
			ch8 const *m_pLastStartLine = nullptr;
			t_CStr m_WhiteSpace[ERegistryWhiteSpaceLocation_Max];
			TCRegistry *m_pLastAdded = nullptr;
			CChar const *m_pStartParse = nullptr;
			bool m_bLastHadChildren = false;
		};

		struct CEmptyParseContext
		{
			int f_GetLocation(ch8 const *_pParse) const;
			t_CStr f_FormatLocation(ch8 const *_pParse) const;
			t_CStr f_FormatLocation(int _Location) const;
		};

	public:
		static constexpr bool mc_bSupportForceCreate = CRegistryKey::mc_bSupportForceCreate;
		static constexpr bool mc_bSupportWhiteSpace = CRegistryKey::mc_bSupportWhiteSpace;
		static constexpr bool mc_bSupportLocation = CRegistryKey::mc_bSupportLocation;

		using CChildren = typename TCChooseType<(t_Flags & ERegistryFlag_PreserveOrder) != 0, CChildren_PreserveOrder, CChildren_Sorted>::CType;
		using CKey = t_CKey;
		using CStr = t_CStr;
		using CData = t_CData;
		using CIterator = typename CChildren::CIterator;
		using CParseContext = typename TCChooseType<mc_bSupportLocation || mc_bSupportWhiteSpace, CPreserveParseContext, CEmptyParseContext>::CType;

		TCRegistry();
		TCRegistry(TCRegistry &&_Source);
		TCRegistry(TCRegistry const &_Source);
		~TCRegistry();

		TCRegistry &operator = (TCRegistry &&_Source);
		TCRegistry &operator = (TCRegistry const &_Source);
		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		TCRegistry &operator = (TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> const &_Source);

		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		bool operator == (TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> const &_Other) const;
		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		auto operator <=> (TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> const &_Other) const;

		void f_Clear();

		t_CKey const &f_GetName() const;
		void f_SetName(t_CKey const &_Name);

		TCRegistry *f_InsertChild(t_CKey _Name, NMib::NStorage::TCUniquePointer<TCRegistry> &&_pChild);
		TCRegistry *f_CreateChild(t_CStr _Name, bool _bForceCreate = false);
		TCRegistry *f_CreateChildNoPath(t_CKey const &_Name, bool _bForceCreate = false);
		bool f_HasChildren() const;
		TCRegistry const *f_GetChild(t_CStr _Str) const;
		TCRegistry *f_GetChild(t_CStr _Str);
		TCRegistry const *f_GetChildNoPath(t_CKey const &_Str) const;
		TCRegistry *f_GetChildNoPath(t_CKey const &_Str);
		void f_DeleteChild(TCRegistry *_pChild);
		bool f_DeleteChild(t_CStr _Name, bool _bDeleteEmptyParentDirs = false);
		bool f_DeleteChildNoPath(t_CKey const &_Name, bool _bDeleteEmptyParentDirs = false);
		void f_DeleteAllChildren();
		void f_MoveChild(TCRegistry *_pChild, TCRegistry *_pAfter);
		CTree const &f_GetChildren() const; // Only to be used for deleting iterators
		CTree &f_GetChildren();
		template <typename tf_CData, typename tf_CStr>
		TCRegistry const *f_GetChild(tf_CStr const &_Str, tf_CData const &_Data) const;
		template <typename tf_CData, typename tf_CStr>
		TCRegistry *f_GetChild(tf_CStr const &_Str, tf_CData const &_Data);

		TCRegistry &f_SetContents(TCRegistry const &_Source);

#ifdef DMibDebug
		void f_DebugTraceTree() const;
#endif
		bool f_DebugIsValid(bool _bIsRoot = true) const;

		void f_FindDiffs
			(
			 	TCRegistry const &_Original
			 	, NContainer::TCVector<t_CStr> &_Changed
			 	, NContainer::TCVector<t_CStr> &_Added
			 	, NContainer::TCVector<t_CStr> &_Deleted
			 	, bool _bRecursive = true
			)
			const
		;
		void f_FindChanges(TCRegistry const &_Original, TCRegistry &_Changed, bool _bIncludeAdded = true) const;
		void f_FindChanges(TCRegistry const &_Original, TCRegistry &_Changed, NContainer::TCVector<t_CKey> &_Deleted, bool _bIncludeAdded = true) const;

		template <typename tf_CStr, bool tf_bIncludeFileLine, bool tf_bEscapeNewLines>
		tf_CStr f_GenerateStr(ch8 const *_pNewLine = DMibNewLine) const;
		t_CStr f_GenerateStr(ch8 const *_pNewLine = DMibNewLine) const;
		t_CStr f_GenerateStrLax(ch8 const *_pNewLine = DMibNewLine) const;

		template <typename t_CTransform>
		void f_Transform(t_CTransform const &_fTransformer);
		template <typename tf_CFunctor>
		void f_TransformFunc(tf_CFunctor const &_fTransformer);
		template <typename tf_CFunctor>
		void f_ForEachInTree(tf_CFunctor const &_fFunctor) const;
		CIterator f_GetChildIterator() const;
		template <typename tf_CStr>
		typename CTree::CIterator f_GetChildIterator(tf_CStr const &_Str) const;

		TCRegistry &f_SetContents(TCRegistry &&_Source);

		template <typename tf_CParseContext>
		void f_ParseStrWithContext(tf_CParseContext &_ParseContext, t_CStr const &_Text, t_CStr const &_File = t_CStr());
		void f_ParseStr(t_CStr const &_Text, t_CStr const &_File = t_CStr());
		void f_Parse(NStream::CBinaryStream &_Stream, t_CStr const &_File = t_CStr());
		void f_ParseLaxStr(t_CStr const &_Text, t_CStr const &_File = t_CStr());
		void f_ParseLax(NStream::CBinaryStream &_Stream, t_CStr const &_File = t_CStr());

		TCRegistry *f_GetParent();
		TCRegistry const *f_GetParent() const;
		t_CStr f_GetPath() const;

		template <bool tf_bSupportLocation = mc_bSupportLocation>
		auto f_GetLocation() const -> TCEnableIfType<tf_bSupportLocation, CLocation> const &;
		template <bool tf_bSupportLocation = mc_bSupportLocation>
		auto f_SetLocation(CLocation const &_Location) -> TCEnableIfType<tf_bSupportLocation>;
		template <bool tf_bSupportLocation = mc_bSupportLocation && (t_Flags & ERegistryFlag_FullLocation) != 0>
		auto f_GetValueLocation() const -> TCEnableIfType<tf_bSupportLocation, NStr::TCParseLocation<t_CStr, true>> const &;
		template <bool tf_bSupportLocation = mc_bSupportLocation && (t_Flags & ERegistryFlag_FullLocation) != 0>
		auto f_SetValueLocation(NStr::TCParseLocation<t_CStr, true> const &_Location) -> TCEnableIfType<tf_bSupportLocation>;

		template <bool tf_bSupportWhiteSpace = mc_bSupportWhiteSpace>
		auto f_GetForceEscapedKey() const -> TCEnableIfType<tf_bSupportWhiteSpace, bool>;
		template <bool tf_bSupportWhiteSpace = mc_bSupportWhiteSpace>
		auto f_GetForceEscapedValue() const -> TCEnableIfType<tf_bSupportWhiteSpace, bool>;
		template <bool tf_bSupportWhiteSpace = mc_bSupportWhiteSpace>
		auto f_SetForceEscapedKey(bool _bForced) -> TCEnableIfType<tf_bSupportWhiteSpace>;
		template <bool tf_bSupportWhiteSpace = mc_bSupportWhiteSpace>
		auto f_SetForceEscapedValue(bool _bForced) -> TCEnableIfType<tf_bSupportWhiteSpace>;

		template <typename tf_CStr, bool tf_bSupportWhiteSpace = mc_bSupportWhiteSpace>
		auto f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation _Location, tf_CStr const &_Str) -> TCEnableIfType<tf_bSupportWhiteSpace, bool>;
		template <bool tf_bSupportWhiteSpace = mc_bSupportWhiteSpace>
		auto f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str) -> TCEnableIfType<tf_bSupportWhiteSpace>;
		template <bool tf_bSupportWhiteSpace = mc_bSupportWhiteSpace>
		auto f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const  -> TCEnableIfType<tf_bSupportWhiteSpace, NStr::CStr>;

		bool f_HasScope() const;

		void f_Merge(TCRegistry const &_MergeWith, bool _bMergeRoot = false);
		template <typename tf_FPredicate>
		void f_MergeIncludingValue(TCRegistry const &_MergeWith, bool _bMergeRoot, tf_FPredicate const &_fPredicate);
		void f_MergeIncludingValue(TCRegistry const &_MergeWith, bool _bMergeRoot = false);
		void f_Add(TCRegistry const &_ToAdd, bool _SetRootValue = false);
		void f_Subtract(TCRegistry const &_ToSubtract);

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);
		template <typename tf_CStream, typename tf_CIndex>
		void f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const;
		template <typename tf_CStream, typename tf_CIndex>
		void f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable);
		template <typename tf_CStream>
		void f_FeedWithStringTable(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_FeedWithStringTable(tf_CStream &_Stream);

		t_CData const &f_GetThisValue() const;
		t_CData &f_GetThisValue();
		t_CData const &f_GetValue(t_CStr _Str, t_CData const &_Default) const;
		t_CData const &f_GetValue(t_CStr const &_Str) const;
		t_CData f_GetValueMove(t_CStr const &_Str);
		t_CData const &f_GetValueNoPath(t_CKey const &_Name) const;
		t_CData f_GetValueNoPathMove(t_CKey const &_Name);
		t_CData const &f_GetValueNoPath(t_CKey const &_Name, t_CData const &_Default) const;
		bool f_GetValueIfExists(t_CStr _Str, t_CData &o_OutValue) const;
		void f_SetThisValue(t_CData const &_Data);
		TCRegistry *f_SetValue(t_CStr _Name, t_CData const &_Data);
		TCRegistry *f_SetValueNoPath(t_CKey const &_Name, t_CData const &_Data);

		template <bool tf_bAllowLineBreakInEscapedString>
		static t_CStr fs_ParseIdentifierStr(ch8 const * &o_pParse, CParseContext &_ParseContext, bool &_bWasEscaped);

	private:
		TCRegistry(TCRegistry *_pParent);

		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		void fp_Copy(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> const &_Source);
		bool fpr_DebugIsValid() const;
		void fp_FixChildren();
		void fpr_AddAllChildren(t_CStr const &_Path, NContainer::TCVector<t_CStr> &_Added) const;
		void fpr_FindDiffs
			(
			 	t_CStr const &_Path
			 	, TCRegistry const &_Original
			 	, NContainer::TCVector<t_CStr> &_Changed
			 	, NContainer::TCVector<t_CStr> &_Added
			 	, NContainer::TCVector<t_CStr> &_Deleted
			 	, bool _bRecursive
			)
			const
		;
		void fpr_FindChanges(t_CStr const &_Path, TCRegistry const &_Original, TCRegistry &_Changed, bool _bIncludeAdded) const;
		void fpr_FindChanges(t_CStr const &_Path, TCRegistry const &_Original, TCRegistry &_Changed, NContainer::TCVector<t_CStr> &_Deleted, bool _bIncludeAdded) const;
		void fpr_Merge(TCRegistry const *_pMergeWith);
		template <typename tf_FPredicate>
		void fpr_MergeIncludingValue(TCRegistry const *_pMergeWith, tf_FPredicate const &_fPredicate);
		void fpr_Add(TCRegistry const *_pAdd);
		TCRegistry const *fp_GetChildParse(t_CStr &_Str, t_CStr *_pNotFound, TCRegistry const **_pPrev = nullptr) const;
		TCRegistry *fp_GetChildParse(t_CStr &_Str, t_CStr *_pNotFound, TCRegistry **_pPrev = nullptr);

		template <typename tf_CParseContext>
		bool fsp_ParseToEndOfLine(ch8 const * &o_pParse, tf_CParseContext &_ParseContext);
		template <typename tf_CParseContext>
		bool fsp_ParseToEndOfComment(ch8 const * &o_pParse, tf_CParseContext &_ParseContext);
		template <bool tf_bAllowLineBreakInEscapedString, typename tf_CParseContext>
		ch8 const *fpr_Parse(ch8 const *_pParse, tf_CParseContext &_ParseContext);
		template <bool tf_bAllowLineBreakInEscapedString, typename tf_CParseContext>
		void fp_Parse(ch8 const *_pParse, tf_CParseContext &_ParseContext);
		template <bool tf_bEscapeNewLines, typename tf_CStr>
		static void fsp_GetEscapedStrAppend(t_CStr const &_Str, tf_CStr &_Dest, bool _bForceEscape, t_CStr const &_PreData, ch8 const *_pNewLine);
		template <typename tf_CStr>
		static bool fsp_OnlyWhiteSpace(tf_CStr const &_Str);
		template <typename tf_CStr>
		static void fsp_ReplaceWithWhitespace(tf_CStr &o_Str);
		void fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str);
		void fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation _Location);
		template <bool tf_bIncludeFileLine, bool tf_bEscapeNewLines, typename tf_CStr>
		static void fspr_GenerateStr(tf_CStr &_Stream, mint _Level, TCRegistry const *_pReg, ch8 const *_pNewLine);
		static t_CStr fsp_AppendPath(t_CStr const &_Path, t_CStr const &_Append);

#ifdef DMibDebug
		inline_never ch16 const *fp_Debug_GetUTF16() const;
#endif

		CRegistryKey mp_Key;
		t_CData mp_Data;

		CChildren mp_Children;
		TCRegistry *mp_pParent;
	};

	using CRegistry = TCRegistry<NStr::CStr, NStr::CStr, ERegistryFlag_DuplicateKeys>;
	using CRegistryPreserveOrder = TCRegistry<NStr::CStr, NStr::CStr, ERegistryFlag_DuplicateKeys | ERegistryFlag_PreserveOrder>;
	using CRegistryPreserveWhitespace = TCRegistry<NStr::CStr, NStr::CStr, ERegistryFlag_DuplicateKeys | ERegistryFlag_PreserveWhitspace>;
	using CRegistryPreserveAll = TCRegistry<NStr::CStr, NStr::CStr, ERegistryFlag_DuplicateKeys | ERegistryFlag_PreserveWhitspace | ERegistryFlag_PreserveOrder>;
	using CRegistryPreserveAllFull = TCRegistry<NStr::CStr, NStr::CStr, ERegistryFlag_DuplicateKeys | ERegistryFlag_PreserveWhitspace | ERegistryFlag_PreserveOrder | ERegistryFlag_FullLocation>;
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif

#include "Registry/Malterlib_Container_Registry.hpp"
