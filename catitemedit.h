#ifndef CATITEMEDIT_H
#define CATITEMEDIT_H

#include <QFrame>

#include "dialogtpl.h"

#include "ui_catitemframe.h"

class QSqlQuery ;

namespace STORE {
namespace Catalogue {
namespace Item {

/*********************************************************************/
// Класс данных, данные упрощённо в public

class Data : public QObject {

    Q_OBJECT

public:
    Data(QObject *parent = 0 ) : QObject(parent) {}
    Data(QObject *parent, QSqlQuery &qry ) ;
    // Пиктограмма
    QVariant  Id           ;
    QString   Code         ; // Код подраздела
    QString   Title        ; // Наименование
    QDateTime From         ; // Действует с .... (дата)
    QDateTime To           ; // Закрыт с .... (дата)
    bool      IsLocal      ; // локальный
    QString   Comment      ; // Комментарий
    Data      *pParentItem ; // (Родительский подраздел), пока будет равен нулю
};

/*********************************************************************/

class Frame : public QFrame{

    Q_OBJECT

public:
    Frame( QWidget *parent = 0 ) ;
    virtual ~Frame() ;

private:
    Ui::CatItemFrame ui;
    Data *Block ;

public:
    void setDataBlock( Data *D ) { Block = D ; load() ; } // чтобы прицепить данные к фрэйму

public slots:
    void is_good(bool *pOK ) ;
    void load() ; // Перенос с данных на диалог
    bool save() ; // Перенос с диалога на данные, bool - действительно ли сохранились данные

signals:
    void error_message(const QString & ) ;

};

/*********************************************************************/

class Dialog : public CommonDialog {

    Q_OBJECT

private:
    Frame *pFrame ;

public:
    Dialog( QWidget *parent = 0 ) ;
    virtual ~Dialog() ;

public:
    void setDataBlock( Data *D) { pFrame->setDataBlock( D ) ; }

} ;

/*********************************************************************/

} // namespace Item
} // namespace Catalogue
} // namespace STORE

#endif // CATITEMEDIT_H
