/* libwpd
 * Copyright (C) 2002 William Lachance (william.lachance@sympatico.ca)
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

#ifndef WPXPAGE_H
#define WPXPAGE_H
#include "libwpd.h"
#include "WP6FileStructure.h"
#include <vector>

// intermediate page representation class: for internal use only (by the high-level content/styles listeners). should not be exported.

class WPXHeaderFooter
{
public:
	WPXHeaderFooter(const WPXHeaderFooterType headerFooterType, const WPXHeaderFooterOccurence occurence, 
			const guint8 internalType, const guint16 textPID);
	const WPXHeaderFooterType getType() const { return m_type; }
	const WPXHeaderFooterOccurence getOccurence() const { return m_occurence; }
	const guint16 getTextPID() const { return m_textPID; }
	const guint8 getInternalType() const { return m_internalType; }
private:
	WPXHeaderFooterType m_type;
	WPXHeaderFooterOccurence m_occurence;
	guint8 m_internalType; // for suppression
	guint16 m_textPID; // for the actual text
};

class WPXPageSpan
{
public:
	WPXPageSpan();
	WPXPageSpan(WPXPageSpan &page, float paragraphMarginLeft=0.0f, float paragraphMarginRight=0.0f);
	
	const bool getHeaderFooterSuppression(const guint8 headerFooterType) const { if (headerFooterType <= WP6_HEADER_FOOTER_GROUP_FOOTER_B) return m_isHeaderFooterSuppressed[headerFooterType]; return false; }
	const float getMarginLeft() const { return m_marginLeft; }
 	const float getMarginRight() const { return m_marginRight; }
 	const float getMarginTop() const { return m_marginTop; }
 	const float getMarginBottom() const { return m_marginBottom; }
	const int getPageSpan() const { return m_pageSpan; }
	const vector<WPXHeaderFooter> & getHeaderFooterList() const { return m_headerFooterList; }

	void setHeaderFooter(const guint8 headerFooterType, const guint8 occurenceBits, const guint16 textPID);
	void setHeadFooterSuppression(const guint8 headerFooterType, const bool suppress) { m_isHeaderFooterSuppressed[headerFooterType] = suppress; }
	void setMarginLeft(const float marginLeft) { m_marginLeft = marginLeft; }
 	void setMarginRight(const float marginRight) { m_marginRight = marginRight; }
 	void setMarginTop(const float marginTop) { m_marginTop = marginTop; }
 	void setMarginBottom(const float marginBottom) { m_marginBottom = marginBottom; }
	void setPageSpan(const int pageSpan) { m_pageSpan = pageSpan; }

	void makeConsistent(int startingPageNumber);
protected:
	void _removeHeaderFooter(WPXHeaderFooterType type, WPXHeaderFooterOccurence occurence);
	bool _containsHeaderFooter(WPXHeaderFooterType type, WPXHeaderFooterOccurence occurence);
	
private:
	bool m_isHeaderFooterSuppressed[WP6_NUM_HEADER_FOOTER_TYPES];
	float m_marginLeft, m_marginRight;
	float m_marginTop, m_marginBottom;
	vector<WPXHeaderFooter> m_headerFooterList;

	int m_pageSpan;
};

bool operator==(const WPXPageSpan &, const WPXPageSpan &);
#endif /* WPXPAGE_H */
