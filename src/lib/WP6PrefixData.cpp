/* libwpd2
 * Copyright (C) 2002 William Lachance (wlach@interlog.com)
 * Copyright (C) 2002 Marc Maurer (j.m.maurer@student.utwente.nl)
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

#include "WP6PrefixData.h"
#include "WP6PrefixIndice.h"
#include "WP6PrefixDataPacket.h"
#include "WP6FontDescriptorPacket.h"
#include "WP6DefaultInitialFontPacket.h"

void destroyPrefixDataPacketKeyNotify(gpointer data);
void destroyPrefixDataPacketNotify(gpointer data);

WP6PrefixData::WP6PrefixData(FILE *stream, const int numPrefixIndices) :
	m_prefixDataPacketHash(g_hash_table_new_full(&g_int_hash, &g_int_equal, 
						     &destroyPrefixDataPacketKeyNotify, 
						     &destroyPrefixDataPacketNotify)),
	m_defaultInitialFontPID((-1))

{
	WP6PrefixIndice ** prefixIndiceArray = new (WP6PrefixIndice *)[(numPrefixIndices-1)];
	for (guint16 i=1; i<numPrefixIndices; i++)
		prefixIndiceArray[(i-1)] = new WP6PrefixIndice(stream, i);
	for (guint16 i=1; i<numPrefixIndices; i++) 
		{
			WP6PrefixDataPacket *prefixDataPacket = WP6PrefixDataPacket::constructPrefixDataPacket(stream, prefixIndiceArray[(i-1)]);
			if (prefixDataPacket) {
				gint *key = new gint;
				*key = i;
				g_hash_table_insert(m_prefixDataPacketHash, (gpointer)key, (gpointer)prefixDataPacket);
				if (dynamic_cast<WP6DefaultInitialFontPacket *>(prefixDataPacket))
					m_defaultInitialFontPID = i;
			}
			
	}

	for (guint16 i=1; i<numPrefixIndices; i++)
		delete(prefixIndiceArray[(i-1)]);

	delete(prefixIndiceArray);

}

WP6PrefixData::~WP6PrefixData()
{
 	g_hash_table_destroy(m_prefixDataPacketHash);
}

const WP6PrefixDataPacket * WP6PrefixData::getPrefixDataPacket(const int prefixID) const
{
	return (const WP6PrefixDataPacket *)g_hash_table_lookup(m_prefixDataPacketHash, &prefixID);
}

// static callbacks for the hash table of prefix data packets

void destroyPrefixDataPacketKeyNotify(gpointer data) 
{	
	gint * key = (gint *)data;
	delete(key);
}

void destroyPrefixDataPacketNotify(gpointer data) 
{
	WP6PrefixDataPacket * prefixDataPacket = (WP6PrefixDataPacket *)data;
	delete(prefixDataPacket);
}

