// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NStream
{
	template <typename t_CStream, typename t_CData, typename t_CAllocator, typename t_COptions>
	class TCBinaryStreamTypeReference<t_CStream, NContainer::TCVector<t_CData, t_CAllocator, t_COptions> >
	{
	public:
		static void fs_Feed(t_CStream &_Stream, NContainer::TCVector<t_CData, t_CAllocator, t_COptions> const &_Data)
		{
			mint nItems = _Data.f_GetLen();

			fg_FeedLenToStream(_Stream, nItems);

			for (mint i = 0; i < nItems; ++i)
				_Stream << _Data[i];
		}

		static void fs_Feed(t_CStream &_Stream, NContainer::TCVector<t_CData, t_CAllocator, t_COptions> &&_Data)
		{
			mint nItems = _Data.f_GetLen();

			fg_FeedLenToStream(_Stream, nItems);

			for (mint i = 0; i < nItems; ++i)
				_Stream << fg_Move(_Data[i]);
		}

		static void fs_Consume(t_CStream &_Stream, NContainer::TCVector<t_CData, t_CAllocator, t_COptions> &_Data)
		{
			uint64 nItems;
			fg_ConsumeLenFromStream(_Stream, nItems);
			fg_CheckLengthLimit(_Stream, nItems);
			_Data.f_SetLen(nItems);

			for (mint i = 0; i < nItems; ++i)
				_Stream >> _Data[i];
		}
	};

	// Optimize for simple data cases

#define DMibListVectorStreamSimpleImplement(_DataType) \
	template <typename t_CStream, typename t_CAllocator, typename t_COptions>\
	class TCBinaryStreamTypeReference<t_CStream, NContainer::TCVector<_DataType, t_CAllocator, t_COptions> >\
	{\
	public:\
		static void fs_Feed(t_CStream &_Stream, NContainer::TCVector<_DataType, t_CAllocator, t_COptions> const &_Data)\
		{\
			mint nItems = _Data.f_GetLen();\
			fg_FeedLenToStream(_Stream, nItems);\
			if (sizeof(_DataType) != 1)\
				fg_FeedEndianArrayToStream(_Stream, _Data.f_GetArray(), nItems, _Stream.f_Endian());\
			else\
				_Stream.f_FeedBytes(_Data.f_GetArray(), nItems * sizeof(_DataType));\
		}\
		static void fs_Consume(t_CStream &_Stream, NContainer::TCVector<_DataType, t_CAllocator, t_COptions> &_Data)\
		{\
			uint64 nItems;\
			fg_ConsumeLenFromStream(_Stream, nItems);\
			fg_CheckLengthLimit(_Stream, nItems);\
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

	template <typename t_CStream>
	class TCBinaryStreamTypeReference<t_CStream, NContainer::CByteVector> : public TCBinaryStreamTypeReference<t_CStream, NContainer::TCVector<uint8>>
	{
	};

	template <typename t_CStream>
	class TCBinaryStreamTypeReference<t_CStream, NContainer::CSecureByteVector> : public TCBinaryStreamTypeReference<t_CStream, NContainer::TCVector<uint8, NMemory::CAllocator_HeapSecure>>
	{
	};
}
