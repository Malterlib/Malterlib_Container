// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

namespace NMib::NContainer
{
	template <typename t_CStr>
	struct TCRegistryKeyStr
	{
		static constexpr bool mc_bSupportForceCreate = false;
		static constexpr bool mc_bSupportLocation = false;
		static constexpr bool mc_bSupportWhiteSpace = false;

		using CKey = t_CStr;
		using CRet = aint;

		void f_Move(TCRegistryKeyStr &&_Other);
		CRet f_CompareKey(TCRegistryKeyStr const &_Right) const;
		void f_Copy(TCRegistryKeyStr const &_Src);
		void f_Set(t_CStr const &_Str);
		t_CStr const &f_GetName() const;

		template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
		TCRegistryKeyStr(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent);

		template <typename tf_CKey>
		CRet f_Compare(tf_CKey const &_Right) const;

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

		t_CStr m_Name;
	};

	template <typename t_CStr>
	struct TCRegistryKeyStrMulti
	{
		static constexpr bool mc_bSupportForceCreate = true;
		static constexpr bool mc_bSupportLocation = false;
		static constexpr bool mc_bSupportWhiteSpace = false;

		struct CKey
		{
			t_CStr m_Name;
			uint32 m_Sequence;
		};

		using CRet = aint;

		void f_Move(TCRegistryKeyStrMulti &&_Other);
		CRet f_CompareKey(TCRegistryKeyStrMulti const &_Right) const;
		void f_Copy(TCRegistryKeyStrMulti const &_Src);
		void f_Set(t_CStr const &_Str);
		t_CStr const &f_GetName() const;

		template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
		TCRegistryKeyStrMulti(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent);

		template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
		void f_NewSequence(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent);

		template <typename tf_CKey>
		CRet f_Compare(tf_CKey const &_Right) const;

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
		TCRegistryKeyStrMulti(TCRegistryKeyStrMulti const &_Other) = delete;
		TCRegistryKeyStrMulti &operator = (TCRegistryKeyStrMulti const &_Other) = delete;
		TCRegistryKeyStrMulti(TCRegistryKeyStrMulti &&_Other) = delete;
		TCRegistryKeyStrMulti &operator = (TCRegistryKeyStrMulti &&_Other) = delete;

		t_CStr m_Name;
		uint32 m_Sequence = 0;
		uint32 m_GenSequence = 0;
	};

	template <typename t_CStr, ERegistryFlag t_Flags>
	struct TCRegistryKeyStrPreserve
	{
		static constexpr bool mc_bSupportWhiteSpace = true;
		static constexpr bool mc_bSupportForceCreate = true;
		static constexpr bool mc_bSupportLocation = true;

		struct CKey
		{
			t_CStr m_Name;
			uint32 m_Sequence;
		};

		using CRet = aint;
		using CValueLocation = typename TCChooseType<(t_Flags & ERegistryFlag_FullLocation) != 0, NStr::TCParseLocation<t_CStr, true>, uint8>::CType;

		void f_Move(TCRegistryKeyStrPreserve &&_Other);
		CRet f_CompareKey(TCRegistryKeyStrPreserve const &_Right) const;
		void f_Copy(TCRegistryKeyStrPreserve const &_Src);
		void f_Set(t_CStr const &_Str);
		t_CStr const &f_GetName() const;
		NStr::TCParseLocation<t_CStr, (t_Flags & ERegistryFlag_FullLocation) != 0> const &f_GetLocation() const;
		void f_SetLocation(NStr::TCParseLocation<t_CStr, (t_Flags & ERegistryFlag_FullLocation) != 0> const &_Location);
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

		template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
		TCRegistryKeyStrPreserve(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent);

		template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
		void f_NewSequence(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent);

		template <typename tf_CKey>
		CRet f_Compare(tf_CKey const &_Right) const;

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

		t_CStr m_Name;
		uint32 m_Sequence = 0;
		uint32 m_GenSequence = 0;

		NStr::TCParseLocation<t_CStr, (t_Flags & ERegistryFlag_FullLocation) != 0> m_Location;

		t_CStr m_WhiteSpace[ERegistryWhiteSpaceLocation_Max];
		CValueLocation m_ValueLocation;
		NContainer::TCBitArray<ERegistryWhiteSpaceLocation_Max> m_Parsed;
		uint8 m_bForceEscapedKey:1;
		uint8 m_bForceEscapedValue:1;
	};
}

#include "Malterlib_Container_Registry_Key.hpp"
