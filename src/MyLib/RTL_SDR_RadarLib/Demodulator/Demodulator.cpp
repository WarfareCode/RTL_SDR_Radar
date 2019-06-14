/* Mode1090, a Mode S messages decoder for RTLSDR devices.
 *
 * Copyright (C) 2012 by Salvatore Sanfilippo <antirez@gmail.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>
#include <QDateTime>
#include <QDebug>

#include "Demodulator.h"
#include "Aircraft.h"

/* Capability table. */
static const char *ca_str[8] = {
    /* 0 */ "Level 1 (Survillance Only)",
    /* 1 */ "Level 2 (DF0,4,5,11)",
    /* 2 */ "Level 3 (DF0,4,5,11,20,21)",
    /* 3 */ "Level 4 (DF0,4,5,11,20,21,24)",
    /* 4 */ "Level 2+3+4 \n(DF0,4,5,11,20,21,24,code7 - is on ground)",
    /* 5 */ "Level 2+3+4 \n(DF0,4,5,11,20,21,24,code7 - is on airborne)",
    /* 6 */ "Level 2+3+4 (DF0,4,5,11,20,21,24,code7)",
    /* 7 */ "Level 7 ???"
};

/* Flight status table. */
static const char *fs_str[8] = {
    /* 0 */ "Normal, Airborne",
    /* 1 */ "Normal, On the ground",
    /* 2 */ "ALERT,  Airborne",
    /* 3 */ "ALERT,  On the ground",
    /* 4 */ "ALERT & Special Position Identification. Airborne or Ground",
    /* 5 */ "Special Position Identification. Airborne or Ground",
    /* 6 */ "Value 6 is not assigned",
    /* 7 */ "Value 7 is not assigned"
};

/* Parity table for MODE S Messages.
 * The table contains 112 elements, every element corresponds to a bit set
 * in the message, starting from the first bit of actual data after the
 * preamble.
 *
 * For messages of 112 bit, the whole table is used.
 * For messages of 56 bits only the last 56 elements are used.
 *
 * The algorithm is as simple as xoring all the elements in this table
 * for which the corresponding bit on the message is set to 1.
 *
 * The latest 24 elements in this table are set to 0 as the checksum at the
 * end of the message should not affect the computation.
 *
 * Note: this function can be used with DF11 and DF17, other modes have
 * the CRC xored with the sender address as they are reply to interrogations,
 * but a casual listener can't split the address from the checksum.
 */
static uint32_t modes_checksum_table[112] =
    {
        0x3935ea, 0x1c9af5, 0xf1b77e, 0x78dbbf, 0xc397db, 0x9e31e9, 0xb0e2f0, 0x587178,
        0x2c38bc, 0x161c5e, 0x0b0e2f, 0xfa7d13, 0x82c48d, 0xbe9842, 0x5f4c21, 0xd05c14,
        0x682e0a, 0x341705, 0xe5f186, 0x72f8c3, 0xc68665, 0x9cb936, 0x4e5c9b, 0xd8d449,
        0x939020, 0x49c810, 0x24e408, 0x127204, 0x093902, 0x049c81, 0xfdb444, 0x7eda22,
        0x3f6d11, 0xe04c8c, 0x702646, 0x381323, 0xe3f395, 0x8e03ce, 0x4701e7, 0xdc7af7,
        0x91c77f, 0xb719bb, 0xa476d9, 0xadc168, 0x56e0b4, 0x2b705a, 0x15b82d, 0xf52612,
        0x7a9309, 0xc2b380, 0x6159c0, 0x30ace0, 0x185670, 0x0c2b38, 0x06159c, 0x030ace,
        0x018567, 0xff38b7, 0x80665f, 0xbfc92b, 0xa01e91, 0xaff54c, 0x57faa6, 0x2bfd53,
        0xea04ad, 0x8af852, 0x457c29, 0xdd4410, 0x6ea208, 0x375104, 0x1ba882, 0x0dd441,
        0xf91024, 0x7c8812, 0x3e4409, 0xe0d800, 0x706c00, 0x383600, 0x1c1b00, 0x0e0d80,
        0x0706c0, 0x038360, 0x01c1b0, 0x00e0d8, 0x00706c, 0x003836, 0x001c1b, 0xfff409,
        0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
        0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
        0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
};

Demodulator::Demodulator(QSharedPointer<IPoolObject> pool)
{
    setAutoDelete(false);

    _pool = pool;

    /* Allocate the ICAO address cache. We use two uint32_t for every
     * entry because it's a addr / timestamp pair for every entry. */
    icao_cache.resize(MODES_ICAO_CACHE_LEN*2);
    memset(icao_cache.data(), 0 ,sizeof(uint32_t) * MODES_ICAO_CACHE_LEN * 2);

    maglut = new uint16_t [129 * 129 * 2];
    for (int i = 0; i <= 128; i++)
        for (int q = 0; q <= 128; q++)
            maglut[i * 129 + q] = round(sqrt(i * i + q * q) * 360.0);
}


Demodulator::~Demodulator()
{
    _pool.clear();
    delete  maglut;
}

bool Demodulator::setDataForDemodulate(const QVector<uint8_t> &vector)
{
    computeMagnitudeVector(vector,_magnitude);
}

void Demodulator::run()
{
    demodulate();
}

QByteArray Demodulator::getRawDump()
{
    QByteArray array;
    for(auto &a: _hashAircrafts.values())
    {
        array.append(a->serialize());
        qDebug()<< a->toString();
    }
    return array;
}

bool Demodulator::demodulate()
{
    detectModeS(_magnitude.data(),_magnitude.size());
}


/* Turn I/Q samples pointed by data into the magnitude vector
 * pointed by magnitude. */
void Demodulator::computeMagnitudeVector(const QVector<uint8_t> &vector,
                                         QVector<uint16_t> &magnitude)
{
    int len = vector.size();
    if(_magnitude.size() < len / 2)
        _magnitude.resize(len / 2);

    uint16_t *m = magnitude.data();
    const unsigned char *p = vector.data();
    uint32_t j;

    /* Compute the magnitudo vector. It's just SQRT(I^2 + Q^2), but
     * we rescale to the 0-255 range to exploit the full resolution. */
    for (j = 0; j < vector.size(); j += 2)
    {
        int i = p[j]-127;
        int q = p[j+1]-127;

        if (i < 0) i = -i;
        if (q < 0) q = -q;
        m[j/2] = maglut[i*129+q];
    }
}


/* Detect a Mode S messages inside the magnitude buffer pointed by 'm' and of
 * size 'mlen' bytes. Every detected Mode S message is convert it into a
 * stream of bits and passed to the function to display it. */
