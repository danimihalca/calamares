#ifndef ADDONSVIEWPAGE_H
#define ADDONSVIEWPAGE_H

#include <QWidget>

class AddonModel;

class QScrollArea;

class AddonsViewPage : public QWidget
{
    Q_OBJECT
public:
    explicit AddonsViewPage(AddonModel& model, QWidget* pParent = nullptr);

signals:

public slots:


private:
    QScrollArea* m_scrollArea;
//    AddonModel& m_model;
};

#endif // ADDONSVIEWPAGE_H
