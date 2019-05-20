#include "posaction.h"

namespace STORE {
PosAction::PosAction( QObject *parent )
    :QAction( parent ) {

    connect( this, SIGNAL(triggered()), this, SLOT(was_triggered()) ) ;

}

PosAction::~PosAction(){
}

void PosAction::was_triggered() {

    emit editItem( I, pWidget ) ;

}

} // namespace STORE