void Demodulator::detectModeS(uint16_t *m, uint32_t mlen)
{
    unsigned char bits[MODES_LONG_MSG_BITS];
    unsigned char msg[MODES_LONG_MSG_BITS/2];
    uint16_t aux[MODES_LONG_MSG_BITS*2];

    bool use_correction = false;

    /* The Mode S preamble is made of impulses of 0.5 microseconds at
     * the following time offsets:
     *
     * 0   - 0.5 usec: first impulse.
     * 1.0 - 1.5 usec: second impulse.
     * 3.5 - 4   usec: third impulse.
     * 4.5 - 5   usec: last impulse.
     *
     * Since we are sampling at 2 Mhz every sample in our magnitude vector
     * is 0.5 usec, so the preamble will look like this, assuming there is
     * an impulse at offset 0 in the array:
     *
     * 0   -----------------
     * 1   -
     * 2   ------------------
     * 3   --
     * 4   -
     * 5   --
     * 6   -
     * 7   ------------------
     * 8   --
     * 9   -------------------
     */
    for (uint32_t j = 0; j < mlen - MODES_FULL_LEN * 2; j++)
    {
        int low, high, delta, errors;
        int good_message = 0;

        if (use_correction)
            goto good_preamble; /* We already checked it. */

        /* First check of relations between the first 10 samples
         * representing a valid preamble. We don't even investigate further
         * if this simple test is not passed. */
        if (!(m[j] > m[j+1] &&
              m[j+1] < m[j+2] &&
              m[j+2] > m[j+3] &&
              m[j+3] < m[j] &&
              m[j+4] < m[j] &&
              m[j+5] < m[j] &&
              m[j+6] < m[j] &&
              m[j+7] > m[j+8] &&
              m[j+8] < m[j+9] &&
              m[j+9] > m[j+6]))
        {
            if (debug & MODES_DEBUG_NOPREAMBLE &&
                m[j] > MODES_DEBUG_NOPREAMBLE_LEVEL)
                dumpRawMessage("Unexpected ratio among first 10 samples",
                               msg, m, j);
            continue;
        }

        /* The samples between the two spikes must be < than the average
         * of the high spikes level. We don't test bits too near to
         * the high levels as signals can be out of phase so part of the
         * energy can be in the near samples. */
        high = (m[j]+m[j+2]+m[j+7]+m[j+9])/6;
        if (m[j+4] >= high ||
            m[j+5] >= high)
        {
            if (debug & MODES_DEBUG_NOPREAMBLE &&
                m[j] > MODES_DEBUG_NOPREAMBLE_LEVEL)
                dumpRawMessage(
                    "Too high level in samples between 3 and 6",
                    msg, m, j);
            continue;
        }

        /* Similarly samples in the range 11-14 must be low, as it is the
         * space between the preamble and real data. Again we don't test
         * bits too near to high levels, see above. */
        if (m[j+11] >= high ||
            m[j+12] >= high ||
            m[j+13] >= high ||
            m[j+14] >= high)
        {
            if (debug & MODES_DEBUG_NOPREAMBLE &&
                m[j] > MODES_DEBUG_NOPREAMBLE_LEVEL)
                dumpRawMessage(
                    "Too high level in samples between 10 and 15",
                    msg, m, j);
            continue;
        }
        stat_valid_preamble++;

    good_preamble:
        /* If the previous attempt with this message failed, retry using
         * magnitude correction. */
        if (use_correction)
        {
            memcpy(aux,m+j+MODES_PREAMBLE_US*2,sizeof(aux));
            if (j && detectOutOfPhase(m+j))
            {
                applyPhaseCorrection(m+j);
                stat_out_of_phase++;
            }
            /* TODO ... apply other kind of corrections. */
        }

        /* Decode all the next 112 bits, regardless of the actual message
         * size. We'll check the actual message type later. */
        errors = 0;
        for (uint32_t i = 0; i < MODES_LONG_MSG_BITS*2; i += 2)
        {
            low = m[j+i+MODES_PREAMBLE_US*2];
            high = m[j+i+MODES_PREAMBLE_US*2+1];
            delta = low-high;
            if (delta < 0) delta = -delta;

            if (i > 0 && delta < 256)
            {
                bits[i/2] = bits[i/2-1];
            }
            else if (low == high)
            {
                /* Checking if two adiacent samples have the same magnitude
                 * is an effective way to detect if it's just random noise
                 * that was detected as a valid preamble. */
                bits[i/2] = 2; /* error */
                if (i < MODES_SHORT_MSG_BITS*2) errors++;
            }
            else if (low > high)
            {
                bits[i/2] = 1;
            }
            else
            {
                /* (low < high) for exclusion  */
                bits[i/2] = 0;
            }
        }

        /* Restore the original message if we used magnitude correction. */
        if (use_correction)
            memcpy(m+j+MODES_PREAMBLE_US*2,aux,sizeof(aux));

        /* Pack bits into bytes */
        for (uint32_t i = 0; i < MODES_LONG_MSG_BITS; i += 8)
        {
            msg[i/8] =
                bits[i]<<7 |
                bits[i+1]<<6 |
                bits[i+2]<<5 |
                bits[i+3]<<4 |
                bits[i+4]<<3 |
                bits[i+5]<<2 |
                bits[i+6]<<1 |
                bits[i+7];
        }

        int msgtype = msg[0]>>3;
        int msglen = modesMessageLenByType(msgtype)/8;

        /* Last check, high and low bits are different enough in magnitude
         * to mark this as real message and not just noise? */
        delta = 0;
        for (uint32_t i = 0; i < msglen*8*2; i += 2)
        {
            delta += abs(m[j+i+MODES_PREAMBLE_US*2] - m[j+i+MODES_PREAMBLE_US*2+1]);
        }
        delta /= msglen*4;

        /* Filter for an average delta of three is small enough to let almost
         * every kind of message to pass, but high enough to filter some
         * random noise. */
        if (delta < 10*255)
        {
            use_correction = false;
            continue;
        }

        /* If we reached this point, and error is zero, we are very likely
         * with a Mode S message in our hands, but it may still be broken
         * and CRC may not be correct. This is handled by the next layer. */
        if (errors == 0 || (aggressive && errors < 3))
        {
            struct modesMessage mm;

            /* Decode the received message and update statistics */
            decodeModesMessage(&mm,msg);

            /* Update statistics. */
            if (mm.crcok || use_correction)
            {
                if (errors == 0)
                {
                    stat_demodulated++;
                    //addDebugMsg(QString("demodulated frame num = %1\n").arg(stat_demodulated));
                }
                if (mm.errorbit == -1)
                {
                    if (mm.crcok)
                        stat_goodcrc++;
                    else
                        stat_badcrc++;
                }
                else
                {
                    stat_badcrc++;
                    stat_fixed++;
                    if (mm.errorbit < MODES_LONG_MSG_BITS)
                        stat_single_bit_fix++;
                    else
                        stat_two_bits_fix++;
                }
            }

            /* Output debug mode info if needed. */
            if (use_correction == false)
            {
                if (debug & MODES_DEBUG_DEMOD)
                    dumpRawMessage("Demodulated with 0 errors", msg, m, j);
                else if (debug & MODES_DEBUG_BADCRC &&
                         mm.msgtype == 17 &&
                         (!mm.crcok || mm.errorbit != -1))
                    dumpRawMessage("Decoded with bad CRC", msg, m, j);
                else if (debug & MODES_DEBUG_GOODCRC && mm.crcok &&
                         mm.errorbit == -1)
                    dumpRawMessage("Decoded with good CRC", msg, m, j);
            }

            /* Skip this message if we are sure it's fine. */
            if (mm.crcok)
            {
                j += (MODES_PREAMBLE_US+(msglen*8))*2;
                good_message = 1;
                if (use_correction)
                    mm.phase_corrected = 1;
            }

            /* Pass data to the next layer */
            useModesMessage(&mm);
        }
        else
        {
            if (debug & MODES_DEBUG_DEMODERR && use_correction)
            {
                qDebug("The following message has %d demod errors\n", errors);
                dumpRawMessage("Demodulated with errors", msg, m, j);
            }
        }

        /* Retry with phase correction if possible. */
        if (!good_message && !use_correction)
        {
            j--;
            use_correction = true;
        }
        else
        {
            use_correction = false;
        }
    }

    interactiveRemoveStaleAircrafts();
}

