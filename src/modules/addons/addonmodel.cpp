#include "addonmodel.h"

#include <utils/Logger.h>

void AddonModel::addAddon(AddonPtr tAddon)
{
cLog() << Q_FUNC_INFO;
beginInsertRows(QModelIndex(), rowCount(), rowCount());
//    tAddon->setParent(&m_addonList);
    //TODO: handle/delete list items
    m_addonList.append(tAddon);
    endInsertRows();
}

const QList<AddonPtr> &AddonModel::getAddons() const
{
    return m_addonList;
}

int AddonModel::rowCount(const QModelIndex& /*parent*/) const
{
cLog() << Q_FUNC_INFO;
return m_addonList.size();
}

QVariant AddonModel::data(const QModelIndex &index, int role) const
{
    static QVariant invalid = {};
    cLog() << index.row() << " : " << Addon::AddonRoles(role) << " | " << Qt::ItemDataRole(role);

    if (!index.isValid() || index.row() >= m_addonList.size())
    {
        return invalid;
    }
    if (role == Addon::NAME_DISPLAY_ROLE)
    {
        return m_addonList.at(index.row())->name;
    }
    if (role == Addon::DESCRIPTION_DISPLAY_ROLE)
    {
        return m_addonList.at(index.row())->description;
    }
    if (role == Addon::INSTALL_DISPLAY_ROLE)
    {
        return m_addonList.at(index.row())->install;
    }
    return QVariant();
}

