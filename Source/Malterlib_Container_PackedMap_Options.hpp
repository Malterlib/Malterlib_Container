// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	constexpr CPackedMapOptions fg_PackedMapOptionsScanOriented(umint _SegmentSize, umint _SegmentBytes)
	{
		return CPackedMapOptions
			{
				.m_SegmentSize = _SegmentSize
				, .m_SegmentBytes = _SegmentBytes
				, .m_bScanOriented = true
				, .m_LeafLowerBound = 0.0
				, .m_RootLowerBound = 0.75
				, .m_ResizeThreshold = 0.50
			}
		;
	}
}
