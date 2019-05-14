#ifndef CATITEMEDIT_H
#define CATITEMEDIT_H

#include <QFrame>

#include "dialogtpl.h"

#include "ui_catitemframe.h"

namespace STORE {
namespace Catalogue {
namespace Item {

/**************************************************************/

class Data : public QObject {

    Q_OBJECT

public:
    // Пиктограмма
    QString   Code         ; // Код подраздела
    QString   Title        ; // Наименование
    QDateTime From         ; // Действует с .... (дата)
    QDateTime To           ; // Закрыт с .... (дата)
    bool      IsLocal      ; // локальный
    Data      *pParentItem ; // (Родительский подраздел), пока будет равен нулю
    // (ID)
};

/**************************************************************/

class Frame : public QFrame{

    Q_OBJECT

public:
    Frame( QWidget *parent = 0) ;
    virtual ~Frame() ;

private:
    Ui::CatItemFrame ui;

public slots:
    void is_good(bool *pOK ) ;

};

/**************************************************************/

class Dialog : public CommonDialog {

    Q_OBJECT

private:
    Frame *pFrame ;

public:
    Dialog( QWidget *parent = 0 ) ;
    virtual ~Dialog() ;

} ;

/**************************************************************/

} // namespace Item
} // namespace Catalogue
} // namespace STORE

#endif // CATITEMEDIT_H
