#include "catitemedit.h"

namespace STORE {
namespace Catalogue {
namespace Item {

/**************************************************************/

Frame::Frame( QWidget *parent )
    : QFrame( parent ) {

    ui.setupUi(this) ;
}

Frame::~Frame() {
}

/** TODO: Тут сделать нормальную проверку данных */
void Frame::is_good(bool *pOK ) {
    *pOK = true ;
}

/**************************************************************/

Dialog::Dialog( QWidget *parent)
    : CommonDialog ( parent ) {

        pFrame = new Frame( this ) ;
        setCentralFrame( pFrame ) ;
}

Dialog::~Dialog() {
}

/**************************************************************/

} // namespace Item
} // namespace Catalogue
} // namespace STORE
