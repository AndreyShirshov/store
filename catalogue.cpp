#include "catalogue.h"

#include <QAction>
#include <QMenu>

#include "posaction.h"

namespace STORE {
namespace Catalogue {

/*********************************************************************/

//class PositionedAction: public QAction {
//    Q_OBJECT
//}

/*********************************************************************/

Model ::Model( QObject *parent )
    :  QAbstractTableModel( parent ) {

    // TODO Это тестовый каталог, заменить на настоящий
    {
        Item::Data *D = new Item::Data( this ) ;
        D->Code = "111" ; // Код подраздела
        D->Title = "Физика" ; // Наименование
        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
        D->To =QDateTime(); // Закрыт с .... (дата) пусто
        D->IsLocal = false ; // локальный
        Cat.append(D) ;
     }
    {
        Item::Data *D = new Item::Data( this ) ;
        D->Code = "222" ; // Код подраздела
        D->Title = "Математика" ; // Наименование
        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
        D->To =QDateTime(); // Закрыт с .... (дата) пусто
        D->IsLocal = false ; // локальный
        Cat.append(D) ;
     }
    {
        Item::Data *D = new Item::Data( this ) ;
        D->Code = "333" ; // Код подраздела
        D->Title = "Биология" ; // Наименование
        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
        D->To =QDateTime(); // Закрыт с .... (дата) пусто
        D->IsLocal = false ; // локальный
        Cat.append(D) ;
     }
    {
        Item::Data *D = new Item::Data( this ) ;
        D->Code = "444" ; // Код подраздела
        D->Title = "Валеология" ; // Наименование
        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
        D->To =QDateTime(); // Закрыт с .... (дата) пусто
        D->IsLocal = true ; // локальный
        Cat.append(D) ;
     }
    {
        Item::Data *D = new Item::Data( this ) ;
        D->Code = "555" ; // Код подраздела
        D->Title = "Литература" ; // Наименование
        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
        D->To =QDateTime(); // Закрыт с .... (дата) пусто
        D->IsLocal = false ; // локальный
        D->Comment = "Проверить правильность" ; // Комментарий
        Cat.append(D) ;
     }

}

/*-------------------------------------------------------------------*/

Model ::~Model() {
    }

/*-------------------------------------------------------------------*/

int Model::rowCount(const QModelIndex &parent ) const {
    if ( ! parent.isValid() ) { // Если родитель не существует
        return Cat.size() ;
        //return 5 ; /// TODO Заменить на правильное количество
    } else {
        return 0 ; // Дочерних нет, количество строк равно 0
    }

}

/*-------------------------------------------------------------------*/

int Model::columnCount( const QModelIndex &parent ) const {
    if ( ! parent.isValid() ) { // Если родитель не существует
        return 6 ; /// TODO Заменить на правильное количество
    } else {
        return 0 ; // Дочерних нет, количество столбцов равно 0
    }

}

/*-------------------------------------------------------------------*/

QVariant Model::dataDisplay( const QModelIndex &I ) const {
    // return QString( "%1,%2" ).arg( I.row() ).arg( I.column() ) ; // Номер строки, номер столбца
    Item::Data *D = Cat[I.row()] ; // Соответствующий элемент списка
    switch ( I.column()) {
    case 0 : return D->Code ;
    case 1 : return D->Title ;
    case 2 : return D->From ;
    case 3 : return D->To ;
    case 4 : return D->IsLocal ? tr ("LOCAL") : QString() ;
    case 5 : return D->Comment.isEmpty() ? QString() : tr("CMT") ;
    default : return QVariant() ;
    }
}

QVariant Model::dataTextAlignment( const QModelIndex &I ) const {
    int Result = Qt::AlignVCenter ; // По вертикали выравниваем по базовой линии AlignBaseline или по центру AlignVCenter
    Result |= I.column() == 1 ?  Qt::AlignLeft : Qt::AlignHCenter ;
    return Result ;
}


// Функция data() Возвращает данные данного элемента

QVariant Model::data( const QModelIndex &I, int role ) const {
    switch ( role ) {
    case Qt::DisplayRole       : return dataDisplay( I ) ;
    case Qt::TextAlignmentRole : return dataTextAlignment( I ) ;
    default : return QVariant() ;
    }
}

/*-------------------------------------------------------------------*/
//Получение координат редактирования

Item::Data *Model::dataDataBlock( const QModelIndex &I ) const {
    int R = I.row() ;
    if ( R < 0 || R >= Cat.size() ) return 0 ;
    return Cat[ R ] ;
}

/*-------------------------------------------------------------------*/
QVariant Model::headerData( int section,
                            Qt::Orientation orientation, int role ) const {
    if ( orientation != Qt::Horizontal )
        return QAbstractTableModel::headerData( section, orientation, role ) ; // Данные по умолчанию, не пишем своих

    switch ( role ) {

    case Qt::DisplayRole :
        switch ( section) {
        case 0 : return tr("Code") ;
        case 1 : return tr("Title") ;
        case 2 : return tr("From") ;
        case 3 : return tr("To") ;
        case 4 : return tr("Local") ;
        default : return QVariant() ;
        }

    case Qt::TextAlignmentRole :
        return QVariant( Qt::AlignBaseline | Qt::AlignHCenter) ; // Выравнивание

    case Qt::ForegroundRole :  // шрифт
        { // TODO Сделать шрифт жирным
            QFont F ;
            F.setBold( true ) ; // Скобки, чтобы избавиться от паразитной переменной
            return F ;
        }

    default: return QVariant() ;

    }

}

/*-------------------------------------------------------------------*/
// Редактирование (аргументы - какой параметр редактировать, на каком виджете показывать)

void Model::editItem( const QModelIndex &I, QWidget *parent ) {
    Item::Dialog Dia ( parent ) ;
    Item::Data *D = dataDataBlock( I ) ;
    if ( !D ) return ; // Если блока данных нет, то редактировать нечего
    Dia.setDataBlock( D ) ;
    beginResetModel() ; // Блокировка запроса изменения модели
    Dia.exec() ;
    endResetModel() ; // Разрешение запроса изменения модели
}

/*********************************************************************/

TableView::TableView( QWidget * parent )
    : QTableView( parent ) {

    //Редактирование данных в контекстном меню
    setContextMenuPolicy( Qt::CustomContextMenu) ;
    connect( this, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(contextMenuRequested(QPoint)) ) ;

    Model *M =new Model( this );
    setModel( M ) ;

    {
        PosAction *A = actEditItem = new PosAction( this ) ; // Создание наследуемого от QAction экшена для редактирования
        A->setText( tr("Edit") ) ;
        connect( A, SIGNAL(editItem(QModelIndex, QWidget*)),
                 M, SLOT(editItem(QModelIndex, QWidget*)) ) ;
        addAction( A ) ;
    }

    {
        QHeaderView *H = verticalHeader() ;
        H->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
    }{
        QHeaderView *H = horizontalHeader() ;
        H->setSectionResizeMode( QHeaderView::ResizeToContents ) ; // Для всех
        H->setSectionResizeMode(1, QHeaderView::Stretch ) ; // Для первой колонки Title
    }

}

/*-------------------------------------------------------------------*/

TableView::~TableView() {
    }

/*-------------------------------------------------------------------*/

//void TableView::editItem() {
//} не нужен

/*-------------------------------------------------------------------*/

void TableView::contextMenuRequested( const QPoint &p ) {
    QMenu M (this ) ;
    QModelIndex I = indexAt( p ) ; // Индекс под курсором
    if ( I.isValid() ) { // Чтобы не кликалось на пустом месте
        actEditItem->I = I ;
        actEditItem->pWidget = this ;
        M.addAction( actEditItem ) ;
    }
    M.exec( mapToGlobal( p ) ) ;

}

/*********************************************************************/

} // namespace Catalogue
} // namespace STORE
