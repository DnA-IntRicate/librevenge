/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* librevenge
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2004 William Lachance (wrlach@gmail.com)
 * Copyright (C) 2006 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 */

#ifndef RVNGSTRING_H
#define RVNGSTRING_H

namespace librevenge
{

class RVNGStringImpl;

class RVNGString
{
public:
	RVNGString();
	RVNGString(const RVNGString &, bool escapeXML = false);
	RVNGString(const char *str);
	~RVNGString();

	const char *cstr() const;
	int len() const;

	void sprintf(const char *format, ...);
	void append(const RVNGString &s);
	void append(const char *s);
	void append(const char c);
	void clear();
	RVNGString &operator=(const RVNGString &str);
	RVNGString &operator=(const char *s);
	bool operator==(const char *s) const;
	bool operator==(const RVNGString &str) const;
	bool operator!() const;
	inline bool operator!=(const char *s) const
	{
		return !operator==(s);
	}
	inline bool operator!=(const RVNGString &str) const
	{
		return !operator==(str);
	}

	class Iter
	{
	public:
		Iter(const RVNGString &str);
		virtual ~Iter();
		void rewind();
		bool next();
		bool last();
		const char *operator()() const;
	private:
		Iter(const Iter &);
		Iter &operator=(const Iter &);
		RVNGStringImpl *m_stringImpl;
		int m_pos;
		mutable char *m_curChar;
	};

private:
	RVNGStringImpl *m_stringImpl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */