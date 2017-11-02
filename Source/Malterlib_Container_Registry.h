// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Container/Map>
#include <Mib/Exception/Exception>
#include <Mib/Container/BitArray>

#include "../../Core/Source/Malterlib_Core_PlatformInterface.h"

namespace NMib
{
	namespace NRegistry
	{

		DMibImpErrorClass(CExceptionRegistry, NMib::NException::CException);
#		define DMibErrorRegistry(d_Description) DMibImpError(NMib::NRegistry::CExceptionRegistry, d_Description)

#		ifndef DMibPNoShortCuts
#			define DErrorRegistry(d_Description) DMibErrorRegistry(d_Description)
#		endif


		template <typename t_CKeyStr, typename t_CData, typename t_CRegistryKey, bool t_bPreserveOrder>
		class TCRegistry;

		enum EWhiteSpaceLocation
		{
			EWhiteSpaceLocation_BeforeKey = 0,
			EWhiteSpaceLocation_Between,
			EWhiteSpaceLocation_After,
			EWhiteSpaceLocation_BeforeChildScopeStart,
			EWhiteSpaceLocation_AfterChildScopeStart,
			EWhiteSpaceLocation_BeforeChildScopeEnd,
			EWhiteSpaceLocation_AfterChildScopeEnd,
			EWhiteSpaceLocation_Max
		};

		template <typename t_CStr, typename t_CIndex>
		class TCStringTable
		{
			class CIndex
			{
			public:
				t_CIndex m_Index;

				class CCompare_Index
				{
				public:
					inline_small t_CIndex const &operator () (CIndex const &_Node) const
					{
						return _Node.m_Index;
					}
				};

				template <typename t_CStream>
				void f_Feed(t_CStream &_Stream) const
				{
					_Stream << m_Index;
				}

				template <typename t_CStream>
				void f_Consume(t_CStream &_Stream)
				{
					_Stream >> m_Index;
				}

				DMibIntrusiveLinkT(CIndex, NIntrusive::TCAVLLink<>, m_IndexLink);
			};
			NContainer::TCMap<t_CStr, CIndex> m_Strings;
			NIntrusive::TCAVLTree<typename CIndex::CLinkTraits_m_IndexLink, typename CIndex::CCompare_Index> m_IndexToString;
			t_CIndex m_CurrentIndex;
		public:
			TCStringTable()
				: m_CurrentIndex(0)
			{
			}

			template <typename t_CStream>
			void f_Feed(t_CStream &_Stream) const
			{
				_Stream << m_CurrentIndex;
				_Stream << m_Strings;
			}

			template <typename t_CStream>
			void f_Consume(t_CStream &_Stream)
			{
				_Stream >> m_CurrentIndex;
				_Stream >> m_Strings;
				for (auto iString = m_Strings.f_GetIterator()
					;iString
					;++iString)
				{
					m_IndexToString.f_Insert(*iString);
				}
			}

			template <typename tf_CStream, typename tf_CStr>
			void f_FeedString(tf_CStream &_Stream, tf_CStr const &_String)
			{
				_Stream << f_GetIndex(_String);
			}

			template <typename tf_CStream, typename tf_CStr>
			void f_ConsumeString(tf_CStream &_Stream, tf_CStr &_String) const
			{
				t_CIndex Index;
				_Stream >> Index;
				t_CStr const *pStr = f_GetString(Index);
				if (pStr)
					_String = *pStr;
				else
				{
					DMibErrorFile("Invalid string index in stream");
				}
			}

			template <typename tf_CStr>
			t_CIndex f_GetIndex(tf_CStr const &_String)
			{
				bint bCreated = false;
				CIndex &Index = m_Strings.f_Map(_String, bCreated);
				if (bCreated)
				{
					Index.m_Index = m_CurrentIndex;
					++m_CurrentIndex;
					m_IndexToString.f_Insert(Index);
				}

				return Index.m_Index;
			}

			t_CStr const *f_GetString(t_CIndex const &_Index) const
			{
				CIndex const *pIndex = m_IndexToString.f_FindEqual(_Index);
				if (pIndex)
					return &m_Strings.fs_GetKey(pIndex);
				return nullptr;
			}

		};

 		template <typename t_CKeyStr>
		class TCRegistryKeyStr;

		template <typename t_CKeyStr, typename t_CData, class t_CRegistryKey = TCRegistryKeyStr<t_CKeyStr>, bool t_bPreserveOrder = false>
		class TCRegistry;

		template <typename t_CKeyStr>
		class TCRegistryKeyStr
		{
		private:
			// Don't allow copy construction
			TCRegistryKeyStr(const TCRegistryKeyStr&_Other);
			TCRegistryKeyStr &operator =(const TCRegistryKeyStr&_Other);
			TCRegistryKeyStr(TCRegistryKeyStr &&_Other);
			TCRegistryKeyStr &operator =(TCRegistryKeyStr &&_Other);

		public:
			void f_Move(TCRegistryKeyStr &&_Other)
			{
				m_Name = fg_Move(_Other.m_Name);
			}

			enum
			{
				ESupportForceCreate = 0,
				ESupportFileLine = 0,
				ESupportWhiteSpace = 0
			};

			t_CKeyStr m_Name;

			typedef t_CKeyStr CKey;

			template <typename t_CKeyStr2, typename t_CData2, bool t_bPreserveOrder2>
			TCRegistryKeyStr(TCRegistry<t_CKeyStr2, t_CData2, TCRegistryKeyStr<t_CKeyStr>, t_bPreserveOrder2> *_pParent)
			{
			}

			template <typename t_CKeyStr2, typename t_CData2, bool t_bPreserveOrder2>
			void f_NewSequence(TCRegistry<t_CKeyStr2, t_CData2, TCRegistryKeyStr<t_CKeyStr>, t_bPreserveOrder2> *_pParent)
			{
			}

			typedef aint CRet;

			CRet f_CompareKey(const TCRegistryKeyStr&_Right) const
			{
				return m_Name.f_Cmp(_Right.m_Name);
			}			

			template <typename t_CKey2>
			CRet f_Compare(const t_CKey2 &_Right) const
			{
				return m_Name.f_Cmp(_Right);
			}			

			void f_Copy(const TCRegistryKeyStr &_Src)
			{
				m_Name = _Src.m_Name;
			}

			void f_Set(const t_CKeyStr &_Str)
			{
				m_Name = _Str;
			}

			const t_CKeyStr &f_GetName() const
			{
				return m_Name;
			}

			template <typename t_CStream>
			void f_Feed(t_CStream &_Stream) const
			{
				_Stream << m_Name;
			}

			template <typename t_CStream>
			void f_Consume(t_CStream &_Stream)
			{
				_Stream >> m_Name;
			}

			template <typename tf_CStream, typename tf_CStr, typename tf_CIndex>
			void f_Feed(tf_CStream &_Stream, TCStringTable<tf_CStr, tf_CIndex> &_StringTable) const
			{
				_StringTable.f_FeedString(_Stream, m_Name);
			}

			template <typename tf_CStream, typename tf_CStr, typename tf_CIndex>
			void f_Consume(tf_CStream &_Stream, TCStringTable<tf_CStr, tf_CIndex> const &_StringTable)
			{
				_StringTable.f_ConsumeString(_Stream, m_Name);
			}

			template <typename t_CRegistry, typename t_CKeyStr2>
			static t_CRegistry *fs_FindEqual(const t_CRegistry &_This, const t_CKeyStr2 &_ToFind)
			{
				return _This.m_Children.m_Tree.f_FindEqual(_ToFind);
			}

			template <typename t_CIterator, typename t_CKeyStr2>
			static void fs_FindIterator(t_CIterator &_Iterator, const t_CKeyStr2 &_ToFind)
			{
				_Iterator.f_FindEqualForward(_ToFind);
			}

			template <typename t_CStr>
			void f_SetFileLine(const t_CStr &_File, int32 _Line)
			{
			}

			t_CKeyStr f_GetFile() const
			{
				return t_CKeyStr();
			}

			int32 f_GetLine() const
			{
				return 0;
			}

			bint f_GetParsed(EWhiteSpaceLocation _Location) const
			{
				return false;
			}
			void f_SetParsed(EWhiteSpaceLocation _Location, bint _bParsed)
			{
			}


			void f_SetForceEscapedKey(bint _bForced)
			{
			}

			void f_SetForceEscapedValue(bint _bForced)
			{
			}

			bint f_GetForceEscapedKey() const
			{
				return false;
			}

			bint f_GetForceEscapedValue() const
			{
				return false;
			}



			void f_SetWhiteSpace(EWhiteSpaceLocation _Location, t_CKeyStr const &_Str)
			{
			}

			t_CKeyStr f_GetWhiteSpace(EWhiteSpaceLocation _Location) const
			{
				return t_CKeyStr();
			}
		};

		template <typename t_CKeyStr>
		class TCRegistryKeyStrMulti
		{
		private:
			TCRegistryKeyStrMulti(const TCRegistryKeyStrMulti &_Other);
			TCRegistryKeyStrMulti &operator =(const TCRegistryKeyStrMulti &_Other);
			TCRegistryKeyStrMulti(TCRegistryKeyStrMulti &&_Other);
			TCRegistryKeyStrMulti &operator =(TCRegistryKeyStrMulti &&_Other);
		public:

			void f_Move(TCRegistryKeyStrMulti &&_Other)
			{
				m_GenSequence = _Other.m_GenSequence;
				m_Name = fg_Move(_Other.m_Name);
			}

			enum
			{
				ESupportForceCreate = 1,
				ESupportFileLine = 0,
				ESupportWhiteSpace = 0
			};

			class CKeyRef;

			class CKey
			{
			public:
				t_CKeyStr m_Name;
				uint32 m_Sequence;
			};

			t_CKeyStr m_Name;
			uint32 m_Sequence;
			uint32 m_GenSequence;

			template <typename t_CKeyStr2, typename t_CData2, bool t_bPreserveOrder2>
			TCRegistryKeyStrMulti(TCRegistry<t_CKeyStr2, t_CData2, TCRegistryKeyStrMulti<t_CKeyStr>, t_bPreserveOrder2> *_pParent)
			{
				if (_pParent)
					m_Sequence = _pParent->m_Key.m_GenSequence++;
				else
					m_Sequence = 0;
				m_GenSequence = 0;
			}

			template <typename t_CKeyStr2, typename t_CData2, bool t_bPreserveOrder2>
			void f_NewSequence(TCRegistry<t_CKeyStr2, t_CData2, TCRegistryKeyStrMulti<t_CKeyStr>, t_bPreserveOrder2> *_pParent)
			{
				m_Sequence = _pParent->m_Key.m_GenSequence++;
			}

			typedef aint CRet;

			CRet f_CompareKey(const TCRegistryKeyStrMulti&_Right) const
			{
				CRet Ret = m_Name.f_Cmp(_Right.m_Name);
				if (Ret)
					return Ret;

				if (m_Sequence > _Right.m_Sequence)
					return 1;
				else if (m_Sequence < _Right.m_Sequence)
					return -1;
				return 0;
			}			


			template <typename t_CKey2>
			CRet f_Compare(const t_CKey2 &_Right) const
			{
				CRet Ret = m_Name.f_Cmp(_Right);
				if (Ret)
					return Ret;

				if (m_Sequence > 0)
					return 1;
/*				else if (m_Sequence < 0)
					return -1;*/
				return 0;
			}

			void f_Copy(const TCRegistryKeyStrMulti &_Src)
			{
				m_Name = _Src.m_Name;
			}

			void f_Set(const t_CKeyStr &_Str)
			{
				m_Name = _Str;
			}

			const t_CKeyStr &f_GetName() const
			{
				return m_Name;
			}

			template <typename t_CStream>
			void f_Feed(t_CStream &_Stream) const
			{
				_Stream << m_Name;
				_Stream << m_Sequence;
				_Stream << m_GenSequence;
			}

			template <typename t_CStream>
			void f_Consume(t_CStream &_Stream)
			{
				_Stream >> m_Name;
				_Stream >> m_Sequence;
				_Stream >> m_GenSequence;
			}


			template <typename tf_CStream, typename tf_CStr, typename tf_CIndex>
			void f_Feed(tf_CStream &_Stream, TCStringTable<tf_CStr, tf_CIndex> &_StringTable) const
			{
				_StringTable.f_FeedString(_Stream, m_Name);
				_Stream << m_Sequence;
				_Stream << m_GenSequence;
			}

			template <typename tf_CStream, typename tf_CStr, typename tf_CIndex>
			void f_Consume(tf_CStream &_Stream, TCStringTable<tf_CStr, tf_CIndex> const &_StringTable)
			{
				_StringTable.f_ConsumeString(_Stream, m_Name);
				_Stream >> m_Sequence;
				_Stream >> m_GenSequence;
			}


			template <typename t_CRegistry, typename t_CKeyStr2>
			static t_CRegistry *fs_FindEqual(const t_CRegistry &_This, const t_CKeyStr2 &_ToFind)
			{
				t_CRegistry *pNode = _This.m_Children.m_Tree.f_FindSmallestGreaterThanEqual(_ToFind);
				if (pNode && pNode->m_Key.m_Name == _ToFind)
					return pNode;
				return nullptr;
			}

			template <typename t_CIterator, typename t_CKeyStr2>
			static void fs_FindIterator(t_CIterator &_Iterator, const t_CKeyStr2 &_ToFind)
			{
				_Iterator.f_FindSmallestGreaterThanEqualForward(_ToFind);
				if (_Iterator && _Iterator->m_Key.m_Name != _ToFind)
					_Iterator.f_Clear();
			}

			template <typename t_CStr>
			void f_SetFileLine(const t_CStr &_File, int32 _Line)
			{
			}

			t_CKeyStr f_GetFile() const
			{
				return t_CKeyStr();
			}

			int32 f_GetLine() const
			{
				return 0;
			}

			bint f_GetParsed(EWhiteSpaceLocation _Location) const
			{
				return false;
			}
			void f_SetParsed(EWhiteSpaceLocation _Location, bint _bParsed)
			{
			}

			void f_SetForceEscapedKey(bint _bForced)
			{
			}

			void f_SetForceEscapedValue(bint _bForced)
			{
			}

			bint f_GetForceEscapedKey() const
			{
				return false;
			}

			bint f_GetForceEscapedValue() const
			{
				return false;
			}

			void f_SetWhiteSpace(EWhiteSpaceLocation _Location, t_CKeyStr const &_Str)
			{
			}

