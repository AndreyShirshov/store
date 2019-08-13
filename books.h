#ifndef BOOKS_H
#define BOOKS_H

#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QItemDelegate>

namespace STORE {
namespace Books{

/*********************************************************************/
// Класс делегата
class StatusDelegate : public QItemDelegate {

    Q_OBJECT

private:
    QMap<int,QString> fAllStatus ;

public:

    StatusDelegate( QObject *parent, const QMap<int,QString> &AllStatus ) ;
    virtual ~StatusDelegate( void ) {}

    //Переопределяем функцию редактора:
    QWidget * createEditor( QWidget *parent,
                            const QStyleOptionViewItem &,
                            const QModelIndex & ) const ;

// Переопределяем функцию перенос данных с модели на редактор:
    void setEditorData( QWidget *editor,
                        const QModelIndex &I ) const ;

// Переопределяем функцию перенос данных с редактора на модель:
    void setModelData( QWidget *editor,
                       QAbstractItemModel *model,
                       const QModelIndex &I ) const ;

// Переопределим функцию рисования paint для штриховки выданного:
    void paint( QPainter *painter,
                const QStyleOptionViewItem &option,
                const QModelIndex &I ) const ;
};

/*********************************************************************/
// Модель

class Model : public QSqlQueryModel {
    Q_OBJECT

private:
    QVariant fAuthor ; // Параметры фильтра хранятся в приватных переменных
    QVariant fTitle ;
    QVariant fYear ;
    QVariant fCatId ;
    void adjust_query() ;

public:
    Model( QObject *parent = nullptr) ;
    virtual ~Model() ;

    int columnCount( const QModelIndex & ) const { return 12 ; } // Подменяем columnCount, чтобы скрыть iid

public slots:
    void cat_item_selected( QVariant Id ) ;
    void apply_filter( QObject *F ) ; // Применить объект, содержащий фильтр
};

/*********************************************************************/
// Модель

class Model_EditOnServer : public QSqlTableModel {
    Q_OBJECT

private:
    QVariant fAuthor ; // Параметры фильтра хранятся в приватных переменных
    QVariant fTitle ;
    QVariant fYear ;
    QVariant fCatId ;
    void adjust_query() ;

    QAction *actDeleteRow ;
    QAction *actNewRow ;
    QAction *actSaveAll    ;
    QAction *actRestoreAll ;

protected slots:
    void on_save_all() ;
    void on_restore_all() ;
    void on_delete_row() ;
    void on_new_row() ;

public:
    QMap<int,QString> AllStatus ; // Для загрузки справочника статусов
    QList< QAction* > AllActions ; // Список Actions, для доступа к нему в представлении

public:
    Model_EditOnServer( QObject *parent = nullptr) ;
    virtual ~Model_EditOnServer() ;

    QVariant data(const QModelIndex &I, int role ) const ; // Подменим функцию data
    bool setData( const QModelIndex &I, const QVariant &val, int role ) ;
    int columnCount( const QModelIndex & ) const { return 12 ; } // Подменяем columnCount, чтобы скрыть iid
    Qt::ItemFlags flags( const QModelIndex & ) const ; // Переопределим флаги для взаимодействия с ячейками

public slots:
    void cat_item_selected( QVariant Id ) ;
    void apply_filter( QObject *F ) ; // Применить объект, содержащий фильтр
};

/*********************************************************************/
// Представление
class View : public QTableView {
    Q_OBJECT
public:
    View( QWidget *parent = nullptr ) ;
    virtual ~View() ;
};

/*********************************************************************/

} // namespace Books
} // namespace STORE

#endif // BOOKS_H
