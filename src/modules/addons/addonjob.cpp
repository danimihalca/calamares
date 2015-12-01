#include "addonjob.h"

#include <utils/CalamaresUtilsSystem.h>

#include <GlobalStorage.h>
#include <JobQueue.h>
AddonJob::AddonJob(AddonPtr addon):
    Calamares::Job(),
    m_addon(addon)
{

}

QString AddonJob::prettyName() const
{
   return tr( "Installling %1" ).arg( m_addon->name );
}

Calamares::JobResult AddonJob::exec()
{
    cLog() << "Installing " + m_addon->name ;
    if (!QFileInfo::exists(m_addon->script))
    {
        cLog() << "Missing " + m_addon->script;
        return Calamares::JobResult::error("Missing script");
    }
    cLog() << "Installing " + m_addon->script;

    QString path = Calamares::JobQueue::instance()->globalStorage()->value("rootMountPoint" ).toString();
//    QDir destDir( gs->value );

    QString dest(path + "/script.sh");
    QFile::remove(dest);
    cLog() << "Copying " << m_addon->script << " to " << dest;
    QFile::copy(m_addon->script, dest);
    cLog() << "setting perm 777 to " << dest;
    QFile::setPermissions(dest,QFile::Permissions(777));
    QString output;

    cLog() << "Running " << dest << " from " << path;
    CalamaresUtils::chrootCall("/script.sh");
//    cLog() << "Output for " << m_addon->script << "[" << result << "]:";
//    cLog() << output;

//    switch (result)
//    {
//        case -1:
//        {
//            return Calamares::JobResult::error("Installation failed");
//            break;
//        }
//        case -2:
//        {
//            return Calamares::JobResult::error("Installation failed", "Cannot start");
//            break;
//        }
//        case -3:
//        {
//            return Calamares::JobResult::error("Installation failed", "Bad arguments");
//            break;
//        }
//        case -4:
//        {
//            return Calamares::JobResult::error("Installation failed", "Timeout");
//            break;
//        }
//        default:
//            return Calamares::JobResult::error("Installation failed", "Unknown error");
//    }
    return Calamares::JobResult::ok();
}