			t_CKeyStr f_GetWhiteSpace(EWhiteSpaceLocation _Location) const
			{
				return t_CKeyStr();
			}
		};

		template <typename t_CKeyStr, typename t_CFileStr = t_CKeyStr, typename t_CWhiteSpaceStr = t_CKeyStr>
		class TCRegistryKeyStrPreserve
		{
		private:
			TCRegistryKeyStrPreserve(const TCRegistryKeyStrPreserve &_Other);
			TCRegistryKeyStrPreserve &operator =(const TCRegistryKeyStrPreserve &_Other);
			TCRegistryKeyStrPreserve(TCRegistryKeyStrPreserve &&_Other);
			TCRegistryKeyStrPreserve &operator =(TCRegistryKeyStrPreserve &&_Other);


			t_CKeyStr m_Name;
			uint32 m_Sequence;
			uint32 m_GenSequence;

			t_CFileStr m_File;
			uint32 m_Line:30;
			uint32 m_bForceEscapedKey:1;
			uint32 m_bForceEscapedValue:1;

			t_CWhiteSpaceStr m_WhiteSpace[EWhiteSpaceLocation_Max];
			NContainer::TCBitArray<EWhiteSpaceLocation_Max> m_Parsed;

		public:

			void f_Move(TCRegistryKeyStrPreserve &&_Other)
			{
				m_GenSequence = _Other.m_GenSequence;
				m_Name = fg_Move(_Other.m_Name);
				m_Parsed = _Other.m_Parsed;
				m_bForceEscapedKey = _Other.m_bForceEscapedKey;
				m_bForceEscapedValue = _Other.m_bForceEscapedValue;
				m_Line = _Other.m_Line;
				m_File = fg_Move(_Other.m_File);
				for (mint i = 0; i < EWhiteSpaceLocation_Max; ++i)
					m_WhiteSpace[i] = fg_Move(_Other.m_WhiteSpace[i]);
			}

			class CKey
			{
			public:
				t_CKeyStr m_Name;
				uint32 m_Sequence;
			};

			enum
			{
				ESupportForceCreate = 1,
				ESupportFileLine = 1,
				ESupportWhiteSpace = 1
			};


			template <typename t_CKeyStr2, typename t_CData2, typename t_CFileStr2, typename t_CWhiteSpaceStr2, bool t_bPreserveOrder2>
			TCRegistryKeyStrPreserve(TCRegistry<t_CKeyStr2, t_CData2, TCRegistryKeyStrPreserve<t_CKeyStr2, t_CFileStr2, t_CWhiteSpaceStr2>, t_bPreserveOrder2> *_pParent)
			{
				if (_pParent)
					m_Sequence = _pParent->m_Key.m_GenSequence++;
				else
					m_Sequence = 0;
				m_GenSequence = 0;
				m_Line = 0;
				m_bForceEscapedKey = false;
				m_bForceEscapedValue = false;
			}

			template <typename t_CKeyStr2, typename t_CData2, typename t_CFileStr2, typename t_CWhiteSpaceStr2, bool t_bPreserveOrder2>
			void f_NewSequence(TCRegistry<t_CKeyStr2, t_CData2, TCRegistryKeyStrPreserve<t_CKeyStr2, t_CFileStr2, t_CWhiteSpaceStr2>, t_bPreserveOrder2> *_pParent)
			{
				m_Sequence = _pParent->m_Key.m_GenSequence++;
			}

			typedef aint CRet;

			template <typename t_CKey2>
			CRet f_Compare(const t_CKey2 &_Right) const
			{
				CRet Ret = m_Name.f_Cmp(_Right);
				if (Ret)
					return Ret;

				if (m_Sequence > 0)
					return 1;
/*				else if (m_Sequence < 0)
					return -1;*/
				return 0;
			}

			CRet f_CompareKey(TCRegistryKeyStrPreserve const &_Right) const
			{
				CRet Ret = m_Name.f_Cmp(_Right.m_Name);
				if (Ret)
					return Ret;

				if (m_Sequence > _Right.m_Sequence)
					return 1;
				else if (m_Sequence < _Right.m_Sequence)
					return -1;
				return 0;
			}			


			void f_Copy(const TCRegistryKeyStrPreserve &_Src)
			{
				m_Name = _Src.m_Name;
				m_File = _Src.m_File;
				m_Line = _Src.m_Line;
				m_Parsed = _Src.m_Parsed;
				m_bForceEscapedKey = _Src.m_bForceEscapedKey;
				m_bForceEscapedValue = _Src.m_bForceEscapedValue;

				for (mint i = 0; i < EWhiteSpaceLocation_Max; ++i)
					m_WhiteSpace[i] = _Src.m_WhiteSpace[i];
			}

			void f_Set(const t_CKeyStr &_Str)
			{
				m_Name = _Str;
			}

			const t_CKeyStr &f_GetName() const
			{
				return m_Name;
			}

			template <typename t_CStream>
			void f_Feed(t_CStream &_Stream) const
			{
				_Stream << m_Name;
				_Stream << m_Sequence;
				_Stream << m_GenSequence;
				_Stream << m_File;
				uint32 Flags = m_Line | m_bForceEscapedKey << 30 | m_bForceEscapedValue << 31;
				_Stream << Flags;

				for (mint i = 0; i < EWhiteSpaceLocation_Max; ++i)
				{
					_Stream << m_WhiteSpace[i];
				}
				
				_Stream << m_Parsed;
			}

			template <typename t_CStream>
			void f_Consume(t_CStream &_Stream)
			{
				_Stream >> m_Name;
				_Stream >> m_Sequence;
				_Stream >> m_GenSequence;
				_Stream >> m_File;

				uint32 Flags;
				_Stream >> Flags;
				m_Line = Flags & DMibBitRangeTyped(0, 29, uint32);
				m_bForceEscapedKey = (Flags & DMibBitRangeTyped(30, 30, uint32)) >> 30;
				m_bForceEscapedValue = (Flags & DMibBitRangeTyped(31, 31, uint32)) >> 31;

				for (mint i = 0; i < EWhiteSpaceLocation_Max; ++i)
				{
					_Stream >> m_WhiteSpace[i];
				}
				
				_Stream >> m_Parsed;
			}


			template <typename tf_CStream, typename tf_CStr, typename tf_CIndex>
			void f_Feed(tf_CStream &_Stream, TCStringTable<tf_CStr, tf_CIndex> &_StringTable) const
			{
				_StringTable.f_FeedString(_Stream, m_Name);
				_Stream << m_Sequence;
				_Stream << m_GenSequence;
				_StringTable.f_FeedString(_Stream, m_File);
				uint32 Flags = m_Line | m_bForceEscapedKey << 30 | m_bForceEscapedValue << 31;
				_Stream << Flags;

				for (mint i = 0; i < EWhiteSpaceLocation_Max; ++i)
				{
					_StringTable.f_FeedString(_Stream, m_WhiteSpace[i]);
				}
				
				_Stream << m_Parsed;
			}

			template <typename tf_CStream, typename tf_CStr, typename tf_CIndex>
			void f_Consume(tf_CStream &_Stream, TCStringTable<tf_CStr, tf_CIndex> const &_StringTable)
			{
				_StringTable.f_ConsumeString(_Stream, m_Name);
				_Stream >> m_Sequence;
				_Stream >> m_GenSequence;
				_StringTable.f_ConsumeString(_Stream, m_File);

				uint32 Flags;
				_Stream >> Flags;
				m_Line = Flags & DMibBitRangeTyped(0, 29, uint32);
				m_bForceEscapedKey = (Flags & DMibBitRangeTyped(30, 30, uint32)) >> 30;
				m_bForceEscapedValue = (Flags & DMibBitRangeTyped(31, 31, uint32)) >> 31;

				for (mint i = 0; i < EWhiteSpaceLocation_Max; ++i)
				{
					_StringTable.f_ConsumeString(_Stream, m_WhiteSpace[i]);
				}
				
				_Stream >> m_Parsed;
			}


			template <typename t_CRegistry, typename t_CKeyStr2>
			static t_CRegistry *fs_FindEqual(const t_CRegistry &_This, const t_CKeyStr2 &_ToFind)
			{
				t_CRegistry *pNode = _This.m_Children.m_Tree.f_FindSmallestGreaterThanEqual(_ToFind);
				if (pNode && pNode->m_Key.m_Name == _ToFind)
					return pNode;
				return nullptr;
			}

			template <typename t_CIterator, typename t_CKeyStr2>
			static void fs_FindIterator(t_CIterator &_Iterator, const t_CKeyStr2 &_ToFind)
			{
				_Iterator.f_FindSmallestGreaterThanEqualForward(_ToFind);
				if (_Iterator && _Iterator->m_Key.m_Name != _ToFind)
					_Iterator.f_Clear();
			}

			template <typename t_CStr>
			void f_SetFileLine(const t_CStr &_File, int32 _Line)
			{
				m_File = _File;
				m_Line = _Line;
			}


			const t_CFileStr &f_GetFile() const
			{
				return m_File;
			}

			int32 f_GetLine() const
			{
				return m_Line;
			}

			bint f_GetParsed(EWhiteSpaceLocation _Location) const
			{
				return m_Parsed.f_GetBit(_Location);
			}
			void f_SetParsed(EWhiteSpaceLocation _Location, bint _bParsed)
			{
				m_Parsed.f_SetBit(_Location, _bParsed);
			}

			bint f_GetForceEscapedKey() const
			{
				return m_bForceEscapedKey;
			}

			bint f_GetForceEscapedValue() const
			{
				return m_bForceEscapedValue;
			}

			void f_SetForceEscapedKey(bint _bForced)
			{
				m_bForceEscapedKey = _bForced;
			}

			void f_SetForceEscapedValue(bint _bForced)
			{
				m_bForceEscapedValue = _bForced;
			}

			void f_SetWhiteSpace(EWhiteSpaceLocation _Location, t_CWhiteSpaceStr const &_Str)
			{
				m_WhiteSpace[_Location] = _Str;
			}

			t_CWhiteSpaceStr const &f_GetWhiteSpace(EWhiteSpaceLocation _Location) const
			{
				return m_WhiteSpace[_Location];
			}

		};

		template <typename t_CStr, typename t_CData>
		struct TCRegistry_ParseValue
		{
			TCRegistry_ParseValue(t_CStr const &_Value, bool _bEscaped)
				: m_Value(_Value)
				, m_bEscaped(_bEscaped)
			{
			}
			t_CStr const &m_Value;
			bool m_bEscaped;
			operator t_CData ()
			{
				return m_Value;
			}
		};

		template <typename t_CStr>
		struct TCRegistry_ParseValue<t_CStr, t_CStr>
		{
			TCRegistry_ParseValue(t_CStr const &_Value, bool _bEscaped)
				: m_Value(_Value)
				, m_bEscaped(_bEscaped)
			{
			}
			t_CStr const &m_Value;
			bool m_bEscaped;
			operator t_CStr const &()
			{
				return m_Value;
			}
		};

		template <typename t_CValue>
		struct TCRegistry_FormatValue
		{
			template <typename tf_CString>
			static void fs_Generate(tf_CString &o_Output, t_CValue const &_Value, bool &o_bForceEscape)
			{
				o_Output = tf_CString::fs_ToStr(_Value);
			}
		};
		
		template <typename t_CKeyStr, typename t_CData, class t_CRegistryKey, bool t_bPreserveOrder>
		class TCRegistry
		{
			template <typename t_CKeyStr2> friend class TCRegistryKeyStr;
			template <typename t_CKeyStr2> friend class TCRegistryKeyStrMulti;
			template <typename t_CKeyStr2, typename t_CFileStr2 , typename t_CWhiteSpaceStr2> friend class TCRegistryKeyStrPreserve;
			
			template <typename t_CKeyStr2, typename t_CData2, class t_CRegistryKey2, bool t_bPreserveOrder2>
			friend class TCRegistry;
			
			template <typename t_CStream2, typename t_CData2> friend class NMib::NStream::TCBinaryStreamTypeReference;
			t_CRegistryKey m_Key;
			t_CData m_Data;

			class CAVLCompare_TCRegistry
			{
			public:

				inline_small t_CRegistryKey const &operator () (TCRegistry const &_Left) const
				{
					return _Left.m_Key;
				}
				inline_small t_CRegistryKey &operator () (TCRegistry &_Left) const
				{
					return _Left.m_Key;
				}
			
				inline_small bint operator () (t_CRegistryKey const &_Left, t_CRegistryKey const &_Right) const
				{
					return _Left.f_CompareKey(_Right) < 0;
				}
				inline_small bint operator () (t_CRegistryKey &_Left, t_CRegistryKey const &_Right) const
				{
					return _Left.f_CompareKey(_Right) < 0;
				}
				inline_small bint operator () (t_CRegistryKey const &_Left, t_CRegistryKey &_Right) const
				{
					return _Left.f_CompareKey(_Right) < 0;
				}
				inline_small bint operator () (t_CRegistryKey &_Left, t_CRegistryKey &_Right) const
				{
					return _Left.f_CompareKey(_Right) < 0;
				}
				

				template <typename tf_CKey>
				inline_small bint operator ()(t_CRegistryKey const &_Left, const tf_CKey &_Right) const
				{
					return _Left.f_Compare(_Right) < 0;
				}
				template <typename tf_CKey>
				inline_small bint operator ()(tf_CKey const &_Left, t_CRegistryKey const &_Right) const
				{
					return _Right.f_Compare(_Left) > 0;
				}
			};

			DMibIntrusiveLinkT(TCRegistry, NIntrusive::TCAVLLink<>, m_ChildLink);
			typedef NIntrusive::TCAVLTree<CLinkTraits_m_ChildLink, CAVLCompare_TCRegistry> CTree;

			struct CChildren_Sorted
			{
				CTree m_Tree;

				typedef typename CTree::CIterator CIterator;

				CIterator f_GetIterator() const
				{
					return m_Tree.f_GetIterator();
				}
				void f_Insert(TCRegistry *_pReg)
				{
					m_Tree.f_Insert(_pReg);
				}
				void f_Insert(TCRegistry *_pReg, TCRegistry *_pAfter)
				{
					m_Tree.f_Insert(_pReg);
				}
				void f_Remove(TCRegistry *_pReg)
				{
					m_Tree.f_Remove(_pReg);
				}

				CChildren_Sorted(CChildren_Sorted &&_Other)
					: m_Tree(fg_Move(_Other.m_Tree))
				{
				}

