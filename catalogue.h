#ifndef CATALOGUE_H
#define CATALOGUE_H

#include <QAbstractTableModel>
#include <QTableView>
#include <QHeaderView>

#include "catitemedit.h"

class QAction ;

namespace STORE {
    class PosAction ;
}

namespace STORE {
namespace Catalogue {

/*********************************************************************/

class Model : public QAbstractTableModel {

    Q_OBJECT

private:
    QList<Item::Data*> Cat ;

protected:
    virtual QVariant    dataDisplay( const QModelIndex &I ) const ;
    virtual QVariant    dataTextAlignment( const QModelIndex &I ) const ;
    virtual QVariant    dataForeground( const QModelIndex &I ) const ; // Для отображения цвета
    virtual QVariant    dataFont( const QModelIndex &I ) const ; // Для отображения зачёркнутого текста
    virtual QVariant    dataToolTipe( const QModelIndex &I ) const ; // Для отображение всплывающей подсказки
    virtual Item::Data *dataDataBlock( const QModelIndex &I ) const ; // Блок данных


public:
    Model( QObject *parent = 0 ) ;
    virtual ~Model() ;

    int rowCount( const QModelIndex &parent ) const ; // Сколько строк образуют элементы данного родителя
    int columnCount( const QModelIndex &parent ) const ; // Сколько столбцов образуют элементы данного родителя

    QVariant data( const QModelIndex &I, int role ) const ;

    QVariant headerData( int section,
                         Qt::Orientation orientation, int role ) const ; // Заголовки

public slots:
    void editItem( const QModelIndex &I     , QWidget *parent = 0 ) ; // Редактирование (аргументы - какой параметр редактировать, на каком виджете показывать)
    void newItem( const QModelIndex &parentI, QWidget *parent = 0 ) ; // Добавление нового элемента
    void delItem( const QModelIndex &I      , QWidget *parent = 0 ) ; // Удаление элемента списка

} ;

/*********************************************************************/

class TableView : public QTableView {

    Q_OBJECT

private:
    PosAction *actEditItem ;
    PosAction *actNewItem  ; // PosAction, потому что надо указывать родителя
    PosAction *actDelItem  ; // Удаление


public:
    TableView( QWidget * parent = 0 ) ;
    virtual ~TableView() ;

private slots:
    void contextMenuRequested( const QPoint &p ) ;
    //void editItem() ; // Не нужен

// signals:
//     void mustEditItem( const QModelIndex &I, QWidget *parent ) ; // Сигнал для редактирования не нужен, его выдаёт Action

} ;

/*********************************************************************/

} // namespace Catalogue
} // namespace STORE


#endif // CATALOGUE_H
