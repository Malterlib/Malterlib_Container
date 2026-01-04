// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename t_CNode, typename t_CKey>
	struct TCMapNodeCompare_Default
	{
		TCMapNodeCompare_Default() = default;

		TCMapNodeCompare_Default(TCMapNodeCompare_Default const &) = default;
		TCMapNodeCompare_Default(TCMapNodeCompare_Default &&) = default;

		TCMapNodeCompare_Default &operator = (TCMapNodeCompare_Default const &) = default;
		TCMapNodeCompare_Default &operator = (TCMapNodeCompare_Default &&) = default;

		template <typename tf_CNode, typename tf_CKey>
		TCMapNodeCompare_Default(TCMapNodeCompare_Default<tf_CNode, tf_CKey> const &)
		{
		}

		template <typename tf_CNode, typename tf_CKey>
		TCMapNodeCompare_Default(TCMapNodeCompare_Default<tf_CNode, tf_CKey> &&)
		{
		}

		template <typename tf_CNode, typename tf_CKey>
		TCMapNodeCompare_Default &operator = (TCMapNodeCompare_Default<tf_CNode, tf_CKey> const &)
		{
			return *this;
		}

		template <typename tf_CNode, typename tf_CKey>
		TCMapNodeCompare_Default &operator = (TCMapNodeCompare_Default<tf_CNode, tf_CKey> &&)
		{
			return *this;
		}

		template <typename tf_CKeyLeft, typename tf_CKeyRight>
		auto f_CompareKeys(tf_CKeyLeft const &_Left, tf_CKeyRight const &_Right) const
		{
			return _Left <=> _Right;
		}

		t_CKey const &operator () (t_CNode const &_Left) const
		{
			return _Left.mp_Key;
		}
	};

	template
	<
		typename t_CNode
		, typename t_CCompare
#if defined(DCompiler_MSVC) || defined(DCompiler_clang_cl)
		, bool t_bIsEmpty
#endif
	>
	struct TCMapNodeCompare_Custom
	{
		TCMapNodeCompare_Custom() = default;

		TCMapNodeCompare_Custom(TCMapNodeCompare_Custom const &) = default;
		TCMapNodeCompare_Custom(TCMapNodeCompare_Custom &&) = default;

		TCMapNodeCompare_Custom &operator = (TCMapNodeCompare_Custom const &) = default;
		TCMapNodeCompare_Custom &operator = (TCMapNodeCompare_Custom &&) = default;

		template <typename tf_CNode>
		TCMapNodeCompare_Custom(TCMapNodeCompare_Custom<tf_CNode, t_CCompare> const &_Other)
			: mp_Compare(_Other.mp_Compare)
		{
		}

		template <typename tf_CNode>
		TCMapNodeCompare_Custom(TCMapNodeCompare_Custom<tf_CNode, t_CCompare> &&_Other)
			: mp_Compare(fg_Move(_Other.mp_Compare))
		{
		}

		template <typename tf_CNode>
		TCMapNodeCompare_Custom &operator = (TCMapNodeCompare_Custom<tf_CNode, t_CCompare> const &_Other)
		{
			mp_Compare = _Other.mp_Compare;
			return *this;
		}

		template <typename tf_CNode>
		TCMapNodeCompare_Custom &operator = (TCMapNodeCompare_Custom<tf_CNode, t_CCompare> &&_Other)
		{
			mp_Compare = fg_Move(_Other.mp_Compare);
			return *this;
		}

		template <typename ...tfp_CParams>
		TCMapNodeCompare_Custom(tfp_CParams && ...p_Params)
			: mp_Compare(fg_Forward<tfp_CParams>(p_Params)...)
		{
		}

		template <typename tf_CKeyLeft, typename tf_CKeyRight>
		auto f_CompareKeys(tf_CKeyLeft const &_Left, tf_CKeyRight const &_Right) const
		{
			return mp_Compare(_Left, _Right);
		}

		auto operator () (t_CNode const &_Left, t_CNode const &_Right) const
		{
			return mp_Compare(_Left.mp_Key, _Right.mp_Key);
		}

		auto operator () (t_CNode &_Left, t_CNode &_Right) const
		{
			return mp_Compare(_Left.mp_Key, _Right.mp_Key);
		}

		template <typename tf_CKey>
		auto operator () (t_CNode const &_Left, tf_CKey const &_Right) const
		{
			return mp_Compare(_Left.mp_Key, _Right);
		}

		template <typename tf_CKey>
		auto operator () (tf_CKey const &_Left, t_CNode const &_Right) const
		{
			return mp_Compare(_Left, _Right.mp_Key);
		}

	private:
#if !defined(DCompiler_MSVC) && !defined(DCompiler_clang_cl)
		DMibNoUniqueAddress
#endif
		t_CCompare mp_Compare;
	};

#if defined(DCompiler_MSVC) || defined(DCompiler_clang_cl)
	template <typename t_CNode, typename t_CCompare>
	struct TCMapNodeCompare_Custom<t_CNode, t_CCompare, true>
	{
		TCMapNodeCompare_Custom() = default;

		TCMapNodeCompare_Custom(TCMapNodeCompare_Custom const &) = default;
		TCMapNodeCompare_Custom(TCMapNodeCompare_Custom &&) = default;

		TCMapNodeCompare_Custom &operator = (TCMapNodeCompare_Custom const &) = default;
		TCMapNodeCompare_Custom &operator = (TCMapNodeCompare_Custom &&) = default;

		template <typename tf_CNode>
		TCMapNodeCompare_Custom(TCMapNodeCompare_Custom<tf_CNode, t_CCompare> const &_Other)
		{
		}

		template <typename tf_CNode>
		TCMapNodeCompare_Custom(TCMapNodeCompare_Custom<tf_CNode, t_CCompare> &&_Other)
		{
		}

		template <typename tf_CNode>
		TCMapNodeCompare_Custom &operator = (TCMapNodeCompare_Custom<tf_CNode, t_CCompare> const &_Other)
		{
			return *this;
		}

		template <typename tf_CNode>
		TCMapNodeCompare_Custom &operator = (TCMapNodeCompare_Custom<tf_CNode, t_CCompare> &&_Other)
		{
			return *this;
		}

		template <typename ...tfp_CParams>
		TCMapNodeCompare_Custom(tfp_CParams && ...p_Params)
		{
		}

		template <typename tf_CKeyLeft, typename tf_CKeyRight>
		auto f_CompareKeys(tf_CKeyLeft const &_Left, tf_CKeyRight const &_Right) const
		{
			return t_CCompare()(_Left, _Right);
		}

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
#endif
}
