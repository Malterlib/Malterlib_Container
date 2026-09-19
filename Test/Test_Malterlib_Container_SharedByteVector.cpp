// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Container/SharedByteVector>
#include <Mib/Stream/ByteVector>
#include <Mib/Stream/BinaryStorage>
#include <Mib/Test/Exception>

namespace
{
	using namespace NMib;
	using namespace NMib::NContainer;
	using namespace NMib::NStorage;
	using namespace NMib::NStream;

	struct CByteOwner : CVirtualDestroyBase
	{
		explicit CByteOwner(umint *_pDestroyed)
			: m_pDestroyed(_pDestroyed)
		{
		}

		~CByteOwner() override
		{
			++*m_pDestroyed;
		}

		umint *m_pDestroyed;
		uint8 m_Data[256] = {10, 20, 30, 40};

		static_assert(sizeof(m_Data) >= CBinaryStorage::mc_AdoptThreshold);
	};

	struct CSharedByteVector_Tests : NTest::CTest
	{
		void f_DoTests()
		{
			DMibTestSuite("Views")
			{
				DMibTestCategory("Empty")
				{
					CSharedByteVector Empty;

					DMibExpectTrue(Empty.f_IsEmpty());
					DMibExpectFalse(Empty.f_HasBuffer());
					DMibExpectTrue(Empty.f_GetArray() == nullptr);

					TCSharedPointer<CIOByteVector const> pNull;
					CSharedByteVector Null(pNull);

					DMibExpectTrue(Null == Empty);
					DMibExpectFalse(Null.f_HasBuffer());

					CSharedByteVector Owned(CIOByteVector{});
					CSharedByteVector Slice(Owned, 0, 0);

					DMibExpectTrue(Slice.f_IsEmpty());
					DMibExpectTrue(Slice.f_HasBuffer());
					DMibExpectTrue(Slice == Empty);

					Slice.f_Clear();

					DMibExpectFalse(Slice.f_HasBuffer());
				};

				DMibTestCategory("OwnershipAndMoves")
				{
					umint nDestroyed = 0;
					TCSharedPointer<CByteOwner> pOwner = fg_Construct(&nDestroyed);
					auto *pData = pOwner->m_Data;
					TCSharedPointer<CVirtualDestroyBase const> pErased = pOwner.f_ShareAsConst();
					CSharedByteVector Original(pData, 4, fg_Move(pErased));
					pOwner.f_Clear();

					CSharedByteVector Copy = Original;
					CSharedByteVector Slice(Copy, 1, 2);
					auto const &OriginalAlias = Original;

					Original = OriginalAlias;
					Original.f_Clear();
					Copy.f_Clear();

					DMibExpect(nDestroyed, ==, umint(0));
					DMibExpect(Slice.f_GetArray(), ==, pData + 1);
					DMibExpect(Slice.f_GetLen(), ==, umint(2));
					DMibExpect(Slice.f_GetArray()[1], ==, uint8(30));

					CSharedByteVector Moved(fg_Move(Slice));

					DMibExpectTrue(Slice.f_IsEmpty());
					DMibExpectFalse(Slice.f_HasBuffer());
					DMibExpectTrue(Slice.f_GetArray() == nullptr);

					CSharedByteVector Assigned;
					Assigned = fg_Move(Moved);

					DMibExpectTrue(Moved.f_IsEmpty());
					DMibExpectFalse(Moved.f_HasBuffer());
					DMibExpectTrue(Moved.f_GetArray() == nullptr);

					{
						DMibTestPath("AfterMoves");
						DMibExpect(nDestroyed, ==, umint(0));
					}

					Assigned.f_Clear();

					DMibExpect(nDestroyed, ==, umint(1));
				};

				DMibTestCategory("SharedVectorAndSubview")
				{
					TCSharedPointer<CIOByteVector> pMutable = fg_Construct();
					for (uint8 Byte = 1; Byte <= 4; ++Byte)
						pMutable->f_InsertLast(Byte);

					auto *pData = pMutable->f_GetArray();
					auto pVector = pMutable.f_ShareAsConst();
					pMutable.f_Clear();

					CSharedByteVector View(pVector, 1, 3);
					CSharedByteVector Nested(View, 1, 1);
					CSharedByteVector End(View, 3, 0);

					pVector.f_Clear();
					View.f_Clear();

					DMibExpect(Nested.f_GetArray(), ==, pData + 2);
					DMibExpect(Nested.f_GetArray()[0], ==, uint8(3));
					DMibExpect(End.f_GetArray(), ==, pData + 4);
					DMibExpectTrue(End.f_IsEmpty());
					DMibExpectTrue(End.f_HasBuffer());
				};

				DMibTestCategory("AllocationAndCoalescing")
				{
					uint8 *pWritable = nullptr;
					auto Bytes = CSharedByteVector::fs_AllocateExact(8, pWritable);
					for (umint i = 0; i < 8; ++i)
						pWritable[i] = uint8(i);

					DMibExpect(Bytes.f_GetLen(), ==, umint(8));
					DMibExpect(Bytes.f_GetArray(), ==, pWritable);

					CSharedByteVector Left(Bytes, 0, 3);
					CSharedByteVector Right(Bytes, 3, 5);

					DMibExpectFalse(Left.f_TryAppendContiguous(CSharedByteVector(Bytes, 4, 2)));
					DMibExpectFalse(Left.f_TryAppendContiguous(CSharedByteVector(Bytes, 2, 2)));
					DMibExpect(Left.f_GetLen(), ==, umint(3));

					DMibExpectTrue(Left.f_TryAppendContiguous(Right));
					DMibExpectTrue(Left == Bytes);
					DMibExpect(Right.f_GetLen(), ==, umint(5));

					{
						DMibTestPath("EmptyNext");
						DMibExpectTrue(Left.f_TryAppendContiguous(CSharedByteVector{}));
						DMibExpectTrue(Left == Bytes);
					}

					CSharedByteVector Borrowed(pWritable + 3, 5, {});
					CSharedByteVector Prefix(Bytes, 0, 3);

					DMibExpectFalse(Prefix.f_TryAppendContiguous(Borrowed));
					DMibExpect(Prefix.f_GetLen(), ==, umint(3));

					CSharedByteVector BorrowedPrefix(pWritable, 3, {});

					DMibExpectFalse(BorrowedPrefix.f_TryAppendContiguous(Borrowed));
					DMibExpect(BorrowedPrefix.f_GetLen(), ==, umint(3));

					Bytes.f_Clear();

					DMibExpect(Left.f_GetArray()[7], ==, uint8(7));
				};

				DMibTestCategory("AdoptingStream")
				{
					umint nDestroyed = 0;
					CSharedByteVector Restored;
					uint8 const *pData = nullptr;

					{
						TCSharedPointer<CByteOwner> pOwner = fg_Construct(&nDestroyed);
						pData = pOwner->m_Data;
						TCSharedPointer<CVirtualDestroyBase const> pErased = pOwner.f_ShareAsConst();
						CSharedByteVector Original(pData, sizeof(CByteOwner::m_Data), fg_Move(pErased));
						TCBinaryStreamStorage<> Output;

						Output << fg_Move(Original);
						pOwner.f_Clear();

						DMibExpectFalse(Original.f_HasBuffer());

						TCBinaryStreamStoragePtr<> Input;
						Input.f_OpenRead(Output.f_GetStorage());
						Input >> Restored;

						DMibExpectTrue(Input.f_IsAtEndOfStream());
					}

					DMibExpect(nDestroyed, ==, umint(0));
					DMibExpect(Restored.f_GetArray(), ==, pData);
					DMibExpect(Restored.f_GetLen(), ==, umint(sizeof(CByteOwner::m_Data)));
					DMibExpect(Restored.f_GetArray()[3], ==, uint8(40));

					Restored.f_Clear();

					DMibExpect(nDestroyed, ==, umint(1));
				};

				DMibTestCategory("EqualityAndWireFormat")
				{
					CIOByteVector Data;
					for (auto Byte : {3, 1, 4, 1, 5})
						Data.f_InsertLast(uint8(Byte));

					CSharedByteVector Shared{CIOByteVector(Data)};
					CSharedByteVector Equal{CIOByteVector(Data)};

					DMibExpectTrue(Shared == Equal);
					DMibExpectFalse(Shared == CSharedByteVector(Shared, 0, 4));

					CIOByteVector Other = Data;
					Other[2] = 9;

					DMibExpectFalse(Shared == CSharedByteVector(fg_Move(Other)));

					auto Encoded = fg_ToByteVector(Shared);

					DMibExpectTrue(Encoded == fg_ToByteVector(Data));
					DMibExpectTrue(fg_FromByteVector<CIOByteVector>(Encoded) == Data);
					DMibExpectTrue(fg_FromByteVector<CSharedByteVector>(Encoded) == Shared);
				};
			};
		}
	};
}

DMibTestRegister(CSharedByteVector_Tests, Malterlib::Container);
