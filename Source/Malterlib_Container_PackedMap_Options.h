// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Core/Core>

#include "Malterlib_Container_PackedMap_Options.h"

namespace NMib::NContainer
{
	struct CPackedMapOptions
	{
		umint m_SegmentSize = 64;                                       // Fixed segment size (slots per segment) when m_SegmentBytes == 0
		umint m_SegmentBytes = 0;                                       // Target segment size in bytes (0 = use m_SegmentSize)
		bool m_bAdaptive = true;                                       // Enable adaptive gap placement
		bool m_bShrink = true;                                         // Allow shrinking on underflow
		umint m_MinSegments = 1;                                        // Minimum number of segments
		bool m_bDetectorStoresKeys = true;                             // Allow detector to store key copies
		bool m_bStats = false;                                         // Track runtime statistics
		uint16 m_DetectorQueueSize = 8;                                // Per-segment detector queue size (ideally O(log n))
		bool m_bScanOriented = false;                                  // Scan-oriented thresholds and resize rules
		uint16 m_MaxCapacityBits = fg_Min(sizeof(void *) * 8 - 2, 51); // Max number of elements you can store in the container

		// Density thresholds from RMA paper
		pfp64 m_LeafLowerBound = 0.08;                                 // Leaf lower bound (rho1)
		pfp64 m_RootLowerBound = 0.30;                                 // Root lower bound (rhoh)
		pfp64 m_LeafUpperBound = 1.0;                                  // Leaf upper bound (tau1)
		pfp64 m_RootUpperBound = 0.75;                                 // Root upper bound (tauh)
		pfp64 m_ResizeThreshold = 0.30;                                // Shrink threshold
	};

	constexpr CPackedMapOptions fg_PackedMapOptionsScanOriented(umint _SegmentSize = 64, umint _SegmentBytes = 0)
	{
		return CPackedMapOptions
		{
			.m_SegmentSize = _SegmentSize
			, .m_SegmentBytes = _SegmentBytes
			, .m_bScanOriented = true
			, .m_LeafLowerBound = 0.0
			, .m_RootLowerBound = 0.75
			, .m_ResizeThreshold = 0.50
		};
	}
}
