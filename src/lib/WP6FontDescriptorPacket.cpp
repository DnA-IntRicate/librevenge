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
#include <string.h>

#include "WP6FontDescriptorPacket.h"
#include "libwpd_internal.h"

static char *fontWeightStrings[] = { "Regular", "Bold", "Standaard", "Standard" };
static int numFontWeightStrings = 4;

WP6FontDescriptorPacket::WP6FontDescriptorPacket(GsfInput *input, int id, guint32 dataOffset, guint32 dataSize) 
	: WP6PrefixDataPacket(input, id)
{
	_read(input, dataOffset, dataSize);
}

WP6FontDescriptorPacket::~WP6FontDescriptorPacket()
{
	delete [] m_fontName;
}

void WP6FontDescriptorPacket::_readContents(GsfInput *input)
{
   // short sized characteristics
   m_characterWidth = *(const guint16 *)gsf_input_read(input, sizeof(guint16), NULL);
   m_ascenderHeight = *(const guint16 *)gsf_input_read(input, sizeof(guint16), NULL);
   m_xHeight = *(const guint16 *)gsf_input_read(input, sizeof(guint16), NULL);
   m_descenderHeight = *(const guint16 *)gsf_input_read(input, sizeof(guint16), NULL);
   m_italicsAdjust = *(const guint16 *)gsf_input_read(input, sizeof(guint16), NULL);
   // byte sized characteristics
   m_primaryFamilyMemberId = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_primaryFamilyId = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_scriptingSystem = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_primaryCharacterSet = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_width = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_weight = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_attributes = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_generalCharacteristics = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_classification = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_fill = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_fontType = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);
   m_fontSourceFileType = *(const guint8 *)gsf_input_read(input, sizeof(guint8), NULL);

   m_fontNameLength = *(const guint16 *)gsf_input_read(input, sizeof(guint16), NULL); 

   // TODO: re-do sanity checking
   //if(m_fontNameLength < WP_FONT_NAME_MAX_LENGTH)
   //{	
   if (m_fontNameLength == 0) 
	   {
		   m_fontName = new gchar[1];
		   m_fontName[0]='\0';
	   }
   
   else 
	   {
		   gchar const *tempFontName = (gchar const *)gsf_input_read(input, sizeof(gchar)*m_fontNameLength, NULL);
		   m_fontName = new gchar[m_fontNameLength];

		   guint16 tempLength1=0;
		   int numTokens=0;
		   int lastTokenPosition=0;
		   for (guint16 i=0; i<m_fontNameLength; i++) 
			   {
				   if (tempFontName[i] == 0x20) {
					   m_fontName[tempLength1]=' ';
					   tempLength1++;
					   numTokens++;
					   lastTokenPosition=tempLength1;
				   }
				   else if (tempFontName[i] != 0x00) {
					   m_fontName[tempLength1]=tempFontName[i];
					   tempLength1++;
				   }
			   }
		   m_fontName[tempLength1]='\0';
		   // TODO/HACK: probably should create a proper static function for doing this
		   // consume the last token (by replacing the first char with a null-terminator) if its a weight signifier
		   // (NB: not all wp fonts are terminated by weight, just enough of them to make this annoying
		   for (int j=0; j<numFontWeightStrings; j++) 
			   {
				   if (!strcmp(fontWeightStrings[j], &m_fontName[lastTokenPosition])) 
					   {
						   if (lastTokenPosition > 0) {
							   m_fontName[lastTokenPosition-1]='\0';
							   tempLength1 = lastTokenPosition-1;
						   }
						   break;
					   }
			   }
		   // also consume any whitespace at the end of the font..
		   while ((tempLength1 - 1) > 0 && m_fontName[tempLength1-1] == ' ')
			   {
				   m_fontName[tempLength1-1] = '\0';
			   }

	   }

   //
   
   //}
   //    else
   //      return gIE_IMPORTERROR;
   WPD_DEBUG_MSG(("WordPerfect: Read Font (primary family id: %i, family member id: %i, font type: %i, font source file type: %i font name length: %i, font name: %s)\n", (int) m_primaryFamilyId, (int) m_primaryFamilyMemberId, (int) m_fontType, (int) m_fontSourceFileType, (int) m_fontNameLength, m_fontName));

}

