/*
 *	Edmund Mergl <E.Mergl@bawue.de>
 *
 *	$Id: oracle_compat.c,v 1.21 2000/03/14 23:06:37 thomas Exp $
 *
 */


#include <ctype.h>
#include "postgres.h"
#include "utils/builtins.h"


/********************************************************************
 *
 * lower
 *
 * Syntax:
 *
 *	 text *lower(text *string)
 *
 * Purpose:
 *
 *	 Returns string, with all letters forced to lowercase.
 *
 ********************************************************************/

text *
lower(text *string)
{
	text	   *ret;
	char	   *ptr,
			   *ptr_ret;
	int			m;

	if ((string == (text *) NULL) || ((m = VARSIZE(string) - VARHDRSZ) <= 0))
		return string;

	ret = (text *) palloc(VARSIZE(string));
	VARSIZE(ret) = VARSIZE(string);

	ptr = VARDATA(string);
	ptr_ret = VARDATA(ret);

	while (m--)
		*ptr_ret++ = tolower((unsigned char) *ptr++);

	return ret;
}


/********************************************************************
 *
 * upper
 *
 * Syntax:
 *
 *	 text *upper(text *string)
 *
 * Purpose:
 *
 *	 Returns string, with all letters forced to uppercase.
 *
 ********************************************************************/

text *
upper(text *string)
{
	text	   *ret;
	char	   *ptr,
			   *ptr_ret;
	int			m;

	if ((string == (text *) NULL) || ((m = VARSIZE(string) - VARHDRSZ) <= 0))
		return string;

	ret = (text *) palloc(VARSIZE(string));
	VARSIZE(ret) = VARSIZE(string);

	ptr = VARDATA(string);
	ptr_ret = VARDATA(ret);

	while (m--)
		*ptr_ret++ = toupper((unsigned char) *ptr++);

	return ret;
}


/********************************************************************
 *
 * initcap
 *
 * Syntax:
 *
 *	 text *initcap(text *string)
 *
 * Purpose:
 *
 *	 Returns string, with first letter of each word in uppercase,
 *	 all other letters in lowercase. A word is delimited by white
 *	 space.
 *
 ********************************************************************/

text *
initcap(text *string)
{
	text	   *ret;
	char	   *ptr,
			   *ptr_ret;
	int			m;

	if ((string == (text *) NULL) || ((m = VARSIZE(string) - VARHDRSZ) <= 0))
		return string;

	ret = (text *) palloc(VARSIZE(string));
	VARSIZE(ret) = VARSIZE(string);

	ptr = VARDATA(string);
	ptr_ret = VARDATA(ret);

	*ptr_ret++ = toupper((unsigned char) *ptr++);
	--m;

	while (m--)
	{
		if (*(ptr_ret - 1) == ' ' || *(ptr_ret - 1) == '	')
			*ptr_ret++ = toupper((unsigned char) *ptr++);
		else
			*ptr_ret++ = tolower((unsigned char) *ptr++);
	}

	return ret;
}


/********************************************************************
 *
 * lpad
 *
 * Syntax:
 *
 *	 text *lpad(text *string1, int4 len, text *string2)
 *
 * Purpose:
 *
 *	 Returns string1, left-padded to length len with the sequence of
 *	 characters in string2.
 *
 ********************************************************************/

text *
lpad(text *string1, int4 len, text *string2)
{
	text	   *ret;
	char	   *ptr1,
			   *ptr2,
			   *ptr_ret;
	int			m,
				n;

	if ((string1 == (text *) NULL) ||
		(len <= (VARSIZE(string1) - VARHDRSZ)) ||
		((m = len - VARSIZE(string1) + VARHDRSZ) <= 0) ||
		(string2 == (text *) NULL) ||
		((VARSIZE(string2) - VARHDRSZ) <= 0))
		return string1;

	ret = (text *) palloc(VARHDRSZ + len);
	VARSIZE(ret) = VARHDRSZ + len;

	ptr2 = VARDATA(string2);
	ptr_ret = VARDATA(ret);

	while (m--)
	{
		*ptr_ret++ = *ptr2;
		ptr2 = ptr2 == VARDATA(string2) + VARSIZE(string2) - VARHDRSZ - 1 ? VARDATA(string2) : ++ptr2;
	}

	n = VARSIZE(string1) - VARHDRSZ;
	ptr1 = VARDATA(string1);

	while (n--)
		*ptr_ret++ = *ptr1++;

	return ret;
}


