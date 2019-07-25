#include "mainwindow.h"

#include <QDockWidget>

#include "catalogue.h"
#include "books.h"

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

    Catalogue::Model *M = nullptr ;

    {
     QDockWidget *D = new QDockWidget( this ) ; // Создаём припаркованный виджет
     D->setWindowTitle( tr("Catalogue") ) ;
     Catalogue::ColumnView *W = new Catalogue::ColumnView( D ) ;
     D->setWidget( W ) ;
//     D->setWidget( new Catalogue::ColumnView( D ) ) ; // Задаём основную модель
     addDockWidget( Qt::TopDockWidgetArea, D ) ;
     M = qobject_cast<Catalogue::Model*>( W->model() ) ; // Вспомогательный класс
    }{
      QDockWidget *D = new QDockWidget( this ) ; // Создаём припаркованный виджет
      D->setWindowTitle( tr("Catalogue") ) ;
      D->setWidget( new Catalogue::TreeView( D, M ) ) ;
      addDockWidget( Qt::LeftDockWidgetArea, D ) ;
     }

}

MainWindow::~MainWindow() {

}

} // namespace STORE