				CChildren_Sorted &operator =(CChildren_Sorted &&_Other)
				{
					m_Tree = fg_Move(_Other.m_Tree);
					return *this;
				}
				CChildren_Sorted()
				{
				}

				template <typename t_CStream>
				void f_Feed(t_CStream &_Stream) const
				{
					_Stream << m_Tree;
				}

				template <typename t_CStream>
				void f_Consume(t_CStream &_Stream)
				{
					_Stream >> m_Tree;
				}
			};

			struct CChildren_PreserveOrder
			{
				struct CDLinkTranslatorm_Link
				{
					template <typename t_CClass> 
					struct TCOffset 
					{ 
						enum
						{
							mc_Offset = DMibPOffsetOf(t_CClass, m_Children.m_Link)
						};
					}; 
				};

				DMibListLinkDS_Member(m_Link);

				typedef NMib::NIntrusive::TCDLinkList
					<
						TCRegistry
						, CDLinkTranslatorm_Link
						, NMib::NIntrusive::TCDLinkAggregateListNoPrevPtr<NMib::NIntrusive::TCDLinkAggregateListNoPrevPtr_Data<NMib::CDefaultPointerHolder>>
						, NMib::NIntrusive::TCDLinkAggregateListNoPrevPtrList<NMib::NMem::CDefaultAllocator>
						, false
						, NMib::NMem::CDefaultAllocator
					>
					CLinkedList
				;
				CLinkedList m_ChildrenOrder;

				CTree m_Tree;

				CChildren_PreserveOrder()
				{
				}

				CChildren_PreserveOrder(CChildren_PreserveOrder &&_Other)
					: m_Tree(fg_Move(_Other.m_Tree))
					, m_ChildrenOrder(fg_Move(_Other.m_ChildrenOrder))
				{
				}

				CChildren_PreserveOrder &operator =(CChildren_PreserveOrder &&_Other)
				{
					m_Tree = fg_Move(_Other.m_Tree);
					m_ChildrenOrder = fg_Move(_Other.m_ChildrenOrder);
					return *this;
				}

				typedef typename CLinkedList::CIterator CIterator;

				CIterator f_GetIterator() const
				{
					return fg_RemoveQualifiers(m_ChildrenOrder).f_GetIterator();
				}

				void f_Insert(TCRegistry *_pReg)
				{
					m_Tree.f_Insert(_pReg);
					m_ChildrenOrder.f_Insert(_pReg);
				}
				void f_Insert(TCRegistry *_pReg, TCRegistry *_pAfter)
				{
					m_Tree.f_Insert(_pReg);
					if (_pAfter)
						m_ChildrenOrder.f_InsertAfter(_pReg, _pAfter);
					else
						m_ChildrenOrder.f_Insert(_pReg);
				}
				void f_Remove(TCRegistry *_pReg)
				{
					m_Tree.f_Remove(_pReg);
					m_ChildrenOrder.f_Remove(_pReg);
				}

				template <typename t_CStream>
				void f_Feed(t_CStream &_Stream) const
				{
					_Stream << m_ChildrenOrder;
				}

				template <typename t_CStream>
				void f_Consume(t_CStream &_Stream)
				{
					_Stream >> m_ChildrenOrder;
					auto Iter = m_ChildrenOrder.f_GetIterator();
					while (Iter)
					{
						m_Tree.f_Insert(Iter);
						++Iter;
					}

				}

			};

			typedef typename TCChooseType<t_bPreserveOrder, CChildren_PreserveOrder, CChildren_Sorted>::CType CChildren;

			CChildren m_Children;

			TCRegistry *m_pParent;

		public:

			enum
			{
				ESupportForceCreate = t_CRegistryKey::ESupportForceCreate,
				ESupportFileLine = t_CRegistryKey::ESupportFileLine,
				ESupportWhiteSpace = t_CRegistryKey::ESupportWhiteSpace
			};

			typedef t_CKeyStr CKeyStr;
			typedef t_CData CData;

			typedef typename CChildren::CIterator CIterator;

		private:

			bint fpr_DebugIsValid() const
			{
				auto Iter = m_Children.f_GetIterator();
				while (Iter)
				{
					const TCRegistry *pReg = Iter;
					++Iter;
					if (pReg->m_pParent != this)
						return false;
					if (!pReg->fpr_DebugIsValid())
						return false;
				}
				return true;
			}

			static t_CRegistryKey &fp_GetKeyHelper();
			void fp_FixChildren()
			{
				auto Iter = m_Children.f_GetIterator();
				while (Iter)
				{
					Iter->m_pParent = this;
					++Iter;
				}
			}


			void fpr_AddAllChildren(const t_CKeyStr &_Path, NContainer::TCVector<t_CKeyStr> &_Added) const
			{
				auto Iter = m_Children.f_GetIterator();
				while (Iter)
				{
					const TCRegistry *pReg = Iter;
					++Iter;
					t_CKeyStr Name = pReg->f_GetName();
					t_CKeyStr Path = fsp_AppendPath(_Path, Name);
					_Added.f_Insert(Path);
					pReg->fpr_AddAllChildren(Path, _Added);
				}
			}

			void fpr_FindDiffs(const t_CKeyStr &_Path, const TCRegistry &_Original, NContainer::TCVector<t_CKeyStr> &_Changed, NContainer::TCVector<t_CKeyStr> &_Added, NContainer::TCVector<t_CKeyStr> &_Deleted, bint _bRecursive) const
			{
				// Find deleted
				{
					auto Iter = _Original.m_Children.f_GetIterator();
					while (Iter)
					{
						const TCRegistry *pReg = Iter;
						++Iter;
						t_CKeyStr Name = pReg->f_GetName();

						const TCRegistry *pChild = f_GetChildNoPath(Name);
						if (!pChild)
						{
							t_CKeyStr Path = fsp_AppendPath(_Path, Name);
							_Deleted.f_Insert(Path);
							if (_bRecursive)
								pReg->fpr_AddAllChildren(Path, _Deleted);
						}
					}
				}
				// Find added and changed
				{
					auto Iter = m_Children.f_GetIterator();
					while (Iter)
					{
						const TCRegistry *pReg = Iter;
						++Iter;
						t_CKeyStr Name = pReg->f_GetName();

						t_CKeyStr Path = fsp_AppendPath(_Path, Name);
						const TCRegistry *pChild = _Original.f_GetChildNoPath(Name);
						if (!pChild)
						{
							_Added.f_Insert(Path);
							if (_bRecursive)
								pReg->fpr_AddAllChildren(Path, _Added);
						}
						else
						{
							if (pChild->f_GetThisValue() != pReg->f_GetThisValue())
								_Changed.f_Insert(Path);

							if (_bRecursive)
								pReg->fpr_FindDiffs(Path, *pChild, _Changed, _Added, _Deleted, _bRecursive);
						}
					}
				}
			}


			void fpr_FindChanges(const t_CKeyStr &_Path, const TCRegistry &_Original, TCRegistry &_Changed, bint _bIncludeAdded) const
			{
				// Find added and changed
				{
					auto Iter = m_Children.f_GetIterator();
					while (Iter)
					{
						const TCRegistry *pReg = Iter;
						++Iter;
						t_CKeyStr Name = pReg->f_GetName();

						t_CKeyStr Path = fsp_AppendPath(_Path, Name);
						const TCRegistry *pChild = _Original.f_GetChildNoPath(Name);
						if (!pChild)
						{
							if (_bIncludeAdded)
							{
								TCRegistry *pCreate = _Changed.f_CreateChild(Path);
								pCreate->f_Add(*pReg, true);
							}
						}
						else
						{
							if (pChild->f_GetThisValue() != pReg->f_GetThisValue())
								_Changed.f_SetValue(Path, pReg->f_GetThisValue());

							pReg->fpr_FindChanges(Path, *pChild, _Changed, _bIncludeAdded);
						}
					}
				}
			}
			void fpr_FindChanges(const t_CKeyStr &_Path, const TCRegistry &_Original, TCRegistry &_Changed, NContainer::TCVector<t_CKeyStr> &_Deleted, bint _bIncludeAdded) const
			{
				// Find deleted
				{
					auto Iter = _Original.m_Children.f_GetIterator();
					while (Iter)
					{
						const TCRegistry *pReg = Iter;
						++Iter;
						t_CKeyStr Name = pReg->f_GetName();

						const TCRegistry *pChild = f_GetChildNoPath(Name);
						if (!pChild)
						{
							t_CKeyStr Path = fsp_AppendPath(_Path, Name);
							_Deleted.f_Insert(Path);
						}
					}
				}
				// Find added and changed
				{
					auto Iter = m_Children.f_GetIterator();
					while (Iter)
					{
						const TCRegistry *pReg = Iter;
						++Iter;
						t_CKeyStr Name = pReg->f_GetName();

						t_CKeyStr Path = fsp_AppendPath(_Path, Name);
						const TCRegistry *pChild = _Original.f_GetChildNoPath(Name);
						if (!pChild)
						{
							if (_bIncludeAdded)
							{
								TCRegistry *pCreate = _Changed.f_CreateChild(Path);
								pCreate->f_Add(*pReg, true);
							}
						}
						else
						{
							if (pChild->f_GetThisValue() != pReg->f_GetThisValue())
								_Changed.f_SetValue(Path, pReg->f_GetThisValue());

							pReg->fpr_FindChanges(Path, *pChild, _Changed, _Deleted, _bIncludeAdded);
						}
					}
				}
			}
			void fpr_Merge(const TCRegistry *_pMergeWith)
			{
				// Find deleted
				{
					auto Iter = _pMergeWith->m_Children.f_GetIterator();
					while (Iter)
					{
						const TCRegistry *pReg = Iter;
						++Iter;
						t_CKeyStr Name = pReg->f_GetName();
						TCRegistry *pChild = f_SetValue(Name, pReg->f_GetThisValue());
						pChild->m_Key.f_Copy(pReg->m_Key);
						pChild->fpr_Merge(pReg);
					}
				}
			}
			template <typename t_CLambda>
			void fpr_MergeIncludingValue(const TCRegistry *_pMergeWith, t_CLambda const &_Lambda)
			{
				// Find deleted
				{
					auto Iter = _pMergeWith->m_Children.f_GetIterator();
					while (Iter)
					{
						const TCRegistry *pReg = Iter;
						++Iter;
						t_CKeyStr Name = pReg->f_GetName();
						TCRegistry *pChild;
						if (_Lambda(*pReg))
							pChild = f_GetChild(Name, pReg->f_GetThisValue());
						else
							pChild = f_GetChildNoPath(Name);
						if (!pChild)
							pChild = f_CreateChild(Name, true);
						pChild->f_SetThisValue(pReg->f_GetThisValue());
						pChild->m_Key.f_Copy(pReg->m_Key);
						pChild->fpr_MergeIncludingValue(pReg, _Lambda);
					}
				}
			}
			void fpr_Add(const TCRegistry *_pAdd)
			{
				auto Iter = _pAdd->m_Children.f_GetIterator();
				while (Iter)
				{
					const TCRegistry *pReg = Iter;
					++Iter;
					t_CKeyStr Name = pReg->f_GetName();
					TCRegistry *pChild = f_CreateChild(Name, true);
					pChild->m_Key.f_Copy(pReg->m_Key);
					pChild->f_SetThisValue(pReg->f_GetThisValue());
					pChild->fpr_Add(pReg);
				}
			}

			template <typename t_CKeyStr2, typename t_CKeyStr3>
			const TCRegistry *fp_GetChildParse(t_CKeyStr2 &_Str, t_CKeyStr3 *_pNotFound, const TCRegistry **_pPrev = nullptr) const
			{
				const TCRegistry *pCurrent = this;
				while (1)
				{
					aint iPos = _Str.f_FindChar('/');

					t_CKeyStr2 Temp;
					if (iPos >= 0)
					{
						Temp = _Str.f_Left(iPos);
						_Str = _Str.f_Extract(iPos + 1);
					}
					else
					{
						Temp = _Str;
						_Str.f_Clear();
					}

					const TCRegistry *pNext = t_CRegistryKey::fs_FindEqual(*pCurrent, Temp);
					if (!pNext)
					{
						if (_pNotFound)
							*_pNotFound = Temp;
						if (_pPrev)
							*_pPrev = pCurrent;
						return nullptr;
					}
					pCurrent = pNext;
					if (_Str.f_IsEmpty())
						break;
				}

				if (pCurrent != this)
					return pCurrent;
				return nullptr;
			}

			template <typename t_CKeyStr2, typename t_CKeyStr3>
			TCRegistry *fp_GetChildParse(t_CKeyStr2 &_Str, t_CKeyStr3 *_pNotFound, TCRegistry **_pPrev = nullptr)
			{
				TCRegistry *pCurrent = this;
				while (1)
				{
					aint iPos = _Str.f_FindChar('/');

					t_CKeyStr2 Temp;
					if (iPos >= 0)
					{
						Temp = _Str.f_Left(iPos);
						_Str = _Str.f_Extract(iPos + 1);
					}
					else
					{
						Temp = _Str;
						_Str.f_Clear();
					}

					TCRegistry *pNext = t_CRegistryKey::fs_FindEqual(*pCurrent, Temp);
					if (!pNext)
					{
						if (_pNotFound)
							*_pNotFound = Temp;
						if (_pPrev)
							*_pPrev = pCurrent;
						return nullptr;
					}
					pCurrent = pNext;
					if (_Str.f_IsEmpty())
						break;
				}

				if (pCurrent != this)
					return pCurrent;
				return nullptr;
			}


			template <typename t_CStrType, bint t_bAllowLineBreakInEscapedString>
			class TCPreserveParseContext
			{
			public:
				TCPreserveParseContext()
				{
					m_Line = 1;
					m_iLastStartWhitespace = 0;
					m_pLastAdded = nullptr;
					m_bLastHadChildren = false;
				}
				t_CStrType m_File;
				int32 m_Line;
				aint m_iLastStartWhitespace;
				t_CKeyStr m_WhiteSpace[EWhiteSpaceLocation_Max];
				TCRegistry *m_pLastAdded;
				bint m_bLastHadChildren;

				enum
				{
					EEnable = 1
				};

				bint f_GetAllowLineBreakInEscapedString()
				{
					return t_bAllowLineBreakInEscapedString;
				}

				void f_SetFile(const t_CStrType &_File)
				{
					m_File = _File;
				}

				const t_CStrType &f_GetFile() const
				{
					return m_File;
				}

				int32 f_GetLine() const
				{
					return m_Line;
				}

