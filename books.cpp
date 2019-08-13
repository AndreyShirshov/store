#include "books.h"

#include <QHeaderView>

#include <QtSql>
#include <QtDebug>
#include <QAction>
#include <QComboBox>
#include <QPainter>

namespace  { // Запрет использования в других файлах (анонимное пространство имён)
void REPORT_ERROR(QSqlQuery &QUERY) {
    qDebug() << QUERY.executedQuery().toUtf8().data() ;
    qCritical() << QUERY.lastError().databaseText().toUtf8().data() ;
}
} // namespace

namespace STORE {
namespace Books {

/*********************************************************************/
// Класс делегата

StatusDelegate::StatusDelegate(QObject *parent, const QMap<int,QString> &AllStatus )
    : QItemDelegate ( parent ),fAllStatus(AllStatus) {

}

/*-------------------------------------------------------------------*/
//Переопределяем функцию редактора:
QWidget *StatusDelegate::createEditor( QWidget *parent,
                                       const QStyleOptionViewItem &,
                                       const QModelIndex & ) const {

    QComboBox *CB = new QComboBox( parent ) ;

    QMapIterator<int,QString> K(fAllStatus) ;
    CB->addItem( QString(), QVariant() ) ;
    while (K.hasNext()) {
        K.next();
        CB->addItem( K.value(), K.key() ) ;// наименование и идентификатор статуса
    }
    return CB ;

} ;

/*-------------------------------------------------------------------*/
// Переопределяем функцию перенос данных с модели на редактор:
void StatusDelegate::setEditorData( QWidget *editor,
                                       const QModelIndex &I ) const {
    QComboBox *CB = qobject_cast<QComboBox*>(editor) ; // Преобразовываем к ComboBox
    if ( ! CB ) return ;

    QVariant IdStatus = I.data(Qt::EditRole) ;

    if ( ! IdStatus.isValid() ) { // Если данные предназначены для редактирования не валидные (статуса не выставлено)
        CB->setCurrentIndex( 0 ) ; // Пустой элемент 0 по счёту
    return ;
    }

    for ( int k = 1 ; k < CB->count() ; k++ ) {
        if( CB->itemData(k) == IdStatus ) {
           CB->setCurrentIndex(k) ;
           break ;
        }
    }
}

/*-------------------------------------------------------------------*/
// Переопределяем функцию перенос данных с редактора на модель:
void StatusDelegate::setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                     const  QModelIndex &I ) const {

    QComboBox *CB = qobject_cast<QComboBox*>(editor) ; // Преобразовываем к ComboBox
    if ( ! CB ) return ;
    model->setData( I, CB->currentData(), Qt::EditRole ) ; // Передавать не текст, а значение

}

/*-------------------------------------------------------------------*/
// Переопределим функцию рисования paint для штриховки выданного:
    void StatusDelegate::paint( QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &I ) const {
        QItemDelegate::paint( painter, option, I ) ;
        if( I.data(Qt::EditRole) != -2 ) return ; // Если не в переплёте
        painter->setBrush( QBrush( QColor("black"), Qt::DiagCrossPattern ) ) ;
        painter->setPen( Qt::NoPen ) ; // Не отрисовывать по контуру
        painter->drawRect( option.rect ) ; //Отрисовка прямоугольника
}

/*********************************************************************/
// Модель
Model::Model( QObject *parent)
    : QSqlQueryModel ( parent ) {

}

/*-------------------------------------------------------------------*/

Model::~Model(){
}
/*-------------------------------------------------------------------*/

void Model::adjust_query() {
    QString QueryText =
        "select                             \n"
        "      b.iid,                       \n"
        "      b.rid_catalogue,             \n"
        "      b.author,                    \n"
        "      b.title,                     \n"
        "      b.eyear,                     \n"
        "      b.location,                  \n"
        "      b.publisher,                 \n"
        "      b.pages,                     \n"
        "      b.annote,                    \n"
        "      b.rid_status,                \n"
        "      s.title,                     \n"
        "      b.acomment                   \n"
        "    from books b                   \n"
        "    left outer join status s       \n"
        "    on b.rid_status = s.iid        \n"
        "    where b.rid_catalogue = :CID   \n" ;
    if ( fAuthor.isValid() )
        QueryText += "     and b.author ~ :AUTHOR \n" ;
    if ( fTitle.isValid() )
        QueryText += "     and b.title ~ :TITLE   \n" ;
    if ( fYear.isValid() )
        QueryText += "     and b.eyear = :YEAR    \n" ;

    QueryText += "; \n" ;

//    qDebug() << QueryText.toUtf8().data() ;

    QSqlQuery qry ;
    qry.prepare( QueryText ) ;
    qry.bindValue(":CID", fCatId ) ; // Если fCatId не валидный, то база данных воспримет его как 0

    if ( fAuthor.isValid() )
        qry.bindValue( ":AUTHOR", "^"+fAuthor.toString() ) ; // Фильтр
    if ( fTitle.isValid() )
        qry.bindValue( ":TITLE", fTitle ) ;
    if ( fYear.isValid() )
        qry.bindValue( ":YEAR", fYear ) ;

    if( ! qry.exec() ) { // Открыть запрос
//        qDebug() <<qry.executedQuery().toUtf8().data() ;
        qCritical() << qry.lastError().databaseText().toUtf8().data() ;
    }
    setQuery( qry ) ;
}

/*-------------------------------------------------------------------*/

void Model::cat_item_selected(QVariant Id ) {
    fCatId = Id ;
    adjust_query() ;
}

/*-------------------------------------------------------------------*/

void Model::apply_filter( QObject *F ) {
    fAuthor = F->property( "author" ) ;
    fTitle  = F->property( "title"  ) ;
    fYear   = F->property( "year"   ) ;
    adjust_query() ;
}

/*-------------------------------------------------------------------*/

//void Model::cat_item_selected( QVariant Id ) {
//    QSqlQuery qry ;
//    qry.prepare(
//        "select                             \n"
//        "      b.iid,                       \n"
//        "      b.rid_catalogue,             \n"
//        "      b.author,                    \n"
//        "      b.title,                     \n"
//        "      b.eyear,                     \n"
//        "      b.location,                  \n"
//        "      b.publisher,                 \n"
//        "      b.pages,                     \n"
//        "      b.annote,                    \n"
//        "	     b.rid_status,                \n"
//        "      s.title,                     \n"
//        "      b.acomment                   \n"
//        "    from books b                   \n"
//        "    left outer join status s       \n"
//        "    on b.rid_status = s.iid        \n"
//        "    where b.rid_catalogue = :CID ; \n"
//    ) ;
//    qry.bindValue(":CID", Id ) ; // Если Id не валидный, то база данных воспримет его как 0
//    if( ! qry.exec() ) { // Открыть запрос
//        qCritical() << qry.lastError().databaseText().toUtf8().data() ;
//    }
//    setQuery( qry ) ;
//}


/*********************************************************************/
// Модель
Model_EditOnServer::Model_EditOnServer( QObject *parent)
    : QSqlTableModel ( parent ) {

    setEditStrategy( OnFieldChange) ; // Выбраем стратегию редактирования временной таблици

    {
    QAction *A = actDeleteRow  = new QAction( this ) ;
    A->setText( tr("Delete") ) ;
    connect( A, SIGNAL(triggered()), this, SLOT(on_delete_row()) ) ;
    AllActions << A ;
    }{
    QAction *A = actNewRow  = new QAction( this ) ;
    A->setText( tr("New") ) ;
    connect( A, SIGNAL(triggered()), this, SLOT(on_new_row()) ) ;
    AllActions << A ;
    }{
    QAction *A = actSaveAll  = new QAction( this ) ;
    A->setText( tr("Save") ) ;
    connect( A, SIGNAL(triggered()), this, SLOT(on_save_all()) ) ;
    AllActions << A ;
    }{
    QAction *A = actRestoreAll  = new QAction( this ) ;
    A->setText( tr("Restore") ) ;
    connect( A, SIGNAL(triggered()), this, SLOT(on_restore_all()) ) ;
    AllActions << A ;
    }

    {
      QSqlQuery QUERY ; // для выгрузки всех статусов из базы в Map (справочник статусов)
      QUERY.prepare("select iid, title from status ;") ;
      bool OK = QUERY.exec() ;
      if( ! OK ) {
          REPORT_ERROR(QUERY) ;
          return ;
      }
      while( QUERY.next() ) {
          int Id = QUERY.value("iid").toInt() ;
          AllStatus[Id] = QUERY.value("title").toString() ;
      }
    }

}

/*-------------------------------------------------------------------*/

Model_EditOnServer::~Model_EditOnServer(){
}

/*-------------------------------------------------------------------*/

void Model_EditOnServer::on_save_all() {
    qDebug() << "on_save_all" ;
}

/*-------------------------------------------------------------------*/
void Model_EditOnServer::on_restore_all() {
    qDebug() << "on_restore_all" ;
}

/*-------------------------------------------------------------------*/

void Model_EditOnServer::on_delete_row() {
    qDebug() << "on_delete_row" ;
}

/*-------------------------------------------------------------------*/

void Model_EditOnServer::on_new_row() {
    insertRow( 0 ) ; // Parent = 0, по структуре аналогично представлению
    setData( index( 0, 1 ), fCatId, Qt::EditRole ) ; // Добавляем каталожный элемент rid_catalogue
    qDebug() << "on_new_row" ;
}

/*-------------------------------------------------------------------*/

void Model_EditOnServer::adjust_query() {

    setTable( QString() ) ; // Отключим таблицу от модели

    {
     QSqlQuery DROP ; // Создадим запрос для удаления временной таблицы, если она была
     bool OK = DROP.exec( "drop table if exists my_books ;" ) ;
     if ( ! OK ) {
         REPORT_ERROR( DROP ) ;
         return ;
     }
    }{
     QSqlQuery CREATE ; // Создадим временную таблицу вручную
     bool OK = CREATE.exec (
         "create temporary table my_books (\n"
         "    iid bigint primary key,      \n"
         "    rid_catalogue bigint,        \n"
         "    author text,                 \n"
         "    title text,                  \n"
         "    eyear int,                   \n"
         "    location text,               \n"
         "    publisher text,              \n"
         "    pages int,                   \n"
         "    annote text,                 \n"
         "    rid_status bigint,           \n"
         "    status_title text,           \n"
         "    acomment text                \n"
         ") ;                              \n"
     ) ;
     if( ! OK ) {
         REPORT_ERROR( CREATE ) ;
         return ;
     }
    }{
     QString QueryText =// Вставим во временную таблицу
//         "create temporary table my_books as \n"
         "insert into my_books (              \n"
         "    iid,                           \n"
         "    rid_catalogue,                 \n"
         "    author,                        \n"
         "    title,                         \n"
         "    eyear,                         \n"
         "    location,                      \n"
         "    publisher,                     \n"
         "    pages,                         \n"
         "    annote,                        \n"
         "    rid_status,                    \n"
         "    status_title,                  \n"
         "    acomment                       \n"
         ")                                  \n"
         "select                             \n"
         "      b.iid,                       \n"
         "      b.rid_catalogue,             \n"
         "      b.author,                    \n"
         "      b.title,                     \n"
         "      b.eyear,                     \n"
         "      b.location,                  \n"
         "      b.publisher,                 \n"
         "      b.pages,                     \n"
         "      b.annote,                    \n"
         "      b.rid_status,                \n"
         "      s.title,                     \n"
         "      b.acomment                   \n"
         "    from books b                   \n"
         "    left outer join status s       \n"
         "    on b.rid_status = s.iid        \n"
         "    where b.rid_catalogue = :CID   \n" ;
     if ( fAuthor.isValid() )
         QueryText += "     and b.author ~ :AUTHOR \n" ;
     if ( fTitle.isValid() )
         QueryText += "     and b.title ~ :TITLE   \n" ;
     if ( fYear.isValid() )
         QueryText += "     and b.eyear = :YEAR    \n" ;
     QueryText += "; \n" ;

     QSqlQuery qry ;
     qry.prepare( QueryText ) ;
     qry.bindValue(":CID", fCatId ) ; // Если fCatId не валидный, то база данных воспримет его как 0
     if ( fAuthor.isValid() )
         qry.bindValue( ":AUTHOR", "^"+fAuthor.toString() ) ; // Фильтр
     if ( fTitle.isValid() )
         qry.bindValue( ":TITLE", fTitle ) ;
     if ( fYear.isValid() )
         qry.bindValue( ":YEAR", fYear ) ;
     if( ! qry.exec() ) { // Открыть запрос
         REPORT_ERROR( qry ) ;
         return ;
     }

     } // Конец блока select

//    setQuery( qry ) ;
    setTable("my_books") ;
    if ( ! select() ) {
        qCritical() << "Error selecting" ;
    } else {
        qDebug() << "Selected successfully" ;
    }
}

/*-------------------------------------------------------------------*/

void Model_EditOnServer::cat_item_selected(QVariant Id ) {
    fCatId = Id ;
    qDebug() << "cat_item_selected" << fCatId ;
    adjust_query() ;
}

/*-------------------------------------------------------------------*/

void Model_EditOnServer::apply_filter( QObject *F ) {
    fAuthor = F->property( "author" ) ;
    fTitle  = F->property( "title"  ) ;
    fYear   = F->property( "year"   ) ;
    qDebug() << "apply_filter" << fAuthor << fTitle << fYear ;
//    adjust_query() ;
}

/*-------------------------------------------------------------------*/
// Подменим функцию data

QVariant Model_EditOnServer::data( const QModelIndex &I, int role ) const {
    if ( ! I.isValid() ) return QSqlTableModel::data( I, role ) ;
    if( role != Qt::EditRole ) return QSqlTableModel::data( I, role ) ;
    if( I.column() != 10 ) return QSqlTableModel::data( I, role ) ;
    return  QSqlTableModel::data( index(I.row(),9), role ) ;

}

/*-------------------------------------------------------------------*/
// Подменим функцию setData

bool Model_EditOnServer::setData( const QModelIndex &I, const QVariant &val, int role ) {
    if ( ! I.isValid() ) return QSqlTableModel::setData( I, val, role ) ;
    if( role != Qt::EditRole ) return QSqlTableModel::setData( I, val, role ) ;
    if( I.column() != 10 ) return QSqlTableModel::setData( I, val, role ) ;
    bool Result = true ;
// Установили Id status
    if ( val.isValid() ) {
        bool OK ;
        int status_id =val.toInt( &OK ) ;
        if (! OK ) { // Если число статуса некорректное
            qWarning() << "Invalid status" << val ;
            return false ;
        } else if ( ! AllStatus.contains(status_id) ){
            qWarning() << "Invalid status" << val ;
            return false ;
        }
        Result |= QSqlTableModel::setData( index(I.row(),9), val, role ) ;
        Result |= QSqlTableModel::setData( I, AllStatus[status_id], role ) ; // Установка статуса
        ///qDebug() << val ;
    } else {
        Result |= QSqlTableModel::setData( index(I.row(),9), QVariant(), role ) ; // Установка пустого статуса
        Result |= QSqlTableModel::setData( I, QString(), role ) ;
    }
    return Result ;
}

/*-------------------------------------------------------------------*/

Qt::ItemFlags Model_EditOnServer::flags( const QModelIndex &I ) const {
    Qt::ItemFlags Result = Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
//    if( I.column() != 0 && I.column() != 9 && I.column() != 10 ) // Запрет редактирования
    if( I.column() != 0  )
        Result |= Qt::ItemIsEditable ;
    return Result ;
}

/*-------------------------------------------------------------------*/

// Представление
View::View( QWidget *parent )
    : QTableView( parent ) {

//    Model *M = new Model( this ) ;
    Model_EditOnServer *M = new Model_EditOnServer( this ) ;
    setModel( M ) ;

    addActions( M->AllActions ) ; // Добавляем Actions на представление
    setContextMenuPolicy( Qt::ActionsContextMenu ) ;

    setColumnHidden( 0, true ) ; // iid никогда не показываем пользователю
    setColumnHidden( 1, true ) ;
//    setColumnHidden( 9, true ) ;

    setWordWrap( false ) ; // запрет разбития текста на несколько строк
    setAlternatingRowColors( true ) ; // Попеременный цвет строк

    {
        QHeaderView *H = verticalHeader() ;
        H->setSectionResizeMode( QHeaderView::ResizeToContents ) ; // Порядок изменения размеров строк
    }{
        QHeaderView *H =horizontalHeader() ;
        H->setSectionResizeMode( QHeaderView::ResizeToContents ) ; // Для всех
        H->setSectionResizeMode( 3, QHeaderView::Stretch ) ; //Заголовок растянуть
    }

    // Устанавливаем делегат для колонки 10 на представление:
    // Указываем словарь статусов M:
    setItemDelegateForColumn( 10 , new StatusDelegate(this, M->AllStatus) ) ;

}

/*-------------------------------------------------------------------*/

View::~View() {
}

/*********************************************************************/

} // namespace Books
} // namespace STORE
