/*
 * util.h
 *
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <assert.h>

#include "core.h"

#ifdef  __cplusplus
extern "C" {
#endif


/* Note. Included in util as some non-ber modules also depend on this */
#if WORD_SIZE==8
extern const asn1SccUint64 ber_aux[];
#else
extern const asn1SccUint32 ber_aux[];
#endif


int GetLengthInBytesOfSInt(asn1SccSint v);
int GetLengthInBytesOfUInt(asn1SccUint64 v);
int GetNumberOfBitsForNonNegativeInteger(asn1SccUint v);

int GetCharIndex(char ch, byte allowedCharSet[], int setLen);

asn1SccUint int2uint(asn1SccSint v);
asn1SccSint uint2int(asn1SccUint v, int uintSizeInBytes);


asn1SccSint milbus_encode(asn1SccSint val);
asn1SccSint milbus_decode(asn1SccSint val);

#define CHECK_BIT_STREAM(pBitStrm)	assert((pBitStrm)->currentByte*8+(pBitStrm)->currentBit<=(pBitStrm)->count*8)

#ifdef _MSC_VER
#pragma warning( disable : 4127)
#endif

#define ASSERT_OR_RETURN_FALSE(_Expression) do { assert(_Expression); if (!(_Expression)) return FALSE;} while(0)

#ifdef  __cplusplus
}
#endif


#endif /* UTIL_H_ */
