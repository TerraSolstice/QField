/***************************************************************************
 fileutils.cpp

 ---------------------
 begin                : 29.02.2020
 copyright            : (C) 2020 by david
 email                : david at opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "fileutils.h"
#include <qgis.h>
#include <QMimeDatabase>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

FileUtils::FileUtils( QObject *parent )
  : QObject( parent )
{

}

QString FileUtils::mimeTypeName( const QString &filePath )
{
  QMimeDatabase db;
  QMimeType mimeType = db.mimeTypeForFile( filePath );
  return mimeType.name();
}

QString FileUtils::fileName( const QString &filePath )
{
  QFileInfo fileInfo( filePath );
  return fileInfo.fileName();
}

QString FileUtils::fileSuffix( const QString &filePath )
{
  QFileInfo fileInfo( filePath );
  return fileInfo.suffix();
}

bool FileUtils::fileExists( const QString &filePath )
{
  QFileInfo fileInfo( filePath );
  return ( fileInfo.exists() && fileInfo.isFile() );
}

bool FileUtils::copyRecursively( const QString &sourceFolder, const QString &destFolder, QgsFeedback *feedback )
{
  QList<QPair<QString, QString>> mapping;

  int fileCount = copyRecursivelyPrepare( sourceFolder, destFolder, mapping );

  int current = 0;
  for ( QPair<QString, QString> srcDestFilePair : qgis::as_const( mapping ) )
  {

    QString srcName = srcDestFilePair.first;
    QString destName = srcDestFilePair.second;

    QFileInfo destInfo( destName );
    QDir destDir( destInfo.absoluteDir() );

    if ( !destDir.exists() )
    {
      destDir.mkpath( destDir.path() );
    }
    if ( QFile::exists( destName ) )
      QFile::remove( destName );

    bool success = QFile::copy( srcName, destName );
    if ( !success )
      return false;

    feedback->setProgress( 100 * current / fileCount );

    ++current;
  }

  return true;
}

int FileUtils::copyRecursivelyPrepare( const QString &sourceFolder, const QString &destFolder, QList<QPair<QString, QString>> &mapping )
{
  QDir sourceDir( sourceFolder );

  if ( !sourceDir.exists() )
    return 0;

  int count = 0;

  const QStringList files = sourceDir.entryList( QDir::Files );
  for ( const QString &file : files )
  {
    QString srcName = sourceFolder + QDir::separator() + file;
    QString destName = destFolder + QDir::separator() + file;

    mapping.append( qMakePair( srcName, destName ) );
    count += 1;
  }

  const QStringList dirs = sourceDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
  for ( const QString &dir : dirs )
  {
    QString srcName = sourceFolder + QDir::separator() + dir;
    QString destName = destFolder + QDir::separator() + dir;
    count += copyRecursivelyPrepare( srcName, destName, mapping );

  }

  return count;
}
