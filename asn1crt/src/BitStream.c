/*
 * BitStream.c
 *
 */

#include <string.h>

#include "asn1scc/BitStream.h"
#include "asn1scc/util.h"

static byte masks[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
static byte masksb[] = { 0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF };

static asn1SccUint32 masks2[] = { 0x0,
                                  0xFF,
                                  0xFF00,
                                  0xFF0000,
                                  0xFF000000 };


/***********************************************************************************************/
/*   Bit Stream Functions                                                                      */
/***********************************************************************************************/

void BitStream_Init(BitStream* pBitStrm, byte* buf, long count)
{
    pBitStrm->count = count;
    pBitStrm->buf = buf;
    memset(pBitStrm->buf,0x0,(size_t)count);
    pBitStrm->currentByte = 0;
    pBitStrm->currentBit=0;
}

void BitStream_AttachBuffer(BitStream* pBitStrm, unsigned char* buf, long count)
{
    pBitStrm->count = count;
    pBitStrm->buf = buf;
    pBitStrm->currentByte = 0;
    pBitStrm->currentBit=0;
}

asn1SccSint BitStream_GetLength(BitStream* pBitStrm)
{
    int ret = pBitStrm->currentByte;
    if (pBitStrm->currentBit)
        ret++;
    return ret;
}


void BitStream_AppendBitOne(BitStream* pBitStrm)
{
    pBitStrm->buf[pBitStrm->currentByte] |= masks[pBitStrm->currentBit];

    if (pBitStrm->currentBit<7)
        pBitStrm->currentBit++;
    else {
        pBitStrm->currentBit=0;
        pBitStrm->currentByte++;
    }
    assert(pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8);
}

void BitStream_AppendBitZero(BitStream* pBitStrm)
{
    if (pBitStrm->currentBit<7)
        pBitStrm->currentBit++;
    else {
        pBitStrm->currentBit=0;
        pBitStrm->currentByte++;
    }
    assert(pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8);
}

void BitStream_AppendNBitZero(BitStream* pBitStrm, int nbits)
{
    int totalBits = pBitStrm->currentBit + nbits;
    pBitStrm->currentBit = totalBits % 8;
    pBitStrm->currentByte += totalBits / 8;
}

void BitStream_AppendNBitOne(BitStream* pBitStrm, int nbits)
{
    int i;

    while(nbits>=8) {
        BitStream_AppendByte(pBitStrm, 0xFF, FALSE);
        nbits-=8;
    }
    for(i=0; i<nbits; i++)
        BitStream_AppendBitOne(pBitStrm);

}

void BitStream_AppendBits(BitStream* pBitStrm, const byte* srcBuffer, int nbits)
{
    int i=0;
    byte lastByte=0;

    while(nbits>=8) {
        BitStream_AppendByte(pBitStrm, srcBuffer[i], FALSE);
        nbits-=8;
        i++;
    }
    if (nbits > 0) {
      lastByte = (byte)(srcBuffer[i]>>(8-nbits));
      BitStream_AppendPartialByte(pBitStrm, lastByte, (byte)nbits, FALSE);
    }
}

void BitStream_AppendBit(BitStream* pBitStrm, flag v)
{
    if (v)
        pBitStrm->buf[pBitStrm->currentByte] |= masks[pBitStrm->currentBit];

    if (pBitStrm->currentBit<7)
        pBitStrm->currentBit++;
    else {
        pBitStrm->currentBit=0;
        pBitStrm->currentByte++;
    }
    assert(pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8);
}


flag BitStream_ReadBit(BitStream* pBitStrm, flag* v)
{
    *v = pBitStrm->buf[pBitStrm->currentByte] & masks[pBitStrm->currentBit];

    if (pBitStrm->currentBit<7)
        pBitStrm->currentBit++;
    else {
        pBitStrm->currentBit=0;
        pBitStrm->currentByte++;
    }
    return pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8;
}

void BitStream_AppendByte(BitStream* pBitStrm, byte v, flag negate)
{
    int cb = pBitStrm->currentBit;
    int ncb = 8 - cb;
    if (negate)
        v=(byte)~v;
    pBitStrm->buf[pBitStrm->currentByte++] |=  (byte)(v >> cb);

    assert(pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8);

    if (cb)
        pBitStrm->buf[pBitStrm->currentByte] |= (byte)(v << ncb);

}

void BitStream_AppendByte0(BitStream* pBitStrm, byte v)
{
    int cb = pBitStrm->currentBit;
    int ncb = 8 - cb;

    pBitStrm->buf[pBitStrm->currentByte++] |=  (byte)(v >> cb);

    assert(pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8);

    if (cb)
        pBitStrm->buf[pBitStrm->currentByte] |= (byte)(v << ncb);

}


flag BitStream_ReadByte(BitStream* pBitStrm, byte* v)
{
    int cb = pBitStrm->currentBit;
    int ncb = 8 - pBitStrm->currentBit;
    *v= (byte)(pBitStrm->buf[pBitStrm->currentByte++]  << cb);

    if (cb) {
        *v |= (byte)(pBitStrm->buf[pBitStrm->currentByte] >> ncb);
    }

    return pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8;
}

flag BitStream_ReadBits(BitStream* pBitStrm, byte* BuffToWrite, int nbits)
{
    int i=0;

    while (nbits >= 8) {
        if (!BitStream_ReadByte(pBitStrm, &BuffToWrite[i]))
            return FALSE;
        nbits-=8;
        i++;
    }

    if (nbits > 0) {
      if (!BitStream_ReadPartialByte(pBitStrm, &BuffToWrite[i], (byte)nbits))
        return FALSE;
      BuffToWrite[i] = (byte)(BuffToWrite[i] << (8 - nbits));
    }

    return TRUE;
}

/* nbits 1..7*/
void BitStream_AppendPartialByte(BitStream* pBitStrm, byte v, byte nbits, flag negate)
{
    int cb = pBitStrm->currentBit;
    int totalBits = cb+nbits;
    int totalBitsForNextByte;
    if (negate)
        v=masksb[nbits]& ((byte)~v);

    if (totalBits<=8) {
        pBitStrm->buf[pBitStrm->currentByte] |=  (byte)(v <<(8 -totalBits));
        pBitStrm->currentBit+=nbits;
        if (pBitStrm->currentBit==8) {
            pBitStrm->currentBit=0;
            pBitStrm->currentByte++;
        }
    }
    else {
        totalBitsForNextByte = totalBits - 8;
        pBitStrm->buf[pBitStrm->currentByte++] |= (byte)(v >> totalBitsForNextByte);
        pBitStrm->buf[pBitStrm->currentByte] |= (byte)(v << (8 - totalBitsForNextByte));
        pBitStrm->currentBit = totalBitsForNextByte;
    }
    assert(pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8);

}

/* nbits 1..7*/
flag BitStream_ReadPartialByte(BitStream* pBitStrm, byte *v, byte nbits)
{
    int cb = pBitStrm->currentBit;
    int totalBits = cb+nbits;
    int totalBitsForNextByte;

    if (totalBits<=8) {
        *v = (byte)((pBitStrm->buf[pBitStrm->currentByte] >> (8 -totalBits)) & masksb[nbits]);
        pBitStrm->currentBit+=nbits;
        if (pBitStrm->currentBit==8) {
            pBitStrm->currentBit=0;
            pBitStrm->currentByte++;
        }
    }
    else {
        totalBitsForNextByte = totalBits - 8;
        *v = (byte)(pBitStrm->buf[pBitStrm->currentByte++] << totalBitsForNextByte);
        *v |= (byte)(pBitStrm->buf[pBitStrm->currentByte] >> (8 - totalBitsForNextByte));
        *v &= masksb[nbits];
        pBitStrm->currentBit = totalBitsForNextByte;
    }
    return pBitStrm->currentByte*8+pBitStrm->currentBit<=pBitStrm->count*8;
}



/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/*   Integer Functions                                                                     */
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/



static void BitStream_EncodeNonNegativeInteger32Neg(BitStream* pBitStrm,
                                                    asn1SccUint32 v,
                                                    flag negate)
{
    int cc;
    asn1SccUint32 curMask;
    int pbits;

    if (v==0)
        return;

    if (v<0x100) {
        cc = 8;
        curMask = 0x80;
    } else if (v<0x10000) {
        cc = 16;
        curMask = 0x8000;
    } else if (v<0x1000000) {
        cc = 24;
        curMask = 0x800000;
    } else {
        cc = 32;
        curMask = 0x80000000;
    }

    while( (v & curMask)==0 ) {
        curMask >>=1;
        cc--;
    }

    pbits = cc % 8;
    if (pbits) {
        cc-=pbits;
        BitStream_AppendPartialByte(pBitStrm,(byte)(v>>cc), (byte)pbits, negate);
    }

    while (cc) {
        asn1SccUint32 t1 = v & masks2[cc>>3];
        cc-=8;
        BitStream_AppendByte(pBitStrm, (byte)(t1 >>cc), negate);
    }

}

static flag BitStream_DecodeNonNegativeInteger32Neg(BitStream* pBitStrm,
                                                    asn1SccUint32* v,
                                                    int nBits)
{
    byte b;
    *v = 0;
    while(nBits>=8) {
        *v<<=8;
        if (!BitStream_ReadByte(pBitStrm, &b))
            return FALSE;
        *v|= b;
        nBits -=8;
    }
    if (nBits)
    {
        *v<<=nBits;
        if (!BitStream_ReadPartialByte(pBitStrm, &b, (byte)nBits))
            return FALSE;
        *v|=b;
    }

    return TRUE;
}



void BitStream_EncodeNonNegativeInteger(BitStream* pBitStrm, asn1SccUint v)
{

#if WORD_SIZE==8
    if (v<0x100000000LL)
        BitStream_EncodeNonNegativeInteger32Neg(pBitStrm, (asn1SccUint32)v, 0);
    else {
        asn1SccUint32 hi = (asn1SccUint32)(v>>32);
        asn1SccUint32 lo = (asn1SccUint32)v;
        int nBits;
        BitStream_EncodeNonNegativeInteger32Neg(pBitStrm, hi, 0);

        nBits = GetNumberOfBitsForNonNegativeInteger(lo);
        BitStream_AppendNBitZero(pBitStrm, 32-nBits);
        BitStream_EncodeNonNegativeInteger32Neg(pBitStrm, lo, 0);
    }
#else
    BitStream_EncodeNonNegativeInteger32Neg(pBitStrm, v, 0);
#endif
}


flag BitStream_DecodeNonNegativeInteger(BitStream* pBitStrm, asn1SccUint* v, int nBits)
{
#if WORD_SIZE==8
        asn1SccUint32 hi=0;
        asn1SccUint32 lo=0;
        flag ret;

        if (nBits<=32)
        {
            ret = BitStream_DecodeNonNegativeInteger32Neg(pBitStrm, &lo, nBits);
            *v = lo;
            return ret;
        }

        ret = BitStream_DecodeNonNegativeInteger32Neg(pBitStrm, &hi, 32) && BitStream_DecodeNonNegativeInteger32Neg(pBitStrm, &lo, nBits-32);

        *v = hi;
        *v <<=nBits-32;
         *v |= lo;
        return ret;
#else
    return BitStream_DecodeNonNegativeInteger32Neg(pBitStrm, v, nBits);
#endif
}


void BitStream_EncodeNonNegativeIntegerNeg(BitStream* pBitStrm, asn1SccUint v, flag negate)
{
#if WORD_SIZE==8
    if (v<0x100000000LL)
        BitStream_EncodeNonNegativeInteger32Neg(pBitStrm, (asn1SccUint32)v, negate);
    else {
        int nBits;
        asn1SccUint32 hi = (asn1SccUint32)(v>>32);
        asn1SccUint32 lo = (asn1SccUint32)v;
        BitStream_EncodeNonNegativeInteger32Neg(pBitStrm, hi, negate);

        //bug !!!!
        if (negate)
            lo = ~lo;
        nBits = GetNumberOfBitsForNonNegativeInteger(lo);
        BitStream_AppendNBitZero(pBitStrm, 32-nBits);
        BitStream_EncodeNonNegativeInteger32Neg(pBitStrm, lo, 0);
    }
#else
    BitStream_EncodeNonNegativeInteger32Neg(pBitStrm, v, negate);
#endif
}




void BitStream_EncodeConstraintWholeNumber(BitStream* pBitStrm, asn1SccSint v, asn1SccSint min, asn1SccSint max)
{
    int nRangeBits;
    int nBits;
    asn1SccUint range;
    assert(min<=max);
    range = (asn1SccUint)(max-min);
    if(!range)
        return;
    nRangeBits = GetNumberOfBitsForNonNegativeInteger(range);
    nBits = GetNumberOfBitsForNonNegativeInteger((asn1SccUint)(v-min));
    BitStream_AppendNBitZero(pBitStrm, nRangeBits-nBits);
    BitStream_EncodeNonNegativeInteger(pBitStrm,(asn1SccUint)(v-min));
}



flag BitStream_DecodeConstraintWholeNumber(BitStream* pBitStrm, asn1SccSint* v, asn1SccSint min, asn1SccSint max)
{
    asn1SccUint uv;
    int nRangeBits;
    asn1SccUint range = (asn1SccUint)(max-min);

    ASSERT_OR_RETURN_FALSE(min<=max);


    *v=0;
    if(!range) {
        *v = min;
        return TRUE;
    }

    nRangeBits = GetNumberOfBitsForNonNegativeInteger(range);


    if (BitStream_DecodeNonNegativeInteger(pBitStrm, &uv, nRangeBits))
    {
        *v = ((asn1SccSint)uv)+min;
        return TRUE;
    }
    return FALSE;
}



void BitStream_EncodeSemiConstraintWholeNumber(BitStream* pBitStrm, asn1SccSint v, asn1SccSint min)
{
    int nBytes;
    assert(v>=min);
    nBytes = GetLengthInBytesOfUInt((asn1SccUint)(v - min));

    /* encode length */
    BitStream_EncodeConstraintWholeNumber(pBitStrm, nBytes, 0, 255); /*8 bits, first bit is always 0*/
    /* put required zeros*/
    BitStream_AppendNBitZero(pBitStrm,nBytes*8-GetNumberOfBitsForNonNegativeInteger((asn1SccUint)(v-min)));
    /*Encode number */
    BitStream_EncodeNonNegativeInteger(pBitStrm,(asn1SccUint)(v-min));
}


flag BitStream_DecodeSemiConstraintWholeNumber(BitStream* pBitStrm, asn1SccSint* v, asn1SccSint min)
{
    asn1SccSint nBytes;
    int i;
    *v=0;
    if (!BitStream_DecodeConstraintWholeNumber(pBitStrm, &nBytes, 0, 255))
        return FALSE;
    for(i=0;i<nBytes;i++) {
        byte b=0;
        if (!BitStream_ReadByte(pBitStrm, &b))
            return FALSE;
        *v = (*v<<8) | b;
    }
    *v+=min;
    return TRUE;
}



void BitStream_EncodeUnConstraintWholeNumber(BitStream* pBitStrm, asn1SccSint v)
{
    int nBytes = GetLengthInBytesOfSInt(v);

    /* encode length */
    BitStream_EncodeConstraintWholeNumber(pBitStrm, nBytes, 0, 255); /*8 bits, first bit is always 0*/

    if (v>=0) {
        BitStream_AppendNBitZero(pBitStrm,nBytes*8-GetNumberOfBitsForNonNegativeInteger((asn1SccUint)v));
        BitStream_EncodeNonNegativeInteger(pBitStrm,(asn1SccUint)(v));
    }
    else {
        BitStream_AppendNBitOne(pBitStrm,nBytes*8-GetNumberOfBitsForNonNegativeInteger((asn1SccUint)(-v-1)));
        BitStream_EncodeNonNegativeIntegerNeg(pBitStrm, (asn1SccUint)(-v-1), 1);
    }
}

flag BitStream_DecodeUnConstraintWholeNumber(BitStream* pBitStrm, asn1SccSint* v)
{
    asn1SccSint nBytes;
    int i;
    flag valIsNegative=FALSE;
    *v=0;


    if (!BitStream_DecodeConstraintWholeNumber(pBitStrm, &nBytes, 0, 255))
        return FALSE;


    for(i=0;i<nBytes;i++) {
        byte b=0;
        if (!BitStream_ReadByte(pBitStrm, &b))
            return FALSE;
        if (!i) {
            valIsNegative = b>0x7F;
            if (valIsNegative)
                *v=-1;
        }
        *v = (*v<<8) | b;
    }
    return TRUE;
}



