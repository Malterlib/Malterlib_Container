// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fpr_FindDiffs
		(
			t_CStr const &_Path
			, TCRegistry const &_Original
			, NContainer::TCVector<t_CStr> &_Changed
			, NContainer::TCVector<t_CStr> &_Added
			, NContainer::TCVector<t_CStr> &_Deleted
			, bool _bRecursive
		) const
	{
		// Find deleted
		{
			auto Iter = _Original.mp_Children.f_GetIterator();
			while (Iter)
			{
				const TCRegistry *pReg = Iter;
				++Iter;
				auto Name = pReg->f_GetName();

				const TCRegistry *pChild = f_GetChildNoPath(Name);
				if (!pChild)
				{
					t_CStr Path = fsp_AppendPath(_Path, Name);
					_Deleted.f_Insert(Path);
					if (_bRecursive)
						pReg->fpr_AddAllChildren(Path, _Deleted);
				}
			}
		}
		// Find added and changed
		{
			auto Iter = mp_Children.f_GetIterator();
			while (Iter)
			{
				const TCRegistry *pReg = Iter;
				++Iter;
				auto Name = pReg->f_GetName();

				t_CStr Path = fsp_AppendPath(_Path, Name);
				const TCRegistry *pChild = _Original.f_GetChildNoPath(Name);
				if (!pChild)
				{
					_Added.f_Insert(Path);
					if (_bRecursive)
						pReg->fpr_AddAllChildren(Path, _Added);
				}
				else
				{
					if (pChild->f_GetThisValue() != pReg->f_GetThisValue())
						_Changed.f_Insert(Path);

					if (_bRecursive)
						pReg->fpr_FindDiffs(Path, *pChild, _Changed, _Added, _Deleted, _bRecursive);
				}
			}
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fpr_FindChanges(const t_CStr &_Path, const TCRegistry &_Original, TCRegistry &_Changed, bool _bIncludeAdded) const
	{
		// Find added and changed
		{
			auto Iter = mp_Children.f_GetIterator();
			while (Iter)
			{
				const TCRegistry *pReg = Iter;
				++Iter;
				t_CStr Name = pReg->f_GetName();

				t_CStr Path = fsp_AppendPath(_Path, Name);
				const TCRegistry *pChild = _Original.f_GetChildNoPath(Name);
				if (!pChild)
				{
					if (_bIncludeAdded)
					{
						TCRegistry *pCreate = _Changed.f_CreateChild(Path);
						pCreate->f_Add(*pReg, true);
					}
				}
				else
				{
					if (pChild->f_GetThisValue() != pReg->f_GetThisValue())
						_Changed.f_SetValue(Path, pReg->f_GetThisValue());

					pReg->fpr_FindChanges(Path, *pChild, _Changed, _bIncludeAdded);
				}
			}
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fpr_FindChanges
		(
			t_CStr const &_Path
			, TCRegistry const &_Original
			, TCRegistry &_Changed
			, NContainer::TCVector<t_CStr> &_Deleted
			, bool _bIncludeAdded
		) const
	{
		// Find deleted
		{
			auto Iter = _Original.mp_Children.f_GetIterator();
			while (Iter)
			{
				const TCRegistry *pReg = Iter;
				++Iter;
				t_CStr Name = pReg->f_GetName();

				const TCRegistry *pChild = f_GetChildNoPath(Name);
				if (!pChild)
				{
					t_CStr Path = fsp_AppendPath(_Path, Name);
					_Deleted.f_Insert(Path);
				}
			}
		}
		// Find added and changed
		{
			auto Iter = mp_Children.f_GetIterator();
			while (Iter)
			{
				const TCRegistry *pReg = Iter;
				++Iter;
				t_CStr Name = pReg->f_GetName();

				t_CStr Path = fsp_AppendPath(_Path, Name);
				const TCRegistry *pChild = _Original.f_GetChildNoPath(Name);
				if (!pChild)
				{
					if (_bIncludeAdded)
					{
						TCRegistry *pCreate = _Changed.f_CreateChild(Path);
						pCreate->f_Add(*pReg, true);
					}
				}
				else
				{
					if (pChild->f_GetThisValue() != pReg->f_GetThisValue())
						_Changed.f_SetValue(Path, pReg->f_GetThisValue());

					pReg->fpr_FindChanges(Path, *pChild, _Changed, _Deleted, _bIncludeAdded);
				}
			}
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_FindDiffs
		(
			TCRegistry const &_Original
			, NContainer::TCVector<t_CStr> &_Changed
			, NContainer::TCVector<t_CStr> &_Added
			, NContainer::TCVector<t_CStr> &_Deleted
			, bool _bRecursive
		) const
	{
		fpr_FindDiffs("", _Original, _Changed, _Added, _Deleted, _bRecursive);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_FindChanges(const TCRegistry &_Original, TCRegistry &_Changed, bool _bIncludeAdded) const
	{
		fpr_FindChanges("", _Original, _Changed, _bIncludeAdded);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_FindChanges
		(
			TCRegistry const &_Original
			, TCRegistry &_Changed
			, NContainer::TCVector<t_CKey> &_Deleted
			, bool _bIncludeAdded
		) const
	{
		fpr_FindChanges("", _Original, _Changed, _Deleted, _bIncludeAdded);
	}
}
