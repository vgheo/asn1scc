/*
 * ber.h
 *
 */

#ifndef BER_H_
#define BER_H_

#include "core.h"
#include "ByteStream.h"

typedef asn1SccUint BerTag;



flag BerEncodeTag(ByteStream* pByteStrm, BerTag tag, int *pErrCode);
flag BerDecodeTag(ByteStream* pByteStrm, BerTag tag, int *pErrCode);

flag BerEncodeLengthStart(ByteStream* pByteStrm, int *pErrCode);
flag BerEncodeLengthEnd(ByteStream* pByteStrm, int *pErrCode);

flag BerDecodeLength(ByteStream* pByteStrm, int* value, int *pErrCode);
flag BerDecodeTwoZeroes(ByteStream* pByteStrm, int *pErrCode);


flag BerEncodeInteger(ByteStream* pByteStrm, BerTag tag, asn1SccSint value, int *pErrCode);
flag BerDecodeInteger(ByteStream* pByteStrm, BerTag tag, asn1SccSint *value, int *pErrCode);

flag BerEncodeBoolean(ByteStream* pByteStrm, BerTag tag, flag value, int *pErrCode);
flag BerDecodeBoolean(ByteStream* pByteStrm, BerTag tag, flag *value, int *pErrCode);

flag BerEncodeReal(ByteStream* pByteStrm, BerTag tag, double value, int *pErrCode);
flag BerDecodeReal(ByteStream* pByteStrm, BerTag tag, double *value, int *pErrCode);

flag BerEncodeIA5String(ByteStream* pByteStrm, BerTag tag, const char* value, int length, int *pErrCode);
flag BerDecodeIA5String(ByteStream* pByteStrm, BerTag tag, char* value, int maxLength, int *pErrCode);

flag BerEncodeNull(ByteStream* pByteStrm, BerTag tag, int *pErrCode);
flag BerDecodeNull(ByteStream* pByteStrm, BerTag tag, int *pErrCode);

flag BerEncodeBitString(ByteStream* pByteStrm, BerTag tag, const byte* value, int bitCount, int *pErrCode);
flag BerDecodeBitString(ByteStream* pByteStrm, BerTag tag, byte* value, int *bitCount, int maxBitCount, int *pErrCode);

flag BerEncodeOctetString(ByteStream* pByteStrm, BerTag tag, const byte* value, int octCount, int *pErrCode);
flag BerDecodeOctetString(ByteStream* pByteStrm, BerTag tag, byte* value, int *octCount, int maxOctCount, int *pErrCode);



flag NextTagMatches(ByteStream* pByteStrm, BerTag tag);

int GetStrmPos(ByteStream* pByteStrm);
flag LA_Next_Two_Bytes_00(ByteStream* pByteStrm);


#endif /* BER_H_ */
