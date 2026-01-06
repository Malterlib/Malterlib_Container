// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename t_CTransform>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_Transform(const t_CTransform &_fTransformer)
	{
		NContainer::TCVector<TCRegistry *> Children;
		{
			auto Iter = mp_Children.f_GetIterator();
			while (Iter)
			{
				Children.f_Insert(Iter);
				++Iter;
			}
		}

		for (auto Iter = Children.f_GetIterator(); Iter; ++Iter)
		{
			TCRegistry **pIter = Iter;
			(*pIter)->f_Transform(_fTransformer);
		}

		_fTransformer.f_Transform(this);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CFunctor>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_TransformFunc(const tf_CFunctor &_fTransformer)
	{
		NContainer::TCVector<TCRegistry *> Children;
		{
			auto Iter = mp_Children.f_GetIterator();
			while (Iter)
			{
				Children.f_Insert(Iter);
				++Iter;
			}
		}

		for (auto Iter = Children.f_GetIterator(); Iter; ++Iter)
		{
			TCRegistry **pIter = Iter;
			(*pIter)->f_TransformFunc(_fTransformer);
		}

		_fTransformer(*this);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CFunctor>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_ForEachInTree(const tf_CFunctor &_fFunctor) const
	{
		auto Iter = mp_Children.f_GetIterator();
		while (Iter)
		{
			Iter->f_ForEachInTree(_fFunctor);
			++Iter;
		}

		_fFunctor(*this);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChildIterator() const -> CIterator
	{
		return mp_Children.f_GetIterator();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CStr>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChildIterator(tf_CStr const &_Str) const -> typename CTree::CIterator
	{
		typename CTree::CIterator Iter;
		Iter.f_InitForSearch(mp_Children.m_Tree);
		mp_Key.fs_FindIterator(Iter, _Str);

		return Iter;
	}
}
