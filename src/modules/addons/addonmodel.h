#ifndef ADDONMODEL_H
#define ADDONMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <QMap>
#include <QMetaObject>
#include <QFlags>
#include <QFileInfo>
#include <QPaintEvent>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

#include <utils/CalamaresUtils.h>
#include <utils/Logger.h>
#include <CalamaresConfig.h>

#include <memory>

#define DEFAULT_SCRIPTS_PATH CMAKE_INSTALL_FULL_DATADIR "/modules/addons/scripts/"

class Addon: public QObject
{
    Q_OBJECT
public:
    enum AddonRoles
    {
        NAME_DISPLAY_ROLE,
        DESCRIPTION_DISPLAY_ROLE,
        INSTALL_DISPLAY_ROLE
    };
    Q_ENUM(AddonRoles)

    Addon(QObject* pParent = nullptr):
        QObject(pParent)
    {
    }

//public:

//    explicit Addon(const QString& name="", const QString& description="", const QString& script=""):
//        m_name(name),
//        m_description(description),
//        m_script(script)
//    {
//    }
//    Addon() = default;
//    Addon(Addon&& a) = default;
//private:
    QString name;
    QString description;
    QString script;
    bool install = false;

};
//Q_DECLARE_METATYPE(Addon)

typedef QSharedPointer<Addon> AddonPtr;
Q_DECLARE_METATYPE(AddonPtr)

inline QDebug operator<<(QDebug dbg, const Addon& addon)
{
    dbg.nospace() << "(Addon, (";
    dbg.nospace() << addon.name;
    dbg.nospace() << ",";
    dbg.nospace() << addon.description;
    dbg.nospace() << ",";
    dbg.nospace() << addon.script;
    dbg.nospace() << "))";
    return dbg.maybeSpace();
}

//namespace AddonState {
//    constexpr int
//            VALID                     = 0,
//            MISSING_NAME_FIELD        = 1 << 0,
//            MISSING_DESCRIPTION_FIELD = 1 << 1,
//            MISSING_SCRIPT_FIELD      = 1 << 2,
//            INEXISTENT_SCRIPT_FILE    = 1 << 7;
//}
//using AddonState::int = AddoanaState;

class AddonHelper
{
public:
    enum class AddonStateInternal : uint8_t
    {
//        VALID                     = 0,
        MISSING_NAME_FIELD        = 1 << 0,
        MISSING_DESCRIPTION_FIELD = 1 << 1,
        MISSING_SCRIPT_FIELD      = 1 << 2,
        INEXISTENT_SCRIPT_FILE    = 1 << 7
    };
    Q_DECLARE_FLAGS(AddonState, AddonStateInternal)

    static AddonPtr createFromMap(const QMap<QString,QVariant>& addonDataMap)
    {
//        auto addon = std::make_shared<Addon>(new Addon);
        auto addon = QSharedPointer<Addon>(new Addon);

        addon->name = addonDataMap.value("name").toString();
        addon->description = addonDataMap.value("description").toString();

        QString scriptPath = addonDataMap.value("script").toString();
        if (!scriptPath.startsWith("/"))
        {
            scriptPath.prepend(DEFAULT_SCRIPTS_PATH);
        }
        if (!scriptPath.isEmpty())
        {
            addon->script = scriptPath;
        }

        return addon;
    }

    static AddonState isValid(const AddonPtr& addon)
    {
        AddonState state; //= AddonState(AddonStateInternal::VALID);
        if (addon->name.isEmpty())
        {
            state |= AddonStateInternal::MISSING_NAME_FIELD;
        }
        if (addon->description.isEmpty())
        {
            state |= AddonStateInternal::MISSING_DESCRIPTION_FIELD;
        }
        if (addon->script.isEmpty())
        {
            state |= AddonStateInternal::MISSING_SCRIPT_FIELD;
            return state;
        }
        else if (!QFileInfo::exists(addon->script))
        {
            state |= AddonStateInternal::INEXISTENT_SCRIPT_FILE;
        }
        //TODO: check if execute flag is set
//        state.testFlag(AddonState::enum_type::VALID);

        return state;
    }

    static QString getStateMessage(const AddonState& state)
    {
        QString errorMessage;
        bool containsMultipleErrors = false;
        if (state.testFlag(AddonStateInternal::MISSING_NAME_FIELD))
        {
            errorMessage += "missing name field";
            containsMultipleErrors = true;
        }
        if (state.testFlag(AddonStateInternal::MISSING_DESCRIPTION_FIELD))
        {
            if (containsMultipleErrors)
            {
                errorMessage += ", ";
            }
            else
            {
                containsMultipleErrors = true;
            }
            errorMessage += "missing description field";
        }
        if (state.testFlag(AddonStateInternal::MISSING_SCRIPT_FIELD))
        {
            if (containsMultipleErrors)
            {
                errorMessage += ", ";
            }
            else
            {
                containsMultipleErrors = true;
            }
            errorMessage += "missing description field";
        }
        else if (state.testFlag(AddonStateInternal::INEXISTENT_SCRIPT_FILE))
        {
            if (containsMultipleErrors)
            {
                errorMessage += ", ";
            }
            //TODO: print script path
            errorMessage += "inexistent script file";
        }
        return errorMessage;
    }
};
//Q_DECLARE_OPERATORS_FOR_FLAGS(AddonHelper::AddonState)


