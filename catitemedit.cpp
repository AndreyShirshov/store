#include "catitemedit.h"

#include <QtSql>

namespace STORE {
namespace Catalogue {
namespace Item {

/*********************************************************************/
// Задаём идентификатор элемента, возвращаем указатель
//(по идентификатору находим поинтер, для распределения элементов по родителям)
Data *List::findPointer( int Id ) const {
    Data *D ; // объявляем переменную
    foreach( D, *this ) { // Перебор всех элементов списка
        bool OK ;
        int cId = D->Id.toInt(&OK) ; // Потому что QVariant (ciD - элемент просматриваемый)
        if( OK && cId == Id ) return D ; // Убедились, что ID целое число
        Data *R = D->Children.findPointer( Id ) ;
        if ( R ) return R ; // Если R найден, возвращаем
    }
    return nullptr ; // Усли нигего не нашли, возвращаем 0
}

/*********************************************************************/
Data::Data( QObject *parent , QSqlQuery &qry )
    : QObject ( parent ) {

    Id      = qry.value( "iid"        ) ;
    Code    = qry.value( "code"       ).toString() ; // Код подраздела
    Title   = qry.value( "title"      ).toString() ; // Наименование
    From    = qry.value( "valid_from" ).toDateTime() ; // Действует с .... (дата)
    To      = qry.value( "valid_to"   ).toDateTime() ; // Закрыт с .... (дата)
    IsLocal = qry.value( "islocal"    ).toBool() ; // локальный
    Comment = qry.value( "acomment"   ).toString() ; // Комментарий
    pParentItem  = nullptr ; // Родительский подраздел, пока будет равен нулю
    Deleted = false ; // Помечен на удаление
    Changed = false ; // Данные подвергались редактированию

//    "      rid_parent,       \n"
//    "      alevel,           \n" // Уровенть иерархии
}

/***********Проверка активный не активный элемент*********************/
bool Data::isActive() const {
    if( From.isValid() ) {
        if (From > QDateTime::currentDateTime() )
            return false ;
    }
    if( To.isValid() ) {
        if( To < QDateTime::currentDateTime() )
            return false ;
    }
    return true ;
}

/*********************************************************************/
// Функция отличия нового элемента от старого
bool Data::isNew()const {
    if( ! Id.isValid() ) return true ;
    if ( Id.isNull() ) return true ;
    return false ;
}
/*********************************************************************/
// Функция возвращает thrue , если данный оъект тот же самый что и указатель на D
// Проверяет, являются ли два элемента Data одинаковыми
bool Data::isSameAs(Data *D) const {
    if( isNew() ) {
        if( ! D->isNew() ) return false ; // Если один новый
        return property("temp_id") == D->property("temp_id") ;
    } else {
        if( D->isNew() ) return false ; // Если один новый
        return D->Id == Id ;
    }
}
/*********************************************************************/
Frame::Frame( QWidget *parent )
    : QFrame( parent ) {

    ui.setupUi(this) ;
    Block = nullptr ; // По умолчанию
}

Frame::~Frame() {
}

/** TODO: Тут сделать нормальную проверку данных */
void Frame::is_good(bool *pOK ) {
    *pOK = true ;
}

/*-------------------------------------------------------------------*/
// Перенос с данных на диалог
void Frame::load() {
    if (! Block ) return ;
    ui.edtCode->setText( Block->Code ) ; // Код подраздела
    ui.edtTitle->setText( Block->Title ) ; // Наименование
    ui.cbxLocal->setChecked( Block->IsLocal ) ; // локальный
    ui.edtComment->setText( Block->Comment ) ; // Комментарий
    ui.cbxFromEnabled->setChecked( Block->From.isValid() ) ; //Редактируем  From
    ui.edtFrom->setDateTime(Block->From) ; // Действует с .... (дата)
    ui.cbxToEnabled->setChecked( Block->To.isValid() ) ; // Редактируем То
    ui.edtTo->setDateTime( Block->To ) ; // Закрыт с .... (дата)

}

/*-------------------------------------------------------------------*/
// Перенос с диалога на данные, bool - действительно ли сохранились данные
bool Frame::save() {

    if( ! Block ) return false ; // проверка делается в CommonDialog::accept_pressed()
    Block->Code = ui.edtCode->text().simplified() ; //концевые пробелы отбросить
    Block->Title = ui.edtTitle->text().simplified() ;
    Block->IsLocal = ui.cbxLocal->isChecked() ;
    Block->Comment  = ui.edtComment->toPlainText().trimmed() ; //концевые пробелы отбросить
    if (ui.cbxFromEnabled->isChecked() ) {
        Block->From = ui.edtFrom->dateTime() ;
    } else {
        Block->From = QDateTime() ;
    }
    if (ui.cbxToEnabled->isChecked() ) {
        Block->To = ui.edtTo->dateTime() ;
    } else {
        Block->To = QDateTime() ;
    }
    Block->Changed = true ; // Данные отредактированы
    return true ;
}

/*********************************************************************/

Dialog::Dialog( QWidget *parent)
    : CommonDialog ( parent ) {

        pFrame = new Frame( this ) ;
        setCentralFrame( pFrame ) ;
}

Dialog::~Dialog() {
}

/*********************************************************************/

} // namespace Item
} // namespace Catalogue
} // namespace STORE
