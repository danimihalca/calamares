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

    int result =  CalamaresUtils::chrootCall({m_addon->script});

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

