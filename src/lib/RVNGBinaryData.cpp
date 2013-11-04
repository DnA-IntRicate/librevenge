/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* librevenge
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2007 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 */

#include <librevenge/librevenge.h>
#include "RVNGMemoryStream.h"

#include <vector>
#include <stdarg.h>
#include <stdio.h>

namespace librevenge
{

class RVNGBinaryDataImpl
{
public:
	RVNGBinaryDataImpl() : m_buf(), m_stream(NULL) {}
	~RVNGBinaryDataImpl()
	{
		if (m_stream) delete m_stream;
	}
	std::vector<unsigned char> m_buf;
	RVNGMemoryInputStream *m_stream;
private:
	// Unimplemented to prevent compiler from creating crasher ones
	RVNGBinaryDataImpl(const RVNGBinaryDataImpl &);
	RVNGBinaryDataImpl &operator=(const RVNGBinaryDataImpl &);
};

RVNGBinaryData::~RVNGBinaryData()
{
	delete m_binaryDataImpl;
}

RVNGBinaryData::RVNGBinaryData() :
	m_binaryDataImpl(new RVNGBinaryDataImpl)
{
}

RVNGBinaryData::RVNGBinaryData(const RVNGBinaryData &data) :
	m_binaryDataImpl(new RVNGBinaryDataImpl)
{
	m_binaryDataImpl->m_buf = data.m_binaryDataImpl->m_buf;
}

RVNGBinaryData::RVNGBinaryData(const unsigned char *buffer, const unsigned long bufferSize) :
	m_binaryDataImpl(new RVNGBinaryDataImpl)
{
	m_binaryDataImpl->m_buf = std::vector<unsigned char> (bufferSize);
	for (unsigned long i = 0; i < bufferSize; i++)
		m_binaryDataImpl->m_buf[i] = buffer[i];
}

void RVNGBinaryData::append(const RVNGBinaryData &data)
{
	unsigned long previousSize = m_binaryDataImpl->m_buf.size();
	m_binaryDataImpl->m_buf.reserve(previousSize + data.m_binaryDataImpl->m_buf.size());
	for (unsigned long i = 0; i < data.m_binaryDataImpl->m_buf.size(); i++)
		m_binaryDataImpl->m_buf.push_back(data.m_binaryDataImpl->m_buf[i]);
}

void RVNGBinaryData::append(const unsigned char *buffer, const unsigned long bufferSize )
{
	unsigned long previousSize = m_binaryDataImpl->m_buf.size();
	m_binaryDataImpl->m_buf.reserve(previousSize + bufferSize);
	for (unsigned long i = 0; i < bufferSize; i++)
		m_binaryDataImpl->m_buf.push_back(buffer[i]);
}

void RVNGBinaryData::append(const unsigned char c)
{
	m_binaryDataImpl->m_buf.push_back(c);
}

void RVNGBinaryData::clear()
{
	m_binaryDataImpl->m_buf.clear();
}

unsigned long RVNGBinaryData::size() const
{
	return (unsigned long)m_binaryDataImpl->m_buf.size();
}

RVNGBinaryData &RVNGBinaryData::operator=(const RVNGBinaryData &dataBuf)
{
	m_binaryDataImpl->m_buf = dataBuf.m_binaryDataImpl->m_buf;
	return *this;
}

const unsigned char *RVNGBinaryData::getDataBuffer() const
{
	if (m_binaryDataImpl->m_buf.empty())
		return 0;
	return &(m_binaryDataImpl->m_buf[0]);
}

const RVNGString RVNGBinaryData::getBase64Data() const
{
	static const char *base64Chars =
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char tempCharsToEncode[3];
	const unsigned long len = size();
	unsigned long i = 0;
	unsigned j = 0;
	unsigned long modifiedLen;
	if (len % 3)
		modifiedLen = 3 * ((len / 3) + 1);
	else
		modifiedLen = len;

	bool shouldIexit = false;
	RVNGString base64;
	for (; i < modifiedLen; i++)
	{
		if (i < len)
			tempCharsToEncode[j++] = m_binaryDataImpl->m_buf[i];
		else
		{
			tempCharsToEncode[j++] = '\0';
			shouldIexit = true;
		}

		if (shouldIexit)
		{
			if (j == 3)
			{
				base64.append(base64Chars[(tempCharsToEncode[0] & 0xfc) >> 2]);
				base64.append(base64Chars[((tempCharsToEncode[0] & 0x03) << 4) | ((tempCharsToEncode[1] & 0xf0) >> 4)]);
				base64.append(base64Chars[((tempCharsToEncode[1] & 0x0f) << 2) | ((tempCharsToEncode[2] & 0xc0) >> 6)]);
				base64.append('=');
				break;
			}
			if (j == 2)
			{
				base64.append(base64Chars[(tempCharsToEncode[0] & 0xfc) >> 2]);
				base64.append(base64Chars[((tempCharsToEncode[0] & 0x03) << 4) | ((tempCharsToEncode[1] & 0xf0) >> 4)]);
				base64.append('=');
				base64.append('=');
				break;
			}
		}
		else if (j == 3)
		{
			base64.append(base64Chars[(tempCharsToEncode[0] & 0xfc) >> 2]);
			base64.append(base64Chars[((tempCharsToEncode[0] & 0x03) << 4) | ((tempCharsToEncode[1] & 0xf0) >> 4)]);
			base64.append(base64Chars[((tempCharsToEncode[1] & 0x0f) << 2) | ((tempCharsToEncode[2] & 0xc0) >> 6)]);
			base64.append(base64Chars[tempCharsToEncode[2] & 0x3f]);
			j = 0;
		}
	}
	return base64;
}

const RVNGInputStream *RVNGBinaryData::getDataStream() const
{
	if (m_binaryDataImpl->m_stream)
	{
		delete (m_binaryDataImpl->m_stream);
		m_binaryDataImpl->m_stream = 0;
	}
	if (m_binaryDataImpl->m_buf.empty())
		return 0;
	m_binaryDataImpl->m_stream = new RVNGMemoryInputStream(&(m_binaryDataImpl->m_buf[0]), m_binaryDataImpl->m_buf.size());
	return m_binaryDataImpl->m_stream;
}

}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */