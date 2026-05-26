// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Container/PagedByteVector>
#include <Mib/Test/Exception>

namespace
{
	class CPagedByteVector_Tests : public NMib::NTest::CTest
	{
	public:
		using CPagedByteVector = NMib::NContainer::CPagedByteVector;
		template <typename t_CInherit>
		using TCBinaryStreamPagedByteVectorPtr = NMib::NContainer::TCBinaryStreamPagedByteVectorPtr<t_CInherit>;
		using CStreamLittleEndian = NMib::NStream::CBinaryStreamLittleEndian;
		using CStreamBigEndian = NMib::NStream::CBinaryStreamBigEndian;

		// Fills the buffer with sequential bytes (0..nBytes-1 mod 256).
		static void fs_FillSequential(CPagedByteVector &_Buffer, umint _nBytes)
		{
			NMib::NContainer::CByteVector Source;
			Source.f_Reserve(_nBytes);
			for (umint i = 0; i < _nBytes; ++i)
				Source.f_InsertLast(uint8(i & 0xFF));

			_Buffer.f_InsertBack(Source.f_GetArray(), Source.f_GetLen());
		}

		void f_BasicReadFullBuffer()
		{
			DMibTestPath("Basic full-buffer read");

			CPagedByteVector Buffer(16);
			fs_FillSequential(Buffer, 32);

			TCBinaryStreamPagedByteVectorPtr<CStreamLittleEndian> Stream;
			Stream.f_OpenRead(Buffer);

			DMibExpect(Stream.f_IsValid(), ==, true);
			DMibExpect(umint(Stream.f_GetLength()), ==, umint(32));
			DMibExpect(umint(Stream.f_GetPosition()), ==, umint(0));
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, false);

			bool bAllMatch = true;
			for (umint i = 0; i < 32; ++i)
			{
				uint8 Byte = 0;
				Stream >> Byte;
				if (umint(Byte) != i)
					bAllMatch = false;
			}

