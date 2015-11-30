#include "addonsviewpage.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include "utils/Retranslator.h"

#include <QListView>

#include <QStringListModel>

#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>

#include "addonmodel.h"
#include <utils/Logger.h>

AddonsViewPage::AddonsViewPage(AddonModel& model, QWidget* pParent) :
    QWidget(pParent),
//    m_model(model),
    m_scrollArea(new QScrollArea(this))
{
    QLayout* layout = new QVBoxLayout( this );
    setLayout(layout);
    layout->setContentsMargins( 0, 0, 0, 0 );

    QLabel* headerLabel = new QLabel( this );
    CALAMARES_RETRANSLATE(
        headerLabel->setText( tr( "Select additional features which are needed" ) );
    )


    layout->addWidget( headerLabel );
    layout->addWidget( m_scrollArea );
    m_scrollArea->setWidgetResizable( true );
    m_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    m_scrollArea->setFrameStyle( QFrame::NoFrame );
    m_scrollArea->setContentsMargins( 0, 0, 0, 0 );


    ////////////////////////////////////////////////////

//    QListView* lv = new QListView(m_scrollArea);
//    QStringListModel* slm = new QStringListModel(lv);
//    lv->setModel(slm);
//    slm->setStringList( {"MATA1", "MATA2", "MATA3"});


    ////////////////////////////////////////////////////

//    QListWidget* lv = new QListWidget(m_scrollArea);

//    QListWidgetItem *item1 = new QListWidgetItem("Maqzcta1");
////    item1->setFlags(Qt::ItemIsUserCheckable);
//    item1->setData( Qt::DisplayRole, "Mata1" );
//    item1->setData( Qt::CheckStateRole, Qt::Unchecked);
//    lv->addItem( item1 );

//    QListWidgetItem *item2 = new QListWidgetItem("Mwqata2");
//    item2->setFlags(Qt::ItemIsUserCheckable);
////    item2->setData( Qt::DisplayRole, "" );
//    item2->setData( Qt::CheckStateRole, Qt::Unchecked );
//    lv->addItem( item2 );

//    QListWidgetItem *item3 = new QListWidgetItem("Mqweata3");
//    item3->setFlags(Qt::ItemIsUserCheckable);
////    item3->setData( Qt::DisplayRole, "Mata3" );
//    item3->setData( Qt::CheckStateRole, Qt::Unchecked );
//    lv->addItem( item3 );

    /////////////////////////////////////////////////////


    auto lv = new QListView(m_scrollArea);
   lv->setModel(&model);
   lv->setSelectionMode(QListView::NoSelection);
    lv->setMouseTracking(false);
    m_scrollArea->setWidget(lv);

//    Addon a = {"asd","qweq","1231"};
//    m_model.addAddon(a);

//    connect(lv,&QListView::clicked,this, []
//    {
//        cLog() << "CLICKED";
//    });

    AddonDelegate* ad= new AddonDelegate;
    ad->setParent(lv);
    lv->setItemDelegate(ad);

    connect(lv, &QListView::clicked, &model, &AddonModel::onItemClicked);
//    connect(&m_model, &AddonModel::ItemClicked, ad, &AddonDelegate::onItemClicked);


}

