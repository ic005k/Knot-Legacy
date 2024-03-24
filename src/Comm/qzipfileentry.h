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

#ifndef QZIPFILEENTRY_H
#define QZIPFILEENTRY_H

#include "unzip.h"
#include <QIODevice>
#include <QDateTime>

namespace qompress {

class QZipFileEntry
{
    friend class QZipFile;

protected:
    QZipFileEntry(QString name, const unz_file_info64 &info);

public:
    QZipFileEntry();
    ~QZipFileEntry();

    /// returns true if entry is valid
    bool isValid() { return m_valid; }
    /// returns entry name
    QString name() const { return m_name; };
    /// returns compressed size in bytes
    qint64 compressedSize() { return m_info.compressed_size; };
    /// returns uncompressed size in bytes
    qint64 uncompressedSize() { return m_info.uncompressed_size; };
    /// reutrns true if entry is encrypted
    bool isEncrypted() { return (m_info.flag & 1) != 0; };
    /// returns modification date/time
    QDateTime modificationTime();

private:
    QString m_name;
    unz_file_info64 m_info;
    bool m_valid;
};

} // namespace

#endif // QZIPFILEENTRY_H
