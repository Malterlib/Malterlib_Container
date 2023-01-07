// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	struct CMapSet
	{
		constexpr auto operator <=> (CMapSet const &_Other) const = default;
	};

	template <typename t_CKey>
	struct TCMapNode<t_CKey, CMapSet> : public CMapNodeBase
	{
		using CKey = t_CKey;
		using CValue = t_CKey;

		inline_small TCMapNode() = default;

		template <typename tf_CKey>
		explicit inline_small TCMapNode(tf_CKey &&_Key);
		template <typename tf_CKey, typename tf_CArg0>
		explicit inline_small TCMapNode(tf_CKey &&_Key, tf_CArg0 &&_Arg0);

		inline_small static mint fs_GetOffset();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		inline_small t_CKey &f_Value();
		inline_small const t_CKey &f_Value() const;

		constexpr static bool mc_bHasValue = false;

		DMibNoUniqueAddress t_CKey m_Key;
	};
}

namespace NMib::NContainer::NPrivate
{
	template <typename t_CKey>
	struct TCMapUserData<t_CKey, NContainer::CMapSet>
	{
		using CType = t_CKey;
	};
}