class AddonModel : /*public QObject,*/ public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AddonModel(QObject* pParent = nullptr):
        QAbstractListModel(pParent)
    {
    }
//    ~AddonModel()
//    {
////        qDeleteAll(m_addonList);
//    }

    void addAddon(AddonPtr tAddon);
    const QList<AddonPtr>& getAddons() const;

public: // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

public slots:
    void onItemClicked(const QModelIndex& index)
    {
        bool install = data(index,Addon::INSTALL_DISPLAY_ROLE).toBool();
        setData(index, !install,Addon::INSTALL_DISPLAY_ROLE);
        emit dataChanged(index,index,{Addon::INSTALL_DISPLAY_ROLE});
//        cLog()<< index;
//        cLog() << index.data();
//        cLog() << index.model();
//        cLog() << index.data().value<AddonPtr>();
        emit itemInstallStatusToggled(m_addonList.at(index.row()));
    }

signals:
    void itemInstallStatusToggled(const AddonPtr& addon);

private:
    QList<AddonPtr> m_addonList;

    // QAbstractItemModel interface
public:
    QHash<int, QByteArray> roleNames() const
    {
        static QHash<int, QByteArray> roles = {};
        if (roles.empty())
        {
            roles[Addon::NAME_DISPLAY_ROLE] = "name";
            roles[Addon::DESCRIPTION_DISPLAY_ROLE] = "description";
            roles[Addon::INSTALL_DISPLAY_ROLE] = "install";
        }

        return roles;
    }

    // QAbstractItemModel interface
public:
    bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid() || index.row() >= m_addonList.size() || role != Addon::INSTALL_DISPLAY_ROLE)
        {
            return false;
        }
        m_addonList.at(index.row())->install = value.toBool();
        return true;
    }
};


class AddonDelegate: public QStyledItemDelegate
{

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        cLog() << Q_FUNC_INFO << " : " << index.row();
        QStyleOptionViewItemV4 opt = option;

        initStyleOption(&opt, index);

        QString text = qvariant_cast<QString>(index.data(Addon::NAME_DISPLAY_ROLE));
        QString text1 = qvariant_cast<QString>(index.data(Addon::DESCRIPTION_DISPLAY_ROLE));
        bool aaa = qvariant_cast<bool>(index.data(Addon::INSTALL_DISPLAY_ROLE));

        // draw correct background
        opt.text = "";
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);


        QRect rect = opt.rect;
//        rect = rect.marginsAdded(QMargins{0,30,0,30});

        QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
            cg = QPalette::Inactive;


        painter->save();

        // set pen color
        if (opt.state & QStyle::State_Selected)
            painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
        else
            painter->setPen(opt.palette.color(cg, QPalette::Text));

        QFont f = painter->font();
        f.setBold(true);
        painter->setFont(f);

        painter->drawText(QRect(rect.left() + 30, rect.top(), rect.width(), rect.height()/2 ),
                          opt.displayAlignment, text);
        f.setBold(false);
        painter->setFont(f);
        painter->drawText(QRect(rect.left() + 30, rect.top()+rect.height()/2, rect.width(), rect.height()/2 ),
                          opt.displayAlignment, text1);

        const QStyle *style1 = QApplication::style();
        QStyleOptionButton opt1;

        opt1.state |= aaa ? QStyle::State_On : QStyle::State_Off;
//        opt1.state |= QStyle::State_On;
        opt1.state |= QStyle::State_Enabled;
//         opt1.palette = QPalette(Qt::red);
        opt1.rect = option.rect;
        cLog() << opt1.rect;
//        opt1.features |= QStyleOptionButton::CommandLinkButton;
        opt1.rect = rect;
//        opt1.rect.setWidth(200);
//        opt1.rect.setHeight(200);
//        opt1.iconSize = QSize(100,100);
//        cLog() << opt1.rect;

//        QTransform t;
//        painter->setTransform(t.scale(1.5,1.5));

        //        opt1.palette = QPalette(Qt::white);
        style1->drawControl(QStyle::CE_CheckBox,&opt1,painter);


        painter->restore();
    }

    // QAbstractItemDelegate interface
public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QStyledItemDelegate::sizeHint(option,index) + QSize(0,20);
    }
};

#endif // ADDONMODEL_H
