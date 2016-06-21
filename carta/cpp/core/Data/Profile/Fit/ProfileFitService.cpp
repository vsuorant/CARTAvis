#include "ProfileFitService.h"
#include "CartaLib/Hooks/Fit1DHook.h"
#include "ProfileFitThread.h"

namespace Carta {
namespace Data {

ProfileFitService::ProfileFitService( QObject * parent ) :
        QObject( parent ),
        m_fitThread( nullptr ){
    m_fitQueued = false;
}


bool ProfileFitService::fitProfile(const std::vector<Carta::Lib::Fit1DInfo>& fitInfos ){
    bool fitCompute = true;
    if ( fitInfos.size() > 0 ){

        _scheduleRender( fitInfos );
    }
    else {
        fitCompute = false;
    }
    return fitCompute;
}


void ProfileFitService::_scheduleRender( const std::vector<Carta::Lib::Fit1DInfo>& fitInfos ){

    if ( m_fitThread && !m_fitThread->isFinished() ) {
        return;
    }
    m_fitQueued = true;

    //Create a worker if we don't have one.
    delete m_fitThread;
    m_fitThread = new ProfileFitThread();
    connect( m_fitThread, SIGNAL(finished()), this, SLOT(_postResult()));
    m_fitThread->setParams( fitInfos );

    m_fitThread->start();

}

void ProfileFitService::_postResult( ){
    std::vector<Carta::Lib::Hooks::FitResult> results = m_fitThread->getResult();
    //RenderRequest request = m_requests.dequeue();
    emit fitResult( results );
    m_fitQueued = false;
    /*if ( m_requests.size() > 0 ){
        RenderRequest& head = m_requests.head();
         _scheduleRender( head.m_image,
                head.m_regionInfo, head.m_profileInfo );
    }*/
}


ProfileFitService::~ProfileFitService(){
    delete m_fitThread;
}
}
}

