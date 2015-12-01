#include "addonjob.h"

#include <utils/CalamaresUtilsSystem.h>


AddonJob::AddonJob(AddonPtr addon):
    Calamares::Job(),
    m_addon(addon)
{

}

QString AddonJob::prettyName() const
{
    return "Installing " + m_addon->name ;
}

Calamares::JobResult AddonJob::exec()
{
    cLog() << "Installing " + m_addon->name ;
    if (!QFileInfo::exists(m_addon->script))
    {
        cLog() << "Missing " + m_addon->script;
        return Calamares::JobResult::error("Missing script");
    }
    cLog() << "Running " + m_addon->script;

    QString output;
    int result =  CalamaresUtils::chrootOutput({m_addon->script}, output);
    cLog() << "Output for " << m_addon->script << "[" << result << "]:";
    cLog() << output;

    switch (result)
    {
        case -1:
        {
            return Calamares::JobResult::error("Installation failed");
            break;
        }
        case -2:
        {
            return Calamares::JobResult::error("Installation failed", "Cannot start");
            break;
        }
        case -3:
        {
            return Calamares::JobResult::error("Installation failed", "Bad arguments");
            break;
        }
        case -4:
        {
            return Calamares::JobResult::error("Installation failed", "Timeout");
            break;
        }
        default:
            return Calamares::JobResult::error("Installation failed", "Unknown error");
    }
    return Calamares::JobResult::ok();
}