				void f_AddLine()
				{
					++m_Line;
				}

				void f_SetStartWhiteSpace(aint _iPos)
				{
					m_iLastStartWhitespace = _iPos;
				}

				aint f_GetStartWhiteSpace() const
				{
					return m_iLastStartWhitespace;
				}

				template <typename t_CStr>
				t_CStr f_GetNextWhiteSpace(t_CStr const &_ToParse, aint _iParse)
				{
					return _ToParse.f_Extract(f_GetStartWhiteSpace(), _iParse - f_GetStartWhiteSpace());
				}				

				void f_SetWhiteSpace(EWhiteSpaceLocation _Location, t_CKeyStr const &_Str)
				{
					m_WhiteSpace[_Location] = _Str;
				}

				t_CKeyStr const &f_GetWhiteSpace(EWhiteSpaceLocation _Location) const
				{
					return m_WhiteSpace[_Location];
				}

				void f_SetLastAdded(TCRegistry *_pReg, bint _bHadChildren)
				{
					m_pLastAdded = _pReg;
					m_bLastHadChildren = _bHadChildren;
				}

				TCRegistry *f_GetLastAdded(bint &_bLastHadChildren) const
				{
					_bLastHadChildren = m_bLastHadChildren;
					return m_pLastAdded;
				}

			};

			template <bint t_bAllowLineBreakInEscapedString>
			class TCEmptyParseContext
			{
			public:


				enum
				{
					EEnable = 0
				};


				bint f_GetAllowLineBreakInEscapedString()
				{
					return t_bAllowLineBreakInEscapedString;
				}

				template <typename t_CStrType>
				void f_SetFile(const t_CStrType &_File)
				{
				}

				const ch8 *f_GetFile() const
				{
					return "";
				}

				int32 f_GetLine() const
				{
					return 0;
				}

				void f_AddLine()
				{

				}

				void f_SetStartWhiteSpace(aint _iPos)
				{
					
				}

				aint f_GetStartWhiteSpace() const
				{
					return -1;
				}

				void f_SetWhiteSpace(EWhiteSpaceLocation _Location, t_CKeyStr const &_Str)
				{
				}

				template <typename t_CStr>
				t_CStr f_GetNextWhiteSpace(t_CStr const &_ToParse, aint _iParse)
				{
					return t_CStr();
				}

				t_CKeyStr f_GetWhiteSpace(EWhiteSpaceLocation _Location) const
				{
					return t_CKeyStr();
				}

				void f_SetLastAdded(TCRegistry *_pReg, bint _bHadChildren)
				{
				}

				TCRegistry *f_GetLastAdded(bint &_bLastHadChildren) const
				{
					return nullptr;
				}
			};

			template <typename t_CTempStr, typename t_CParseContext>
			static t_CTempStr fsp_ParseIdentifierStr(const t_CTempStr &_ToParse, aint &_Pos, t_CParseContext &_ParseContext, bint &_bWasEscaped)
			{

/*
				aint iBeforeParse = iParse;
				t_CTempStr Temp;
				while (Current)
				{
					if (Current == '\\')
					{
						Temp += _ToParse.f_Extract(iStart, iParse - iStart);
						Current = _ToParse.f_GetAt(++iParse)
						while (Current && NStr::fg_CharIsWhiteSpace(Current))
							Current = _ToParse.f_GetAt(++iParse)
					}
					if (NStr::fg_CharIsWhiteSpace(Current))
						break;
					Current = _ToParse.f_GetAt(++iParse);
				}

				_ToParse.f_Extract(iStart, iParse - iStart);
				*/

				t_CTempStr Ret;
				_bWasEscaped = false;
				aint iPos = _Pos;
				aint Mode = 0;
				aint iStart = iPos;
				aint iStartEscapeSequenceLine = 0;
				typename t_CTempStr::CMaxChar Current = _ToParse.f_GetAt(iPos);
				while (Current)
				{
					if (Mode == 0)
					{
						if (Current == '\"')
						{
							Ret += _ToParse.f_Extract(iStart, iPos - iStart);
							iStartEscapeSequenceLine = _ParseContext.f_GetLine();
							iStart = iPos + 1;
							Mode = 1;
							_bWasEscaped = true;
						}
						else if (Current == '\\')
						{
							Ret += _ToParse.f_Extract(iStart, iPos - iStart);
							// Parse away any whitespace
							Current = _ToParse.f_GetAt(++iPos);
							while (Current && NStr::fg_CharIsWhiteSpace(Current))
							{
								if (ESupportFileLine)
								{
									if (Current == '\n')
									{
										_ParseContext.f_AddLine();
									}
								}
								Current = _ToParse.f_GetAt(++iPos);
							}

							if
							(
								!Current || Current == '{' || Current == '}'
								|| (Current == '/' && _ToParse.f_GetAt(iPos+1) == '/')
								|| (Current == '/' && _ToParse.f_GetAt(iPos+1) == '*')
							)
							{
								DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " \\ needs to be followed with a continuation of the text") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
							}

							iStart = iPos;
							continue;
						}
						else if
						(
							NStr::fg_CharIsWhiteSpace(Current) || Current == '{' || Current == '}'
							|| (Current == '/' && _ToParse.f_GetAt(iPos+1) == '/')
							|| (Current == '/' && _ToParse.f_GetAt(iPos+1) == '*')
						)
						{
							break;
						}
					}
					else
					{
						if (ESupportFileLine)
						{
							if (Current == '\n')
							{
								_ParseContext.f_AddLine();
							}
						}

						if (Current == '\\')
						{
							typename t_CTempStr::CMaxChar Next = _ToParse.f_GetAt(iPos+1);
							Ret += _ToParse.f_Extract(iStart, iPos - iStart);
							switch (Next)
							{
							case 'r':
								Ret.f_AddChar('\r');
								break;
							case 'n':
								Ret.f_AddChar('\n');
								break;
							case 't':
								Ret.f_AddChar('\t');
								break;
							case '\\':
								Ret.f_AddChar('\\');
								break;
							case '\"':
								Ret.f_AddChar('\"');
								break;
							default:
								DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Invalid escape character") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
								break;
							}
							++iPos;
							iStart = iPos + 1;
						}
						else if (Current == '\r' || Current == '\n')
						{
							if (!_ParseContext.f_GetAllowLineBreakInEscapedString())
								DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching end for escaped text (\") before end of line") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
						}
						else if (Current == '\"')
						{
							Ret += _ToParse.f_Extract(iStart, iPos - iStart);
							Mode = 0;
							iStart = iPos + 1;
						}
					}

					Current = _ToParse.f_GetAt(++iPos);
				}
				if (Mode == 1)
				{
					DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching end for escaped text (\")") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
				}
				Ret += _ToParse.f_Extract(iStart, iPos - iStart);
				_Pos = iPos;
				return Ret;
			}

			template <typename t_CTempStr, typename t_CParseContext>
			bint fsp_ParseToEndOfLine(const t_CTempStr &_ToParse, aint &_Pos, t_CParseContext &_ParseContext)
			{
				aint iPos = _Pos + 2;
				bint bRet = false;

				typename t_CTempStr::CMaxChar Current = _ToParse.f_GetAt(iPos);
				while (Current)
				{
					if (Current == '\n')
					{
						if (ESupportFileLine)
							_ParseContext.f_AddLine();
						++iPos;
						bRet = true;
						break;
					}
					++iPos;
					Current = _ToParse.f_GetAt(iPos);
				}

				_Pos = iPos;
				return bRet;
			}

			template <typename t_CTempStr, typename t_CParseContext>
			bint fsp_ParseToEndOfComment(const t_CTempStr &_ToParse, aint &_Pos, t_CParseContext &_ParseContext)
			{
				aint iPos = _Pos + 2;
				bint bRet = false;

				typename t_CTempStr::CMaxChar Current = _ToParse.f_GetAt(iPos);
				while (Current)
				{
					if (ESupportFileLine)
					{
						if (Current == '\n')
							_ParseContext.f_AddLine();
					}

					if (Current == '*' && _ToParse.f_GetAt(iPos+1) == '/')
					{
						iPos += 2;;
						bRet = true;
						break;
					}
					++iPos;
					Current = _ToParse.f_GetAt(iPos);
				}

				_Pos = iPos;
				return bRet;
			}

