#include "dialogtpl.h"

#include <QLayout>

#include <QMessageBox>

namespace STORE {

/**************************************************************/

ButtonsDlg::ButtonsDlg( QWidget *parent )
    : QFrame(parent) {

    ui.setupUi( this ) ;

    connect( ui.btnOK, SIGNAL(clicked()), this, SIGNAL(accepted()) ) ;
    connect( ui.btnCancel, SIGNAL(clicked()), this, SIGNAL(rejected()) ) ;

}

ButtonsDlg::~ButtonsDlg(){
}

/**************************************************************/

void ButtonsDlg::error_message( const QString & msg ) {
    ui.lblErrorMsg->setText( msg ) ;
}

/**************************************************************/

CommonDialog::CommonDialog( QWidget *parent )
    : QDialog (parent ) {

    QVBoxLayout *L = new QVBoxLayout( this ) ;

    {
        ButtonsDlg *B = pButtons = new ButtonsDlg( this ) ;
        connect( B, SIGNAL(rejected()), this, SLOT(reject()) ) ;
        connect( B, SIGNAL(accepted()), this, SLOT(accept_pressed()) ) ;
        L->addWidget( B ) ;
    }

}

CommonDialog::~CommonDialog() {
}

void CommonDialog::setCentralFrame(QFrame *central) {

    QVBoxLayout *L = qobject_cast<QVBoxLayout*>( layout()) ; // метод layout выдёргивает разметку данного оъекта
    // и приводим разметку к виду QVboxLayout

    central->setParent( this ) ; // Переписываем его к данному паренту
    pFrame = central ;
    L->insertWidget( 0, pFrame ) ; // Вставляем в начало разметки

    connect( pFrame,   SIGNAL(error_message(const QString &)),
             pButtons, SLOT(error_message(const QString &)) ) ;

    connect( this, SIGNAL(check_data(bool*)), pFrame, SLOT(is_good(bool*))) ;
}

void CommonDialog::accept_pressed(){
    bool OK = true ;
    emit check_data( &OK ) ;
    if ( OK ) accept() ;
}

/**************************************************************/
} // namespace STORE
