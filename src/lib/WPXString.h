/* libwpd
 * Copyright (C) 2004 William Lachance (william.lachance@sympatico.ca)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * For further information visit http://libwpd.sourceforge.net
 */

/* "This product is not manufactured, approved, or supported by
 * Corel Corporation or Corel Corporation Limited."
 */

#ifndef WPXSTRING_H
#define WPXSTRING_H
#include <string>

class WPXString
{
public:
	WPXString();
	WPXString(const WPXString &, bool escapeXML = false);
	WPXString(const char *str);
	static WPXString createFromAscii(const char *_str);

	const char * cstr() const;
	int len() const;

	void sprintf(const char *format, ...);
	void append(const WPXString &s);
	void append(const char *s);
	void appendx(const char c);
//	void append(const uint16_t ucs2);
	void clear();
	bool operator==(const char *);
	bool operator==(const WPXString &str);

	class Iter
	{
	public:
		Iter(const WPXString &str);
		virtual ~Iter();
		void rewind();
		bool next();
		bool last();
		const char * operator()() const;
	private:
		std::string m_buf;
		int m_pos;
		mutable char *m_curChar;
	};
		

private:
	std::string m_buf;
};
#endif