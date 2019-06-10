#include "catalogue.h"

#include <QAction>
#include <QMenu>

#include <QtDebug>
#include <QtSql>

#include "posaction.h"

namespace STORE {
namespace Catalogue {

/*********************************************************************/

Model ::Model( QObject *parent )
    :  QAbstractTableModel( parent ) {

    QSqlQuery qry ;
    qry.setForwardOnly( true ) ;//  // Включение курсора в режим ForwardOnly
    qry.prepare(
        "select                  \n"
        "      iid,              \n"
        "      code,             \n"
        "      title,            \n"
        "      valid_from,       \n"
        "      valid_to,         \n"
        "      islocal,          \n"
        "      acomment,         \n"
        "      rid_parent,       \n"
        "      alevel            \n"
        "    from catalogue ;    \n"
    ) ;

//    qry.prepare("SELECT procedure_name (:parametr)"); // C подготовкой
//    qry.bindValue(":parametr",myValue); // Запрос с параметрами

//    qry.exec() ;
    if ( qry.exec() ) { // Выполняем запрос к базе и устанавливает курсор перед первой записью множества
        while ( qry.next() ) {
            Item::Data *D = new Item::Data( this, qry ) ;
            Cat.append( D ) ;
        }
    } else {
//        qCritical() << "Cannot execute query" ;
        QSqlError Err = qry.lastError() ;
        qCritical() << Err.nativeErrorCode() ; // Код ошибки полученной от родной библиотеки
        qCritical() << Err.databaseText().toUtf8().data() ; //Текст ошибки полученой от базы данных
        qCritical() << Err.driverText().toUtf8().data() ; // Текст ошибки полученной от драйвера
//        qDebug() << qry.executedQuery().toUtf8().data() ; // Смотрим текст запроса
    }

    // TODO Это тестовый каталог, заменить на настоящий
//    {
//        Item::Data *D = new Item::Data( this ) ;
//        D->Code = "111" ; // Код подраздела
//        D->Title = "Физика" ; // Наименование
//        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
//        D->To =QDateTime(); // Закрыт с .... (дата) пусто
//        D->IsLocal = false ; // локальный
//        Cat.append(D) ;
//     }
//    {
//        Item::Data *D = new Item::Data( this ) ;
//        D->Code = "222" ; // Код подраздела
//        D->Title = "Математика" ; // Наименование
//        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
//        D->To =QDateTime(); // Закрыт с .... (дата) пусто
//        D->IsLocal = false ; // локальный
//        Cat.append(D) ;
//     }
//    {
//        Item::Data *D = new Item::Data( this ) ;
//        D->Code = "333" ; // Код подраздела
//        D->Title = "Биология" ; // Наименование
//        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
//        D->To =QDateTime(); // Закрыт с .... (дата) пусто
//        D->IsLocal = false ; // локальный
//        Cat.append(D) ;
//     }
//    {
//        Item::Data *D = new Item::Data( this ) ;
//        D->Code = "444" ; // Код подраздела
//        D->Title = "Валеология" ; // Наименование
//        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
//        D->To =QDateTime(); // Закрыт с .... (дата) пусто
//        D->IsLocal = true ; // локальный
//        Cat.append(D) ;
//     }
//    {
//        Item::Data *D = new Item::Data( this ) ;
//        D->Code = "555" ; // Код подраздела
//        D->Title = "Литература" ; // Наименование
//        D->From  = QDateTime::currentDateTime() ; // Действует с .... (дата) текущая
//        D->To =QDateTime(); // Закрыт с .... (дата) пусто
//        D->IsLocal = false ; // локальный
//        D->Comment = "Проверить правильность" ; // Комментарий
//        Cat.append(D) ;
//     }

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
    case 2 : return D->From.isValid() ? D->From.toString("dd.MM.yyyy") : "" ;
    case 3 : return D->To.isValid() ? D->To.toString("dd.MM.yyyy") : "" ;
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

QVariant Model::dataFont(const QModelIndex &I) const {
    Item::Data *D = dataDataBlock(I) ;
    if( !D ) return QVariant() ; //Использовать значение по умолчанию
    QFont F ; // Шрифт
    if( D->Deleted ) F.setStrikeOut( true) ; // Сделать зачёркнутым
    //F.setItalic( true ) ; Выделение остальных курсивом
    return F ;
}

QVariant Model::dataForeground(const QModelIndex &I) const { // Цвет шрифта
    Item::Data *D = dataDataBlock(I) ;
    if( ! D ) return QVariant() ; //Использовать значение по умолчанию
    QColor Result = D->IsLocal ? QColor( "blue" ) : QColor( "black" ) ;
    if( ! D->isActive() ) Result.setAlphaF( 1.0/3.0) ; // Прозрачность
    return Result ;
}

QVariant Model::dataToolTipe( const QModelIndex &I) const { // Всплывающая подсказка
    Item::Data *D = dataDataBlock(I) ;
    if( ! D ) return QVariant() ; // Если пустой, использовать значение по умолчанию
    switch( I.column() ) {
    case 2 : {
        if( ! D->To.isValid() ) return QVariant() ; // Срок действия никогда не истекает
        return tr("Valid to: %1").arg(D->To.toString("dd.MM.yyyy") ) ; // Возвращаем отформатированную дату закрытия
    }
    default: return QVariant() ;
    }
}

/*----------Функция data() Возвращает данные данного элемента--------*/
QVariant Model::data( const QModelIndex &I, int role ) const {
    switch ( role ) {
    case Qt::DisplayRole       : return dataDisplay(I) ;
    case Qt::TextAlignmentRole : return dataTextAlignment(I) ;
    case Qt::ForegroundRole    : return dataForeground(I) ; // Для выделения цветом
    case Qt::FontRole          : return dataFont(I) ; // Для выделения зачёркиванием текста
    case Qt::ToolTipRole       : return dataToolTipe(I) ; // Для всплывающей подсказки
    //case Qt::UserRole          : return QVariant( dataDataBlock(I) ) ;
    // Возвращает указатель на соответствующий DataBlock, но передавать в конструктор указатель нельзя, закомментировано
    case Qt::UserRole+1        : {
        Item::Data *D = dataDataBlock(I) ;
        if ( !D ) return false ; // Элемент удалён
        return D->Deleted ; // Иначе Deleted
    }
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

/*-------------------------------------------------------------------*/
// Удаление (аргументы - какой параметр редактировать, на каком виджете показывать)

void Model::delItem( const QModelIndex &I, QWidget *parent ) {
    //TODO Спросить у пользователя, уверен ли он, что хочет удалить элемент
    Item::Data *D = dataDataBlock( I ) ; // Получаем блок данных
    if ( !D ) return ; // Если блока данных нет, то удалять нечего
    //TODO Исходим из того , что модель линейна
    beginResetModel() ; // Блокировка запроса изменения модели
    if ( D->Id.isNull() || ! D->Id.isValid() ) { // Если элемент новый
        Cat.removeAt( I.row() ) ; // Удаляем строку модели
        delete D ; // Удаляем данные строки
    } else {
        D->Deleted = ! D->Deleted ; // Помечен на удаление или не помечен (инверсия true or false)
    }
    endResetModel() ; // Разрешение запроса изменения модели
}

/*-------------------------------------------------------------------*/
// Добавление нового элемента (аргументы - какой родитель, на каком виджете показывать)

void Model::newItem( const QModelIndex &parentI, QWidget *parent ) {

    if ( parentI.isValid() ) {
        // TODO: Сделать добавление нового элемента необязательно в корень каталога
        qWarning() << "Cannot add non-toplevel item" ;
        return ;
    }
    Item::Data *D = new Item::Data( this ) ;
    if ( !D ) {
        qWarning() << "Cannot create new item" ;
        return ; // Если блока данных нет, то редактировать нечего
    }
    Item::Dialog Dia ( parent ) ;
    Dia.setDataBlock( D ) ;
    if( Dia.exec() == QDialog::Accepted ) {
    beginResetModel() ; // Блокировка запроса изменения модели
    Cat.append( D ) ;
    endResetModel() ; // Разрешение запроса изменения модели
    } else {
        delete D ; // если пользователь нажал на cansel, вновь созданный объект надо убить
    }
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
        PosAction *A = actNewItem = new PosAction( this ) ; // Создание наследуемого от QAction экшена для создания
        A->setText( tr("Add") ) ;
        connect( A, SIGNAL(editItem(QModelIndex, QWidget*)),
                 M, SLOT(newItem(QModelIndex, QWidget*)) ) ;
        addAction( A ) ;
    }

    {
        PosAction *A = actDelItem = new PosAction( this ) ; // Создание наследуемого от QAction экшена для удаления
        A->setText( tr("Delete") ) ;
        connect( A, SIGNAL(editItem(QModelIndex, QWidget*)),
                 M, SLOT(delItem(QModelIndex, QWidget*)) ) ;
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

    setColumnHidden( 3, true ) ; // Скрываем колонку "To"
    setColumnHidden( 4, true ) ; // Скрываем колонку "Local"

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
        actDelItem->I = I ;
        actDelItem->pWidget = this ;
        if ( I.data( Qt::UserRole+1 ).toBool() ) { // Если элемент удалён
            actDelItem->setText( tr("Restore") ) ; // Меняем надпись на экшене на Restore
        } else {
            actDelItem->setText( tr("Deleted") ) ; // Меняем надпись на экшене на Deleted
        }
        M.addAction( actDelItem ) ;
    }
    actNewItem->I = QModelIndex() ;
    actNewItem->pWidget = this ;
    M.addAction( actNewItem ) ;
    M.exec( mapToGlobal( p ) ) ;

}

/*********************************************************************/

} // namespace Catalogue
} // namespace STORE
