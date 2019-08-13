#include "mainwindow.h"

#include <QDockWidget>

#include "catalogue.h"
#include "books.h"
#include "filter.h"

namespace STORE {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

//    Catalogue::TableView *W = new Catalogue::TableView( this ) ; // Табличное представление
//    Catalogue::TreeView *W = new Catalogue::TreeView( this ) ; // Древовидное представление
//    Catalogue::ColumnView *W = new Catalogue::ColumnView( this ) ; // Колоночное представление
//    setCentralWidget( W ) ;

//    Catalogue::Model *M = qobject_cast<Catalogue::Model*>( W->model() ) ; // Вспомогательный класс

    Books::View *B = new Books::View( this ) ;
    setCentralWidget( B ) ;

//    Catalogue::Model *M = nullptr ;
    Catalogue::ColumnView *W = nullptr ;
    Filter *F = nullptr ;

    {
     QDockWidget *D = new QDockWidget( this ) ; // Создаём припаркованный виджет
     D->setWindowTitle( tr("Catalogue") ) ;
     W = new Catalogue::ColumnView( D ) ;
     D->setWidget( W ) ;
//     D->setWidget( new Catalogue::ColumnView( D ) ) ; // Задаём основную модель
     addDockWidget( Qt::TopDockWidgetArea, D ) ;
//     M = qobject_cast<Catalogue::Model*>( W->model() ) ; // Вспомогательный класс
    }{
      QDockWidget *D = new QDockWidget( this ) ; // Создаём припаркованный виджет
//      D->setWindowTitle( tr("Catalogue") ) ;
//      D->setWidget( new Catalogue::TreeView( D, M ) ) ;
      D->setWindowTitle( tr("Filter") ) ;
      F = new Filter( D ) ;
      D->setWidget( F ) ; // Установка фильтра на виджет
      addDockWidget( Qt::LeftDockWidgetArea, D ) ;
     }

    connect( W, SIGNAL( item_selected(QVariant)), // Выбор каталожного элемента
             B->model(), SLOT( cat_item_selected(QVariant) ) ) ;
    connect( F, SIGNAL(apply_filter(QObject*)),
             B->model(), SLOT(apply_filter(QObject*)) ) ;
}

MainWindow::~MainWindow() {

}

} // namespace STORE