			template <typename t_CTempStr, typename t_CParseContext>
			aint fpr_Parse(const t_CTempStr &_ToParse, aint _Pos, t_CParseContext &_ParseContext)
			{
				aint iParse = _Pos;
				typename t_CTempStr::CMaxChar Current = _ToParse.f_GetAt(iParse);

				aint ParseMode = 0;

				t_CTempStr KeyName;
				bint bKeyNameWasEscaped = false;
				
				TCRegistry *pReg = nullptr;
				TCRegistry *pLastChildReg = nullptr;
				bint bHadChildren = false;


				auto fl_FixupWhitespaceBeforeKey = [&] (aint _iBeforeParse)
				{
					if (pReg || this->f_GetParent())
					{
						t_CTempStr Temp = _ParseContext.f_GetNextWhiteSpace(_ToParse, _iBeforeParse);
						aint iFind = Temp.f_FindCharReverse('\n');
						if (iFind >= 0)
						{
							t_CTempStr Left = Temp.f_Left(iFind+1);
							t_CTempStr Right = Temp.f_Extract(iFind+1);
							if (this->f_IsValidWhiteSpace(EWhiteSpaceLocation_After, Left) && this->f_IsValidWhiteSpace(EWhiteSpaceLocation_BeforeKey, Right))
							{
								if (pReg)
								{
									if (bHadChildren)
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeEnd, Left);
									else
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After, Left);
								}
								else
									this->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeStart, Left);
								_ParseContext.f_SetWhiteSpace(EWhiteSpaceLocation_BeforeKey, Right);
							}
							else
							{
								if (pReg)
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After);
								_ParseContext.f_SetWhiteSpace(EWhiteSpaceLocation_BeforeKey, Temp);
							}
						}
						else
						{
							if (pReg)
								pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After);
							_ParseContext.f_SetWhiteSpace(EWhiteSpaceLocation_BeforeKey, Temp);
						}
					}
					else
						_ParseContext.f_SetWhiteSpace(EWhiteSpaceLocation_BeforeKey, _ParseContext.f_GetNextWhiteSpace(_ToParse, _iBeforeParse));
					_ParseContext.f_SetStartWhiteSpace(iParse);
				};

				auto fl_FixupWhitespaceBeforeValue = [&] (aint _iBeforeParse)
				{
					pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_BeforeKey, _ParseContext.f_GetWhiteSpace(EWhiteSpaceLocation_BeforeKey));
					pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_Between, _ParseContext.f_GetNextWhiteSpace(_ToParse, _iBeforeParse));
					_ParseContext.f_SetStartWhiteSpace(iParse);
				};

				while (Current)
				{
					// Parse away white space
					while (NStr::fg_CharIsWhiteSpace(Current))
					{
						if (ESupportFileLine)
						{
							if (Current == '\n')
								_ParseContext.f_AddLine();
						}

						Current = _ToParse.f_GetAt(++iParse);
					}

					if (Current == '/' && _ToParse.f_GetAt(iParse+1) == '/') // Comment
					{
						fsp_ParseToEndOfLine(_ToParse, iParse, _ParseContext);
					}
					else if (Current == '/' && _ToParse.f_GetAt(iParse+1) == '*') // Comment
					{
						aint StartParseLine = _ParseContext.f_GetLine();
						if (!fsp_ParseToEndOfComment(_ToParse, iParse, _ParseContext))
						{
							DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No end found for block comment") << _ParseContext.f_GetFile() << StartParseLine).f_GetStr());
						}
					}
					else if (Current == '{')
					{
						aint LineBeforeStart = _ParseContext.f_GetLine();
						aint iBeforeParse = iParse;
						++iParse;
						if (ParseMode == 1)
						{
							pReg = f_CreateChildNoPath(KeyName, ESupportForceCreate);
							pReg->f_SetThisValue("");
							if (ESupportFileLine)
								pReg->m_Key.f_SetFileLine(_ParseContext.f_GetFile(), _ParseContext.f_GetLine());

							if (ESupportWhiteSpace)
							{
								if (bKeyNameWasEscaped)
									pReg->f_SetForceEscapedKey(true);

								pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_BeforeKey, _ParseContext.f_GetWhiteSpace(EWhiteSpaceLocation_BeforeKey));
							}

							ParseMode = 0;
						}
						else if (!pReg)
						{
							DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Children without key not supported") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
						}
						else if (pReg == pLastChildReg)
						{
							DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " You cannot specify two child sections for one key") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
						}

						if (pReg)
						{
							pLastChildReg = pReg;
							if (ESupportWhiteSpace)
							{
								t_CTempStr Temp = _ParseContext.f_GetNextWhiteSpace(_ToParse, iBeforeParse);
								aint iFind = Temp.f_FindCharReverse('\n');
								if (iFind >= 0)
								{
									t_CTempStr Left = Temp.f_Left(iFind+1);
									t_CTempStr Right = Temp.f_Extract(iFind+1);
									if (f_IsValidWhiteSpace(EWhiteSpaceLocation_After, Left) && f_IsValidWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeStart, Right))
									{
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After, Left);
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeStart, Right);
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeStart);
									}
									else
									{
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After);
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeStart, Temp);
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeStart);
									}
								}
								else
								{
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After);
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeStart, Temp);
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeStart);
								}
								_ParseContext.f_SetStartWhiteSpace(iParse);
							}

							iParse = pReg->fpr_Parse(_ToParse, iParse, _ParseContext);
							Current = _ToParse.f_GetAt(iParse);
							if (Current != '}')
							{
								DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching '}' found") << _ParseContext.f_GetFile() << LineBeforeStart).f_GetStr());
							}
							++iParse;
						}

						bHadChildren = true;
						Current = _ToParse.f_GetAt(iParse);
					}
					else if (Current == '}')
					{
						if (ParseMode == 1)
							DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Mismatching key/value pairs within scope") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
						if (ESupportWhiteSpace)
						{
							t_CTempStr Temp = _ParseContext.f_GetNextWhiteSpace(_ToParse, iParse);
							aint iFind = Temp.f_FindCharReverse('\n');
							if (iFind >= 0)
							{
								t_CTempStr Left = Temp.f_Left(iFind+1);
								t_CTempStr Right = Temp.f_Extract(iFind+1);
								if (f_IsValidWhiteSpace(EWhiteSpaceLocation_After, Left) && f_IsValidWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeEnd, Right))
								{
									if (pReg)
									{
										if (bHadChildren)
											pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeEnd, Left);
										else
											pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After, Left);
									}
									else
										fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeStart, Left);

									fp_SetParsedWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeEnd, Right);
								}
								else
								{
									if (pReg)
										pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After);
									fp_SetParsedWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeEnd, Temp);
								}
							}
							else
							{
								if (pReg)
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After);
								fp_SetParsedWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeEnd, Temp);
							}
							_ParseContext.f_SetStartWhiteSpace(iParse+1);
						}
						return iParse;
					}
					else if (Current)
					{
						aint iBeforeParse = iParse;
						bint bWasEscaped = false;
						t_CTempStr Temp = fsp_ParseIdentifierStr(_ToParse, iParse, _ParseContext, bWasEscaped);
						if (ParseMode == 0)
						{
							if (ESupportWhiteSpace)
								fl_FixupWhitespaceBeforeKey(iBeforeParse);
							bHadChildren = false;
							KeyName = Temp;
							bKeyNameWasEscaped = bWasEscaped;
							ParseMode = 1;
						}
						else
						{
							bHadChildren = false;
							pReg = f_CreateChildNoPath(KeyName, ESupportForceCreate);
							if (bKeyNameWasEscaped)
								pReg->f_SetForceEscapedKey(true);
							if (bWasEscaped)
								pReg->f_SetForceEscapedValue(true);
							pReg->f_SetThisValue(TCRegistry_ParseValue<t_CTempStr, t_CData>(Temp, bWasEscaped));
							if (ESupportFileLine)
								pReg->m_Key.f_SetFileLine(_ParseContext.f_GetFile(), _ParseContext.f_GetLine());
							if (ESupportWhiteSpace)
								fl_FixupWhitespaceBeforeValue(iBeforeParse);
							ParseMode = 0;
						}
					}

					_ParseContext.f_SetLastAdded(pReg, bHadChildren);
					Current = _ToParse.f_GetAt(iParse);
				}

				if (ParseMode == 1)
					DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Mismatching key/value pairs within scope") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());

				return iParse;
			}

			template <typename t_CTempStr, typename t_CParseContext>
			void fp_Parse(const t_CTempStr &_ToParse, t_CParseContext &_ParseContext)
			{
				aint iParse = fpr_Parse(_ToParse, 0, _ParseContext);
				{
					ch32 Current = _ToParse.f_GetAt(iParse);
					if (Current == '}')
					{
						DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching '{{' found") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
					}
				}

				if (ESupportWhiteSpace)
				{
					aint StartWhiteSpace = _ParseContext.f_GetStartWhiteSpace();
					{
						bint bLastHadChildren = false;
						TCRegistry *pReg = _ParseContext.f_GetLastAdded(bLastHadChildren);
						if (pReg)
						{
							if (StartWhiteSpace != iParse)
							{
								t_CTempStr Temp = _ToParse.f_Extract(StartWhiteSpace , iParse - StartWhiteSpace);

								if (bLastHadChildren)
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeEnd, Temp);
								else
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After, Temp);
							}
							else
							{
								if (bLastHadChildren)
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_AfterChildScopeEnd);
								else
									pReg->fp_SetParsedWhiteSpace(EWhiteSpaceLocation_After);
							}
						}
						
					}
				}
			}


			template <bint t_bEscapeNewLines, typename t_CTempStr, typename t_CTempStr2, typename t_CTempStr3>
			static void fsp_GetEscapedStr(const t_CTempStr &_Str, t_CTempStr2 &_Dest, bint _bForceEscape, const t_CTempStr3 &_PreData)
			{
				bint bNeedEscape = _bForceEscape;

				if (_Str.f_GetAt(0) == 0)
					bNeedEscape = true;

				if (!bNeedEscape)
				{
					mint Len = _Str.f_GetLen();

					typename t_CKeyStr::CMaxChar Current, Prev = 0;
					for (mint i = 0; i < Len; ++i, (Prev = Current))
					{
						Current = _Str.f_GetAt(i);
						if (Current == '\"' || Current == '{' || Current == '}' || Current == '#' || Current == '\\')
						{
							bNeedEscape = true;
							break;
						}
						else if (Prev == '/' && (Current == '*' || Current == '/'))
						{
							// Strings containing comments.
							bNeedEscape = true;
							break;
						}
						else if (Current <= ' ')
						{
							bNeedEscape = true;
							break;
						}
						else if (t_bEscapeNewLines)
						{
							if (Current == '\n')
							{
								bNeedEscape = true;
								break;
							}
						}
					}

				}
				if (bNeedEscape)
				{
					if (t_bEscapeNewLines)
					{
						mint Len = _Str.f_GetLen();
						mint iStart = 0;
						for (mint i = 0; i < Len; ++i)
						{
							typename t_CKeyStr::CMaxChar Current = _Str.f_GetAt(i);
							if (Current == '\n')
							{
								_Dest += _Str.f_Extract(iStart, (i+1)-iStart).f_EscapeStr("\\\"\r\n\t", "\\\"rnt");
								_Dest += "\\" DMibNewLine;
								_Dest += _PreData;
								iStart = i+1;
							}
						}
						_Dest += _Str.f_Extract(iStart, Len - iStart).f_EscapeStr("\\\"\r\n\t", "\\\"rnt");
					}
					else
						_Dest = _Str.f_EscapeStr();
				}
				else
				{
					_Dest = _Str;
				}
			}

			template <typename t_CStr>
			static bint fsp_OnlyWhiteSpace(t_CStr const &_Str)
			{
				mint Len = _Str.f_GetLen();
				for (mint i = 0; i < Len; ++i)
				{
					if (!NStr::fg_CharIsWhiteSpace(_Str.f_GetAt(i)))
						return false;
				}
				return true;
			}

			template <typename t_CStr>
			static void fsp_ReplaceWhithWhiteSpace(t_CStr &_Str)
			{
				mint Len = _Str.f_GetLen();
				for (mint i = 0; i < Len; ++i)
				{
					if (!NStr::fg_CharIsWhiteSpace(_Str.f_GetAt(i)))
						_Str.f_SetAt(i, ' ');
				}
			}

			template <typename t_CKeyStr2>
			void fp_SetParsedWhiteSpace(EWhiteSpaceLocation _Location, t_CKeyStr2 const &_Str)
			{
				DMibFastCheck(f_IsValidWhiteSpace(_Location, _Str));
				m_Key.f_SetWhiteSpace(_Location, _Str);
				m_Key.f_SetParsed(_Location, true);
			}

			void fp_SetParsedWhiteSpace(EWhiteSpaceLocation _Location)
			{
				m_Key.f_SetParsed(_Location, true);
			}

			template <bint t_bIncludeFileLine, bint t_bEscapeNewLines, typename t_CTempStr>
			static void fspr_GenerateStr(t_CTempStr &_Stream, mint _Level, const TCRegistry *_pReg)
			{
				t_CTempStr PreData;
				if (t_bIncludeFileLine)
					PreData = (typename t_CTempStr::CFormat(DMibPFileLineFormat " ") << _pReg->m_Key.f_GetFile() << _pReg->m_Key.f_GetLine()).f_GetStr();

				PreData.f_AddChars('\t', _Level);

				t_CTempStr KeyNameEscaped;
				t_CTempStr ValueEscaped;
				t_CTempStr DataStr;
				
				bool bForceEscaped = _pReg->f_GetForceEscapedValue();
				TCRegistry_FormatValue<t_CData>::fs_Generate(DataStr, _pReg->m_Data, bForceEscaped);

/*				t_CTempStr Space;
				Space.f_AddChar(' ');
				t_CTempStr NewLine = DMibNewLine;*/
				const ch8 *Space = " ";
				const ch8 *NewLine = DMibNewLine;

				t_CTempStr PreDataValue;
				if (ESupportWhiteSpace)
				{
					auto WhiteSpace = _pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_BeforeKey);

					if ((!WhiteSpace.f_IsEmpty() || _pReg->m_Key.f_GetParsed(EWhiteSpaceLocation_BeforeKey)) && !fsp_OnlyWhiteSpace(WhiteSpace))
					{
						PreDataValue = WhiteSpace;
						_Stream += WhiteSpace;
					}
					else
					{
						PreDataValue = PreData;
						_Stream += PreData;
					}
				}
				else
				{
					PreDataValue = PreData;
					_Stream += PreData;
				}

				fsp_ReplaceWhithWhiteSpace(PreDataValue);
				fsp_GetEscapedStr<t_bEscapeNewLines>(_pReg->f_GetName(), KeyNameEscaped, _pReg->f_GetForceEscapedKey(), PreDataValue);
				PreDataValue += KeyNameEscaped;
				_Stream += KeyNameEscaped;


				if (!DataStr.f_IsEmpty() || _pReg->m_Children.m_Tree.f_IsEmpty() || bForceEscaped )
				{
					if (ESupportWhiteSpace)
					{
						auto WhiteSpace = _pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_Between);

						if (!WhiteSpace.f_IsEmpty() || (_pReg->m_Key.f_GetParsed(EWhiteSpaceLocation_Between) && (!_pReg->m_Children.m_Tree.f_IsEmpty() || !DataStr.f_IsEmpty())))
						{
							aint iFind = WhiteSpace.f_FindCharReverse('\n');
							if (iFind >= 0)
								PreDataValue = WhiteSpace.f_Extract(iFind + 1);
							else
								PreDataValue += WhiteSpace;
							_Stream += WhiteSpace;
						}
						else
						{
							PreDataValue += Space;
							_Stream += Space;
						}
					}
					else
					{
						PreDataValue += Space;
						_Stream += Space;
					}

					fsp_ReplaceWhithWhiteSpace(PreDataValue);
					fsp_GetEscapedStr<t_bEscapeNewLines>(DataStr, ValueEscaped, bForceEscaped, PreDataValue);

					_Stream += ValueEscaped;
				}
	
				if (ESupportWhiteSpace)
				{
					auto WhiteSpace = _pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_After);

					if (!WhiteSpace.f_IsEmpty() || _pReg->m_Key.f_GetParsed(EWhiteSpaceLocation_After))
						_Stream += WhiteSpace;
					else
						_Stream += NewLine;
				}
				else
					_Stream += NewLine;

	
				if 
				(
					!_pReg->m_Children.m_Tree.f_IsEmpty() || 
					(
						ESupportWhiteSpace 
						&& 
						(
							!fsp_OnlyWhiteSpace(_pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_AfterChildScopeStart))
							|| !fsp_OnlyWhiteSpace(_pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeStart))
							|| !fsp_OnlyWhiteSpace(_pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeEnd))
							|| !fsp_OnlyWhiteSpace(_pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_AfterChildScopeEnd))
						)
					)
				)
				{
					if (ESupportWhiteSpace)
					{
						auto WhiteSpace = _pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeStart);

						if ((!WhiteSpace.f_IsEmpty() || _pReg->m_Key.f_GetParsed(EWhiteSpaceLocation_BeforeChildScopeStart)) && !fsp_OnlyWhiteSpace(WhiteSpace))
							_Stream += WhiteSpace;
						else
							_Stream += PreData;
					}
					else
						_Stream += PreData;

					_Stream += "{";

					if (ESupportWhiteSpace)
					{
						auto WhiteSpace = _pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_AfterChildScopeStart);

						if (!WhiteSpace.f_IsEmpty() || _pReg->m_Key.f_GetParsed(EWhiteSpaceLocation_AfterChildScopeStart))
							_Stream += WhiteSpace;
						else
							_Stream += NewLine;
					}
					else
						_Stream += NewLine;

					auto Iter = _pReg->m_Children.f_GetIterator();
					while (Iter)
					{
						fspr_GenerateStr<t_bIncludeFileLine, t_bEscapeNewLines>(_Stream, _Level + 1, Iter);
						++Iter;
					}

					if (ESupportWhiteSpace)
					{
						auto WhiteSpace = _pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_BeforeChildScopeEnd);

						if ((!WhiteSpace.f_IsEmpty() || _pReg->m_Key.f_GetParsed(EWhiteSpaceLocation_BeforeChildScopeEnd)) && !fsp_OnlyWhiteSpace(WhiteSpace))
							_Stream += WhiteSpace;
						else
							_Stream += PreData;
					}
					else
						_Stream += PreData;

					_Stream += "}";

					if (ESupportWhiteSpace)
					{
						auto WhiteSpace = _pReg->m_Key.f_GetWhiteSpace(EWhiteSpaceLocation_AfterChildScopeEnd);

						if (!WhiteSpace.f_IsEmpty() || _pReg->m_Key.f_GetParsed(EWhiteSpaceLocation_AfterChildScopeEnd))
							_Stream += WhiteSpace;
						else
							_Stream += NewLine;
					}
					else
						_Stream += NewLine;
				}
			}

			static t_CKeyStr fsp_AppendPath(const t_CKeyStr &_Path, const t_CKeyStr &_Append)
			{
				if (_Path.f_IsEmpty())
					return _Append;
				else
					return _Path + t_CKeyStr("/") + _Append;
			}

			TCRegistry(TCRegistry *_pParent) :
				m_Key(_pParent)	
			{
				m_pParent = _pParent;
			}


#ifdef DMibDebug
			inline_never const ch16 * fp_Debug_GetUTF16() const
			{
				NMib::NStr::g_DebugOutputTemp16 = f_GenerateStr();
				return NMib::NStr::g_DebugOutputTemp16;
			}
