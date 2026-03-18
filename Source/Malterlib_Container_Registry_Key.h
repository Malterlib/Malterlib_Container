// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CStr>
	struct TCRegistryKeyStr
	{
		static constexpr bool mc_bSupportForceCreate = false;
		static constexpr bool mc_bSupportLocation = false;
		static constexpr bool mc_bSupportWhiteSpace = false;

		using CKey = t_CKey;
		using CStr = t_CStr;

		void f_Move(TCRegistryKeyStr &&_Other);
		COrdering_Partial operator <=> (TCRegistryKeyStr const &_Right) const noexcept;
		void f_Copy(TCRegistryKeyStr const &_Src);
		void f_Set(t_CKey const &_Str);
		t_CKey const &f_GetName() const;

		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		TCRegistryKeyStr(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent);

		template <typename tf_CKey>
		COrdering_Partial operator <=> (tf_CKey const &_Right) const noexcept;

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;

		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		template <typename tf_CStream, typename tf_CIndex>
		void f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const;

		template <typename tf_CStream, typename tf_CIndex>
		void f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable);

		template <typename tf_CRegistry, typename tf_CStr>
		static tf_CRegistry *fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind);

		template <typename t_CIterator, typename tf_CStr>
		static void fs_FindIterator(t_CIterator &_Iterator, tf_CStr const &_ToFind);

	private:
		// Don't allow copy construction
		TCRegistryKeyStr(TCRegistryKeyStr const &_Other) = delete;
		TCRegistryKeyStr &operator = (TCRegistryKeyStr const &_Other) = delete;
		TCRegistryKeyStr(TCRegistryKeyStr &&_Other) = delete;
		TCRegistryKeyStr &operator = (TCRegistryKeyStr &&_Other) = delete;

		t_CKey m_Name;
	};

	template <typename t_CKey, typename t_CStr>
	struct TCRegistryKeyStrMulti
	{
		static constexpr bool mc_bSupportForceCreate = true;
		static constexpr bool mc_bSupportLocation = false;
		static constexpr bool mc_bSupportWhiteSpace = false;

		struct CKey
		{
			t_CKey m_Name;
			uint32 m_Sequence;
		};

		void f_Move(TCRegistryKeyStrMulti &&_Other);
		COrdering_Partial operator <=> (TCRegistryKeyStrMulti const &_Right) const noexcept;
		void f_Copy(TCRegistryKeyStrMulti const &_Src);
		void f_Set(t_CStr const &_Str);
		t_CKey const &f_GetName() const;

		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		TCRegistryKeyStrMulti(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent);

		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		void f_NewSequence(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent);

		template <typename tf_CKey>
		COrdering_Partial operator <=> (tf_CKey const &_Right) const noexcept;

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;

		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		template <typename tf_CStream, typename tf_CIndex>
		void f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const;

		template <typename tf_CStream, typename tf_CIndex>
		void f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable);

		template <typename tf_CRegistry, typename tf_CKey>
		static tf_CRegistry *fs_FindEqual(tf_CRegistry const &_This, tf_CKey const &_ToFind);

		template <typename t_CIterator, typename tf_CKey>
		static void fs_FindIterator(t_CIterator &_Iterator, tf_CKey const &_ToFind);

	private:
		TCRegistryKeyStrMulti(TCRegistryKeyStrMulti const &_Other) = delete;
		TCRegistryKeyStrMulti &operator = (TCRegistryKeyStrMulti const &_Other) = delete;
		TCRegistryKeyStrMulti(TCRegistryKeyStrMulti &&_Other) = delete;
		TCRegistryKeyStrMulti &operator = (TCRegistryKeyStrMulti &&_Other) = delete;

		t_CKey m_Name;
		uint32 m_Sequence = 0;
		uint32 m_GenSequence = 0;
	};

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	struct TCRegistryKeyStrPreserve
	{
		static constexpr bool mc_bSupportWhiteSpace = true;
		static constexpr bool mc_bSupportForceCreate = true;
		static constexpr bool mc_bSupportLocation = true;

		struct CKey
		{
			t_CKey m_Name;
			uint32 m_Sequence;
		};

		using CValueLocation = TCConditional<(t_Flags & ERegistryFlag_FullLocation) != 0, NStr::TCParseLocation<t_CStr, true>, uint8>;
		using CLocation = NStr::TCParseLocation<t_CStr, (t_Flags & ERegistryFlag_FullLocation) != 0>;

		void f_Move(TCRegistryKeyStrPreserve &&_Other);
		COrdering_Partial operator <=> (TCRegistryKeyStrPreserve const &_Right) const noexcept;
		void f_Copy(TCRegistryKeyStrPreserve const &_Src);
		void f_Set(t_CKey const &_Str);
		t_CKey const &f_GetName() const;
		CLocation const &f_GetLocation() const;
		void f_SetLocation(CLocation const &_Location);
		CValueLocation const &f_GetValueLocation() const;
		void f_SetValueLocation(CValueLocation const &_Location);
		bool f_GetParsed(ERegistryWhiteSpaceLocation _Location) const;
		void f_SetParsed(ERegistryWhiteSpaceLocation _Location, bool _bParsed);
		bool f_GetForceEscapedKey() const;
		bool f_GetForceEscapedValue() const;
		void f_SetForceEscapedKey(bool _bForced);
		void f_SetForceEscapedValue(bool _bForced);
		void f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str);
		t_CStr const &f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const;

		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		TCRegistryKeyStrPreserve(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent);

		template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
		void f_NewSequence(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent);

		template <typename tf_CKey>
		COrdering_Partial operator <=> (tf_CKey const &_Right) const noexcept;

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;

		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);
		template <typename tf_CStream, typename tf_CIndex>
		void f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const;

		template <typename tf_CStream, typename tf_CIndex>
		void f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable);

		template <typename tf_CRegistry, typename tf_CStr>
		static tf_CRegistry *fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind);

		template <typename t_CIterator, typename tf_CStr>
		static void fs_FindIterator(t_CIterator &_Iterator, tf_CStr const &_ToFind);

	private:
		TCRegistryKeyStrPreserve(TCRegistryKeyStrPreserve const &_Other) = delete;
		TCRegistryKeyStrPreserve &operator = (TCRegistryKeyStrPreserve const &_Other) = delete;
		TCRegistryKeyStrPreserve(TCRegistryKeyStrPreserve &&_Other) = delete;
		TCRegistryKeyStrPreserve &operator = (TCRegistryKeyStrPreserve &&_Other) = delete;

		t_CKey m_Name;
		uint32 m_Sequence = 0;
		uint32 m_GenSequence = 0;

		CLocation m_Location;

		t_CStr m_WhiteSpace[ERegistryWhiteSpaceLocation_Max];
		CValueLocation m_ValueLocation;
		NContainer::TCBitArray<ERegistryWhiteSpaceLocation_Max> m_Parsed;
		uint8 m_bForceEscapedKey:1;
		uint8 m_bForceEscapedValue:1;
	};

	template <typename tf_CKey>
	auto fg_RegistryNameStringForPath(tf_CKey const &_Key);
}

#include "Malterlib_Container_Registry_Key.hpp"
