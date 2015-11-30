#ifndef ADDONSVIEWSTEP_H
#define ADDONSVIEWSTEP_H

#include <QObject>

#include "viewpages/ViewStep.h"
#include "PluginDllMacro.h"

#include "addonmodel.h"

class PLUGINDLLEXPORT AddonsViewStep : public Calamares::ViewStep
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "calamares.ViewModule/1.0" )

    Q_INTERFACES( Calamares::ViewStep )
public:
    explicit AddonsViewStep(QObject* pParent = nullptr);
    ~AddonsViewStep();

public: //From Calamares::ViewStep interface
    QString prettyName() const;
    QWidget *widget();
    void next();
    void back();
    bool isNextEnabled() const;
    bool isBackEnabled() const;
    bool isAtBeginning() const;
    bool isAtEnd() const;
    QList<Calamares::job_ptr> jobs() const;


private:
    QWidget* m_widget;
    AddonModel m_addons;

    // ViewStep interface
public:
    void setConfigurationMap(const QVariantMap &configurationMap);


};

#endif // ADDONSVIEWSTEP_H