/* Try to fix single bit errors using the checksum. On success modifies
 * the original buffer with the fixed version, and returns the position
 * of the error bit. Otherwise if fixing failed -1 is returned. */
int Demodulator::fixSingleBitErrors(unsigned char *msg, int bits)
{
    unsigned char aux[MODES_LONG_MSG_BITS/8];

    for (int j = 0; j < bits; j++)
    {
        int byte = j/8;
        int bitmask = 1 << (7-(j%8));
        uint32_t crc1, crc2;

        memcpy(aux,msg,bits/8);
        aux[byte] ^= bitmask; /* Flip j-th bit. */

        crc1 = ((uint32_t)aux[(bits/8)-3] << 16) |
            ((uint32_t)aux[(bits/8)-2] << 8) |
            (uint32_t)aux[(bits/8)-1];
        crc2 = modesChecksum(aux,bits);

        if (crc1 == crc2)
        {
            /* The error is fixed. Overwrite the original buffer with
             * the corrected sequence, and returns the error bit
             * position. */
            memcpy(msg,aux,bits/8);
            return j;
        }
    }
    return -1;
}

/* Similar to fixSingleBitErrors() but try every possible two bit combination.
 * This is very slow and should be tried only against DF17 messages that
 * don't pass the checksum, and only in Aggressive Mode. */
int Demodulator::fixTwoBitsErrors(unsigned char *msg, int bits)
{
    int j, i;
    unsigned char aux[MODES_LONG_MSG_BITS/8];

    for (j = 0; j < bits; j++)
    {
        int byte1 = j/8;
        int bitmask1 = 1 << (7-(j%8));

        /* Don't check the same pairs multiple times, so i starts from j+1 */
        for (i = j+1; i < bits; i++)
        {
            int byte2 = i/8;
            int bitmask2 = 1 << (7-(i%8));
            uint32_t crc1, crc2;

            memcpy(aux,msg,bits/8);

            aux[byte1] ^= bitmask1; /* Flip j-th bit. */
            aux[byte2] ^= bitmask2; /* Flip i-th bit. */

            crc1 = ((uint32_t)aux[(bits/8)-3] << 16) |
                ((uint32_t)aux[(bits/8)-2] << 8) |
                (uint32_t)aux[(bits/8)-1];
            crc2 = modesChecksum(aux,bits);

            if (crc1 == crc2) {
                /* The error is fixed. Overwrite the original buffer with
                 * the corrected sequence, and returns the error bit
                 * position. */
                memcpy(msg,aux,bits/8);
                /* We return the two bits as a 16 bit integer by shifting
                 * 'i' on the left. This is possible since 'i' will always
                 * be non-zero because i starts from j+1. */
                return j | (i<<8);
            }
        }
    }
    return -1;
}

/* Return -1 if the message is out of fase left-side
 * Return  1 if the message is out of fase right-size
 * Return  0 if the message is not particularly out of phase.
 *
 * Note: this function will access m[-1], so the caller should make sure to
 * call it only if we are not at the start of the current buffer. */
int Demodulator::detectOutOfPhase(uint16_t *m)
{
    if (m[3] > m[2]/3) return 1;
    if (m[10] > m[9]/3) return 1;
    if (m[6] > m[7]/3) return -1;
    if (m[-1] > m[1]/3) return -1;
    return 0;
}


void Demodulator::applyPhaseCorrection(uint16_t *m)
{
    m += 16; /* Skip preamble. */
    for (int j = 0; j < (MODES_LONG_MSG_BITS-1)*2; j += 2)
    {
        if (m[j] > m[j+1])
        {
            /* One */
            m[j+2] = (m[j+2] * 5) / 4;
        }
        else
        {
            /* Zero */
            m[j+2] = (m[j+2] * 4) / 5;
        }
    }
}

/* Given the Downlink Format (DF) of the message, return the message length
 * in bits. */
int Demodulator::modesMessageLenByType(int type)
{
    if (type == 16 || type == 17 ||
        type == 19 || type == 20 ||
        type == 21)
        return MODES_LONG_MSG_BITS;
    else
        return MODES_SHORT_MSG_BITS;
}

/* Decode a raw Mode S message demodulated as a stream of bytes by
 * detectModeS(), and split it into fields populating a modesMessage
 * structure. */
