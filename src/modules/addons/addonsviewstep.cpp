#include "addonsviewstep.h"

#include "addonsviewpage.h"

#include <utils/Logger.h>

AddonsViewStep::AddonsViewStep(QObject* pParent):
    ViewStep(pParent),
    m_widget(nullptr)
{
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
    return QList<Calamares::job_ptr>();
}

void AddonsViewStep::setConfigurationMap(const QVariantMap &configurationMap)
{
    if (configurationMap.contains("addons"))
    {
        auto addonsList = configurationMap.value("addons").toList();
        for(const auto& item: addonsList)
        {
            Addon* addon = AddonHelper::createFromMap(item.toMap());
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

