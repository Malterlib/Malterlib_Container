// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NStream
{
	namespace NPrivate
	{
		// Detects streams that can adopt a shared byte buffer by reference instead of
		// copying its contents (segmented storage streams)
		template <typename t_CStream, typename t_CEnableIf = void>
		struct TCStreamHasFeedShared
		{
			static constexpr bool mc_Value = false;
		};

		template <typename t_CStream>
		struct TCStreamHasFeedShared
		<
			t_CStream
			, TCEnableIf
			<
				!NTraits::cIsSame
				<
					decltype(fg_GetReference<t_CStream>().f_FeedShared(fg_GetType<NContainer::CSharedByteVector>())), NPrivate::CDummy
				>
			>
		>
		{
			static constexpr bool mc_Value = true;
		};

		// Detects streams that can hand out consumed bytes as views of their backing shared
		// buffer instead of copying them
		template <typename t_CStream, typename t_CEnableIf = void>
		struct TCStreamHasConsumeShared
		{
			static constexpr bool mc_Value = false;
		};

		template <typename t_CStream>
		struct TCStreamHasConsumeShared
		<
			t_CStream
			, TCEnableIf
			<
				!NTraits::cIsSame
				<
					decltype(fg_GetReference<t_CStream>().f_ConsumeShared(fg_GetType<umint>())), NPrivate::CDummy
				>
			>
		>
		{
			static constexpr bool mc_Value = true;
		};
	}

	// Wire compatible with a byte vector (length prefix + bytes): either side of a stream
	// can use CSharedByteVector or CIOByteVector interchangeably
	template <typename t_CStream>
	class TCBinaryStreamTypeReference<t_CStream, NContainer::CSharedByteVector>
	{
	public:
		static void fs_Feed(t_CStream &_Stream, NContainer::CSharedByteVector const &_Data)
		{
			umint nItems = _Data.f_GetLen();
			fg_FeedLenToStream(_Stream, nItems);
			if constexpr (NPrivate::TCStreamHasFeedShared<t_CStream>::mc_Value)
			{
				if (_Stream.f_CanAdopt())
				{
					_Stream.f_FeedShared(NContainer::CSharedByteVector(_Data));
					return;
				}
			}

			_Stream.f_FeedBytes(_Data.f_GetArray(), nItems);
		}

		static void fs_Feed(t_CStream &_Stream, NContainer::CSharedByteVector &&_Data)
		{
			umint nItems = _Data.f_GetLen();
			fg_FeedLenToStream(_Stream, nItems);
			if constexpr (NPrivate::TCStreamHasFeedShared<t_CStream>::mc_Value)
			{
				if (_Stream.f_CanAdopt())
				{
					_Stream.f_FeedShared(fg_Move(_Data));
					return;
				}
			}

			_Stream.f_FeedBytes(_Data.f_GetArray(), nItems);
		}

		static void fs_Consume(t_CStream &_Stream, NContainer::CSharedByteVector &_Data)
		{
			uint64 nItems;
			fg_ConsumeLenFromStream(_Stream, nItems);
			fg_CheckLengthLimit(_Stream, nItems);

			if constexpr (NPrivate::TCStreamHasConsumeShared<t_CStream>::mc_Value)
			{
				_Data = _Stream.f_ConsumeShared(nItems);
			}
			else
			{
				NContainer::CIOByteVector Vector;
				Vector.f_SetLen(nItems);
				_Stream.f_ConsumeBytes(Vector.f_GetArray(), nItems);

				_Data = NContainer::CSharedByteVector(fg_Move(Vector));
			}
		}
	};
}