void Demodulator::decodeModesMessage(struct modesMessage *mm, unsigned char *msg)
{
    uint32_t crc2;   /* Computed CRC, used to verify the message CRC. */
    static const char *ais_charset = "?ABCDEFGHIJKLMNOPQRSTUVWXYZ????? ???????????????0123456789??????";

    /* Work on our local copy */
    memcpy(mm->msg,msg,MODES_LONG_MSG_BYTES);
    msg = mm->msg;

    /* Get the message type ASAP as other operations depend on this */
    mm->msgtype = msg[0]>>3;    /* Downlink Format */
    mm->msgbits = modesMessageLenByType(mm->msgtype);

    /* CRC is always the last three bytes. */
    mm->crc = ((uint32_t)msg[(mm->msgbits/8)-3] << 16) |
        ((uint32_t)msg[(mm->msgbits/8)-2] << 8) |
        (uint32_t)msg[(mm->msgbits/8)-1];
    crc2 = modesChecksum(msg,mm->msgbits);

    /* Check CRC and fix single bit errors using the CRC when
     * possible (DF 11 and 17). */
    mm->errorbit = -1;  /* No error */
    mm->crcok = (mm->crc == crc2);

    if (!mm->crcok && fix_errors &&
        (mm->msgtype == 11 || mm->msgtype == 17))
    {
        if ((mm->errorbit = fixSingleBitErrors(msg,mm->msgbits)) != -1)
        {
            mm->crc = modesChecksum(msg,mm->msgbits);
            mm->crcok = 1;
        } else
            if (aggressive && mm->msgtype == 17 &&
                (mm->errorbit = fixTwoBitsErrors(msg,mm->msgbits)) != -1)
        {
            mm->crc = modesChecksum(msg,mm->msgbits);
            mm->crcok = 1;
        }
    }

    /* Note that most of the other computation happens *after* we fix
     * the single bit errors, otherwise we would need to recompute the
     * fields again. */
    mm->ca = msg[0] & 7;        /* Responder capabilities. */

    /* ICAO address */
    mm->aa1 = msg[1];
    mm->aa2 = msg[2];
    mm->aa3 = msg[3];

    /* DF 17 type (assuming this is a DF17, otherwise not used) */
    mm->metype = msg[4] >> 3;   /* Extended squitter message type. */
    mm->mesub = msg[4] & 7;     /* Extended squitter message subtype. */

    /* Fields for DF4,5,20,21 */
    mm->fs = msg[0] & 7;        /* Flight status for DF4,5,20,21 */
    mm->dr = msg[1] >> 3 & 31;  /* Request extraction of downlink request. */
    mm->um = ((msg[1] & 7)<<3)| /* Request extraction of downlink request. */
        msg[2]>>5;

    /* In the squawk (identity) field bits are interleaved like that
     * (message bit 20 to bit 32):
     *
     * C1-A1-C2-A2-C4-A4-ZERO-B1-D1-B2-D2-B4-D4
     *
     * So every group of three bits A, B, C, D represent an integer
     * from 0 to 7.
     *
     * The actual meaning is just 4 octal numbers, but we convert it
     * into a base ten number tha happens to represent the four
     * octal numbers.
     *
     * For more info: http://en.wikipedia.org/wiki/Gillham_code */
    {
        int a,b,c,d;

        a = ((msg[3] & 0x80) >> 5) |
            ((msg[2] & 0x02) >> 0) |
            ((msg[2] & 0x08) >> 3);
        b = ((msg[3] & 0x02) << 1) |
            ((msg[3] & 0x08) >> 2) |
            ((msg[3] & 0x20) >> 5);
        c = ((msg[2] & 0x01) << 2) |
            ((msg[2] & 0x04) >> 1) |
            ((msg[2] & 0x10) >> 4);
        d = ((msg[3] & 0x01) << 2) |
            ((msg[3] & 0x04) >> 1) |
            ((msg[3] & 0x10) >> 4);
        mm->identity = a*1000 + b*100 + c*10 + d;
    }

    /* DF 11 & 17: try to populate our ICAO addresses whitelist.
     * DFs with an AP field (xored addr and crc), try to decode it. */
    if (mm->msgtype != 11 && mm->msgtype != 17)
    {
        /* Check if we can check the checksum for the Downlink Formats where
         * the checksum is xored with the aircraft ICAO address. We try to
         * brute force it using a list of recently seen aircraft addresses. */
        if (bruteForceAP(msg,mm))
        {
            /* We recovered the message, mark the checksum as valid. */
            mm->crcok = 1;
        }
        else
        {
            mm->crcok = 0;
        }
    }
    else {
        /* If this is DF 11 or DF 17 and the checksum was ok,
         * we can add this address to the list of recently seen
         * addresses. */
        if (mm->crcok && mm->errorbit == -1)
        {
            uint32_t addr = (mm->aa1 << 16) | (mm->aa2 << 8) | mm->aa3;
            addRecentlySeenICAOAddr(addr);
        }
    }

    /* Decode 13 bit altitude for DF0, DF4, DF16, DF20 */
    if (mm->msgtype == 0 || mm->msgtype == 4 ||
        mm->msgtype == 16 || mm->msgtype == 20)
    {
        mm->altitude = decodeAC13Field(msg, &mm->unit);
    }

    /* Decode extended squitter specific stuff. */
    if (mm->msgtype == 17)
    {
        /* Decode the extended squitter message. */

        if (mm->metype >= 1 && mm->metype <= 4)
        {
            /* Aircraft Identification and Category */
            mm->aircraft_type = mm->metype-1;
            mm->flight[0] = ais_charset[msg[5]>>2];
            mm->flight[1] = ais_charset[((msg[5]&3)<<4)|(msg[6]>>4)];
            mm->flight[2] = ais_charset[((msg[6]&15)<<2)|(msg[7]>>6)];
            mm->flight[3] = ais_charset[msg[7]&63];
            mm->flight[4] = ais_charset[msg[8]>>2];
            mm->flight[5] = ais_charset[((msg[8]&3)<<4)|(msg[9]>>4)];
            mm->flight[6] = ais_charset[((msg[9]&15)<<2)|(msg[10]>>6)];
            mm->flight[7] = ais_charset[msg[10]&63];
            mm->flight[8] = '\0';
        }
        else if (mm->metype >= 9 && mm->metype <= 18)
        {
            /* Airborne position Message */
            mm->fflag = msg[6] & (1<<2);
            mm->tflag = msg[6] & (1<<3);
            mm->altitude = decodeAC12Field(msg,&mm->unit);
            mm->raw_latitude = ((msg[6] & 3) << 15) |
                (msg[7] << 7) |
                (msg[8] >> 1);
            mm->raw_longitude = ((msg[8]&1) << 16) |
                (msg[9] << 8) |
                msg[10];
        }
        else if (mm->metype == 19 && mm->mesub >= 1 && mm->mesub <= 4)
        {
            /* Airborne Velocity Message */
            if (mm->mesub == 1 || mm->mesub == 2)
            {
                mm->ew_dir = (msg[5]&4) >> 2;
                mm->ew_velocity = ((msg[5]&3) << 8) | msg[6];
                mm->ns_dir = (msg[7]&0x80) >> 7;
                mm->ns_velocity = ((msg[7]&0x7f) << 3) | ((msg[8]&0xe0) >> 5);
                mm->vert_rate_source = (msg[8]&0x10) >> 4;
                mm->vert_rate_sign = (msg[8]&0x8) >> 3;
                mm->vert_rate = ((msg[8]&7) << 6) | ((msg[9]&0xfc) >> 2);
                /* Compute velocity and angle from the two speed
                 * components. */
                mm->velocity = sqrt(mm->ns_velocity*mm->ns_velocity+
                                    mm->ew_velocity*mm->ew_velocity);
                if (mm->velocity)
                {
                    int ewv = mm->ew_velocity;
                    int nsv = mm->ns_velocity;
                    double heading;

                    if (mm->ew_dir) ewv *= -1;
                    if (mm->ns_dir) nsv *= -1;
                    heading = atan2(ewv,nsv);

                    /* Convert to degrees. */
                    mm->heading = heading * 360 / (M_PI*2);
                    /* We don't want negative values but a 0-360 scale. */
                    if (mm->heading < 0) mm->heading += 360;
                }
                else
                {
                    mm->heading = 0;
                }
            }
            else if (mm->mesub == 3 || mm->mesub == 4)
            {
                mm->heading_is_valid = msg[5] & (1<<2);
                mm->heading = (360.0/128) * (((msg[5] & 3) << 5) |
                                               (msg[6] >> 3));
            }
        }
    }
    mm->phase_corrected = 0; /* Set to 1 by the caller if needed. */
}


