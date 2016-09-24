/*
 * BitStream_real.c
 *
 */

#include "asn1scc/BitStream.h"
#include "asn1scc/util.h"
#include "asn1scc/real.h"

#ifndef INFINITY
  #ifdef __GNUC__
    #define INFINITY (__builtin_inf())
  #endif
#endif

/*
            Bynary encoding will be used
            REAL = M*B^E
            where
            M = S*N*2^F

            ENCODING is done within three parts
            part 1 is 1 byte header
            part 2 is 1 or more byte for exponent
            part 3 is 3 or more byte for mantissa (N)

            First byte
            S :0-->+, S:1-->-1
            Base will be always be 2 (implied by 6th and 5th bit which are zero)
            ab: F  (0..3)
            cd:00 --> 1 byte for exponent as 2's complement
            cd:01 --> 2 byte for exponent as 2's complement
            cd:10 --> 3 byte for exponent as 2's complement
            cd:11 --> 1 byte for encoding the length of the exponent, then the expoent

             8 7 6 5 4 3 2 1
            +-+-+-+-+-+-+-+-+
            |1|S|0|0|a|b|c|d|
            +-+-+-+-+-+-+-+-+
*/


void BitStream_EncodeReal(BitStream* pBitStrm, double v)
{
    byte header=0x80;
    int nExpLen;
    int nManLen;
    int exponent;
    asn1SccUint64 mantissa;


    if (v==0.0)
    {
        BitStream_EncodeConstraintWholeNumber(pBitStrm, 0, 0, 0xFF);
        return;
    }

    if (v == INFINITY )
    {
        BitStream_EncodeConstraintWholeNumber(pBitStrm, 1, 0, 0xFF);
        BitStream_EncodeConstraintWholeNumber(pBitStrm, 0x40, 0, 0xFF);
        return;
    }

    if (v == -INFINITY)
    {
        BitStream_EncodeConstraintWholeNumber(pBitStrm, 1, 0, 0xFF);
        BitStream_EncodeConstraintWholeNumber(pBitStrm, 0x41, 0, 0xFF);
        return;
    }
    if (v < 0) {
        header |= 0x40;
        v=-v;
    }

    CalculateMantissaAndExponent(v, &exponent, &mantissa);
    nExpLen = GetLengthInBytesOfSInt(exponent);
    nManLen = GetLengthInBytesOfUInt(mantissa);
    assert(nExpLen<=3);
    if (nExpLen == 2)
        header |= 1;
    else if (nExpLen == 3)
        header |= 2;


    /* encode length */
    BitStream_EncodeConstraintWholeNumber(pBitStrm, 1+nExpLen+nManLen, 0, 0xFF);

    /* encode header */
    BitStream_EncodeConstraintWholeNumber(pBitStrm, header, 0, 0xFF);

    /* encode exponent */
    if (exponent>=0) {
        BitStream_AppendNBitZero(pBitStrm,nExpLen*8-GetNumberOfBitsForNonNegativeInteger((asn1SccUint)exponent));
        BitStream_EncodeNonNegativeInteger(pBitStrm,(asn1SccUint)exponent);
    }
    else {
        BitStream_AppendNBitOne(pBitStrm,nExpLen*8-GetNumberOfBitsForNonNegativeInteger((asn1SccUint)(-exponent-1)));
        BitStream_EncodeNonNegativeIntegerNeg(pBitStrm,(asn1SccUint)(-exponent-1), 1);
    }


    /* encode mantissa */
    BitStream_AppendNBitZero(pBitStrm,nManLen*8-GetNumberOfBitsForNonNegativeInteger((asn1SccUint)(mantissa)));
    BitStream_EncodeNonNegativeInteger(pBitStrm,mantissa);

}



flag DecodeRealAsBinaryEncoding(BitStream* pBitStrm, int length, byte header, double* v);
flag DecodeRealUsingDecimalEncoding(BitStream* pBitStrm, int length, byte header, double* v);

flag BitStream_DecodeReal(BitStream* pBitStrm, double* v)
{
    byte header;
    byte length;

    if (!BitStream_ReadByte(pBitStrm, &length))
        return FALSE;
    if (length == 0)
    {
        *v=0.0;
        return TRUE;
    }

    if (!BitStream_ReadByte(pBitStrm, &header))
        return FALSE;

    if (header==0x40)
    {
        *v = INFINITY;
        return TRUE;
    }

    if (header==0x41)
    {
        *v = -INFINITY;
        return TRUE;
    }
    if (header & 0x80)
        return DecodeRealAsBinaryEncoding(pBitStrm, length-1, header, v);


    return DecodeRealUsingDecimalEncoding(pBitStrm, length-1, header, v);
}



flag DecodeRealAsBinaryEncoding(BitStream* pBitStrm, int length, byte header, double* v)
{
    int sign=1;
    //int base=2;
    int F;
    unsigned factor=1;
    int expLen;
    int exponent = 0;
    int expFactor = 1;
    asn1SccUint N=0;
    int i;

    if (header & 0x40)
        sign = -1;
    if (header & 0x10) {
        //base = 8;
        expFactor = 3;
    }
    else if (header & 0x20) {
        //base = 16;
        expFactor = 4;
    }

    F= (header & 0x0C)>>2;
    factor<<=F;

    expLen = (header & 0x03) + 1;

    if (expLen>length)
        return FALSE;

    for(i=0;i<expLen;i++) {
        byte b=0;
        if (!BitStream_ReadByte(pBitStrm, &b))
            return FALSE;
        if (!i) {
            if (b>0x7F)
                exponent=-1;
        }
        exponent = exponent<<8 | b;
    }
    length-=expLen;

    for(i=0;i<length;i++) {
        byte b=0;
        if (!BitStream_ReadByte(pBitStrm, &b))
            return FALSE;
        N = N<<8 | b;
    }


//  *v = N*factor * pow(base,exp);
    *v = GetDoubleByMantissaAndExp(N*factor, expFactor*exponent);

    if (sign<0)
        *v = -(*v);


    return TRUE;
}

flag DecodeRealUsingDecimalEncoding(BitStream* pBitStrm, int length, byte header, double* v)
{
    ASSERT_OR_RETURN_FALSE(0);

    (void) pBitStrm;
    (void) length;
    (void) header;
    (void) v;

    return FALSE;
}

