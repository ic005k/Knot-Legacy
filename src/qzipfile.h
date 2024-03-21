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

#ifndef QZIPFILE_H
#define QZIPFILE_H

#include "unzip.h"
#include "zip.h"
#include <QIODevice>
#include <QStringList>

#include "qzipfileentry.h"

namespace qompress {

/**
 * \brief Interface to ZIP file. Instance of this class provides interface to 
 * archive entries. 
 *
 * QZipFile represents archive as a list of entries and maintain
 * "pointer" to one of them, this entry is called current. It's possible to 
 * navigate through list using gotoFirstEntry() and nextEntry() methods
 */
class QZipFile : public QObject
{
public:
    enum OpenMode {
        NotOpened,          /// archive is not opened
        ReadOnly,           /// open archive for read
        WriteOnlyAppend,    /// open archive for appending files
        WriteOnlyTruncate,  /// recreate archive
    };

    /// constructor
    QZipFile(const QString &name, QObject *parent = 0);
    ~QZipFile();

    /// open archive in specified \a mode
    bool open(OpenMode mode);
    /// close archive
    void close();

    /// get human-readable description of last error
    QString errorString() const { return m_error; };
    /// returns mode archive has been opened in
    OpenMode openMode() { return m_mode; };
    
    // **
    // Unarchive API
    // **

    /// returns info for current entry
    QZipFileEntry currentEntry();
    /// set pointer to first archive entry, returns true if succeded, false otherwise
    bool gotoFirstEntry();
    /// move pointer to the next archive entry. True if succeded, false if pointer is at the last entry
    bool nextEntry();
    /// Exctract current entry and write it to \a out
    bool extractCurrentEntry(QIODevice &out, const QString &password = "");
    /// Exctract entry named \a file and write it to \a out
    bool extractEntry(QIODevice &out, const QString &file, const QString &password = "");
    /// returns list with entry names
    QStringList filenames();

    // **
    // Archive API
    // **
    bool addEntry(QIODevice &in, const QString &file, const QString &password = "");
    void setCompressionLevel(int level) { m_compressionLevel = level; };
    int compressionLevel() const { return m_compressionLevel; }
    

private:
    void setErrorString(const QString &s) { m_error = s; };

    QString m_fileName;
    QString m_error;
    OpenMode m_mode;
    int m_compressionLevel;

    unzFile m_unzFile;
    zipFile m_zipFile;
};

} // namespace

#endif // QZIPFILE_H