/* When a new message is available, because it was decoded from the
 * RTL device, file, or received in the TCP input port, or any other
 * way we can receive a decoded message, we call this function in order
 * to use the message.
 *
 * Basically this function passes a raw message to the upper layers for
 * further processing and visualization. */
void Demodulator::useModesMessage(struct modesMessage *mm)
{
    if (check_crc == 0 || mm->crcok)
    {
        interactiveReceiveData(mm);
        displayModesMessage(mm);
    }
}

uint32_t Demodulator::modesChecksum(unsigned char *msg, int bits)
{
    uint32_t crc = 0;
    int offset = (bits == 112) ? 0 : (112-56);
    int j;

    for(j = 0; j < bits; j++)
    {
        int byte = j/8;
        int bit = j%8;
        int bitmask = 1 << (7-bit);

        /* If bit is set, xor with corresponding table entry. */
        if (msg[byte] & bitmask)
            crc ^= modes_checksum_table[j+offset];
    }
    return crc; /* 24 bit checksum. */
}

/* If the message type has the checksum xored with the ICAO address, try to
 * brute force it using a list of recently seen ICAO addresses.
 *
 * Do this in a brute-force fashion by xoring the predicted CRC with
 * the address XOR checksum field in the message. This will recover the
 * address: if we found it in our cache, we can assume the message is ok.
 *
 * This function expects mm->msgtype and mm->msgbits to be correctly
 * populated by the caller.
 *
 * On success the correct ICAO address is stored in the modesMessage
 * structure in the aa3, aa2, and aa1 fiedls.
 *
 * If the function successfully recovers a message with a correct checksum
 * it returns 1. Otherwise 0 is returned. */
int Demodulator::bruteForceAP(unsigned char *msg, struct modesMessage *mm)
{
    unsigned char aux[MODES_LONG_MSG_BYTES];
    int msgtype = mm->msgtype;
    int msgbits = mm->msgbits;

    if (msgtype == 0 ||         /* Short air surveillance */
        msgtype == 4 ||         /* Surveillance, altitude reply */
        msgtype == 5 ||         /* Surveillance, identity reply */
        msgtype == 16 ||        /* Long Air-Air survillance */
        msgtype == 20 ||        /* Comm-A, altitude request */
        msgtype == 21 ||        /* Comm-A, identity request */
        msgtype == 24)          /* Comm-C ELM */
    {
        uint32_t addr;
        uint32_t crc;
        int lastbyte = (msgbits/8)-1;

        /* Work on a copy. */
        memcpy(aux,msg,msgbits/8);

        /* Compute the CRC of the message and XOR it with the AP field
         * so that we recover the address, because:
         *
         * (ADDR xor CRC) xor CRC = ADDR. */
        crc = modesChecksum(aux,msgbits);
        aux[lastbyte] ^= crc & 0xff;
        aux[lastbyte-1] ^= (crc >> 8) & 0xff;
        aux[lastbyte-2] ^= (crc >> 16) & 0xff;

        /* If the obtained address exists in our cache we consider
         * the message valid. */
        addr = aux[lastbyte] | (aux[lastbyte-1] << 8) | (aux[lastbyte-2] << 16);

        if (ICAOAddressWasRecentlySeen(addr))
        {
            mm->aa1 = aux[lastbyte-2];
            mm->aa2 = aux[lastbyte-1];
            mm->aa3 = aux[lastbyte];
            return 1;
        }
    }
    return 0;
}

/* Add the specified entry to the cache of recently seen ICAO addresses.
 * Note that we also add a timestamp so that we can make sure that the
 * entry is only valid for MODES_ICAO_CACHE_TTL seconds. */
void Demodulator::addRecentlySeenICAOAddr(uint32_t addr)
{
    uint32_t h = ICAOCacheHashAddress(addr);
    icao_cache[h*2] = addr;
    icao_cache[h*2+1] = (uint32_t) time(NULL);
}

/* Hash the ICAO address to index our cache of MODES_ICAO_CACHE_LEN
 * elements, that is assumed to be a power of two. */
uint32_t Demodulator::ICAOCacheHashAddress(uint32_t a)
{
    /* The following three rounds wil make sure that every bit affects
     * every output bit with ~ 50% of probability. */
    a = ((a >> 16) ^ a) * 0x45d9f3b;
    a = ((a >> 16) ^ a) * 0x45d9f3b;
    a = ((a >> 16) ^ a);
    return a & (MODES_ICAO_CACHE_LEN-1);
}

/* Decode the 13 bit AC altitude field (in DF 20 and others).
 * Returns the altitude, and set 'unit' to either MODES_UNIT_METERS
 * or MDOES_UNIT_FEETS. */
int Demodulator::decodeAC13Field(unsigned char *msg, int *unit)
{
    int m_bit = msg[3] & (1<<6);
    int q_bit = msg[3] & (1<<4);

    if (!m_bit)
    {
        *unit = MODES_UNIT_FEET;
        if (q_bit)
        {
            /* N is the 11 bit integer resulting from the removal of bit
             * Q and M */
            int n = ((msg[2] & 31) << 6) |
                ((msg[3] & 0x80) >> 2) |
                ((msg[3] & 0x20) >> 1) |
                (msg[3] & 15);
            /* The final altitude is due to the resulting number multiplied
             * by 25, minus 1000. */
            return n * 25 - 1000;
        } else {
            /* TODO: Implement altitude where Q=0 and M=0 */
        }
    } else {
        *unit = MODES_UNIT_METERS;
        /* TODO: Implement altitude when meter unit is selected. */
    }
    return 0;
}

/* Decode the 12 bit AC altitude field (in DF 17 and others).
 * Returns the altitude or 0 if it can't be decoded. */
int Demodulator::decodeAC12Field(unsigned char *msg, int *unit)
{
    int q_bit = msg[5] & 1;

    if (q_bit)
    {
        /* N is the 11 bit integer resulting from the removal of bit Q */
        *unit = MODES_UNIT_FEET;
        int n = ((msg[5] >> 1) << 4) | ((msg[6] & 0xF0) >> 4);
        /* The final altitude is due to the resulting number multiplied
         * by 25, minus 1000. */
        return n*25-1000;
    }
    else
        return 0;

}

