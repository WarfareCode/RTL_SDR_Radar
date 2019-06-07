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

#ifndef DEMODULATOR_H
#define DEMODULATOR_H

#include <QSharedPointer>
#include <QRunnable>

#include "demodulator_global.h"
#include "IDemodulator.h"
#include "ILogger.h"

#include "../../../../import/sdr_dev/include/constant.h"

/* The struct we use to store information about a decoded message. */
struct modesMessage
{
    /* Generic fields */
    unsigned char msg[MODES_LONG_MSG_BYTES]; /* Binary message. */
    int msgbits;                /* Number of bits in message */
    int msgtype;                /* Downlink format # */
    int crcok;                  /* True if CRC was valid */
    uint32_t crc;               /* Message CRC */
    int errorbit;               /* Bit corrected. -1 if no bit corrected. */
    int aa1, aa2, aa3;          /* ICAO Address bytes 1 2 and 3 */
    int phase_corrected;        /* True if phase correction was applied. */

    /* DF 11 */
    int ca;                     /* Responder capabilities. */

    /* DF 17 */
    int metype;                 /* Extended squitter message type. */
    int mesub;                  /* Extended squitter message subtype. */
    int heading_is_valid;
    int heading;
    int aircraft_type;
    int fflag;                  /* 1 = Odd, 0 = Even CPR message. */
    int tflag;                  /* UTC synchronized? */
    int raw_latitude;           /* Non decoded latitude */
    int raw_longitude;          /* Non decoded longitude */
    char flight[9];             /* 8 chars flight number. */
    int ew_dir;                 /* 0 = East, 1 = West. */
    int ew_velocity;            /* E/W velocity. */
    int ns_dir;                 /* 0 = North, 1 = South. */
    int ns_velocity;            /* N/S velocity. */
    int vert_rate_source;       /* Vertical rate source. */
    int vert_rate_sign;         /* Vertical rate sign. */
    int vert_rate;              /* Vertical rate. */
    int velocity;               /* Computed from EW and NS velocity. */

    /* DF4, DF5, DF20, DF21 */
    int fs;                     /* Flight status for DF4,5,20,21 */
    int dr;                     /* Request extraction of downlink request. */
    int um;                     /* Request extraction of downlink request. */
    int identity;               /* 13 bits identity (Squawk). */

    /* Fields used by multiple message types. */
    int altitude, unit;
};

class Aircraft;

class DEMODULATORSHARED_EXPORT Demodulator : public IDemodulator
{
    QVector<uint16_t> _magnitude;
    QVector<uint32_t> icao_cache;

    QHash<uint32_t,QSharedPointer<Aircraft>> _hashAircrafts;
    QSharedPointer<IPoolObject> _pool = nullptr;
    QSharedPointer<ILogger> _log = nullptr;

    /* Statistics */
    uint64_t stat_valid_preamble = 0;
    uint64_t stat_demodulated = 0;
    uint64_t stat_goodcrc = 0;
    uint64_t stat_badcrc = 0;
    uint64_t stat_fixed = 0;
    uint64_t stat_single_bit_fix = 0;
    uint64_t stat_two_bits_fix = 0;
    uint64_t stat_out_of_phase = 0;

    /* Configuration */
    bool fix_errors = true;                 /* Single bit error correction if true. */
    bool check_crc = true;                  /* Only display messages with good CRC. */
    bool debug = false;                      /* Debugging mode. */
    int interactive_ttl = MODES_INTERACTIVE_TTL;            /* Interactive mode: TTL before deletion. */
    bool onlyaddr = false;                   /* Print only ICAO addresses. */
    bool metric = true;                     /* Use metric units. */
    bool aggressive = false;          /* Aggressive detection algorithm. */

    uint16_t* maglut;
public:
    Demodulator(QSharedPointer<IPoolObject> pool);
    ~Demodulator() override ;

    /*!
     *  \brief внедрение зависимости модуля логгирования
     */
    void setLogger(QSharedPointer<ILogger> log) override { _log = log;}

    bool setDataForDemodulate(const QVector<uint8_t>& vector) override;
    void run() override;
    QByteArray getRawDump() override;
    bool demodulate() override;

    int32_t getCountObject() override { return _hashAircrafts.count(); }

private:
    void computeMagnitudeVector(const QVector<uint8_t> &vector,
                                QVector<uint16_t> &magnitude);
    void detectModeS(uint16_t *m, uint32_t mlen);
    void dumpRawMessage(const QString &descr,
                        unsigned char *msg,
                        uint16_t *m,
                        uint32_t offset);
    int fixTwoBitsErrors(unsigned char *msg, int bits);
    int fixSingleBitErrors(unsigned char *msg, int bits);
    void dumpMagnitudeBar(int index, int magnitude);
    void dumpMagnitudeVector(uint16_t *m, uint32_t offset);
    int detectOutOfPhase(uint16_t *m);
    void applyPhaseCorrection(uint16_t *m);
    int modesMessageLenByType(int type);
    void decodeModesMessage(modesMessage *mm, unsigned char *msg);
    void useModesMessage(modesMessage *mm);
    uint32_t modesChecksum(unsigned char *msg, int bits);
    int bruteForceAP(unsigned char *msg, modesMessage *mm);
    void addRecentlySeenICAOAddr(uint32_t addr);
    uint32_t ICAOCacheHashAddress(uint32_t a);
    int decodeAC13Field(unsigned char *msg, int *unit);
    int decodeAC12Field(unsigned char *msg, int *unit);
    void interactiveReceiveData(modesMessage *mm);
    void displayModesMessage(modesMessage *mm);
    int ICAOAddressWasRecentlySeen(uint32_t addr);
    void decodeCPR(QSharedPointer<Aircraft> a);
    int cprModFunction(int a, int b);
    int cprNLFunction(double lat);
    int cprNFunction(double lat, int isodd);
    double cprDlonFunction(double lat, int isodd);
    QString getMEDescription(int metype, int mesub);
    void interactiveRemoveStaleAircrafts();
    void addDebugMsg(const QString &str);
};

#endif // DEMODULATOR_H
