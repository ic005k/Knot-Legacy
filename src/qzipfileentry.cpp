/*
 * Copyright (c) 2011 Oleksandr Tymoshenko <gonzo@bluezbox.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "qzipfileentry.h"

namespace qompress {

QZipFileEntry::QZipFileEntry() :
    m_valid(false)
{
}

QZipFileEntry::QZipFileEntry(QString name, const unz_file_info64 &info) :
    m_name(name), m_info(info), m_valid(true)
{

}

QZipFileEntry::~QZipFileEntry()
{
}


QDateTime QZipFileEntry::modificationTime()
{
    QDateTime mtime;
    if (!isValid())
        return mtime;
    quint32 lDate, lTime;
    ZPOS64_T uDate;
    lDate = m_info.dosDate >> 16;
    lTime = m_info.dosDate & 0xffff;;

    mtime.setTime(QTime(lTime >> 11, (lTime >> 5) & 0x3f,
        lTime & 0x1f));
    mtime.setDate(QDate((lDate >> 9) + 1980, (lTime >> 5) & 0xf,
        lTime & 0x1f));

    return mtime;
}

} // namespace