/* Receive new messages and populate the interactive mode with more info. */
void Demodulator::interactiveReceiveData(struct modesMessage *mm)
{
    uint32_t addr;

    if (check_crc && mm->crcok == 0)
        return;

    addr = (mm->aa1 << 16) | (mm->aa2 << 8) | mm->aa3;
    if(addr == 0)
        return;
    /* Loookup our aircraft or create a new one. */
    QSharedPointer<Aircraft> a;

    if (!_hashAircrafts.contains(addr))
    {
        a = QSharedPointer<Aircraft>(new Aircraft(addr));
        _hashAircrafts.insert(addr,a);
        addDebugMsg(QString("Add new aircraft with ICAO : %1\nCount aircraft = %2")
                        .arg(addr,16,16)
                        .arg(_hashAircrafts.count()));
    }
    else
    {
        a = _hashAircrafts.value(addr);
        addDebugMsg(QString("Update info aircraft with ICAO : %1\nCount aircraft = %2")
                        .arg(addr,16,16)
                        .arg(_hashAircrafts.count()));
    }

    a->setSeenTime(QDateTime::currentMSecsSinceEpoch());
    a->incNumberMsg();

    if (mm->msgtype == 0 || mm->msgtype == 4 || mm->msgtype == 20)
        a->setAltitude( mm->altitude / CONVERT_FT_TO_METERS);
    if(mm->msgtype == 17)
    {
        if (mm->metype >= 1 && mm->metype <= 4)
            a->setFlightInfo(mm->flight);
        else if (mm->metype >= 9 && mm->metype <= 18)
        {
            a->setAltitude( mm->altitude / CONVERT_FT_TO_METERS);

            if (mm->fflag)
            {
                a->setOddCprLat( mm->raw_latitude);
                a->setOddCprLon(mm->raw_longitude);
                a->setOddCprTime(QDateTime::currentMSecsSinceEpoch());
            }
            else
            {
                a->setEvenCprLat(mm->raw_latitude);
                a->setEvenCprLon(mm->raw_longitude);
                a->setEvenCprTime(QDateTime::currentMSecsSinceEpoch());
            }
            /* If the two data is less than 10 seconds apart, compute
             * the position. */
            if (abs(a->getEvenCprTime() - a->getOddCprTime()) <= 10000)
                decodeCPR(a);
        }
        else if (mm->metype == 19)
        {
            if (mm->mesub == 1 || mm->mesub == 2)
            {
                a->setSpeed( mm->velocity * CONVERT_KN_TO_KM_P_H);
                a->setCourse(mm->heading);
            }
        }
    }
}

/* This algorithm comes from:
 * http://www.lll.lu/~edward/edward/adsb/DecodingADSBposition.html.
 *
 *
 * A few remarks:
 * 1) 131072 is 2^17 since CPR latitude and longitude are encoded in 17 bits.
 * 2) We assume that we always received the odd packet as last packet for
 *    simplicity. This may provide a position that is less fresh of a few
 *    seconds.
 */
void Demodulator::decodeCPR(QSharedPointer<Aircraft> a)
{
    const double AirDlat0 = 360.0 / 60.0;
    const double AirDlat1 = 360.0 / 59.0;
    double lat0 = a->getEvenCprLat();
    double lat1 = a->getOddCprLat();
    double lon0 = a->getEvenCprLon();
    double lon1 = a->getOddCprLon();

    /* Compute the Latitude Index "j" */
    int j = int(floor(((59.0 * lat0 - 60.0 *lat1) / 131072.0) + 0.5));
    double rlat0 = AirDlat0 * (cprModFunction(j,60.0) + lat0 / 131072.0);
    double rlat1 = AirDlat1 * (cprModFunction(j,59.0) + lat1 / 131072.0);

    if (rlat0 >= 270.0)
        rlat0 -= 360.0;
    if (rlat1 >= 270.0)
        rlat1 -= 360.0;

    /* Check that both are in the same latitude zone, or abort. */
    if (cprNLFunction(rlat0) != cprNLFunction(rlat1))
        return;

    /* Compute ni and the longitude index m */
    if (a->getEvenCprTime() > a->getOddCprTime())
    {
        /* Use even packet. */
        int ni = cprNFunction(rlat0,0);
        int m = int(floor((((lon0 * (cprNLFunction(rlat0)-1.0)) -
                            (lon1 * cprNLFunction(rlat0))) / 131072.0) + 0.5));
        a->setLongitude(cprDlonFunction(rlat0,0) * (cprModFunction(m,ni)+lon0/131072));
        a->setLatitude(rlat0);
    }
    else
    {
        /* Use odd packet. */
        int ni = cprNFunction(rlat1,1);
        int m = int(floor((((lon0 * (cprNLFunction(rlat1) - 1.0)) -
                            (lon1 * cprNLFunction(rlat1))) / 131072.0) + 0.5));
        a->setLongitude(cprDlonFunction(rlat1,1) * (cprModFunction(m,ni)+lon1/131072.0));
        a->setLatitude(rlat1);
    }
    if (a->getLongitude() > 180.0)
        a->setLongitude(a->getLongitude() - 360.0);
}

int Demodulator::cprModFunction(int a, int b)
{
    int res = a % b;
    if (res < 0)
        res += b;
    return res;
}

/* The NL function uses the precomputed table from 1090-WP-9-14 */
int Demodulator::cprNLFunction(double lat)
{
    if (lat < 0) lat = -lat; /* Table is simmetric about the equator. */
    if (lat < 10.47047130) return 59;
    if (lat < 14.82817437) return 58;
    if (lat < 18.18626357) return 57;
    if (lat < 21.02939493) return 56;
    if (lat < 23.54504487) return 55;
    if (lat < 25.82924707) return 54;
    if (lat < 27.93898710) return 53;
    if (lat < 29.91135686) return 52;
    if (lat < 31.77209708) return 51;
    if (lat < 33.53993436) return 50;
    if (lat < 35.22899598) return 49;
    if (lat < 36.85025108) return 48;
    if (lat < 38.41241892) return 47;
    if (lat < 39.92256684) return 46;
    if (lat < 41.38651832) return 45;
    if (lat < 42.80914012) return 44;
    if (lat < 44.19454951) return 43;
    if (lat < 45.54626723) return 42;
    if (lat < 46.86733252) return 41;
    if (lat < 48.16039128) return 40;
    if (lat < 49.42776439) return 39;
    if (lat < 50.67150166) return 38;
    if (lat < 51.89342469) return 37;
    if (lat < 53.09516153) return 36;
    if (lat < 54.27817472) return 35;
    if (lat < 55.44378444) return 34;
    if (lat < 56.59318756) return 33;
    if (lat < 57.72747354) return 32;
    if (lat < 58.84763776) return 31;
    if (lat < 59.95459277) return 30;
    if (lat < 61.04917774) return 29;
    if (lat < 62.13216659) return 28;
    if (lat < 63.20427479) return 27;
    if (lat < 64.26616523) return 26;
    if (lat < 65.31845310) return 25;
    if (lat < 66.36171008) return 24;
    if (lat < 67.39646774) return 23;
    if (lat < 68.42322022) return 22;
    if (lat < 69.44242631) return 21;
    if (lat < 70.45451075) return 20;
    if (lat < 71.45986473) return 19;
    if (lat < 72.45884545) return 18;
    if (lat < 73.45177442) return 17;
    if (lat < 74.43893416) return 16;
    if (lat < 75.42056257) return 15;
    if (lat < 76.39684391) return 14;
    if (lat < 77.36789461) return 13;
    if (lat < 78.33374083) return 12;
    if (lat < 79.29428225) return 11;
    if (lat < 80.24923213) return 10;
    if (lat < 81.19801349) return 9;
    if (lat < 82.13956981) return 8;
    if (lat < 83.07199445) return 7;
    if (lat < 83.99173563) return 6;
    if (lat < 84.89166191) return 5;
    if (lat < 85.75541621) return 4;
    if (lat < 86.53536998) return 3;
    if (lat < 87.00000000) return 2;
    else return 1;
}

