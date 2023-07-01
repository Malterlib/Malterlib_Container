// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename t_CNode, typename t_CKey>
	struct TCMapNodeCompare_Default
	{
		t_CKey const &operator () (t_CNode const &_Left) const
		{
			return _Left.mp_Key;
		}
	};

	template <typename t_CNode, typename t_CCompare>
	struct TCMapNodeCompare_Custom
	{
		auto operator () (t_CNode const &_Left, t_CNode const &_Right) const
		{
			return t_CCompare()(_Left.mp_Key, _Right.mp_Key);
		}

		auto operator () (t_CNode &_Left, t_CNode &_Right) const
		{
			return t_CCompare()(_Left.mp_Key, _Right.mp_Key);
		}

		template <typename tf_CKey>
		auto operator () (t_CNode const &_Left, tf_CKey const &_Right) const
		{
			return t_CCompare()(_Left.mp_Key, _Right);
		}

		template <typename tf_CKey>
		auto operator () (tf_CKey const &_Left, t_CNode const &_Right) const
		{
			return t_CCompare()(_Left, _Right.mp_Key);
		}
	};
}
