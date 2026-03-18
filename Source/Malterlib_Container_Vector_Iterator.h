// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CIteratorData>
	struct TCVectorIterator
	{
		TCVectorIterator();
		TCVectorIterator(t_CIteratorData *_Begin,t_CIteratorData *_End);
		TCVectorIterator(TCVectorIterator const& _ToCopy);

		inline_small TCVectorIterator& operator = (TCVectorIterator const& _ToCopy);
		inline_small bool operator == (TCVectorIterator const& _Other) const noexcept;
		inline_small operator t_CIteratorData *() const;
		inline_small t_CIteratorData * operator -> () const;
		inline_small TCVectorIterator& operator ++ ();
		inline_small TCVectorIterator& operator += (smint _Offset);

		inline_small mint f_GetLen() const;
		inline_small TCVectorIterator f_GetIterator() const;

	private:
		inline_small t_CIteratorData *fp_GetCurrent() const;

		t_CIteratorData *m_pBegin;
		t_CIteratorData *m_pEnd;
	};
};