int Demodulator::cprNFunction(double lat, int isodd)
{
    int nl = cprNLFunction(lat) - isodd;
    if (nl < 1)
        nl = 1;
    return nl;
}

double Demodulator::cprDlonFunction(double lat, int isodd)
{
    return 360.0 / cprNFunction(lat, isodd);
}


/* ============================== Debugging ================================= */

/* Helper function for dumpMagnitudeVector().
 * It prints a single bar used to display raw signals.
 *
 * Since every magnitude sample is between 0-255, the function uses
 * up to 63 characters for every bar. Every character represents
 * a length of 4, 3, 2, 1, specifically:
 *
 * "O" is 4
 * "o" is 3
 * "-" is 2
 * "." is 1
 */
void Demodulator::dumpMagnitudeBar(int index, int magnitude)
{
    const char *set = " .-o";
    char buf[256];
    int div = magnitude / 256 / 4;
    int rem = magnitude / 256 % 4;

    memset(buf,'O',div);
    buf[div] = set[rem];
    buf[div+1] = '\0';

    if (index >= 0)
        qDebug("[%.3d] |%-66s %d\n", index, buf, magnitude);
    else
        qDebug("[%.2d] |%-66s %d\n", index, buf, magnitude);
}

/* Display an ASCII-art alike graphical representation of the undecoded
 * message as a magnitude signal.
 *
 * The message starts at the specified offset in the "m" buffer.
 * The function will display enough data to cover a short 56 bit message.
 *
 * If possible a few samples before the start of the messsage are included
 * for context. */

void Demodulator::dumpMagnitudeVector(uint16_t *m, uint32_t offset)
{
    uint32_t padding = 5; /* Show a few samples before the actual start. */
    uint32_t start = (offset < padding) ? 0 : offset-padding;
    uint32_t end = offset + (MODES_PREAMBLE_US*2)+(MODES_SHORT_MSG_BITS*2) - 1;
    uint32_t j;

    for (j = start; j <= end; j++)
        dumpMagnitudeBar(j-offset, m[j]);

}

/* This is a wrapper for dumpMagnitudeVector() that also show the message
 * in hex format with an additional description.
 *
 * descr  is the additional message to show to describe the dump.
 * msg    points to the decoded message
 * m      is the original magnitude vector
 * offset is the offset where the message starts
 *
 * The function also produces the Javascript file used by debug.html to
 * display packets in a graphical format if the Javascript output was
 * enabled.
 */
void Demodulator::dumpRawMessage(const QString &descr,
                                 unsigned char *msg,
                                 uint16_t *m,
                                 uint32_t offset)
{
    int j;
    int msgtype = msg[0]>>3;
    int fixable = -1;

    if (msgtype == 11 || msgtype == 17)
    {
        int msgbits = (msgtype == 11) ? MODES_SHORT_MSG_BITS :
                                      MODES_LONG_MSG_BITS;
        fixable = fixSingleBitErrors(msg,msgbits);
        if (fixable == -1)
            fixable = fixTwoBitsErrors(msg,msgbits);
    }

    qDebug("\n--- %s\n    ", descr.toStdString().c_str());

    for (j = 0; j < MODES_LONG_MSG_BYTES; j++) {
        qDebug("%02x",msg[j]);
        if (j == MODES_SHORT_MSG_BYTES-1) qDebug(" ... ");
    }

    qDebug(" (DF %d, Fixable: %d)\n", msgtype, fixable);

    dumpMagnitudeVector(m,offset);

    qDebug("---\n\n");
}

/* This function gets a decoded Mode S Message and prints it on the screen
 * in a human readable format. */