/********************************************************************
 *
 * rpad
 *
 * Syntax:
 *
 *	 text *rpad(text *string1, int4 len, text *string2)
 *
 * Purpose:
 *
 *	 Returns string1, right-padded to length len with the sequence of
 *	 characters in string2.
 *
 ********************************************************************/

text *
rpad(text *string1, int4 len, text *string2)
{
	text	   *ret;
	char	   *ptr1,
			   *ptr2,
			   *ptr_ret;
	int			m,
				n;

	if ((string1 == (text *) NULL) ||
		(len <= (VARSIZE(string1) - VARHDRSZ)) ||
		((m = len - VARSIZE(string1) + VARHDRSZ) <= 0) ||
		(string2 == (text *) NULL) ||
		((VARSIZE(string2) - VARHDRSZ) <= 0))
		return string1;

	ret = (text *) palloc(VARHDRSZ + len);
	VARSIZE(ret) = VARHDRSZ + len;

	n = VARSIZE(string1) - VARHDRSZ;
	ptr1 = VARDATA(string1);
	ptr_ret = VARDATA(ret);

	while (n--)
		*ptr_ret++ = *ptr1++;

	ptr2 = VARDATA(string2);

	while (m--)
	{
		*ptr_ret++ = *ptr2;
		ptr2 = ptr2 == VARDATA(string2) + VARSIZE(string2) - VARHDRSZ - 1 ? VARDATA(string2) : ++ptr2;
	}

	return ret;
}


/********************************************************************
 *
 * btrim
 *
 * Syntax:
 *
 *	 text *btrim(text *string, text *set)
 *
 * Purpose:
 *
 *	 Returns string with characters removed from the front and back
 *	 up to the first character not in set.
 *
 ********************************************************************/

text *
btrim(text *string, text *set)
{
	text	   *ret;
	char	   *ptr,
			   *end,
			   *ptr2,
			   *end2;
	int			m;

	if ((string == (text *) NULL) ||
		((m = VARSIZE(string) - VARHDRSZ) <= 0) ||
		(set == (text *) NULL) ||
		((VARSIZE(set) - VARHDRSZ) <= 0))
		return string;

	ptr = VARDATA(string);
	ptr2 = VARDATA(set);
	end2 = VARDATA(set) + VARSIZE(set) - VARHDRSZ - 1;

	while (m--)
	{
		while (ptr2 <= end2)
		{
			if (*ptr == *ptr2)
				break;
			++ptr2;
		}
		if (ptr2 > end2)
			break;
		ptr++;
		ptr2 = VARDATA(set);
	}

	++m;

	end = VARDATA(string) + VARSIZE(string) - VARHDRSZ - 1;
	ptr2 = VARDATA(set);

	while (m--)
	{
		while (ptr2 <= end2)
		{
			if (*end == *ptr2)
				break;
			++ptr2;
		}
		if (ptr2 > end2)
			break;
		--end;
		ptr2 = VARDATA(set);
	}

	++m;

	ret = (text *) palloc(VARHDRSZ + m);
	VARSIZE(ret) = VARHDRSZ + m;
	memcpy(VARDATA(ret), ptr, m);

	return ret;
}	/* btrim() */


/********************************************************************
 *
 * ltrim
 *
 * Syntax:
 *
 *	 text *ltrim(text *string, text *set)
 *
 * Purpose:
 *
 *	 Returns string with initial characters removed up to the first
 *	 character not in set.
 *
 ********************************************************************/

text *
ltrim(text *string, text *set)
{
	text	   *ret;
	char	   *ptr,
			   *ptr2,
			   *end2;
	int			m;

	if ((string == (text *) NULL) ||
		((m = VARSIZE(string) - VARHDRSZ) <= 0) ||
		(set == (text *) NULL) ||
		((VARSIZE(set) - VARHDRSZ) <= 0))
		return string;

	ptr = VARDATA(string);
	ptr2 = VARDATA(set);
	end2 = VARDATA(set) + VARSIZE(set) - VARHDRSZ - 1;

	while (m--)
	{
		while (ptr2 <= end2)
		{
			if (*ptr == *ptr2)
				break;
			++ptr2;
		}
		if (ptr2 > end2)
			break;
		ptr++;
		ptr2 = VARDATA(set);
	}

	++m;

	ret = (text *) palloc(VARHDRSZ + m);
	VARSIZE(ret) = VARHDRSZ + m;

	memcpy(VARDATA(ret), ptr, m);

	return ret;
}


