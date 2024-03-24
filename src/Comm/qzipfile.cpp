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

#include "qzipfile.h"
namespace qompress {

QZipFile::QZipFile(const QString &name, QObject *parent)
    : QObject(parent), m_fileName(name), m_unzFile(NULL), m_zipFile(NULL) {}

QZipFile::~QZipFile() { close(); }

bool QZipFile::open(OpenMode mode) {
  if (m_unzFile || m_zipFile) {
    setErrorString("File is already open");
    return false;
  }

  if (mode == ReadOnly) {
    m_unzFile = unzOpen64(m_fileName.toUtf8());
    if (m_unzFile == NULL) {
      // no valid information at this point
      setErrorString("Error opening Zip file for reading");
      return false;
    }
  } else if ((mode == WriteOnlyTruncate) || (mode == WriteOnlyAppend)) {
    m_zipFile =
        zipOpen64(m_fileName.toUtf8(), (mode == WriteOnlyAppend) ? 2 : 0);
    if (m_zipFile == NULL) {
      // no valid information at this point
      setErrorString("Error opening Zip file for writing");
      return false;
    }
  } else
    return false;

  m_mode = mode;
  return true;
}

void QZipFile::close() {
  if (m_unzFile) {
    unzClose(m_unzFile);
    m_unzFile = 0;
  }

  if (m_zipFile) {
    zipClose(m_zipFile, NULL);
    m_zipFile = NULL;
  }
}

//
// Un-archive API
//

QZipFileEntry QZipFile::currentEntry() {
  int err;
  char filename_inzip[256];
  unz_file_info64 file_info;
  err = unzGetCurrentFileInfo64(m_unzFile, &file_info, filename_inzip,
                                sizeof(filename_inzip), NULL, 0, NULL, 0);
  if (err) {
    setErrorString("Failed to current get entry info");
    return QZipFileEntry();
  } else
    return QZipFileEntry(filename_inzip, file_info);
}

bool QZipFile::gotoFirstEntry() {
  return (unzGoToFirstFile(m_unzFile) == UNZ_OK);
}

bool QZipFile::nextEntry() {
  int err = unzGoToNextFile(m_unzFile);
  return (err == UNZ_OK);
}

bool QZipFile::extractCurrentEntry(QIODevice &out, const QString &password,
                                   const qint64 &bufSize) {
  int err;
  char buf[8192];
  qint64 totalBytes = bufSize;

  bool result = true;

  if (!password.isEmpty())
    err = unzOpenCurrentFilePassword(m_unzFile, password.toUtf8());
  else
    err = unzOpenCurrentFile(m_unzFile);
  if (err != UNZ_OK) {
    setErrorString("Error opening archive entry");
    return false;
  }

  do {
    err = unzReadCurrentFile(m_unzFile, buf, sizeof(buf));
    if (err < 0) {
      setErrorString("Error reading current archive entry");
      break;
    }
    if (err > 0) {
      qint64 written;

      do {
        written = out.write(buf, err);
        if (written < 0) break;
        err -= written;
        totalBytes = totalBytes - written;
      } while (err > 0);

      if (written < 0) err = -1;
    }

  } while (totalBytes > 0);

  if (err < 0) result = false;

  err = unzCloseCurrentFile(m_unzFile);

  if (err != UNZ_OK) result = false;

  return result;
}

bool QZipFile::extractEntry(QIODevice &out, const QString &file,
                            const QString &password, const qint64 &bufSize) {
  if (unzLocateFile(m_unzFile, file.toUtf8(), 1) != UNZ_OK) {
    setErrorString("File '" + file + "' not found in archive");
    return false;
  }

  return extractCurrentEntry(out, password, bufSize);
}

QStringList QZipFile::filenames() {
  QStringList result;
  gotoFirstEntry();
  do {
    QZipFileEntry info = currentEntry();
    if (info.isValid()) result << info.name();
  } while (nextEntry());

  return result;
}

//
// Archive API
//

bool QZipFile::addEntry(QIODevice &in, const QString &file,
                        const QString &password) {
  zip_fileinfo zi;
  unsigned long crcFile = 0;
  int isZip64 = 0;
  char buf[8192];
  qint64 readed;

  zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
      zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
  zi.dosDate = 0;
  zi.internal_fa = 0;
  zi.external_fa = 0;

  int err = zipOpenNewFileInZip3_64(
      m_zipFile, file.toUtf8(), &zi, NULL, 0, NULL, 0, NULL /* comment*/,
      (m_compressionLevel != 0) ? Z_DEFLATED : 0, m_compressionLevel, 0,
      -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, crcFile, isZip64);

  if (err != ZIP_OK) {
    setErrorString("FIXME");
    return false;
  }

  while ((readed = in.read(buf, sizeof(buf))) > 0) {
    err = zipWriteInFileInZip(m_zipFile, buf, readed);
    if (err < 0) {
      setErrorString("Error writing to zip file");
      return false;
    }
  }

  if (readed < 0) {
    setErrorString("Failed to read data");
    return false;
  }

  err = zipCloseFileInZip(m_zipFile);
  if (err < 0) {
    setErrorString("Error writing to zip file");
    return false;
  }

  return true;
}

}  // namespace qompress
