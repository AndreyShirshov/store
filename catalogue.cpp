#include "catalogue.h"

#include <QAction>
#include <QMenu>
#include <QMessageBox>

#include <QtDebug>
#include <QtSql>

#include "posaction.h"

// Объявляем макрос с проверкой, нашли указатель на родителя, что возвращать, если указатель нулевой
// D - указатель народительский элемент
#define DATA_PTR(D,I,VALUE)                            \
    Item::Data *D = static_cast<Item::Data*>(I.internalPointer()) ;     \
    if ( ! D ) return VALUE
//Объявляем макрос форматирования времени
#define DATE_STR(DATE) ( \
    DATE.isValid() ? DATE.toString("dd.MM.yyyy") : QString() \
    )

namespace STORE {
namespace Catalogue {

/*********************************************************************/

Model ::Model( QObject *parent )
    :  QAbstractItemModel( parent ),
       LastTempId( 1 ) {

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
        "    from catalogue      \n"
        "    order by alevel ;   \n"
    ) ;

//    qry.prepare("SELECT procedure_name (:parametr)"); // C подготовкой
//    qry.bindValue(":parametr",myValue); // Запрос с параметрами

//    qry.exec() ;
    if ( qry.exec() ) { // Выполняем запрос к базе и устанавливает курсор перед первой записью множества
        while ( qry.next() ) {
            bool OK = false ;
            QVariant PI =qry.value("rid_parent") ;// Считываем значение rid_parent
            int ParentId = PI.toInt( &OK ) ; // Гарантировано, что Id родителя целое число
            // (OK = false, если число не правильное)
            Item::Data *P = nullptr ; // Родителя нет
            if( OK && !PI.isNull() )
                P = Cat.findPointer( ParentId ) ;
            if( P ) {
                Item::Data *D = new Item::Data( P, qry ) ; // Создали элемент
                P->Children.append( D ) ; // Добавляем элемент к списку дочерних
                D->pParentItem = P ; // Указатель на родителя
            } else { // Если родителя не нашли
                Item::Data *D = new Item::Data( this, qry ) ; // Создали элемент
                Cat.append( D ) ; // Добавляем элемент в корень
                D->pParentItem = nullptr ; // Нет родителя
            }
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
        //qDebug() << parent << Cat.size() ;
        return Cat.size() ; // Количество элементов в списке Cat
        //return 5 ; /// TODO Заменить на правильное количество
    } else {// Если родитель правильный
        DATA_PTR( P, parent, 0 ) ; // макрос
        // Item::Data *P = static_cast<Item::Data*>( parent.internalPointer() ) ; // Было до макроса
        //if( ! P )
        //return 0 ; // Дочерних нет, количество строк равно 0
        //qDebug() << parent << P->Children.size() ;
        return P->Children.size() ; // Иначе количество дочерних элементов
    }
}

/*-------------------------------------------------------------------*/

int Model::columnCount( const QModelIndex & parent) const {
    //if ( ! parent.isValid() )  // Если родитель не существует
        return 6 ; /// TODO Заменить на правильное количество
    //    DATA_PTR(P, parent, 0) ; // Иначе
    //return P->Children.size() > 0 ? 6 : 0 ;
    //    return 0 ; // Дочерних нет, количество столбцов равно 0
}

QVariant Model::dataDisplay( const QModelIndex &I ) const {
    // return QString( "%1,%2" ).arg( I.row() ).arg( I.column() ) ; // Номер строки, номер столбца
    DATA_PTR( D, I, QVariant() ) ; // Макрос
//    Item::Data *D = Cat[I.row()] ; // Соответствующий элемент списка, было до макроса
//    qDebug() << I.row() << I.column() << D->Code ;
    switch ( I.column()) {
    case 1 : return D->Code ;
    case 0 : return D->Title ; // 0 - Для Колоночного представления, Чтобы показывал заголовок
    // case 2 : return D->From.isValid() ? D->From.toString("dd.MM.yyyy") : "" ; // До макроса
    case 2 : return DATE_STR( D->From ) ; // Макрос
    //case 3 : return D->To.isValid() ? D->To.toString("dd.MM.yyyy") : "" ;      // До макроса
    case 3 : return DATE_STR( D->To   ) ; // Макрос
    case 4 : return D->IsLocal ? tr ("LOCAL") : QString() ;
    case 5 : return D->Comment.isEmpty() ? QString() : tr("CMT") ;
    default : return QVariant() ;
    }
}
QVariant Model::dataTextAlignment( const QModelIndex &I ) const {
    int Result = Qt::AlignVCenter ; // По вертикали выравниваем по базовой линии AlignBaseline или по центру AlignVCenter
    Result |= I.column() == 0 ?  Qt::AlignLeft : Qt::AlignHCenter ; // 0 колонку выравниваем влево, остальные по центру
    return Result ;
}

/*-------------------------------------------------------------------*/
//Получение координат редактирования, не нужна
//Item::Data *Model::dataDataBlock( const QModelIndex &I ) const {
//    int R = I.row() ;
//    if ( R < 0 || R >= Cat.size() ) return nullptr ;
//    return Cat[ R ] ;
//}
/*-------------------------------------------------------------------*/

QVariant Model::dataFont(const QModelIndex &I) const {
    DATA_PTR( D, I, QVariant() ) ; // Макрос
    //Item::Data *D = dataDataBlock(I) ; // До макроса
    //if( !D ) return QVariant() ; //Использовать значение по умолчанию
    QFont F ; // Шрифт
    if( D->Deleted ) F.setStrikeOut( true ) ; // Сделать зачёркнутым
    if( D->Changed ) F.setItalic( true ) ; // Выделение изменённых курсивом
    return F ;
}

QVariant Model::dataForeground(const QModelIndex &I) const { // Цвет шрифта
    DATA_PTR( D, I, QVariant() ) ; // Макрос
    //Item::Data *D = dataDataBlock(I) ; // До макроса
    //if( ! D ) return QVariant() ; //Использовать значение по умолчанию
    QColor Result = D->IsLocal ? QColor( "blue" ) : QColor( "black" ) ;
    if( ! D->isActive() ) Result.setAlphaF( 1.0/3.0) ; // Прозрачность
    return Result ;
}

QVariant Model::dataBackground(const QModelIndex &I) const { // Цвет шрифта
    DATA_PTR( D, I, QVariant() ) ; // Макрос
    if( !D->isNew() ) return QVariant() ;
    QColor Result = QColor( "green" ) ;
    Result.setAlphaF( 1.0/10.0) ; // Прозрачность
    return Result ;
}

QVariant Model::dataToolTipe( const QModelIndex &I) const { // Всплывающая подсказка
    DATA_PTR( D, I, QVariant() ) ; // Макрос
    //Item::Data *D = dataDataBlock(I) ; До макроса
    //if( ! D ) return QVariant() ; // Если пустой, использовать значение по умолчанию

    // Всплывающая подскаска на вторую колонку
//    switch( I.column() ) {
//    case 2 : {
//        if( ! D->To.isValid() ) return QVariant() ; // Срок действия никогда не истекает
//        return tr("Valid to: %1").arg(D->To.toString("dd.MM.yyyy") ) ; // Возвращаем отформатированную дату закрытия
//    }
//    default: return QVariant() ;
// }
    // Всплывающая подскаска на вторую колонку

    if( ! D->To.isValid() ) return QVariant() ; // Срок действия никогда не истекает
    return tr("Valid to: %1").arg(D->To.toString("dd.MM.yyyy") ) ; // Возвращаем отформатированную дату закрытия

}

/*----------Функция data() Возвращает данные данного элемента--------*/
QVariant Model::data( const QModelIndex &I, int role ) const {
    switch ( role ) {
    case Qt::DisplayRole       : return dataDisplay(I) ;
    case Qt::TextAlignmentRole : return dataTextAlignment(I) ;
    case Qt::ForegroundRole    : return dataForeground(I) ; // Для выделения цветом
    case Qt::FontRole          : return dataFont(I) ; // Для выделения зачёркиванием текста
    case Qt::ToolTipRole       : return dataToolTipe(I) ; // Для всплывающей подсказки
    case Qt::BackgroundRole    : return dataBackground(I) ; // Для цвета фона
    //case Qt::UserRole          : return QVariant( dataDataBlock(I) ) ;
    // Возвращает указатель на соответствующий DataBlock, но передавать в конструктор указатель нельзя, закомментировано
    case Qt::UserRole+1        : {
        DATA_PTR(D, I, false ) ; //Макрос
        //Item::Data *D = dataDataBlock(I) ; До макроса
        //if ( !D ) return false ; // Элемент удалён
        return D->Deleted ; // Иначе Deleted
    }
    default : return QVariant() ;
    }
}

QVariant Model::headerData( int section,
                            Qt::Orientation orientation, int role ) const {
    if ( orientation != Qt::Horizontal )
        return QAbstractItemModel::headerData( section, orientation, role ) ; // Данные по умолчанию, не пишем своих

    switch ( role ) {

    case Qt::DisplayRole :
        switch ( section) {
        case 1 : return tr("Code") ;
        case 0 : return tr("Title") ;
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
//Функция Index

QModelIndex Model::index( int row, int column,
                          const QModelIndex &parent ) const {
    //qDebug() << row << column << parent ;
    if( parent.isValid() ) { // D - ссылка на родителя
        DATA_PTR( D, parent, QModelIndex() ) ; // Макрос нахождения родителя, если парент не содержит блока D, возвращаем пустой QModelIndex
        if( row < 0 || row >= D->Children.size() ) return QModelIndex() ; // Если элемента не существует
        return createIndex( row, column, static_cast<void*>( D->Children[row] ) ) ; // строка, столбец, указатель текущий блок данных, куда приписан элемент
        //return QModelIndex() ; // Возвращаем пока пустой
    } else {
        if ( row < 0 ||row >= Cat.size() ) return QModelIndex() ; // Если элемента не существует
        return createIndex( row, column, static_cast<void*>(Cat[row]) ) ; // Возвращаем индекс. строка, столбец, указатель на блок данных
    }
}

/*-------------------------------------------------------------------*/

QModelIndex Model::parent( const QModelIndex &I ) const {
    DATA_PTR( D, I, QModelIndex() ) ;
    Item::Data *P = D->pParentItem ;
    if ( ! P )  //Если родитель отсутствует
        return QModelIndex() ; // Индекс родителя пуст
    // Если родитель присутствует, продолжаем
    int Row = -1 ;
    Item::Data *GP = P->pParentItem ; // Указатель на дедушку
    if( GP ) {
        for( int i = 0; i < GP->Children.size() ; i++ ) {
            if( GP->Children[i]->isSameAs( P ) ) { // Если это один и тот же элемент
                Row = i ; // Элемент найден (ищем индекс родителя в контексте дедушки)
              break ;
            }
        }
    } else { // Если GP пустой, то дедушка отсутствует
        for( int i = 0; i < Cat.size() ; i++ ) {
            if( Cat[i]->isSameAs( P ) ) { // Если это один и тот же элемент
                Row = i ; // Элемент найден (ищем индекс родителя в контексте дедушки)
                break ;
            }
        }
    }
    if( Row < 0  ) {
        qWarning() << "Cannot find item" ;
        return QModelIndex() ;
    }
    return createIndex( Row, 0, P ) ; // Если не нулевой
}

/*-------------------------------------------------------------------*/
// Редактирование (аргументы - какой параметр редактировать, на каком виджете показывать)

void Model::editItem( const QModelIndex &I, QWidget *parent ) {
    DATA_PTR(D,I,) ; // Макрос
    Item::Dialog Dia ( parent ) ;
    //Item::Data *D = static_cast<Item::Data*>(I.internalPointer()) ; // Указатель на правильный блок, до макроса
    // или Item::Data *D = dataDataBlock( I ) ;
    //if ( !D ) return ; // Если блока данных нет, то редактировать нечего
    Dia.setDataBlock( D ) ;
    beginResetModel() ; // Блокировка запроса изменения модели
    Dia.exec() ;
    endResetModel() ; // Разрешение запроса изменения модели
}

/*-------------------------------------------------------------------*/
// Добавление нового элемента (аргументы - какой родитель, на каком виджете показывать)
// &parentI - индекс родительского элемента, к которому добавляем новый элемент

void Model::newItem( const QModelIndex &parentI, QWidget *parent ) {
//    if ( parentI.isValid() ) {
//        // TODO: Сделать добавление нового элемента необязательно в корень каталога
//        qWarning() << "Cannot add non-toplevel item" ;
//        return ;
//    }
    Item::Data *P = nullptr ;
    if ( parentI.isValid() ) {
        P = static_cast<Item::Data*>(parentI.internalPointer()) ;
        if( !P ) {
            qWarning() << "Invalid internal pointer" ;
            return ;
        }
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
        if( P ) {
            P->Children.append( D ) ; // Добавляем элемент в список дочерних
            D->pParentItem = P ; // Проставляем указатель на Родителя
        } else {
            Cat.append( D ) ;
        }
        D->setProperty( "temp_id", tempId() ) ; // Зададим динамическое свойство временный ID
        endResetModel() ; // Разрешение запроса изменения модели
    } else {
        delete D ; // !!!!если пользователь нажал на cansel, вновь созданный объект надо убить
    }
}

/*-------------------------------------------------------------------*/
// Удаление (аргументы - какой параметр редактировать, на каком виджете показывать)

void Model::delItem( const QModelIndex &I, QWidget *parent ) {
    //TODO Спросить у пользователя, уверен ли он, что хочет удалить элемент
    DATA_PTR(D,I,) ; //Макрос
    //Item::Data *D = dataDataBlock( I ) ; // Получаем блок данных , до макроса
    //if ( !D ) return ; // Если блока данных нет, то удалять нечего
    //TODO Исходим из того , что модель линейна
    beginResetModel() ; // Блокировка запроса изменения модели
    //if ( D->Id.isNull() || ! D->Id.isValid() ) { // Если элемент новый
    if ( D->isNew() ) { // Если элемент новый
        Item::Data *P = D->pParentItem ; // чему равен указатель на родителя
        if( P ) {
            P->Children.removeAt( I.row() ) ;
        } else {
            Cat.removeAt( I.row() ) ; // Удаляем строку модели
        }
        delete D ; // Удаляем данные строки
    } else {
        D->Deleted = ! D->Deleted ; // Помечен на удаление или не помечен (инверсия true or false)
    }
    endResetModel() ; // Разрешение запроса изменения модели
}

/*-------------------------------------------------------------------*/

bool Model::delete_all_from_db( Item::Data *D ) { // Удаление D и его дочерних элементов
    Item::List *Children = D ? &(D->Children) : &Cat ; // если нет дочерних Children равен указателю на Cat
    Item::Data *X ;
    foreach( X, *Children) {
        if( ! delete_all_from_db( X ) ) // Удалили все дочерние элементы
        return false ; // Если не удалили
    }
    if( ! D ) return true ; // Если D не указано, то удалять нечего
    if( ! D->Deleted ) return true ; // Если D не помечен для удаления, тоне удалять
    {
        QSqlQuery DEL ;
        DEL.setForwardOnly(true) ;
        DEL.prepare( "delete from catalogue where iid = :IID ;" ) ;
        DEL.bindValue( ":IID", D->Id ) ;
        if (DEL.exec()) return true;
        qCritical() << DEL.lastError().databaseText() ; // Если не можем записать
        qCritical() << DEL.lastError().driverText() ;
        qCritical() << DEL.lastError().nativeErrorCode() ;
    }
    return false ;
}

bool Model::delete_all_from_model(Item::Data *D) {
    Item::List *Children = D ? &(D->Children) : &Cat ;
    bool Result = true ;
    beginResetModel() ; // Блокировка запроса изменения модели
    for( int k = Children->size() - 1 ; k >= 0 ; k-- ) { // Перебираем список с конца в начало
        if( Children->at(k)->Deleted ) { // Удалили все дочерние элементы
            Item::Data *X = Children->takeAt(k) ; // Удаляем элемент из списка и возвращает его
            delete X ; // Удалили
        } else {
            if( ! delete_all_from_model( Children->at(k))) { // Удаляем все
                Result = false ;
                break ; // Если не удалены
            }
        }
    }
    endResetModel() ; // Разрешение запроса изменения модели
    return Result ;
}

bool Model::delete_all() {
    QSqlDatabase DB = QSqlDatabase::database() ; // Доступ в дескриптор база данных по умолчанию (верхней)
    DB.transaction() ; // Транзакция по базе
    if( delete_all_from_db() ) {
        DB.commit() ; // Сохранили транзакцию
        return delete_all_from_model() ;
    } else {
        DB.rollback() ; // Откат транзакции, если хотябы один элемент удалить не удалось
        return false ;
    }
}

/*-------------------------------------------------------------------*/

bool Model::save_all_to_db(Item::Data *D ) { //Сохранение изменённых в базу
    Item::List *Children = D ? &(D->Children) : &Cat ; // если нет дочерних Children равен указателю на Cat
    Item::Data *X ;
    foreach( X, *Children) { // Пробегаемся по списку
        if( ! save_all_to_db( X ) ) // Сохраняем все дочерние элементы
        return false ; // Если не сохранили
    }
    if( ! D ) return true ; // Если D не указано, то сохранять нечего
    if( ! D->Changed ) return true ; // Если D не помечен как изменённый, то не сохраняем
     {
        QSqlQuery UPD ;
        UPD.setForwardOnly(true) ;
        UPD.prepare(
            "update catalogue set       \n"
            "   code       =:CODE,      \n"
            "   title      =:TITLE,     \n"
            "   valid_from =:FROM,      \n"
            "   valid_to   =:TO,        \n"
            "   islocal    =:LOCAL,     \n"
            "   acomment   =:COMMENT    \n"
            "where iid =:IID    ;\n"
        ) ;
        UPD.bindValue( ":CODE"     , D->Code   ) ;
        UPD.bindValue( ":TITLE"    , D->Title  ) ;
        UPD.bindValue( ":FROM"     , D->From    ) ;
        UPD.bindValue( ":TO"       , D->To      ) ;
        UPD.bindValue( ":LOCAL"    , D->IsLocal ) ;
        UPD.bindValue( ":COMMENT"  , D->Comment ) ;
        UPD.bindValue( ":IID"      , D->Id      ) ;
        if (UPD.exec()) return true;
        qCritical() << UPD.lastError().databaseText() ; // Если не можем записать
        qCritical() << UPD.lastError().driverText() ;
        qCritical() << UPD.lastError().nativeErrorCode() ;
    }
    return false ;
}

/*-------------------------------------------------------------------*/

bool Model::drop_changed_mark( Item::Data *D ) { // Сброс метки на изменение
    Item::List *Children = D ? &(D->Children) : &Cat ; // если нет дочерних Children равен указателю на Cat
    Item::Data *X ;
    foreach( X, *Children) { // Пробегаемся по списку
        X->Changed = false ;
        drop_changed_mark( X ) ; //Сбрасываем метку изменения
    }
    return true ;
}

/*-------------------------------------------------------------------*/

bool Model::save_all  () {
    QSqlDatabase DB = QSqlDatabase::database() ; // Доступ в дескриптор база данных по умолчанию (верхней)
    DB.transaction() ; // Транзакция по базе
    if( save_all_to_db() ) {
        DB.commit() ; // Сохранили транзакцию
        return drop_changed_mark() ;
    } else {
        DB.rollback() ; // Откат транзакции, если хотябы один элемент сохранить не удалось
        return false ;
    }
}

/*-------------------------------------------------------------------*/
 // Сохраняет в базу все новые элементы
bool Model::insert_all_to_db( Item::Data *D ) {
    bool must_be_save = D ? D->isNew() : false ;
    if( must_be_save ) {
        QSqlQuery INS ;
        INS.setForwardOnly(true) ;
        INS.prepare(
            "insert into catalogue (          \n"
            "      code ,        title ,       \n"
            "      valid_from ,  valid_to ,    \n"
            "      islocal ,     acomment ,    \n"
            "      rid_parent                  \n"
            ") values (                        \n"
            "      :CODE ,     :TITLE ,        \n"
            "      :FROM ,     :TO ,           \n"
            "      :LOCAL ,    :COMMENT ,      \n"
            "      :PARENT                     \n"
            ") returning iid,                  \n"
            "      code ,        title ,       \n"
            "      valid_from ,  valid_to ,    \n"
            "      islocal ,     acomment ,    \n"
            "      rid_parent,   alevel      ; \n"
        ) ;
        //                      alevel  - уровень иерархии сведствами базы данных
        INS.bindValue( ":CODE"     , D->Code    ) ;
        INS.bindValue( ":TITLE"    , D->Title   ) ;
        INS.bindValue( ":FROM"     , D->From    ) ;
        INS.bindValue( ":TO"       , D->To      ) ;
        INS.bindValue( ":LOCAL"    , D->IsLocal ) ;
        INS.bindValue( ":COMMENT"  , D->Comment ) ;
        QVariant IdParent = QVariant() ;
        if( D->pParentItem ) {
            if( D->pParentItem->isNew() )
                // Если родитель новый, то ID ещё нет, появляется когда все элементы сохранены
                IdParent = D->pParentItem->property( "new_id" ) ;
            else
                IdParent = D->pParentItem->Id ;
        }

        INS.bindValue( ":PARENT"   , IdParent   ) ;
        if ( ! INS.exec() ) {
            qCritical() << INS.lastError().databaseText().toUtf8().data() ; // Если не можем записать
            qCritical() << INS.lastError().driverText() ;
            qCritical() << INS.lastError().nativeErrorCode() ;
            return false ;
        }
        while ( INS.next() ) { // Считываем, что вставилось в базу
            D->Code    = INS.value( "code"       ).toString()   ;
            D->Title   = INS.value( "title"      ).toString()   ;
            D->From    = INS.value( "valid_from" ).toDateTime() ;
            D->To      = INS.value( "valid_to"   ).toDateTime() ;
            D->IsLocal = INS.value( "islocal"    ).toBool()     ;
            D->Comment = INS.value( "acomment"   ).toString()   ;
            qDebug() << INS.value("iid").isNull()
                     << INS.value( "rid_parent" ).isNull()
                     << INS.value("alevel").isNull() ;
            qDebug() << INS.value("iid")
                     << INS.value( "rid_parent" )
                     << INS.value("alevel") ;
            qDebug() <<  INS.value( "rid_parent" )
                     << INS.value("alevel") ;
            D->setProperty( "new_id", INS.value("iid") ) ; // Запишем ID во временное свойство
            qDebug() << D->property("new_id") ;
        }
    }{ // Пробегаемся по дочерним
        Item::List *Children = D ? &(D->Children) : &Cat ; // если нет дочерних Children равен указателю на Cat
        Item::Data *X ;
        foreach( X, *Children) { // Пробегаемся по списку
            if( ! insert_all_to_db( X ) ) // Сохраняем все дочерние элементы
            return false ; // Если не сохранили
        }
    }
    return true ; // Если всё в порядке
}

/*-------------------------------------------------------------------*/

bool Model::adjust_id_for_new(Item::Data *D ) { // Прописываем ID новым элементам
    bool must_be_save = D ? D->isNew() : false ;
    if( must_be_save ) {
        D->Id = D->property("new_id") ; // new_id - временный ID
    }
    Item::List *Children = D ? &(D->Children) : &Cat ; // если нет дочерних Children равен указателю на Cat
    Item::Data *X ;
    foreach( X, *Children) { // Пробегаемся по списку дочерних элеменов
        if( ! adjust_id_for_new( X ) ) // Сохраняем все дочерние элементы
        return false ; // Если не сохранили
    }
    // qDebug() << "Model::adjust_id_for_new" ;
    return true ;
} // Выставляет правильный ID новому элементу

/*-------------------------------------------------------------------*/

bool Model::insert_all() {
    QSqlDatabase DB = QSqlDatabase::database() ; // Доступ в дескриптор база данных по умолчанию (верхней)
    DB.transaction() ; // Транзакция по базе
    if( insert_all_to_db() ) {
        DB.commit() ; // Сохранили транзакцию
        return adjust_id_for_new() ;
    } else {
        DB.rollback() ; // Откат транзакции, если хотябы один элемент сохранить не удалось
        return false ;
    }
}

/*-------------------------------------------------------------------*/

void Model::save( void ) {
    try{ // Локальное сключение, которое перехватывается в той же функции, в которой выброшено
        if( ! delete_all() ) throw static_cast<int>(1) ; // Удаляем или выкидываем исключение
        if( ! save_all()   ) throw static_cast<int>(2) ; // Изменяем или выкидываем исключение
        if( ! insert_all() ) throw static_cast<int>(3) ; // Добавляем или выкидываем исключение
        QMessageBox::information( nullptr, tr("Info"),tr("Changes save succesfully")) ;
        //qDebug() << "Model::save()" ;
        // Удалить все элементы, помеченные для удаления
        // Сохранить все изменённые элементы
        // Сохранить все новые элементы
        // Сбросить со всех элементов пометку "Изменено"
    } catch ( int ) {
        QMessageBox::critical( nullptr ,tr("Error"),tr("Cannot save changes") ) ;
    }
}

/*********************************************************************/

TableView::TableView( QWidget * parent, Model *xModel )
    : QTableView( parent ) {

    //Редактирование данных в контекстном меню
    setContextMenuPolicy( Qt::CustomContextMenu) ;
    connect( this, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(contextMenuRequested(QPoint)) ) ;

    Model *M = xModel ? xModel : new Model( this ); // Если модель не задана, то создаём новую
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
        PosAction *A = actRootItem = new PosAction( this ) ; // Установка элемента корневым (шаг на ступеньку вниз)
        A->setText( tr("Show Children") ) ;
        connect( A, SIGNAL(editItem(QModelIndex, QWidget*)),
                 this, SLOT(showChildren(QModelIndex, QWidget*)) ) ;
        addAction( A ) ;
    }

    {
        QAction *A = actParentRootItem = new QAction( this ) ; // Установка элемента (шаг на ступеньку вверх)
        A->setText( tr("Show Parent") ) ;
        connect( A, SIGNAL(triggered()), this, SLOT(showParent()) ) ;
        addAction( A ) ;
    }

    {
        QAction *A = actSave = new QAction( this ) ; // Сохранение изменёных и добавленных элементов в базу
        A->setText( tr("Save") ) ;
        connect( A, SIGNAL(triggered()), M, SLOT(save()) ) ;
        addAction( A ) ;
    }

    {
        QHeaderView *H = verticalHeader() ;
        H->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
    }{
        QHeaderView *H = horizontalHeader() ;
        H->setSectionResizeMode( QHeaderView::ResizeToContents ) ; // Для всех
        H->setSectionResizeMode(0, QHeaderView::Stretch ) ; // Для колонки Title
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
    //actNewItem->I = QModelIndex() ; // родительский элемент всегда невалидный
    actNewItem->I = rootIndex() ;
    actNewItem->pWidget = this ;
    M.addAction( actNewItem ) ;
    if ( I.isValid() ) { // Чтобы не кликалось на пустом месте
        actRootItem->I = I ;
        actRootItem->pWidget = this ;
        M.addAction( actRootItem ) ;
    }
    if( rootIndex().isValid() )
        M.addAction( actParentRootItem ) ;
    M.addAction( actSave ) ;
    M.exec( mapToGlobal( p ) ) ;
}

/*-------------------------------------------------------------------*/
//Слот показывать Children
void TableView::showChildren( const QModelIndex &I, QWidget* ) {
    setRootIndex( I ) ;
}

/*-------------------------------------------------------------------*/
//Слот скрывать Children
void TableView::showParent( void ) {
    if( rootIndex().isValid() )
    setRootIndex( rootIndex().parent() ) ;
}

/*********************************************************************/
// Древовидное представление
TreeView::TreeView( QWidget *parent, Model *xModel )
    : QTreeView( parent ) {

    Model *M = xModel ? xModel : new Model( this ); // Если модель не задана, то создаём новую
    setModel( M ) ;

    {
    QHeaderView *H = header() ;
    H->setSectionResizeMode( QHeaderView::ResizeToContents ) ; // Для всех
    H->setSectionResizeMode(1, QHeaderView::Stretch ) ; // Для первой колонки Title
    }

    setColumnHidden( 3, true ) ; // Скрываем колонку "To"
    setColumnHidden( 4, true ) ; // Скрываем колонку "Local"
//    setColumnHidden( 5, true ) ; // Скрываем колонку "Comment"
}

/*-------------------------------------------------------------------*/

TreeView::~TreeView() {
}

/*********************************************************************/
// Колоночное представление
ColumnView::ColumnView( QWidget *parent, Model *xModel )
    : QColumnView( parent ) {

    Model *M = xModel ? xModel : new Model( this ); // Если модель не задана, то создаём новую
    setModel( M ) ;

    QList<int> L ;
    L << 150 ;
    for ( int k =0 ; k < 10 ; k++ ) L << 200 ;
    setColumnWidths( L ) ;
}

/*-------------------------------------------------------------------*/

ColumnView::~ColumnView() {
}

/*********************************************************************/
} // namespace Catalogue
} // namespace STORE
