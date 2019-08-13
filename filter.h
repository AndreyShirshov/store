#ifndef FILTER_H
#define FILTER_H

#include <QFrame>

#include "ui_filter.h"

namespace STORE {

class Filter : public QFrame {

    Q_OBJECT
    Q_PROPERTY( QVariant author READ author ) // Зарегистрируем свойство этому объекту
    Q_PROPERTY( QVariant title  READ title  )
    Q_PROPERTY( QVariant year   READ year   )

public:
    Filter(QWidget *parent = nullptr ) ;
    virtual ~Filter() {}
private:
    Ui::Filter ui ;

private slots:
    void apply_filter_triggered() ; // Слот, который выстреливает нужный сигнал

public:
    QVariant author() const ;
    QVariant title () const ;
    QVariant year  () const ;

signals:
    void apply_filter( QObject *pFilter ) ; // Передаёт указатель на объект, который содержит нужный фильтр

};

} // namespace STORE

#endif // FILTER_H
