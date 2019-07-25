#ifndef CATITEMEDIT_H
#define CATITEMEDIT_H

#include <QFrame>

#include "dialogtpl.h"

#include "ui_catitemframe.h"

class QSqlQuery ;

namespace STORE {
namespace Catalogue {
namespace Item {

class Data ; // Объявляем для class List
/*********************************************************************/
// Класс список дочерних элементов

class List : public QList<Data*> {
public:
    List() : QList<Data*>() {}
    Data *findPointer( int Id ) const ; // Функция в списке находит указатель на блок данных по идентификатору
} ;

/*********************************************************************/
// Класс данных, данные упрощённо в public

class Data : public QObject {

    Q_OBJECT

public:
    Data(QObject *parent = nullptr ) :
        QObject(parent) , IsLocal(true) , pParentItem(nullptr) , Deleted(false), Changed(false) {}
    Data(QObject *parent, QSqlQuery &qry ) ;
public:
    // Пиктограмма
    QVariant  Id           ;
    QString   Code         ; // Код подраздела
    QString   Title        ; // Наименование
    QDateTime From         ; // Действует с .... (дата)
    QDateTime To           ; // Закрыт с .... (дата)
    bool      IsLocal      ; // локальный
    QString   Comment      ; // Комментарий
    Data      *pParentItem ; // (Родительский подраздел), пока будет равен нулю
    bool      Deleted      ; // Элемент помечен на удаление
    List      Children     ; // Список дочерних элементов
    bool      Changed      ; // Блок данных подвергался редактированию
public:
    bool isActive() const         ; // Активный элемент
    bool isNew()    const         ; // Новый элемент или нет
    bool isSameAs(Data *D ) const ; // Возвращает thrue , если данный оъект тот же самый что и указатель на D
} ;

/*********************************************************************/

class Frame : public QFrame{

    Q_OBJECT

public:
    Frame( QWidget *parent = nullptr ) ;
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
    Dialog( QWidget *parent = nullptr ) ;
    virtual ~Dialog() ;

public:
    void setDataBlock( Data *D) { pFrame->setDataBlock( D ) ; }

} ;

/*********************************************************************/

} // namespace Item
} // namespace Catalogue
} // namespace STORE

#endif // CATITEMEDIT_H
