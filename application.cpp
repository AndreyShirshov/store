#include "application.h"

#include <QtSql>

namespace STORE {

Application::Application(int argc, char *argv[] )
    : QApplication( argc, argv ) {

    QSqlDatabase db = QSqlDatabase::addDatabase( "QPSQL" ) ; // Подключение к базе данных (функция addDatabase)
    db.setHostName( "localhost" ) ;
    db.setPort( 5432 ) ;
    db.setDatabaseName( "store" ) ;
    /// TODO Так логин и пароль указывают только глупые люди
    /// TODO Сделать, чтобы логин и пароль спрашивались у пользователя
    db.setUserName( "admin" ) ;
    db.setPassword( "1" ) ;
    db.open( "admin", "1" ) ;

}

Application::~Application() {
}

} // namespace STORE