#endif

		public:

			TCRegistry()
				: m_Key((TCRegistry *)nullptr) // No parent
				, m_pParent (nullptr)
			{

			}

			TCRegistry(TCRegistry &&_Source) 
				: m_Key((TCRegistry *)nullptr)
				, m_Data(fg_Move(_Source.m_Data))
				, m_pParent(nullptr)
				, m_Children(fg_Move(_Source.m_Children))
			{
				if (_Source.m_pParent && _Source.m_ChildLink.f_IsInTree())
					m_Key.f_Copy(_Source.m_Key); // We need to copy the key as this would break the tree
				else
					m_Key.f_Move(fg_Move(_Source.m_Key));
				auto Iter = m_Children.f_GetIterator();
				while (Iter)
				{
					Iter->m_pParent = this;
					++Iter;
				}
			}

			TCRegistry( const TCRegistry &_Source)
				: m_Key((TCRegistry *)nullptr) // No parent
				, m_pParent(nullptr)
			{
				m_Key.f_Copy(_Source.m_Key);
				m_Data = _Source.m_Data;
				auto Iter = _Source.m_Children.f_GetIterator();

				while (Iter)
				{                                        
					TCRegistry *pChild;
					pChild = DMibNew TCRegistry(this);
					*pChild = *Iter;
					++Iter;
				}
			}

			TCRegistry &operator = (TCRegistry &&_Source)
			{
				if (m_pParent && m_ChildLink.f_IsInTree())
					m_pParent->m_Children.f_Remove(this);
				if (_Source.m_pParent && _Source.m_ChildLink.f_IsInTree())
					m_Key.f_Copy(_Source.m_Key); // In this case we need to copy the key otherwise the tree would break
				else
					m_Key.f_Move(fg_Move(_Source.m_Key));

				if (m_pParent)
				{
					TCRegistry *pOldChild = m_pParent->m_Children.m_Tree.f_FindEqual(m_Key);
					if (pOldChild)
						delete pOldChild;
					m_pParent->m_Children.f_Insert(this);
				}
				m_Data = fg_Move(_Source.m_Data);
				m_Children.m_Tree.f_DeleteAll();
				m_Children = fg_Move(_Source.m_Children);
				auto Iter = m_Children.f_GetIterator();
				while (Iter)
				{
					Iter->m_pParent = this;
					++Iter;
				}
				return *this;
			}

			TCRegistry &operator = ( const TCRegistry &_Source)
			{
				if (m_pParent && m_ChildLink.f_IsInTree())
					m_pParent->m_Children.f_Remove(this);
				m_Key.f_Copy(_Source.m_Key);

				if (m_pParent)
				{
					TCRegistry *pOldChild = m_pParent->m_Children.m_Tree.f_FindEqual(m_Key);
					if (pOldChild)
						delete pOldChild;
					m_pParent->m_Children.f_Insert(this);
				}
				m_Data = _Source.m_Data;
				m_Children.m_Tree.f_DeleteAll();

				auto Iter = _Source.m_Children.f_GetIterator();
				while (Iter)
				{                                        
					TCRegistry *pChild;
					pChild = DMibNew TCRegistry(this);
					*pChild = *Iter;
					++Iter;
				}

				return *this;
			}


			template <typename tf_CKeyStr, typename tf_CData, class tf_CRegistryKey, bool tf_bPreserveOrder>
			void fp_Copy(TCRegistry<tf_CKeyStr, tf_CData, tf_CRegistryKey, tf_bPreserveOrder> const &_Source)
			{
				f_SetThisValue(_Source.f_GetThisValue());
				if (m_Key.ESupportFileLine)
				{
					f_SetFile(_Source.f_GetFile());
					f_SetLine(_Source.f_GetLine());
				}
				if (m_Key.ESupportWhiteSpace)
				{
					f_SetForceEscapedKey(_Source.f_GetForceEscapedKey());
					f_SetForceEscapedValue(_Source.f_GetForceEscapedValue());
					for (mint i = 0; i < EWhiteSpaceLocation_Max; ++i)
					{
						f_SetWhiteSpace((EWhiteSpaceLocation)i, _Source.f_GetWhiteSpace((EWhiteSpaceLocation)i));
						m_Key.f_SetParsed((EWhiteSpaceLocation)i, _Source.m_Key.f_GetParsed((EWhiteSpaceLocation)i));
					}
				}
				
				for (auto iChild = _Source.m_Children.f_GetIterator(); iChild; ++iChild)
				{                                        
					auto pChild = f_CreateChildNoPath(iChild->f_GetName());
					pChild->fp_Copy(*iChild);
				}
			}
			
			template <typename tf_CKeyStr, typename tf_CData, class tf_CRegistryKey, bool tf_bPreserveOrder>
			TCRegistry &operator =(TCRegistry<tf_CKeyStr, tf_CData, tf_CRegistryKey, tf_bPreserveOrder> const &_Source)
			{
				f_SetName(_Source.f_GetName());
				fp_Copy(_Source);

				return *this;
			}

			TCRegistry &f_SetContents(TCRegistry &&_Source)
			{
				m_Data = fg_Move(_Source.m_Data);
				m_Children.m_Tree.f_DeleteAll();
				m_Children = fg_Move(_Source.m_Children);
				auto Iter = m_Children.f_GetIterator();
				while (Iter)
				{
					Iter->m_pParent = this;
					++Iter;
				}
				return *this;
			}

			TCRegistry &f_SetContents( const TCRegistry &_Source)
			{
				m_Data = _Source.m_Data;
				m_Children.m_Tree.f_DeleteAll();

				auto Iter = _Source.m_Children.f_GetIterator();
				while (Iter)
				{                                        
					TCRegistry *pChild;
					pChild = DMibNew TCRegistry(this);
					*pChild = *Iter;
					++Iter;
				}

				return *this;
			}


			~TCRegistry()
			{
				if (m_pParent && m_ChildLink.f_IsInTree())
					m_pParent->m_Children.f_Remove(this);

				m_Children.m_Tree.f_DeleteAll();

#ifdef DMibDebug
				if (NSys::fg_Compiler_AlwaysFalse())
				{
					NSys::fg_Compiler_MakeActive(0, &TCRegistry::fp_Debug_GetUTF16);
					NSys::fg_Compiler_MakeActive(0, &TCRegistry::f_DebugTraceTree);
				}
#endif
			}

			void f_Clear()
			{
				m_Children.m_Tree.f_DeleteAll();
				m_Data = t_CData();
			}

			template <typename t_CTransform>
			void f_Transform(const t_CTransform &_Transformer)
			{
				NContainer::TCVector<TCRegistry *> Children;
				{
					auto Iter = m_Children.f_GetIterator();
					while (Iter)
					{
						Children.f_Insert(Iter);
						++Iter;
					}
				}

				for (auto Iter = Children.f_GetIterator(); Iter; ++Iter)
				{
					TCRegistry **pIter = Iter;
					(*pIter)->f_Transform(_Transformer);
				}

				_Transformer.f_Transform(this);
			}

			template <typename tf_CFunctor>
			void f_TransformFunc(const tf_CFunctor &_Transformer)
			{
				NContainer::TCVector<TCRegistry *> Children;
				{
					auto Iter = m_Children.f_GetIterator();
					while (Iter)
					{
						Children.f_Insert(Iter);
						++Iter;
					}
				}

				for (auto Iter = Children.f_GetIterator(); Iter; ++Iter)
				{
					TCRegistry **pIter = Iter;
					(*pIter)->f_TransformFunc(_Transformer);
				}

				_Transformer(*this);
			}

			template <typename tf_CFunctor>
			void f_ForEachInTree(const tf_CFunctor &_Functor) const
			{
				auto Iter = m_Children.f_GetIterator();
				while (Iter)
				{
					Iter->f_ForEachInTree(_Functor);
					++Iter;
				}

				_Functor(*this);
			}

			auto f_GetFile() const -> decltype(fp_GetKeyHelper().f_GetFile())
			{
				return m_Key.f_GetFile();
			}

			template <typename t_CFileStr>
			void f_SetFile(t_CFileStr const &_File)
			{
				return m_Key.f_SetFileLine(_File, m_Key.f_GetLine());
			}

			template <typename t_CFileStr>
			void f_SetLine(t_CFileStr const &_Line)
			{
				return m_Key.f_SetFileLine(m_Key.f_GetFile(), _Line);
			}

			auto f_GetLine() const -> decltype(fp_GetKeyHelper().f_GetLine())
			{
				return m_Key.f_GetLine();
			}

			auto f_GetForceEscapedKey() const -> decltype(fp_GetKeyHelper().f_GetForceEscapedKey())
			{
				return m_Key.f_GetForceEscapedKey();
			}

			auto f_GetForceEscapedValue() const -> decltype(fp_GetKeyHelper().f_GetForceEscapedValue())
			{
				return m_Key.f_GetForceEscapedValue();
			}

			void f_SetForceEscapedKey(bint _bForced)
			{
				return m_Key.f_SetForceEscapedKey(_bForced);
			}

			void f_SetForceEscapedValue(bint _bForced)
			{
				return m_Key.f_SetForceEscapedValue(_bForced);
			}

			template <typename t_CKeyStr2>
			bint f_IsValidWhiteSpace(EWhiteSpaceLocation _Location, t_CKeyStr2 const &_Str)
			{
				bint bValidWhiteSpace = true;
				bint bLineCommentValid = false;

				switch (_Location)
				{
				case EWhiteSpaceLocation_BeforeKey:
				case EWhiteSpaceLocation_After:
				case EWhiteSpaceLocation_AfterChildScopeStart:
				case EWhiteSpaceLocation_AfterChildScopeEnd:
				case EWhiteSpaceLocation_Between:
					bLineCommentValid = true;
					break;

				case EWhiteSpaceLocation_BeforeChildScopeStart:
				case EWhiteSpaceLocation_BeforeChildScopeEnd:
					bLineCommentValid = false;
					break;
				}

				{
					NMib::NStr::CStr Temp = _Str;
					aint iParse = 0;
					typename t_CKeyStr::CMaxChar Current = Temp.f_GetAt(iParse);
					TCEmptyParseContext<false> ParseContext;

					while (Current)
					{
						// Parse away white space
						while (NStr::fg_CharIsWhiteSpace(Current))
							Current = Temp.f_GetAt(++iParse);

						//aint iStart = iParse;
						if (Current == '/' && Temp.f_GetAt(iParse+1) == '/') // Comment
						{
							if (bLineCommentValid)
							{
								if (!fsp_ParseToEndOfLine(Temp, iParse, ParseContext))
								{
									bValidWhiteSpace = false;
									break;
								}
							}
							else
							{
								bValidWhiteSpace = false;
								break;
							}
						}
						else if (Current == '/' && Temp.f_GetAt(iParse+1) == '*') // Comment
						{
							if (!fsp_ParseToEndOfComment(Temp, iParse, ParseContext))
							{
								bValidWhiteSpace = false;
								break;
							}
						}
						else if (Current)
						{
							bValidWhiteSpace = false;
							break;
						}
						Current = Temp.f_GetAt(iParse);
					}
				}
				return bValidWhiteSpace;
			}

			template <typename t_CKeyStr2>
			void f_SetWhiteSpace(EWhiteSpaceLocation _Location, t_CKeyStr2 const &_Str)
			{
				DMibFastCheck(f_IsValidWhiteSpace(_Location, _Str));
				m_Key.f_SetWhiteSpace(_Location, _Str);
			}

			auto f_GetWhiteSpace(EWhiteSpaceLocation _Location) const -> decltype(fp_GetKeyHelper().f_GetWhiteSpace(_Location))
			{
				return m_Key.f_GetWhiteSpace(_Location);
			}

			// Only to be used for deleting iterators
			CTree const &f_GetChildren() const
			{
				return m_Children.m_Tree;
			}

			CTree &f_GetChildren()
			{
				return m_Children.m_Tree;
			}

			CIterator f_GetChildIterator() const
			{
				return m_Children.f_GetIterator();
			}
			
			template <typename t_CKeyStr2>
			typename CTree::CIterator f_GetChildIterator(t_CKeyStr2 const &_Str) const
			{
				typename CTree::CIterator Iter;
				Iter.f_InitForSearch(m_Children.m_Tree);
				m_Key.fs_FindIterator(Iter, _Str);
				
				return Iter;
			}

			template <typename t_CData2, typename t_CKeyStr2>
			TCRegistry const *f_GetChild(t_CKeyStr2 const &_Str, t_CData2 const &_Data) const
			{
				typename CTree::CIterator Iter;
				Iter.f_InitForSearch(m_Children.m_Tree);
				m_Key.fs_FindIterator(Iter, _Str);

				while (Iter)
				{
					TCRegistry const *pReg = Iter;

					if (pReg->f_GetName() != _Str)
						break;

					if (pReg->f_GetValue() == _Data)
						return pReg;

					++Iter;
				}

				return nullptr;				
			}

			template <typename t_CData2, typename t_CKeyStr2>
			TCRegistry *f_GetChild(t_CKeyStr2 const &_Str, t_CData2 const &_Data)
			{
				typename CTree::CIterator Iter;
				Iter.f_InitForSearch(m_Children.m_Tree);
				m_Key.fs_FindIterator(Iter, _Str);

				while (Iter)
				{
					TCRegistry *pReg = Iter;

					if (pReg->f_GetName() != _Str)
						break;

					if (pReg->f_GetThisValue() == _Data)
						return pReg;

					++Iter;
				}

				return nullptr;				
			}


			const t_CKeyStr &f_GetName() const
			{
				return m_Key.f_GetName();
			}

			void f_SetName(const t_CKeyStr &_Name)
			{
				if (m_pParent && m_ChildLink.f_IsInTree())
					m_pParent->m_Children.f_Remove(this);
				m_Key.f_Set(_Name);
				if (m_pParent)
					m_pParent->m_Children.f_Insert(this);
			}

			void f_MoveChild(TCRegistry *_pChild, TCRegistry *_pAfter)
			{
				if (_pChild->m_pParent && _pChild->m_ChildLink.f_IsInTree())
					_pChild->m_pParent->m_Children.f_Remove(_pChild);

				_pChild->m_pParent = this;
				_pChild->m_Key.f_NewSequence(this);

				m_Children.f_Insert(_pChild, _pAfter);
			}

			const t_CData &f_GetThisValue() const
			{
				return m_Data;
			}

			void f_SetThisValue(const t_CData &_Data)
			{
				m_Data = _Data;
			}

			void f_FindDiffs(const TCRegistry &_Original, NContainer::TCVector<t_CKeyStr> &_Changed, NContainer::TCVector<t_CKeyStr> &_Added, NContainer::TCVector<t_CKeyStr> &_Deleted, bint _bRecursive = true) const
			{
				fpr_FindDiffs("", _Original, _Changed, _Added, _Deleted, _bRecursive);
			}

			void f_FindChanges(const TCRegistry &_Original, TCRegistry &_Changed, bint _bIncludeAdded = true) const
			{
				fpr_FindChanges("", _Original, _Changed, _bIncludeAdded);
			}


			void f_FindChanges(const TCRegistry &_Original, TCRegistry &_Changed, NContainer::TCVector<t_CKeyStr> &_Deleted, bint _bIncludeAdded = true) const
			{
				fpr_FindChanges("", _Original, _Changed, _Deleted, _bIncludeAdded);
			}

			void f_Merge(const TCRegistry &_MergeWith, bint _bMergeRoot = false)
			{
				if (_bMergeRoot)
				{
					f_SetThisValue(_MergeWith.f_GetThisValue());
					if (m_pParent && m_ChildLink.f_IsInTree())
						m_pParent->m_Children.f_Remove(this);
					m_Key.f_Copy(_MergeWith.m_Key);
					if (m_pParent)
						m_pParent->m_Children.f_Insert(this);
				}

				fpr_Merge(&_MergeWith);
			}

			template <typename t_CLambda>
			void f_MergeIncludingValue(const TCRegistry &_MergeWith, bint _bMergeRoot, t_CLambda const &_Lambda)
			{
				if (_bMergeRoot)
				{
					f_SetThisValue(_MergeWith.f_GetThisValue());
					if (m_pParent && m_ChildLink.f_IsInTree())
						m_pParent->m_Children.f_Remove(this);
					m_Key.f_Copy(_MergeWith.m_Key);
					if (m_pParent)
						m_pParent->m_Children.f_Insert(this);
				}

				fpr_MergeIncludingValue(&_MergeWith, _Lambda);
			}
			void f_MergeIncludingValue(const TCRegistry &_MergeWith, bint _bMergeRoot = false)
			{
				f_MergeIncludingValue(_MergeWith, _bMergeRoot, [](TCRegistry const &_Reg) -> bint { return true; });
			}

			void f_Add(const TCRegistry &_ToAdd, bint _SetRootValue = false)
			{
				if (_SetRootValue)
					f_SetThisValue(_ToAdd.f_GetThisValue());
				fpr_Add(&_ToAdd);
			}

			void f_Subtract(const TCRegistry &_ToSubtract)
			{
				for (auto Iter = _ToSubtract.m_Children.f_GetIterator(); Iter; ++Iter)
				{
					const TCRegistry *pReg = Iter;
					t_CKeyStr Name = pReg->f_GetName();
					auto *pChild = f_GetChild(Name);
					if (pChild)
						f_DeleteChild(pChild);
				}
			}

			const t_CData &f_GetValue(t_CKeyStr _Str, const t_CData &_Default) const
			{

				const TCRegistry *pRegistry = fp_GetChildParse(_Str, fg_NullPtr<t_CKeyStr>());
				if (pRegistry)
				{
					return pRegistry->m_Data;
				}
				else
					return _Default;
			}

			const t_CData &f_GetValue(t_CKeyStr const &_Str) const
			{
				t_CKeyStr Str = _Str;
				const TCRegistry *pRegistry = fp_GetChildParse(Str, fg_NullPtr<t_CKeyStr>());
				if (pRegistry)
				{
					return pRegistry->m_Data;
				}
				else
					DMibErrorRegistry(typename t_CKeyStr::CFormat("No such key '{}'") << _Str);
			}

			t_CData f_GetValueMove(t_CKeyStr const &_Str)
			{
				t_CKeyStr Str = _Str;
				const TCRegistry *pRegistry = fp_GetChildParse(Str, fg_NullPtr<t_CKeyStr>());
				if (pRegistry)
				{
					return fg_Move(pRegistry->m_Data);
				}
				else
					DMibErrorRegistry(typename t_CKeyStr::CFormat("No such key '{}'") << _Str);
			}

			const t_CData &f_GetValueNoPath(t_CKeyStr const &_Str) const
			{
				TCRegistry const *pRegistry = t_CRegistryKey::fs_FindEqual(*this, _Str);
				if (pRegistry)
				{
					return pRegistry->m_Data;
				}
				else
					DMibErrorRegistry(typename t_CKeyStr::CFormat("No such key '{}'") << _Str);
			}

			t_CData f_GetValueNoPathMove(t_CKeyStr const &_Str)
			{
				TCRegistry const *pRegistry = t_CRegistryKey::fs_FindEqual(*this, _Str);
				if (pRegistry)
				{
					return fg_Move(pRegistry->m_Data);
				}
				else
					DMibErrorRegistry(typename t_CKeyStr::CFormat("No such key '{}'") << _Str);
			}

			const t_CData &f_GetValueNoPath(t_CKeyStr const &_Str, const t_CData &_Default) const
			{
				TCRegistry const *pRegistry = t_CRegistryKey::fs_FindEqual(*this, _Str);
				if (pRegistry)
				{
					return pRegistry->m_Data;
				}
				else
					return _Default;
			}
			

			bint f_GetValueIfExists(t_CKeyStr _Str, t_CData & _OutValue) const
			{

				const TCRegistry *pRegistry = fp_GetChildParse(_Str, fg_NullPtr<t_CKeyStr>());
				if (pRegistry)
				{
					_OutValue = pRegistry->m_Data;
					return true;
				}
				else
					return false;
			}


			TCRegistry *f_SetValue(t_CKeyStr _Name, const t_CData &_Data)
			{
				t_CKeyStr NotFound;
				TCRegistry *pPrev = this;
				TCRegistry *pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);

				while (!pChild)
				{
					pChild = DMibNew TCRegistry(pPrev);
					pChild->m_Key.f_Set(NotFound);
					pPrev->m_Children.f_Insert(pChild);
					pPrev = pChild;
					if (_Name.f_IsEmpty())
						break;
					pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);
				}
				pChild->m_Data = _Data;

				return pChild;
			}

			TCRegistry *f_SetValueNoPath(t_CKeyStr const &_Name, const t_CData &_Data)
			{
				TCRegistry *pChild = t_CRegistryKey::fs_FindEqual(*this, _Name);

				if (!pChild)
				{
					pChild = DMibNew TCRegistry(this);
					pChild->m_Key.f_Set(_Name);
					m_Children.f_Insert(pChild);
				}
				pChild->m_Data = _Data;

				return pChild;
			}

			TCRegistry *f_InsertChild(t_CKeyStr _Name, NMib::NPtr::TCUniquePointer<TCRegistry> &&_Child)
			{
				if (!ESupportForceCreate)
					DMibError("Foreceinsert not supported");
				_Child->m_pParent = this;
				_Child->m_Key.f_Set(_Name);
				TCRegistry *pChild = _Child.f_Detach();
				m_Children.f_Insert(pChild);
				return pChild;
			}

			TCRegistry *f_CreateChild(t_CKeyStr _Name, bint _bForceCreate = false)
			{
				if (!ESupportForceCreate && _bForceCreate)
					DMibError("You cannot do a force create in TCRegistry when the type does not support it.");

				if (_bForceCreate)
				{
					t_CKeyStr NotFound;
					TCRegistry *pPrev = this;
					TCRegistry *pChild = nullptr;
					if (_Name.f_FindChar('/') >= 0)
					{
						pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);
						while (!pChild)
						{
							pChild = DMibNew TCRegistry(pPrev);
							pChild->m_Key.f_Set(NotFound);
							pPrev->m_Children.f_Insert(pChild);
							pPrev = pChild;
							if (_Name.f_FindChar('/') < 0)
								break;
							pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);
						}
					}
					pChild = DMibNew TCRegistry(pPrev);
					pChild->m_Key.f_Set(_Name);
					pPrev->m_Children.f_Insert(pChild);
					return pChild;
				}
				else
				{
					t_CKeyStr NotFound;
					TCRegistry *pPrev = this;
					TCRegistry *pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);

					bint bCreated = false;

					while (!pChild)
					{
						pChild = DMibNew TCRegistry(pPrev);
						pChild->m_Key.f_Set(NotFound);
						pPrev->m_Children.f_Insert(pChild);
						pPrev = pChild;
						bCreated = true;
						if (_Name.f_IsEmpty())
							break;
						pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);
					}
					return pChild;
				}
			}

			TCRegistry *f_CreateChildNoPath(t_CKeyStr const &_Name, bint _bForceCreate = false)
			{
				if (!ESupportForceCreate && _bForceCreate)
					DMibError("You cannot do a force create in TCRegistry when the type does not support it.");

				if (_bForceCreate)
				{
					TCRegistry *pChild = DMibNew TCRegistry(this);
					pChild->m_Key.f_Set(_Name);
					m_Children.f_Insert(pChild);
					return pChild;
				}
				else
				{
					TCRegistry *pChild = t_CRegistryKey::fs_FindEqual(*this, _Name);

					if (!pChild)
					{
						pChild = DMibNew TCRegistry(this);
						pChild->m_Key.f_Set(_Name);
						m_Children.f_Insert(pChild);
					}
					return pChild;
				}
			}


			TCRegistry *f_GetParent()
			{
				return m_pParent;
			}

			const TCRegistry *f_GetParent() const
			{
				return m_pParent;
			}

			t_CKeyStr f_GetPath() const
			{
				t_CKeyStr Ret = f_GetName();

				TCRegistry *pParent = m_pParent;
				while (pParent && pParent->m_pParent)
				{
					Ret = pParent->f_GetName() + "/" + Ret;
					pParent = pParent->m_pParent;
				}

				return Ret;				
			}

			bint f_HasChildren() const
			{
				return !m_Children.m_Tree.f_IsEmpty();
			}

			const TCRegistry *f_GetChild(t_CKeyStr _Str) const
			{
				return fp_GetChildParse(_Str, fg_NullPtr<t_CKeyStr>());
			}
			TCRegistry *f_GetChild(t_CKeyStr _Str)
			{
				return fp_GetChildParse(_Str, fg_NullPtr<t_CKeyStr>());
			}

			template <typename t_CStr2>
			const TCRegistry *f_GetChildNoPath(t_CStr2 const &_Str) const
			{
				return m_Key.fs_FindEqual(*this, _Str);
			}

			template <typename t_CStr2>
			TCRegistry *f_GetChildNoPath(t_CStr2 const &_Str)
			{
				return m_Key.fs_FindEqual(*this, _Str);
			}

			void f_DeleteChild(TCRegistry *_pChild)
			{
				delete _pChild;
			}
			bint f_DeleteChild(t_CKeyStr _Name, bint _bDeleteEmptyParentDirs = false)
			{
				TCRegistry *pChild = fp_GetChildParse(_Name, fg_NullPtr<t_CKeyStr>());

				if (pChild)
				{
					delete pChild;
					if (_bDeleteEmptyParentDirs)
					{
						TCRegistry *pParent = this;
						while (pParent->m_Children.m_Tree.f_IsEmpty())
						{
							TCRegistry *pToDelete = pParent;
							pParent = pParent->m_pParent;
							if (pParent)
							{
								delete pToDelete;
							}
						}						
					}
					return true;
				}
				else
					return false;
			}

			bint f_DeleteChildNoPath(t_CKeyStr const &_Name, bint _bDeleteEmptyParentDirs = false)
			{
				TCRegistry *pChild = t_CRegistryKey::fs_FindEqual(*this, _Name);

				if (pChild)
				{
					delete pChild;
					if (_bDeleteEmptyParentDirs)
					{
						TCRegistry *pParent = this;
						while (pParent->m_Children.m_Tree.f_IsEmpty())
						{
							TCRegistry *pToDelete = pParent;
							pParent = pParent->m_pParent;
							if (pParent)
							{
								delete pToDelete;
							}
						}						
					}
					return true;
				}
				else
					return false;
			}



			void f_DeleteAllChildren()
			{
				m_Children.m_Tree.f_DeleteAll();
			}

			template <typename t_CTempStr>
			void f_ParseStr(const t_CTempStr &_Text)
			{
				// Remove anything existing
				m_Children.m_Tree.f_DeleteAll();

				if (ESupportFileLine || ESupportWhiteSpace)
				{
					TCPreserveParseContext<typename NTraits::TCDecay<t_CTempStr>::CType, false> Context;
					fp_Parse(_Text, Context);
				}
				else
				{
					TCEmptyParseContext<false> Context;
					fp_Parse(_Text, Context);
				}
			}

			template <typename t_CTempStr, typename t_CTempStr2>
			void f_ParseStr(const t_CTempStr &_Text, t_CTempStr2 &_File)
			{
				// Remove anything existing
				m_Children.m_Tree.f_DeleteAll();

				if (ESupportFileLine || ESupportWhiteSpace)
				{
					m_Key.f_SetFileLine(_File, 0);
					TCPreserveParseContext<typename NTraits::TCDecay<t_CTempStr2>::CType, false> Context;
					Context.f_SetFile(_File);
					fp_Parse(_Text, Context);
				}
				else
				{
					TCEmptyParseContext<false> Context;
					fp_Parse(_Text, Context);
				}
			}

			template <typename t_CTempStr>
			void f_Parse(NStream::CBinaryStream &_Stream)
			{
				// Remove anything existing
				m_Children.m_Tree.f_DeleteAll();

				t_CTempStr ToParse = t_CTempStr::fs_ReadTextStream(_Stream);

				f_ParseStr(ToParse);
			}

			template <typename t_CTempStr, typename t_CFile>
			void f_Parse(NStream::CBinaryStream &_Stream, t_CFile &_File)
			{
				// Remove anything existing
				m_Children.m_Tree.f_DeleteAll();

				t_CTempStr ToParse = t_CTempStr::fs_ReadTextStream(_Stream);

				f_ParseStr(ToParse, _File);
			}

			void f_Parse(NStream::CBinaryStream &_Stream)
			{
				f_Parse<t_CKeyStr>(_Stream);
			}

			template <typename t_CFile>
			void f_Parse(NStream::CBinaryStream &_Stream, t_CFile &_File)
			{
				f_Parse<t_CKeyStr, t_CFile>(_Stream, _File);
			}


			template <typename t_CTempStr>
			void f_ParseLaxStr(const t_CTempStr &_Text)
			{
				// Remove anything existing
				m_Children.m_Tree.f_DeleteAll();

				if (ESupportFileLine || ESupportWhiteSpace)
				{
					TCPreserveParseContext<typename NTraits::TCDecay<t_CTempStr>::CType, true> Context;
					fp_Parse(_Text, Context);
				}
				else
				{
					TCEmptyParseContext<true> Context;
					fp_Parse(_Text, Context);
				}
			}

			template <typename t_CTempStr, typename t_CTempStr2>
			void f_ParseLaxStr(const t_CTempStr &_Text, t_CTempStr2 &_File)
			{
				// Remove anything existing
				m_Children.m_Tree.f_DeleteAll();

				if (ESupportFileLine || ESupportWhiteSpace)
				{
					m_Key.f_SetFileLine(_File, 0);
					TCPreserveParseContext<typename NTraits::TCDecay<t_CTempStr2>::CType, true> Context;
					Context.f_SetFile(_File);
					fp_Parse(_Text, Context);
				}
				else
				{
					TCEmptyParseContext<true> Context;
					fp_Parse(_Text, Context);
				}
			}

			template <typename t_CTempStr>
			void f_ParseLax(NStream::CBinaryStream &_Stream)
			{
				// Remove anything existing
				m_Children.m_Tree.f_DeleteAll();

				t_CTempStr ToParse = t_CTempStr::fs_ReadTextStream(_Stream);

				f_ParseLaxStr(ToParse);
			}

			template <typename t_CTempStr, typename t_CFile>
			void f_ParseLax(NStream::CBinaryStream &_Stream, t_CFile &_File)
			{
				// Remove anything existing
				m_Children.m_Tree.f_DeleteAll();

				t_CTempStr ToParse = t_CTempStr::fs_ReadTextStream(_Stream);

				f_ParseLaxStr(ToParse, _File);
			}

			void f_ParseLax(NStream::CBinaryStream &_Stream)
			{
				f_ParseLax<t_CKeyStr>(_Stream);
			}

			template <typename t_CFile>
			void f_ParseLax(NStream::CBinaryStream &_Stream, t_CFile &_File)
			{
				f_ParseLax<t_CKeyStr, t_CFile>(_Stream, _File);
			}



			template <typename t_CTempStr, bint t_bIncludeFileLine, bint t_bEscapeNewLines>
			t_CTempStr f_GenerateStr() const
			{
				t_CTempStr Temp;
				auto Iter = m_Children.f_GetIterator();
				while (Iter)
				{
					fspr_GenerateStr<t_bIncludeFileLine, t_bEscapeNewLines>(Temp, 0, Iter);
					++Iter;
				}
				return Temp;
			}

			t_CKeyStr f_GenerateStr() const
			{
				return f_GenerateStr<t_CKeyStr, 0, 1>();
			}

			t_CKeyStr f_GenerateStrLax() const
			{
				return f_GenerateStr<t_CKeyStr, 0, 0>();
			}

			template <typename t_CKeyStr2, typename t_CData2, typename t_CRegistryKey2, bool t_bPreserveOrder2>
			bint operator == (TCRegistry<t_CKeyStr2, t_CData2, t_CRegistryKey2, t_bPreserveOrder2> const &_Other) const
			{
				if (!(f_GetName() == _Other.f_GetName()))
					return false;
				if (!(f_GetThisValue() == _Other.f_GetThisValue()))
					return false;
				auto Iter = f_GetChildIterator();
				auto IterOther = _Other.f_GetChildIterator();
				while (Iter && IterOther)
				{
					if (!(*Iter == *IterOther))
						return false;
					++Iter;
					++IterOther;
				}
				if (IterOther || Iter)
					return false;
				return true;
			}

			template <typename t_CKeyStr2, typename t_CData2, typename t_CRegistryKey2, bool t_bPreserveOrder2>
			bint operator < (TCRegistry<t_CKeyStr2, t_CData2, t_CRegistryKey2, t_bPreserveOrder2> const &_Other) const
			{
				NMisc::ECompareResult Compare = NMisc::fg_Compare(f_GetName(), _Other.f_GetName());
				Compare = NMisc::fg_Compare(Compare, f_GetThisValue(), _Other.f_GetThisValue());
				if (Compare)
					return Compare < 0;

				auto Iter = f_GetChildIterator();
				auto IterOther = _Other.f_GetChildIterator();
				while (Iter && IterOther)
				{
					Compare = NMisc::fg_Compare(*Iter, *IterOther);
					if (Compare)
						return Compare < 0;
					++Iter;
					++IterOther;
				}
				if (IterOther && !Iter)
					return false;
				if (!IterOther && Iter)
					return true;
				return false;
			}

