/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef _UTF8ITER_H_INCLUDED_
#define _UTF8ITER_H_INCLUDED_
/* @(#$Id: utf8iter.h,v 1.10 2007-09-22 08:51:29 dockes Exp $  (C) 2004 J.F.Dockes */

#ifdef UTF8ITER_CHECK
#include "assert.h"
#endif

/** 
 * A small helper class to iterate over utf8 strings. This is not an
 * STL iterator and does not much error checking. It is designed purely
 * for recoll usage, where the utf-8 string comes out of iconv in most cases
 * and is assumed legal. We just try to catch cases where there would be 
 * a risk of crash.
 */
class Utf8Iter {
public:
    Utf8Iter(const string &in) 
	: m_s(in), m_cl(0), m_pos(0), m_charpos(0), m_error(false)
    {
	update_cl();
    }

    const string& buffer() const {return m_s;}

    void rewind() 
    {
	m_cl = 0; 
	m_pos = 0; 
	m_charpos = 0; 
	m_error = false;
	update_cl();
    }

    /** "Direct" access. Awfully inefficient as we skip from start or current
     * position at best. This can only be useful for a lookahead from the
     * current position */
    unsigned int operator[](unsigned int charpos) const 
    {
	string::size_type mypos = 0;
	unsigned int mycp = 0;
	if (charpos >= m_charpos) {
	    mypos = m_pos;
	    mycp = m_charpos;
	}
	int l;
	while (mypos < m_s.length() && mycp != charpos) {
	    l = get_cl(mypos);
	    if (l <= 0)
		return (unsigned int)-1;
	    mypos += l;
	    ++mycp;
	}
	if (mypos < m_s.length() && mycp == charpos) {
	    l = get_cl(mypos);
	    if (poslok(mypos, l))
		return getvalueat(mypos, get_cl(mypos));
	}
	return (unsigned int)-1;
    }

    /** Increment current position to next utf-8 char */
    string::size_type operator++(int) 
    {
	// Note: m_cl may be zero at eof if user's test not right
	// this shouldn't crash the program until actual data access
#ifdef UTF8ITER_CHECK
	assert(m_cl != 0);
#endif
	if (m_cl <= 0) 
	    return string::npos;

	m_pos += m_cl;
	m_charpos++;
	update_cl();
	return m_pos;
    }

    /** operator* returns the ucs4 value as a machine integer*/
    unsigned int operator*() 
    {
#ifdef UTF8ITER_CHECK
	assert(m_cl != 0);
#endif
	return getvalueat(m_pos, m_cl);
    }

    /** Append current utf-8 possibly multi-byte character to string param.
	This needs to be fast. No error checking. */
    unsigned int appendchartostring(string &out) {
#ifdef UTF8ITER_CHECK
	assert(m_cl != 0);
#endif
	out.append(&m_s[m_pos], m_cl);
	return m_cl;
    }

    /** Return current character as string */
    operator string() {
#ifdef UTF8ITER_CHECK
	assert(m_cl != 0);
#endif
	return m_s.substr(m_pos, m_cl);
    }

    bool eof() {
	return m_pos == m_s.length();
    }

    bool error() {
	return m_error;
    }

    /** Return current byte offset in input string */
    string::size_type getBpos() const {
	return m_pos;
    }

    /** Return current character length */
    string::size_type getBlen() const {
	return m_cl;
    }

    /** Return current unicode character offset in input string */
    string::size_type getCpos() const {
	return m_charpos;
    }

private:
    // String we're working with
    const string&     m_s; 
    // Character length at current position. A value of zero indicates
    // an error.
    unsigned int      m_cl; 
    // Current byte offset in string.
    string::size_type m_pos; 
    // Current character position
    unsigned int      m_charpos; 
    // Am I ok ?
    mutable bool      m_error;

    // Check position and cl against string length
    bool poslok(string::size_type p, int l) const {
#ifdef UTF8ITER_CHECK
	assert(p != string::npos && l > 0 && p + l <= m_s.length());
#endif
	return p != string::npos && l > 0 && p + l <= m_s.length();
    }

    // Update current char length in object state, minimum checking
    // for errors
    inline void update_cl() 
    {
	m_cl = 0;
	if (m_pos >= m_s.length())
	    return;
	m_cl = get_cl(m_pos);
	if (!poslok(m_pos, m_cl)) {
	    // Used to set eof here for safety, but this is bad because it
	    // basically prevents the caller to discriminate error and eof.
	    //	    m_pos = m_s.length();
	    m_cl = 0;
	    m_error = true;
	}
    }

    // Get character byte length at specified position. Returns 0 for error.
    inline int get_cl(string::size_type p) const 
    {
	unsigned int z = (unsigned char)m_s[p];
	if (z <= 127) {
	    return 1;
	} else if ((z & 224) == 192) {
	    return 2;
	} else if ((z & 240) == 224) {
	    return 3;
	} else if ((z & 248) == 240) {
	    return 4;
	}
#ifdef UTF8ITER_CHECK
	assert(z <= 127 || (z & 224) == 192 || (z & 240) == 224 ||
	       (z & 248) == 240);
#endif
	return 0;
    }

    // Compute value at given position. No error checking.
    inline unsigned int getvalueat(string::size_type p, int l) const
    {
	switch (l) {
	case 1: 
#ifdef UTF8ITER_CHECK
	    assert((unsigned char)m_s[p] < 128);
#endif
	    return (unsigned char)m_s[p];
	case 2: 
#ifdef UTF8ITER_CHECK
	    assert(
		   ((unsigned char)m_s[p] & 224) == 192
		   && ((unsigned char)m_s[p+1] & 192) ==  128
		   );
#endif
	    return ((unsigned char)m_s[p] - 192) * 64 + 
		(unsigned char)m_s[p+1] - 128 ;
	case 3: 
#ifdef UTF8ITER_CHECK
	    assert(
		   (((unsigned char)m_s[p]) & 240) == 224
		   && (((unsigned char)m_s[p+1]) & 192) ==  128
		   && (((unsigned char)m_s[p+2]) & 192) ==  128
		   );
#endif

	    return ((unsigned char)m_s[p] - 224) * 4096 + 
		((unsigned char)m_s[p+1] - 128) * 64 + 
		(unsigned char)m_s[p+2] - 128;
	case 4: 
#ifdef UTF8ITER_CHECK
	    assert(
		   (((unsigned char)m_s[p]) & 248) == 240
		   && (((unsigned char)m_s[p+1]) & 192) ==  128
		   && (((unsigned char)m_s[p+2]) & 192) ==  128
		   && (((unsigned char)m_s[p+3]) & 192) ==  128
		   );
#endif

	    return ((unsigned char)m_s[p]-240)*262144 + 
		((unsigned char)m_s[p+1]-128)*4096 + 
		((unsigned char)m_s[p+2]-128)*64 + 
		(unsigned char)m_s[p+3]-128;

	default:
#ifdef UTF8ITER_CHECK
	    assert(l <= 4);
#endif
	    m_error = true;
	    return (unsigned int)-1;
	}
    }

};


#endif /* _UTF8ITER_H_INCLUDED_ */