			DMibExpect(bAllMatch, ==, true);
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, true);
			DMibExpect(umint(Stream.f_GetPosition()), ==, umint(32));
		}

		void f_CrossPageRead()
		{
			DMibTestPath("Cross-page byte-by-byte");

			CPagedByteVector Buffer(8);
			fs_FillSequential(Buffer, 24);

			TCBinaryStreamPagedByteVectorPtr<CStreamLittleEndian> Stream;
			Stream.f_OpenRead(Buffer);

			bool bAllMatch = true;
			for (umint i = 0; i < 24; ++i)
			{
				uint8 Byte = 0;
				Stream >> Byte;
				if (umint(Byte) != (i & 0xFF))
					bAllMatch = false;
			}

			DMibExpect(bAllMatch, ==, true);
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, true);
		}

		void f_MultiByteSpanningPages()
		{
			DMibTestPath("Multi-byte primitives spanning pages");

			CPagedByteVector Buffer(4);
			fs_FillSequential(Buffer, 16);

			// Big-endian read: byte i has value i.
			TCBinaryStreamPagedByteVectorPtr<CStreamBigEndian> Stream;
			Stream.f_OpenRead(Buffer);

			uint16 V16 = 0;
			Stream >> V16;
			DMibExpect(uint32(V16), ==, uint32((uint16(0) << 8) | uint16(1)));

			uint32 V32 = 0;
			Stream >> V32; // bytes 2..5: 0x02030405
			DMibExpect(V32, ==, uint32(0x02030405));

			uint64 V64 = 0;
			Stream >> V64; // bytes 6..13: 0x060708090A0B0C0D
			DMibExpect(V64, ==, uint64(0x060708090A0B0C0Dull));

			uint16 Tail = 0;
			Stream >> Tail; // bytes 14..15: 0x0E0F
			DMibExpect(uint32(Tail), ==, uint32(0x0E0F));

			DMibExpect(Stream.f_IsAtEndOfStream(), ==, true);
		}

		void f_BulkReadAcrossPages()
		{
			DMibTestPath("Bulk f_ConsumeBytes across many pages");

			CPagedByteVector Buffer(13); // odd page size to force varied splits
			fs_FillSequential(Buffer, 100);

			TCBinaryStreamPagedByteVectorPtr<CStreamLittleEndian> Stream;
			Stream.f_OpenRead(Buffer);

			uint8 Out[100] = {};
			Stream.f_ConsumeBytes(Out, sizeof(Out));

			bool bAllMatch = true;
			for (umint i = 0; i < 100; ++i)
			{
				if (umint(Out[i]) != (i & 0xFF))
					bAllMatch = false;
			}

			DMibExpect(bAllMatch, ==, true);
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, true);
		}

		void f_SubRange()
		{
			DMibTestPath("Sub-range stream open");

			CPagedByteVector Buffer(8);
			fs_FillSequential(Buffer, 40);

			// Open window [10, 30).
			TCBinaryStreamPagedByteVectorPtr<CStreamLittleEndian> Stream;
			Stream.f_OpenRead(Buffer, 10, 20);

			DMibExpect(umint(Stream.f_GetLength()), ==, umint(20));

			bool bAllMatch = true;
			for (umint i = 0; i < 20; ++i)
			{
				uint8 Byte = 0;
				Stream >> Byte;
				if (umint(Byte) != ((10 + i) & 0xFF))
					bAllMatch = false;
			}

			DMibExpect(bAllMatch, ==, true);
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, true);
		}

		void f_SubRangeAfterRemoveFront()
		{
			DMibTestPath("Sub-range after f_RemoveFront shifts first-page-start");

			CPagedByteVector Buffer(8);
			fs_FillSequential(Buffer, 40);
			Buffer.f_RemoveFront(5); // First-page-start is now 5; logical offset 0 maps to byte 5.

			TCBinaryStreamPagedByteVectorPtr<CStreamLittleEndian> Stream;
			Stream.f_OpenRead(Buffer);

			DMibExpect(umint(Stream.f_GetLength()), ==, umint(35));

			bool bAllMatch = true;
			for (umint i = 0; i < 35; ++i)
			{
				uint8 Byte = 0;
				Stream >> Byte;
				if (umint(Byte) != ((5 + i) & 0xFF))
					bAllMatch = false;
			}

			DMibExpect(bAllMatch, ==, true);
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, true);
		}

		void f_Seek()
		{
			DMibTestPath("Seek operations");

			CPagedByteVector Buffer(8);
			fs_FillSequential(Buffer, 32);

			TCBinaryStreamPagedByteVectorPtr<CStreamLittleEndian> Stream;
			Stream.f_OpenRead(Buffer);

			Stream.f_SetPosition(17);
			DMibExpect(umint(Stream.f_GetPosition()), ==, umint(17));
			uint8 SeekByteA = 0;
			Stream >> SeekByteA;
			DMibExpect(umint(SeekByteA), ==, umint(17));

			Stream.f_AddPosition(5);
			DMibExpect(umint(Stream.f_GetPosition()), ==, umint(23));
			uint8 SeekByteB = 0;
			Stream >> SeekByteB;
			DMibExpect(umint(SeekByteB), ==, umint(23));

			Stream.f_SetPositionFromEnd(-4);
			DMibExpect(umint(Stream.f_GetPosition()), ==, umint(28));
			uint8 SeekByteC = 0;
			Stream >> SeekByteC;
			DMibExpect(umint(SeekByteC), ==, umint(28));

			Stream.f_SetPosition(0);
			DMibExpect(umint(Stream.f_GetPosition()), ==, umint(0));
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, false);
		}

		void f_EmptyBuffer()
		{
			DMibTestPath("Empty buffer");

			CPagedByteVector Buffer(16);

			TCBinaryStreamPagedByteVectorPtr<CStreamLittleEndian> Stream;
			Stream.f_OpenRead(Buffer);

			DMibExpect(Stream.f_IsValid(), ==, true);
			DMibExpect(umint(Stream.f_GetLength()), ==, umint(0));
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, true);
		}

		void f_EndOfStreamThrows()
		{
			DMibTestPath("Reading past end throws");

			CPagedByteVector Buffer(8);
			fs_FillSequential(Buffer, 4);

			TCBinaryStreamPagedByteVectorPtr<CStreamLittleEndian> Stream;
			Stream.f_OpenRead(Buffer);

			uint32 V32 = 0;
			Stream >> V32; // consumes all 4 bytes
			DMibExpect(Stream.f_IsAtEndOfStream(), ==, true);

			DMibExpectExceptionType
				(
					[&]
					{
						uint8 Byte = 0;
						Stream >> Byte;
					}
					()
					, NMib::NException::CException
				)
			;
		}

		void f_DoTests()
		{
			DMibTestSuite("Read") -> void
			{
				f_BasicReadFullBuffer();
				f_CrossPageRead();
				f_MultiByteSpanningPages();
				f_BulkReadAcrossPages();
			};

			DMibTestSuite("Ranges") -> void
			{
				f_SubRange();
				f_SubRangeAfterRemoveFront();
				f_EmptyBuffer();
			};

			DMibTestSuite("Seek") -> void
			{
				f_Seek();
			};

			DMibTestSuite("Errors") -> void
			{
				f_EndOfStreamThrows();
			};
		}
	};

	DMibTestRegister(CPagedByteVector_Tests, Malterlib::Container);
}