/********************************************************************
 *
 * rtrim
 *
 * Syntax:
 *
 *	 text *rtrim(text *string, text *set)
 *
 * Purpose:
 *
 *	 Returns string with final characters removed after the last
 *	 character not in set.
 *
 ********************************************************************/

text *
rtrim(text *string, text *set)
{
	text	   *ret;
	char	   *ptr,
			   *ptr2,
			   *end2,
			   *ptr_ret;
	int			m;

	if ((string == (text *) NULL) ||
		((m = VARSIZE(string) - VARHDRSZ) <= 0) ||
		(set == (text *) NULL) ||
		((VARSIZE(set) - VARHDRSZ) <= 0))
		return string;

	ptr = VARDATA(string) + VARSIZE(string) - VARHDRSZ - 1;
	ptr2 = VARDATA(set);
	end2 = VARDATA(set) + VARSIZE(set) - VARHDRSZ - 1;

	while (m--)
	{
		while (ptr2 <= end2)
		{
			if (*ptr == *ptr2)
				break;
			++ptr2;
		}
		if (ptr2 > end2)
			break;
		--ptr;
		ptr2 = VARDATA(set);
	}

	++m;

	ret = (text *) palloc(VARHDRSZ + m);
	VARSIZE(ret) = VARHDRSZ + m;
#ifdef NOT_USED
	memcpy(VARDATA(ret), ptr - VARSIZE(ret) + m, m);
#endif

	ptr_ret = VARDATA(ret) + m - 1;

	while (m--)
		*ptr_ret-- = *ptr--;

	return ret;
}


/********************************************************************
 *
 * substr
 *
 * Syntax:
 *
 *	 text *substr(text *string, int4 m, int4 n)
 *
 * Purpose:
 *
 *	 Returns a portion of string, beginning at character m, n
 *	 characters long. The first position of string is 1.
 *
 ********************************************************************/

#ifdef NOT_USED
text *
substr(text *string, int4 m, int4 n)
{
	text	   *ret;
	char	   *ptr,
			   *ptr_ret;
	int			len;

	if ((string == (text *) NULL) ||
		(m <= 0) || (n <= 0) ||
		((len = VARSIZE(string) - VARHDRSZ - m + 1) <= 0))
		return string;

	len = len + 1 < n ? len + 1 : n;

	ret = (text *) palloc(VARHDRSZ + len);
	VARSIZE(ret) = VARHDRSZ + len;

	ptr = VARDATA(string) + m - 1;
	ptr_ret = VARDATA(ret);

	while (len--)
		*ptr_ret++ = *ptr++;

	return ret;
}
#endif

/********************************************************************
 *
 * translate
 *
 * Syntax:
 *
 *	 text *translate(text *string, text *from, text *to)
 *
 * Purpose:
 *
 *	 Returns string after replacing all occurences of from with
 *	 the corresponding character in to. TRANSLATE will not remove
 *	  characters.
 *	 Modified to work with strings rather than single character
 *	  for the substitution arguments.
 *	 Modifications from Edwin Ramirez <ramirez@doc.mssm.edu>.
 *
 ********************************************************************/

text *
translate(text *string, text *from, text *to)
{
	text   *ret;
	char   *ptr_ret, *from_ptr, *to_ptr;
	char   *source, *target, *temp, rep;
	int		m, fromlen, tolen, retlen, i;

	if ((string == (text *) NULL) ||
		((m = VARSIZE(string) - VARHDRSZ) <= 0))
		return string;

	target   = (char *) palloc(VARSIZE(string) - VARHDRSZ);
	source   = VARDATA(string);
	temp     = target;

	fromlen = VARSIZE(from) - VARHDRSZ;
	from_ptr = VARDATA(from);
	tolen = VARSIZE(to) - VARHDRSZ;
	to_ptr   = VARDATA(to);
	retlen = 0;
	while (m--)
	{
		rep = *source;
		for(i=0;i<fromlen;i++) {
            if(from_ptr[i] == *source)  {
				if(i < tolen) {
					rep = to_ptr[i];
				} else {
					rep = 0;
				}
				break;
            }
		}
		if(rep != 0) {
            *target++ = rep;
            retlen++;
		}
		source++;
	}

	ret = (text *) palloc(retlen + VARHDRSZ);
	VARSIZE(ret) = retlen + VARHDRSZ;
	ptr_ret = VARDATA(ret);
	for(i=0;i<retlen;i++) {
		*ptr_ret++ = temp[i];
	}
	pfree(target);
	return ret;
}

/* EOF */
