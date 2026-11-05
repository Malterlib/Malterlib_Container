// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Container/BitArrayPowerTwo>
#include <Mib/Stream/Streams/Vector>

namespace
{
	using namespace NMib;
	using namespace NMib::NStr;
	using namespace NMib::NContainer;
	using namespace NMib::NStream;

	struct CBitArrayPowerTwo_Tests : public NMib::NTest::CTest
	{
		void f_DoTests();
	};

	void CBitArrayPowerTwo_Tests::f_DoTests()
	{
		DMibTestSuite("Stream")
		{
			constexpr umint c_nBits = 37;
			using CBits = TCBitArrayPowerTwo<c_nBits>;

			// The bits of every level, one line per level, so a consumed array can be compared with the one it was fed from.
			auto fGetLevels = [](CBits const &_Bits)
				{
					CStr Levels;
					for (umint iLevel = 0; (c_nBits >> iLevel) > 0; ++iLevel)
					{
						for (umint iBit = 0; iBit < (c_nBits >> iLevel); ++iBit)
							Levels += _Bits.f_GetBit(iLevel, iBit) ? "1" : "0";
						Levels += "\n";
					}
					return Levels;
				}
			;

			static constexpr umint c_SetBits[] = {0, 5, 6, 17, 30, 36};

			CBits Original;
			for (umint Bit : c_SetBits)
				Original.f_SetBit(0, Bit, true);

			TCBinaryStreamMemory<> Stream;
			Original.f_Feed(Stream);

			CByteVector RawData = Stream.f_GetVector();
			TCBinaryStreamMemoryRef<> Input(RawData);

			CBits Consumed;
			Consumed.f_SetBit(0, 1, true);
			Consumed.f_Consume(Input);

			DMibExpect(fGetLevels(Consumed), ==, fGetLevels(Original));
		};
	}

	DMibTestRegister(CBitArrayPowerTwo_Tests, Malterlib::Container);
}
