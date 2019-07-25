#ifndef BOOKS_H
#define BOOKS_H

#include <QTableView>
#include <QSqlQueryModel>

namespace STORE {
namespace Books{

/*********************************************************************/
// Модель
class Model : public QSqlQueryModel {
    Q_OBJECT

public:
    Model( QObject *parent = nullptr) ;
    virtual ~Model() ;
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
