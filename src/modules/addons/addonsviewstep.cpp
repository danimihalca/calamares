#include "addonsviewstep.h"

#include "addonsviewpage.h"

#include <utils/Logger.h>
#include "addonjob.h"

#include <QVBoxLayout>

AddonsViewStep::AddonsViewStep(QObject* pParent):
    ViewStep(pParent),
    m_widget(nullptr),
    m_jobs()
{
    connect(&m_addons,&AddonModel::itemInstallStatusToggled, this, &AddonsViewStep::onAddonInstallStatusChanged);
//    emit nextStatusChanged(true);
}

AddonsViewStep::~AddonsViewStep()
{
    if ( m_widget && m_widget->parent() == nullptr )
        m_widget->deleteLater();
}

QString AddonsViewStep::prettyName() const
{
    return "Additional features";
}

QWidget *AddonsViewStep::widget()
{
    return m_widget;
}

void AddonsViewStep::next()
{
    emit done();
}

void AddonsViewStep::back()
{
}

bool AddonsViewStep::isNextEnabled() const
{
    return true;
}

bool AddonsViewStep::isBackEnabled() const
{
    return true;
}

bool AddonsViewStep::isAtBeginning() const
{
    return true;
}

bool AddonsViewStep::isAtEnd() const
{
    return true;

}

QList<Calamares::job_ptr> AddonsViewStep::jobs() const
{
    return m_jobs.values();
}

void AddonsViewStep::setConfigurationMap(const QVariantMap &configurationMap)
{
    if (configurationMap.contains("addons"))
    {
        auto addonsList = configurationMap.value("addons").toList();
        for(const auto& item: addonsList)
        {
            auto addon = AddonHelper::createFromMap(item.toMap());
//            cLog() << addon;
            AddonHelper::AddonState state = AddonHelper::isValid(addon);
//            cLog() << state;


            if (state)
            {
                cLog() << "Error processing addon "
                       << "["
                       << addonsList.indexOf(item)
                       << "]"
                       << (addon->name.isEmpty()? "" : " aaa" + addon->name)
                       << ": " <<AddonHelper::getStateMessage(state);
                continue;
            }
            m_addons.addAddon(addon);
        }
    }
    if (m_widget)
    {
        delete m_widget;
    }
    m_widget = new AddonsViewPage(m_addons);
}

void AddonsViewStep::onAddonInstallStatusChanged(const AddonPtr &addon)
{
    if (addon->install && !m_jobs.contains(addon->name))
    {
        m_jobs.insert(addon->name, Calamares::job_ptr(new AddonJob(addon)));
    }
    else if (!addon->install && m_jobs.contains(addon->name))
    {
        m_jobs.remove(addon->name);
    }
}


QString AddonsViewStep::prettyStatus() const
{
    if (m_jobs.empty())
    {
        return "No additional features have been selected.";
    }
    {
        QString message = "The following features will be installed: ";
        for(auto addonName: m_jobs.keys())
        {
            message.append("<strong>"+addonName+"</strong> ");
        }
        return message;
    }
}



