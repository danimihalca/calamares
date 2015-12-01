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
    QString prettyName() const override;
    QWidget *widget() override;
    void next() override;
    void back() override;
    bool isNextEnabled() const override;
    bool isBackEnabled() const override;
    bool isAtBeginning() const override;
    bool isAtEnd() const override;
    QList<Calamares::job_ptr> jobs() const override;


private:
    QWidget* m_widget;
    AddonModel m_addons;
    QMap<QString, Calamares::job_ptr > m_jobs;

    // ViewStep interface
public:
    void setConfigurationMap(const QVariantMap &configurationMap) override;

    QString prettyStatus() const override;
private slots:
    void onAddonInstallStatusChanged(const AddonPtr& addon);

};

#endif // ADDONSVIEWSTEP_H
