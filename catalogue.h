#ifndef CATALOGUE_H
#define CATALOGUE_H

#include <QAbstractItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QTreeView>
#include <QColumnView>

#include "catitemedit.h"

class QAction ;

namespace STORE {
    class PosAction ;
}

namespace STORE {
namespace Catalogue {

/*********************************************************************/
class Model : public QAbstractItemModel {

    Q_OBJECT

private:
    mutable int LastTempId ; // Вспомогательная переменная, которая может изменяться внутри константных функций
    //(Временный ID для новых элементов)

    Item::List Cat ;

protected:
    virtual QVariant    dataDisplay( const QModelIndex &I ) const ;
    virtual QVariant    dataTextAlignment( const QModelIndex &I ) const ;
    virtual QVariant    dataForeground( const QModelIndex &I ) const ; // Для отображения цвета текста
    virtual QVariant    dataBackground( const QModelIndex &I ) const ; // Для отображения цвета фона
    virtual QVariant    dataFont( const QModelIndex &I ) const ; // Для отображения зачёркнутого текста
    virtual QVariant    dataToolTipe( const QModelIndex &I ) const ; // Для отображение всплывающей подсказки
    //virtual Item::Data *dataDataBlock( const QModelIndex &I ) const ; // Блок данных
    int tempId() const { return ++LastTempId ; } // Декримент временных ID

public:
    Model( QObject *parent = nullptr ) ;
    virtual ~Model() ;

    int rowCount( const QModelIndex &parent ) const ; // Сколько строк образуют элементы данного родителя
    int columnCount( const QModelIndex & ) const ; // Сколько столбцов образуют элементы данного родителя
    QModelIndex index( int row, int column, const QModelIndex &parent ) const ; // Индекс
    QModelIndex parent(const QModelIndex &I) const ; // Родитель

    QVariant data( const QModelIndex &I, int role ) const ;

    QVariant headerData( int section,
                         Qt::Orientation orientation, int role ) const ; // Заголовки
private:
    // Впомогательная функция удаления элементов, принадлежащих данному родителю
    bool delete_all_from_db( Item::Data *D  = nullptr) ; // Удаление из базы данных
    bool delete_all_from_model( Item::Data *D  = nullptr) ; // Удаление из модели
    bool save_all_to_db(Item::Data *D = nullptr ) ; //Сохранение изменённых в базу
    bool drop_changed_mark( Item::Data *D = nullptr ) ; // Сброс метки на изменение
    bool insert_all_to_db( Item::Data *D = nullptr ) ; // Сохраняет в базу все новые элементы
    bool adjust_id_for_new(Item::Data *D = nullptr ) ; // Выставляет правильный ID новому элементу

protected:
    bool delete_all() ;
    bool save_all  () ;
    bool insert_all() ;
public slots:
    void editItem( const QModelIndex &I     , QWidget *parent = nullptr ) ; // Редактирование (аргументы - какой параметр редактировать, на каком виджете показывать)
    void newItem( const QModelIndex &parentI, QWidget *parent = nullptr ) ; // Добавление нового элемента
    void delItem( const QModelIndex &I      , QWidget *parent = nullptr ) ; // Удаление элемента списка
    void save() ; // Для сохранения в базу

} ;

/*********************************************************************/

class TableView : public QTableView {

    Q_OBJECT

private:
    PosAction *actEditItem ;
    PosAction *actNewItem  ; // PosAction, потому что надо указывать родителя
    PosAction *actDelItem  ; // Удаление
    PosAction *actRootItem ; // Установка элемента корневым (шаг на ступеньку вниз)
    QAction   *actParentRootItem ; // Установка элемента (шаг на ступеньку вверх)
    QAction   *actSave     ; // Для сохранения изменённых и добавленных элементов в базу

public:
    TableView( QWidget *parent = nullptr, Model *xModel = nullptr ) ;
    virtual ~TableView() ;

private slots:
    void contextMenuRequested( const QPoint &p ) ;
    void showChildren( const QModelIndex &I, QWidget* ) ;
    void showParent() ;
    //void editItem() ; // Не нужен

// signals:
//     void mustEditItem( const QModelIndex &I, QWidget *parent ) ; // Сигнал для редактирования не нужен, его выдаёт Action

} ;

/*********************************************************************/

class TreeView : public QTreeView {
    Q_OBJECT

public:
    TreeView( QWidget *parent = nullptr, Model *xModel = nullptr ) ;
    virtual ~TreeView() ;
};

/*********************************************************************/

class ColumnView : public QColumnView {
    Q_OBJECT

public:
    ColumnView( QWidget *parent = nullptr, Model *xModel = nullptr ) ;
    virtual ~ColumnView() ;
};

/*********************************************************************/

} // namespace Catalogue
} // namespace STORE


#endif // CATALOGUE_H
