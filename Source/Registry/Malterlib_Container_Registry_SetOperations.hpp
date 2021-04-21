// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fpr_AddAllChildren(const t_CStr &_Path, NContainer::TCVector<t_CStr> &_Added) const
	{
		auto Iter = mp_Children.f_GetIterator();
		while (Iter)
		{
			const TCRegistry *pReg = Iter;
			++Iter;
			t_CStr Name = pReg->f_GetName();
			t_CStr Path = fsp_AppendPath(_Path, Name);
			_Added.f_Insert(Path);
			pReg->fpr_AddAllChildren(Path, _Added);
		}
	}


	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fpr_Merge(const TCRegistry *_pMergeWith)
	{
		// Find deleted
		{
			auto Iter = _pMergeWith->mp_Children.f_GetIterator();
			while (Iter)
			{
				const TCRegistry *pReg = Iter;
				++Iter;
				t_CStr Name = pReg->f_GetName();
				TCRegistry *pChild = f_SetValue(Name, pReg->f_GetThisValue());
				pChild->mp_Key.f_Copy(pReg->mp_Key);
				pChild->fpr_Merge(pReg);
			}
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	template <typename tf_FPredicate>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fpr_MergeIncludingValue(const TCRegistry *_pMergeWith, tf_FPredicate const &_fPredicate)
	{
		// Find deleted
		{
			auto Iter = _pMergeWith->mp_Children.f_GetIterator();
			while (Iter)
			{
				const TCRegistry *pReg = Iter;
				++Iter;
				t_CStr Name = pReg->f_GetName();
				TCRegistry *pChild;
				if (_fPredicate(*pReg))
					pChild = f_GetChild(Name, pReg->f_GetThisValue());
				else
					pChild = f_GetChildNoPath(Name);
				if (!pChild)
					pChild = f_CreateChild(Name, true);
				pChild->f_SetThisValue(pReg->f_GetThisValue());
				pChild->mp_Key.f_Copy(pReg->mp_Key);
				pChild->fpr_MergeIncludingValue(pReg, _fPredicate);
			}
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fpr_Add(const TCRegistry *_pAdd)
	{
		auto Iter = _pAdd->mp_Children.f_GetIterator();
		while (Iter)
		{
			const TCRegistry *pReg = Iter;
			++Iter;
			t_CStr Name = pReg->f_GetName();
			TCRegistry *pChild = f_CreateChild(Name, true);
			pChild->mp_Key.f_Copy(pReg->mp_Key);
			pChild->f_SetThisValue(pReg->f_GetThisValue());
			pChild->fpr_Add(pReg);
		}
	}
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_Merge(const TCRegistry &_MergeWith, bool _bMergeRoot)
	{
		if (_bMergeRoot)
		{
			f_SetThisValue(_MergeWith.f_GetThisValue());
			if (mp_pParent && mp_ChildLink.f_IsInTree())
				mp_pParent->mp_Children.f_Remove(this);
			mp_Key.f_Copy(_MergeWith.mp_Key);
			if (mp_pParent)
				mp_pParent->mp_Children.f_Insert(this);
		}

		fpr_Merge(&_MergeWith);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	template <typename tf_FPredicate>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_MergeIncludingValue(const TCRegistry &_MergeWith, bool _bMergeRoot, tf_FPredicate const &_fPredicate)
	{
		if (_bMergeRoot)
		{
			f_SetThisValue(_MergeWith.f_GetThisValue());
			if (mp_pParent && mp_ChildLink.f_IsInTree())
				mp_pParent->mp_Children.f_Remove(this);
			mp_Key.f_Copy(_MergeWith.mp_Key);
			if (mp_pParent)
				mp_pParent->mp_Children.f_Insert(this);
		}

		fpr_MergeIncludingValue(&_MergeWith, _fPredicate);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_MergeIncludingValue(const TCRegistry &_MergeWith, bool _bMergeRoot)
	{
		f_MergeIncludingValue(_MergeWith, _bMergeRoot, [](TCRegistry const &_Reg) -> bool { return true; });
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_Add(const TCRegistry &_ToAdd, bool _SetRootValue)
	{
		if (_SetRootValue)
			f_SetThisValue(_ToAdd.f_GetThisValue());
		fpr_Add(&_ToAdd);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_Subtract(const TCRegistry &_ToSubtract)
	{
		for (auto Iter = _ToSubtract.mp_Children.f_GetIterator(); Iter; ++Iter)
		{
			const TCRegistry *pReg = Iter;
			t_CStr Name = pReg->f_GetName();
			auto *pChild = f_GetChild(Name);
			if (pChild)
				f_DeleteChild(pChild);
		}
	}
}
