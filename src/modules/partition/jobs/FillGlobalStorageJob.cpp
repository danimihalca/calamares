/* === This file is part of Calamares - <http://github.com/calamares> ===
 *
 *   Copyright 2014, Aurélien Gâteau <agateau@kde.org>
 *   Copyright 2015, Teo Mrnjavac <teo@kde.org>
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 */

#include <jobs/FillGlobalStorageJob.h>

#include <GlobalStorage.h>
#include <JobQueue.h>
#include <core/PartitionInfo.h>
#include <core/PartitionIterator.h>
#include <core/PMUtils.h>
#include "Branding.h"
#include "utils/Logger.h"

// CalaPM
#include <core/device.h>
#include <core/partition.h>
#include <fs/filesystem.h>

// Qt
#include <QDebug>
#include <QDir>
#include <QFileInfo>

typedef QHash<QString, QString> UuidForPartitionHash;

static const char* UUID_DIR = "/dev/disk/by-uuid";

static UuidForPartitionHash
findPartitionUuids( QList < Device* > devices )
{
    cDebug() << "Gathering UUIDs for partitions that exist now.";
    UuidForPartitionHash hash;
    foreach ( Device* device, devices )
    {
        for ( auto it = PartitionIterator::begin( device );
              it != PartitionIterator::end( device ); ++it )
        {
            Partition* p = *it;
            QString path = p->partitionPath();
            QString uuid = p->fileSystem().readUUID( p->partitionPath() );
            hash.insert( path, uuid );
        }
    }
    cDebug() << hash;
    return hash;
}

static QVariant
mapForPartition( Partition* partition, const QString& uuid )
{
    QVariantMap map;
    map[ "device" ] = partition->partitionPath();
    map[ "mountPoint" ] = PartitionInfo::mountPoint( partition );
    map[ "fs" ] = partition->fileSystem().name();
    map[ "uuid" ] = uuid;
    cDebug() << partition->partitionPath()
             << "mtpoint:" << PartitionInfo::mountPoint( partition )
             << "fs:" << partition->fileSystem().name()
             << uuid;
    return map;
}

FillGlobalStorageJob::FillGlobalStorageJob( QList< Device* > devices, const QString& bootLoaderPath )
    : m_devices( devices )
    , m_bootLoaderPath( bootLoaderPath )
{
}

QString
FillGlobalStorageJob::prettyName() const
{
    return tr( "Set partition information" );
}


QString
FillGlobalStorageJob::prettyDescription() const
{
    QStringList lines;

    foreach ( QVariant partitionItem, createPartitionList().toList() )
    {
        if ( partitionItem.type() == QVariant::Map )
        {
            QVariantMap partitionMap = partitionItem.toMap();
            QString path = partitionMap.value( "device" ).toString();
            QString mountPoint = partitionMap.value( "mountPoint" ).toString();
            QString fsType = partitionMap.value( "fs" ).toString();
            qDebug() << partitionMap.value( "uuid" ) << path << mountPoint << fsType;
            if ( mountPoint.isEmpty() || fsType.isEmpty() )
                continue;
            if ( path.isEmpty() )
            {
                if ( mountPoint == "/" )
                    lines.append( tr( "Install %1 on <strong>new</strong> %2 system partition." )
                                  .arg( Calamares::Branding::instance()->string(
                                        Calamares::Branding::ShortProductName ) )
                                  .arg( fsType ) );
                else
                    lines.append( tr( "Set up <strong>new</strong> %2 partition with mount point "
                                      "<strong>%1</strong>." )
                                  .arg( mountPoint )
                                  .arg( fsType ) );
            }
            else
            {
                if ( mountPoint == "/" )
                    lines.append( tr( "Install %2 on %3 system partition <strong>%1</strong>." )
                                  .arg( path )
                                  .arg( Calamares::Branding::instance()->string(
                                        Calamares::Branding::ShortProductName ) )
                                  .arg( fsType ) );
                else
                    lines.append( tr( "Set up %3 partition <strong>%1</strong> with mount point "
                                      "<strong>%2</strong>." )
                                  .arg( path )
                                  .arg( mountPoint )
                                  .arg( fsType ) );
            }
        }
    }

    QVariant bootloaderMap = createBootLoaderMap();
    if ( !m_bootLoaderPath.isEmpty() )
    {
        lines.append( tr( "Install boot loader on <strong>%1</strong>." )
                      .arg( m_bootLoaderPath ) );
    }
    return lines.join( "<br/>" );
}

Calamares::JobResult
FillGlobalStorageJob::exec()
{
    Calamares::GlobalStorage* storage = Calamares::JobQueue::instance()->globalStorage();
    storage->insert( "partitions", createPartitionList() );
    QVariant var = createBootLoaderMap();
    if ( !var.isValid() )
        return Calamares::JobResult::error( tr( "Failed to find path for boot loader" ) );
    storage->insert( "bootLoader", var );
    return Calamares::JobResult::ok();
}

QVariant
FillGlobalStorageJob::createPartitionList() const
{
    UuidForPartitionHash hash = findPartitionUuids( m_devices );
    QVariantList lst;
    cDebug() << "Writing to GlobalStorage[\"partitions\"]";
    for ( auto device : m_devices )
    {
        for ( auto it = PartitionIterator::begin( device );
              it != PartitionIterator::end( device ); ++it )
        {
            lst << mapForPartition( *it, hash.value( ( *it )->partitionPath() ) );
        }
    }
    return lst;
}

QVariant
FillGlobalStorageJob::createBootLoaderMap() const
{
    QVariantMap map;
    QString path = m_bootLoaderPath;
    if ( !path.startsWith( "/dev/" ) )
    {
        Partition* partition = PMUtils::findPartitionByMountPoint( m_devices, path );
        if ( !partition )
            return QVariant();
        path = partition->partitionPath();
    }
    map[ "installPath" ] = path;
    return map;
}
