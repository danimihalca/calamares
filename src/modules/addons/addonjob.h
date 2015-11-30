#ifndef ADDONJOB_H
#define ADDONJOB_H

#include <Job.h>

#include <memory>

#include <addonmodel.h>

class AddonJob: public Calamares::Job
{
public:
    AddonJob(AddonPtr addon);

    // Job interface
public:
    QString prettyName() const;
    Calamares::JobResult exec();

private:
    AddonPtr m_addon;
};

#endif // ADDONJOB_H
