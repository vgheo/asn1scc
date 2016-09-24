/*
 * ByteStream.h
 *
 */

#ifndef BYTESTREAM_H_
#define BYTESTREAM_H_

#include "core.h"

typedef struct {
	byte* buf;
	long count;
	long currentByte;
	flag EncodeWhiteSpace;
} ByteStream;

void ByteStream_Init(ByteStream* pStrm, byte* buf, long count);
void ByteStream_AttachBuffer(ByteStream* pStrm, unsigned char* buf, long count);
asn1SccSint ByteStream_GetLength(ByteStream* pStrm);
flag ByteStream_PutByte(ByteStream* pStrm, byte v);
flag ByteStream_GetByte(ByteStream* pStrm, byte* v);


#endif /* BYTESTREAM_H_ */
