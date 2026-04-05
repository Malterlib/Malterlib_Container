// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChildren() const -> CTree const &
	{
		return mp_Children.m_Tree;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChildren() -> CTree &
	{
		return mp_Children.m_Tree;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CData, typename tf_CKey>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChild(tf_CKey const &_Str, tf_CData const &_Data) const -> TCRegistry const *
	{
		typename CTree::CIterator Iter;
		Iter.f_InitForSearch(mp_Children.m_Tree);
		mp_Key.fs_FindIterator(Iter, _Str);

		while (Iter)
		{
			TCRegistry const *pReg = Iter;

			if (pReg->f_GetName() != _Str)
				break;

			if (pReg->f_GetValue() == _Data)
				return pReg;

			++Iter;
		}

		return nullptr;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CData, typename tf_CKey>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChild(tf_CKey const &_Str, tf_CData const &_Data) -> TCRegistry *
	{
		typename CTree::CIterator Iter;
		Iter.f_InitForSearch(mp_Children.m_Tree);
		mp_Key.fs_FindIterator(Iter, _Str);

		while (Iter)
		{
			TCRegistry *pReg = Iter;

			if (pReg->f_GetName() != _Str)
				break;

			if (pReg->f_GetThisValue() == _Data)
				return pReg;

			++Iter;
		}

		return nullptr;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_MoveChild(TCRegistry *_pChild, TCRegistry *_pAfter)
	{
		if (_pChild->mp_pParent && _pChild->mp_ChildLink.f_IsInTree())
			_pChild->mp_pParent->mp_Children.f_Remove(_pChild);

		_pChild->mp_pParent = this;
		if constexpr (CRegistryKey::mc_bSupportForceCreate)
			_pChild->mp_Key.f_NewSequence(this);

		mp_Children.f_Insert(_pChild, _pAfter);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_InsertChild(t_CKey _Name, NMib::NStorage::TCUniquePointer<TCRegistry> &&_pChild) -> TCRegistry *
	{
		if constexpr (!mc_bSupportForceCreate)
			DMibError("Foreceinsert not supported");
		_pChild->mp_pParent = this;
		_pChild->mp_Key.f_Set(_Name);
		TCRegistry *pChild = _pChild.f_Detach();
		mp_Children.f_Insert(pChild);
		return pChild;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_CreateChild(t_CStr _Name, bool _bForceCreate) -> TCRegistry *
	{
		if (!mc_bSupportForceCreate && _bForceCreate)
			DMibError("You cannot do a force create in TCRegistry when the type does not support it.");

		if (_bForceCreate)
		{
			t_CStr NotFound;
			TCRegistry *pPrev = this;
			TCRegistry *pChild = nullptr;
			if (_Name.f_FindChar('/') >= 0)
			{
				pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);
				while (!pChild)
				{
					pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), pPrev);
					pChild->mp_Key.f_Set(NotFound);
					pPrev->mp_Children.f_Insert(pChild);
					pPrev = pChild;
					if (_Name.f_FindChar('/') < 0)
						break;
					pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);
				}
			}
			pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), pPrev);
			pChild->mp_Key.f_Set(_Name);
			pPrev->mp_Children.f_Insert(pChild);
			return pChild;
		}
		else
		{
			t_CStr NotFound;
			TCRegistry *pPrev = this;
			TCRegistry *pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);

			while (!pChild)
			{
				pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), pPrev);
				pChild->mp_Key.f_Set(NotFound);
				pPrev->mp_Children.f_Insert(pChild);
				pPrev = pChild;
				if (_Name.f_IsEmpty())
					break;
				pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);
			}
			return pChild;
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_CreateChildNoPath(t_CKey const &_Name, bool _bForceCreate) -> TCRegistry *
	{
		if (!mc_bSupportForceCreate && _bForceCreate)
			DMibError("You cannot do a force create in TCRegistry when the type does not support it.");

		if (_bForceCreate)
		{
			TCRegistry *pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), this);
			pChild->mp_Key.f_Set(_Name);
			mp_Children.f_Insert(pChild);
			return pChild;
		}
		else
		{
			TCRegistry *pChild = CRegistryKey::fs_FindEqual(*this, _Name);

			if (!pChild)
			{
				pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), this);
				pChild->mp_Key.f_Set(_Name);
				mp_Children.f_Insert(pChild);
			}
			return pChild;
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_HasChildren() const
	{
		return !mp_Children.m_Tree.f_IsEmpty();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChild(t_CStr _Str) const -> TCRegistry const *
	{
		return fp_GetChildParse(_Str, fg_NullPtr<t_CStr>());
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChild(t_CStr _Str) -> TCRegistry *
	{
		return fp_GetChildParse(_Str, fg_NullPtr<t_CStr>());
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChildNoPath(t_CKey const &_Name) const -> TCRegistry const *
	{
		return mp_Key.fs_FindEqual(*this, _Name);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetChildNoPath(t_CKey const &_Name) -> TCRegistry *
	{
		return mp_Key.fs_FindEqual(*this, _Name);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_DeleteChild(TCRegistry *_pChild)
	{
		fg_DeleteObject(NMemory::CDefaultAllocator(), _pChild);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_DeleteChild(t_CStr _Name, bool _bDeleteEmptyParentDirs)
	{
		TCRegistry *pChild = fp_GetChildParse(_Name, fg_NullPtr<t_CStr>());

		if (pChild)
		{
			f_DeleteChild(pChild);
			if (_bDeleteEmptyParentDirs)
			{
				TCRegistry *pParent = this;
				while (pParent->mp_Children.m_Tree.f_IsEmpty())
				{
					TCRegistry *pToDelete = pParent;
					pParent = pParent->mp_pParent;
					if (pParent)
					{
						f_DeleteChild(pToDelete);
					}
				}
			}
			return true;
		}
		else
			return false;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_DeleteChildNoPath(t_CKey const &_Name, bool _bDeleteEmptyParentDirs)
	{
		TCRegistry *pChild = CRegistryKey::fs_FindEqual(*this, _Name);

		if (pChild)
		{
			f_DeleteChild(pChild);
			if (_bDeleteEmptyParentDirs)
			{
				TCRegistry *pParent = this;
				while (pParent->mp_Children.m_Tree.f_IsEmpty())
				{
					TCRegistry *pToDelete = pParent;
					pParent = pParent->mp_pParent;
					if (pParent)
					{
						f_DeleteChild(pToDelete);
					}
				}
			}
			return true;
		}
		else
			return false;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_DeleteAllChildren()
	{
		mp_Children.m_Tree.f_DeleteAllDefiniteType();
	}
}
