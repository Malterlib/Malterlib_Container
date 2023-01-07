// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	struct CMapNodeBase
	{
		NIntrusive::TCAVLLink<> m_Link;
	};

	template <typename t_CKey, typename t_CValue>
	struct TCMapNode : public CMapNodeBase
	{
		using CKey = t_CKey;
		using CValue = t_CValue;

		TCMapNode() = default;

		template <typename tf_CKey, typename... tfp_CArg>
		explicit inline_small TCMapNode(tf_CKey &&_Key, tfp_CArg && ... p_Args);

		inline_small static mint fs_GetOffset();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		inline_small t_CValue &f_Value();
		inline_small const t_CValue &f_Value() const;

		constexpr static bool mc_bHasValue = true;

		DMibNoUniqueAddress t_CKey m_Key;
		DMibNoUniqueAddress t_CValue m_Value;
	};
}