void Demodulator::displayModesMessage(struct modesMessage *mm)
{
    /* Handle only addresses mode first. */
    if (onlyaddr)
    {
        qDebug("%02x%02x%02x", mm->aa1, mm->aa2, mm->aa3);
        return;
    }

    QString strLog = QString("ICAO Address   : %1%2%3\n")
                         .arg(mm->aa1,2,16)
                         .arg(mm->aa2,2,16)
                         .arg(mm->aa3,2,16);

    strLog.append(QString("CRC: %1 (%2)\n").arg(mm->crc,6,16).arg(mm->crcok ? "ok" : "wrong"));


    if (mm->errorbit != -1)
        strLog.append(QString("Single bit error fixed, bit %1\n").arg(mm->errorbit));

    if (mm->msgtype == 0)
    {
        /* DF 0 */
        strLog.append("DF 0: Short Air-Air Surveillance.\n");
        strLog.append(QString("  Altitude       : %1 %2\n")
                          .arg(mm->altitude)
                          .arg((mm->unit == MODES_UNIT_METERS) ? "meters" : "feet"));
        strLog.append(QString("ICAO Address   : %1%2%3\n")
                          .arg(mm->aa1,2,16)
                          .arg(mm->aa2,2,16)
                          .arg(mm->aa3,2,16));
    }

    else if (mm->msgtype == 4 || mm->msgtype == 20)
    {
        strLog.append(QString("DF %1: %2, Altitude Reply.\n")
                          .arg(mm->msgtype)
                          .arg((mm->msgtype == 4) ? "Surveillance" : "Comm-B"));

        strLog.append(QString("  Flight Status  : %1\n").arg(fs_str[mm->fs]));
        strLog.append(QString("  DR             : %1\n").arg(mm->dr));
        strLog.append(QString("  UM             : %1\n").arg(mm->um));
        strLog.append(QString("  Altitude       : %1 %2\n")
                          .arg( mm->altitude)
                          .arg((mm->unit == MODES_UNIT_METERS) ? "meters" : "feet"));
        strLog.append(QString("  ICAO Address   : %1%2%3\n")
                          .arg(mm->aa1,2,16)
                          .arg(mm->aa2,2,16)
                          .arg(mm->aa3,2,16));

        if (mm->msgtype == 20)
        {
            /* TODO: 56 bits DF20 MB additional field. */
        }
    }
    else if (mm->msgtype == 5 || mm->msgtype == 21)
    {
        strLog.append(QString("DF %1: %2, Identity Reply.\n")
                          .arg(mm->msgtype)
                          .arg((mm->msgtype == 5) ? "Surveillance" : "Comm-B"));

        strLog.append(QString("  Flight Status  : %1\n").arg(fs_str[mm->fs]));
        strLog.append(QString("  DR             : %1\n").arg(mm->dr));
        strLog.append(QString("  UM             : %1\n").arg(mm->um));
        strLog.append(QString("  Squawk         : %1\n").arg(mm->identity));
        strLog.append(QString("  ICAO Address   : %1%2%3\n")
                          .arg(mm->aa1,2,16)
                          .arg(mm->aa2,2,16)
                          .arg(mm->aa3,2,16));

        if (mm->msgtype == 21)
        {
            /* TODO: 56 bits DF21 MB additional field. */
        }
    }
    else if (mm->msgtype == 11)
    {
        /* DF 11 */
        strLog.append(QString("DF 11: All Call Reply.\n"));
        strLog.append(QString("  Capability  : %1\n").arg(ca_str[mm->ca]));
        strLog.append(QString("  ICAO Address   : %1%2%3\n")
                          .arg(mm->aa1,2,16)
                          .arg(mm->aa2,2,16)
                          .arg(mm->aa3,2,16));
    }
    else if (mm->msgtype == 17)
    {
        /* DF 17 */
        strLog.append(QString("DF 17: ADS-B message.\n"));
        strLog.append(QString("  Capability     : %1 (%2)\n").arg(mm->ca).arg(ca_str[mm->ca]));
        strLog.append(QString("  ICAO Address   : %1%2%3\n")
                          .arg(mm->aa1,2,16)
                          .arg(mm->aa2,2,16)
                          .arg(mm->aa3,2,16));
        strLog.append(QString("  Extended Squitter  Type: %1\n").arg(mm->metype));
        strLog.append(QString("  Extended Squitter  Sub : %1\n").arg(mm->mesub));
        strLog.append(QString("  Extended Squitter  Name: %1\n")
                          .arg(getMEDescription(mm->metype,mm->mesub)));

        /* Decode the extended squitter message. */
        if (mm->metype >= 1 && mm->metype <= 4)
        {
            /* Aircraft identification. */
            const char *ac_type_str[4] =
                {
                    "Aircraft Type D",
                    "Aircraft Type C",
                    "Aircraft Type B",
                    "Aircraft Type A"
                };

            strLog.append(QString("    Aircraft Type  : %1\n").arg(ac_type_str[mm->aircraft_type]));
            strLog.append(QString("    Identification : %1\n").arg(mm->flight));
        }
        else if (mm->metype >= 9 && mm->metype <= 18)
        {
            strLog.append(QString("    F flag   : %1\n").arg(mm->fflag ? "odd" : "even"));
            strLog.append(QString("    T flag   : %1\n").arg(mm->tflag ? "UTC" : "non-UTC"));
            strLog.append(QString("    Altitude : %1 feet\n").arg(mm->altitude));
            strLog.append(QString("    Latitude : %1 (not decoded)\n").arg(mm->raw_latitude));
            strLog.append(QString("    Longitude: %1 (not decoded)\n").arg(mm->raw_longitude));
        }
        else if (mm->metype == 19 && mm->mesub >= 1 && mm->mesub <= 4)
        {
            if (mm->mesub == 1 || mm->mesub == 2)
            {
                /* Velocity */
                strLog.append(QString("    EW direction      : %1\n").arg(mm->ew_dir));
                strLog.append(QString("    EW velocity       : %1\n").arg(mm->ew_velocity));
                strLog.append(QString("    NS direction      : %1\n").arg(mm->ns_dir));
                strLog.append(QString("    NS velocity       : %1\n").arg(mm->ns_velocity));
                strLog.append(QString("    Vertical rate src : %1\n").arg(mm->vert_rate_source));
                strLog.append(QString("    Vertical rate sign: %1\n").arg(mm->vert_rate_sign));
                strLog.append(QString("    Vertical rate     : %1\n").arg(mm->vert_rate));
            }
            else if (mm->mesub == 3 || mm->mesub == 4)
            {
                strLog.append(QString("    Heading status: %1\n").arg(mm->heading_is_valid));
                strLog.append(QString("    Heading: %1\n").arg(mm->heading));
            }
        }
        else
        {
            strLog.append(QString("    Unrecognized ME type: %1 subtype: %2\n")
                              .arg(mm->metype)
                              .arg(mm->mesub));
        }
    }
    else
    {
        if (check_crc)
            strLog.append(QString("DF %1 with good CRC received "
                                  "(decoding still not implemented).\n").arg(mm->msgtype));
    }
    strLog.append("=========================================================\n");
    addDebugMsg(strLog);
}

/* Returns 1 if the specified ICAO address was seen in a DF format with
 * proper checksum (not xored with address) no more than * MODES_ICAO_CACHE_TTL
 * seconds ago. Otherwise returns 0. */
int Demodulator::ICAOAddressWasRecentlySeen(uint32_t addr)
{
    uint32_t h = ICAOCacheHashAddress(addr);
    uint32_t a = icao_cache[h*2];
    uint32_t t = icao_cache[h*2+1];

    return a && a == addr && time(NULL) - t <= MODES_ICAO_CACHE_TTL;
}

QString Demodulator::getMEDescription(int metype, int mesub)
{
    QString mename("Unknown");

    if (metype >= 1 && metype <= 4)
        mename = "Aircraft Identification and Category";
    else if (metype >= 5 && metype <= 8)
        mename = "Surface Position";
    else if (metype >= 9 && metype <= 18)
        mename = "Airborne Position (Baro Altitude)";
    else if (metype == 19 && mesub >=1 && mesub <= 4)
        mename = "Airborne Velocity";
    else if (metype >= 20 && metype <= 22)
        mename = "Airborne Position (GNSS Height)";
    else if (metype == 23 && mesub == 0)
        mename = "Test Message";
    else if (metype == 24 && mesub == 1)
        mename = "Surface System Status";
    else if (metype == 28 && mesub == 1)
        mename = "Extended Squitter Aircraft Status (Emergency)";
    else if (metype == 28 && mesub == 2)
        mename = "Extended Squitter Aircraft Status (1090ES TCAS RA)";
    else if (metype == 29 && (mesub == 0 || mesub == 1))
        mename = "Target State and Status Message";
    else if (metype == 31 && (mesub == 0 || mesub == 1))
        mename = "Aircraft Operational Status Message";
    return mename;
}


/* When in interactive mode If we don't receive new nessages within
 * MODES_INTERACTIVE_TTL seconds we remove the aircraft from the list. */
void Demodulator::interactiveRemoveStaleAircrafts()
{
    int64_t now = QDateTime::currentMSecsSinceEpoch();

    for(auto &a: _hashAircrafts.values())
    {
        if ((now - a->getSeenTime()) > MODES_INTERACTIVE_TTL)
        {
            addDebugMsg(QString("Remove aircraft with ICAO : %1 last update : %2\nCount aircraft = %3\n")
                            .arg(a->getICAO(),16,16)
                            .arg(QDateTime::fromMSecsSinceEpoch(a->getSeenTime()).toString("hh:mm:ss.zzz"))
                            .arg(_hashAircrafts.count()));

            addDebugMsg(QString("Remove aircraft  : \n%1")
                            .arg(a->toString()));

            _hashAircrafts.remove(a->getICAO());
            a.clear();

        }
    }
}


void Demodulator::addDebugMsg(const QString &str)
{
    if(_log)
        _log->push(str);
}
