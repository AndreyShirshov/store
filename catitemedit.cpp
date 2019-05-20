#include "catitemedit.h"

namespace STORE {
namespace Catalogue {
namespace Item {

/*********************************************************************/

Frame::Frame( QWidget *parent )
    : QFrame( parent ) {

    ui.setupUi(this) ;
    Block = 0 ; // По умолчанию
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