#ifdef DMibDebug
			void f_DebugTraceTree() const
			{
				NStr::CStr Temp = f_GenerateStr<NStr::CStr, ESupportFileLine, 1>();
				DMibDTrace("{}" DMibNewLine, Temp);
			}
#endif

			bint f_DebugIsValid(bint _bIsRoot = true) const
			{
				if (_bIsRoot && m_pParent != nullptr)
					return false;
				return fpr_DebugIsValid();
			}

			template <typename t_CData2, typename t_CKeyStr2> 
			inline_small TCRegistry &operator << (NStream::TCNamedStreamInfo<t_CKeyStr2, t_CData2 const> const &_Data)
			{
				NMib::NStream::TCBinaryStreamTypeReference<TCRegistry, NStream::TCNamedStreamInfo<t_CKeyStr2, t_CData2 const>>::fs_Feed(*this, _Data);
				return *this;
			}
			template <typename t_CData2, typename t_CKeyStr2> 
			inline_small TCRegistry &operator << (NStream::TCNamedStreamInfo<t_CKeyStr2, const t_CData2 *> const &_Data)
			{
				NMib::NStream::TCBinaryStreamTypePtr<TCRegistry, NStream::TCNamedStreamInfo<t_CKeyStr2, const t_CData2 *>>::fs_Feed(*this, _Data);
				return *this;
			}
			template <typename t_CData2, typename t_CKeyStr2> 
			inline_small TCRegistry &operator >> (NStream::TCNamedStreamInfo<t_CKeyStr2, t_CData2> const &_Data)
			{
				NMib::NStream::TCBinaryStreamTypeReference<TCRegistry, NStream::TCNamedStreamInfo<t_CKeyStr2, t_CData2>>::fs_Consume(*this, _Data);
				return *this;
			}
			template <typename t_CData2, typename t_CKeyStr2>
			inline_small TCRegistry &operator >> (NStream::TCNamedStreamInfo<t_CKeyStr2, t_CData2 *> const &_Data)
			{
				NMib::NStream::TCBinaryStreamTypePtr<TCRegistry, NStream::TCNamedStreamInfo<t_CKeyStr2, t_CData2 *>>::fs_Consume(*this, _Data);
				return *this;
			}

			template <typename t_CStream>
			void f_Feed(t_CStream &_Stream) const
			{
				m_Key.f_Feed(_Stream);
				_Stream << m_Data;
				_Stream << m_Children;
			}

			template <typename t_CStream>
			void f_Consume(t_CStream &_Stream)
			{
				m_Key.f_Consume(_Stream);
				_Stream >> m_Data;
				_Stream >> m_Children;
				fp_FixChildren();
			}


			template <typename tf_CStream, typename tf_CStr, typename tf_CIndex>
			void f_Feed(tf_CStream &_Stream, TCStringTable<tf_CStr, tf_CIndex> &_StringTable) const
			{
				m_Key.f_Feed(_Stream, _StringTable);
				_StringTable.f_FeedString(_Stream, m_Data);
				mint Len = m_Children.m_Tree.f_GetLen();
				fg_FeedLenToStream(_Stream, Len);
				
				for (auto iChild = m_Children.f_GetIterator()
					;iChild
					;++iChild)
				{
					(*iChild).f_Feed(_Stream, _StringTable);
				}
			}

			template <typename tf_CStream, typename tf_CStr, typename tf_CIndex>
			void f_Consume(tf_CStream &_Stream, TCStringTable<tf_CStr, tf_CIndex> const &_StringTable)
			{
				m_Key.f_Consume(_Stream, _StringTable);
				_StringTable.f_ConsumeString(_Stream, m_Data);
				uint64 nItems;
				fg_ConsumeLenFromStream(_Stream, nItems);
				while (nItems)
				{
					TCRegistry *pChild = DMibNew TCRegistry(this);
					auto Cleanup = g_OnScopeExit > [&]
						{
							delete pChild;
						}
					;
					pChild->f_Consume(_Stream, _StringTable);
					Cleanup.f_Clear();
					m_Children.f_Insert(pChild);
					--nItems;
				}
			}


			template <typename t_CStream>
			void f_FeedWithStringTable(t_CStream &_Stream) const
			{
				NStream::TCBinaryStreamNull<> NullStream;
				TCStringTable<t_CKeyStr, uint32> StringTable;
				// Build string table;
				f_Feed(NullStream, StringTable);

				// Save string table
				_Stream << StringTable;

				// Feed for real
				f_Feed(_Stream, StringTable);
			}

			template <typename t_CStream>
			void f_FeedWithStringTable(t_CStream &_Stream)
			{
				TCStringTable<t_CKeyStr, uint32> StringTable;
				_Stream >> StringTable;
				f_Consume(_Stream, StringTable);
			}

		};

		typedef TCRegistry<NStr::CStr, NStr::CStr, TCRegistryKeyStrMulti<NStr::CStr> > CRegistry_CStr;
		typedef TCRegistry<NStr::CWStr, NStr::CWStr, TCRegistryKeyStrMulti<NStr::CWStr> > CRegistry_CWStr;
		typedef TCRegistry<NStr::CUStr, NStr::CUStr, TCRegistryKeyStrMulti<NStr::CUStr> > CRegistry_CUStr;

		typedef TCRegistry<NStr::CStr, NStr::CStr, TCRegistryKeyStrMulti<NStr::CStr>, true> CRegistryPreserveOrder_CStr;
		typedef TCRegistry<NStr::CWStr, NStr::CWStr, TCRegistryKeyStrMulti<NStr::CWStr>, true> CRegistryPreserveOrder_CWStr;
		typedef TCRegistry<NStr::CUStr, NStr::CUStr, TCRegistryKeyStrMulti<NStr::CUStr>, true> CRegistryPreserveOrder_CUStr;

		typedef TCRegistry<NStr::CStr, NStr::CStr, TCRegistryKeyStrPreserve<NStr::CStr> > CRegistryPreserve_CStr;
		typedef TCRegistry<NStr::CWStr, NStr::CWStr, TCRegistryKeyStrPreserve<NStr::CWStr> > CRegistryPreserve_CWStr;
		typedef TCRegistry<NStr::CUStr, NStr::CUStr, TCRegistryKeyStrPreserve<NStr::CUStr> > CRegistryPreserve_CUStr;

		typedef TCRegistry<NStr::CStr, NStr::CStr, TCRegistryKeyStrPreserve<NStr::CStr>, true> CRegistryPreserveAndOrder_CStr;
		typedef TCRegistry<NStr::CWStr, NStr::CWStr, TCRegistryKeyStrPreserve<NStr::CWStr>, true> CRegistryPreserveAndOrder_CWStr;
		typedef TCRegistry<NStr::CUStr, NStr::CUStr, TCRegistryKeyStrPreserve<NStr::CUStr>, true> CRegistryPreserveAndOrder_CUStr;

	}

	namespace NStream
	{


		template <typename t_CNamedStream, typename t_CKeyStr, typename t_CData, typename t_CRegistryKey, typename t_CKeyStrInfo, bool t_bPreserveOrder>
		class TCBinaryStreamTypeReferenceNamed<t_CNamedStream, NMib::NStream::TCNamedStreamInfo<t_CKeyStrInfo, NRegistry::TCRegistry<t_CKeyStr, t_CData, t_CRegistryKey, t_bPreserveOrder>> >
		{
		public:
			typedef NRegistry::TCRegistry<t_CKeyStr, t_CData, t_CRegistryKey, t_bPreserveOrder> CDataRegistry;

			template <typename t_CAddTo, typename t_CAdd>
			static void fsr_Add(t_CAddTo *_pAddTo, const t_CAdd *_pAdd)
			{
				auto Iter = _pAdd->f_GetChildIterator();
				while (Iter)
				{
					const t_CAdd *pReg = Iter;
					++Iter;
					t_CAddTo *pChild = _pAddTo->f_CreateChild(pReg->f_GetName(), true);
					pChild->f_SetThisValue(pReg->f_GetThisValue());
					fsr_Add(pChild, pReg);
				}
			}

			static void fs_Feed(t_CNamedStream &_Stream,  NMib::NStream::TCNamedStreamInfo<t_CKeyStrInfo, CDataRegistry const> const &_Data)
			{
				const CDataRegistry &Registry = _Data.f_GetValue();
				fsr_Add(_Stream.f_CreateChild(_Data.f_GetKey()), &Registry);
			}

			static void fs_Consume(t_CNamedStream &_Stream, NMib::NStream::TCNamedStreamInfo<t_CKeyStrInfo, CDataRegistry> const &_Data)
			{
				CDataRegistry &DestReg = _Data.f_GetValue();

				t_CNamedStream *pSourceReg = _Stream.f_GetChild(_Data.f_GetKey());
				if (pSourceReg)
				{
					DestReg.f_Clear();
					fsr_Add(&DestReg, pSourceReg);
				}
				else
					DestReg = _Data.f_GetDefault();				
			}
		};

		template <typename t_CKeyStrStream, typename t_CDataStream, typename t_CRegistryKeyStream, typename t_CKeyStr, typename t_CData, bool t_bPreserveOrder> 
		class TCBinaryStreamTypeReference<NRegistry::TCRegistry<t_CKeyStrStream, t_CDataStream, t_CRegistryKeyStream, t_bPreserveOrder>, NMib::NStream::TCNamedStreamInfo<t_CKeyStr, t_CData> >
		{
			typedef NRegistry::TCRegistry<t_CKeyStrStream, t_CDataStream, t_CRegistryKeyStream, t_bPreserveOrder> CStreamRegistry;
		public:
			static void fs_Consume(CStreamRegistry &_Stream, NMib::NStream::TCNamedStreamInfo<t_CKeyStr, t_CData> const &_Data)
			{
				TCBinaryStreamTypeReferenceNamed<CStreamRegistry, NMib::NStream::TCNamedStreamInfo<t_CKeyStr, t_CData> >::fs_Consume(_Stream, _Data);
			}
		};
		template <typename t_CKeyStrStream, typename t_CDataStream, typename t_CRegistryKeyStream, typename t_CKeyStr, typename t_CData, bool t_bPreserveOrder>
		class TCBinaryStreamTypeReference<NRegistry::TCRegistry<t_CKeyStrStream, t_CDataStream, t_CRegistryKeyStream, t_bPreserveOrder>, NMib::NStream::TCNamedStreamInfo<t_CKeyStr, t_CData const> >
		{
			typedef NRegistry::TCRegistry<t_CKeyStrStream, t_CDataStream, t_CRegistryKeyStream, t_bPreserveOrder> CStreamRegistry;
		public:
			static void fs_Feed(CStreamRegistry &_Stream, NMib::NStream::TCNamedStreamInfo<t_CKeyStr, t_CData const> const &_Data)
			{
				TCBinaryStreamTypeReferenceNamed<CStreamRegistry, NMib::NStream::TCNamedStreamInfo<t_CKeyStr, t_CData> >::fs_Feed(_Stream, _Data);
			}
		};

	}
}

