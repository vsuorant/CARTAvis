#include "Data/Image/Draw/DrawGroupSynchronizer.h"
#include "Data/Image/Layer.h"
#include "Data/Image/LayerCompositionModes.h"
#include "CartaLib/IRemoteVGView.h"
#include "Data/Image/RenderRequest.h"
#include "Data/Image/RenderResponse.h"

#include <QtCore/QDebug>

namespace Carta {

namespace Data {


DrawGroupSynchronizer::DrawGroupSynchronizer( ){
    m_repaintFrameQueued = false;
}


void DrawGroupSynchronizer::render(
        const std::shared_ptr<RenderRequest>& request ){
    if ( m_repaintFrameQueued ){
        return;
    }

    m_images.clear();
    int dataCount = m_layers.size();
    if ( dataCount > 0 ){
        m_repaintFrameQueued = true;

        m_renderCount = 0;
        m_redrawCount = dataCount;
        int stackIndex = 0;
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_layers[i]->_isVisible() ){
                connect( m_layers[i].get(),
                        SIGNAL( renderingDone(const RenderResponse& ) ),
                        this,
                        SLOT(_scheduleFrameRepaint( const RenderResponse& ) ),
                        Qt::UniqueConnection);
                bool topOfStack = false;
                if ( i == request->getTopIndex() ){
                    topOfStack = true;
                }
                std::shared_ptr<RenderRequest> layerRequest( new RenderRequest(
                        request->getFrames(), request->getCoordinateSystem(),
                        topOfStack, request->getOutputSize() ));
                m_layers[i]->_render( layerRequest );
                stackIndex++;
            }
        }
    }
    else {
        //Just say we are done since there is nothing to draw.
        QImage img;
        emit done( img );
    }
}

void DrawGroupSynchronizer::_scheduleFrameRepaint( const std::shared_ptr<RenderResponse>& response){
    m_renderCount++;
    m_images[response->getLayerName()] = response;
    //If we are still waiting for other layers to finish, do nothing.
    if ( m_renderCount != m_redrawCount ) {
        return;
    }
    int dataCount = m_layers.size();
    QImage image;
    if ( m_imageSize.height() > 0 && m_imageSize.width() > 0 ){
        if ( m_combineMode == LayerCompositionModes::PLUS ){
            std::shared_ptr<Carta::Lib::PixelMaskCombiner> pmc =
                    std::make_shared < Carta::Lib::PixelMaskCombiner > ();
            image = QImage(m_imageSize, QImage::Format_ARGB32 );
            image.fill( QColor(0,0,0,0));
            for ( int i = 0; i < dataCount; i++ ){
                m_layers[i]->disconnect( this );

                QString layerName = m_layers[i]->_getLayerId();
                if ( m_images.contains( layerName ) ){
                    std::shared_ptr<RenderResponse> response = m_images[layerName];
                    QImage layerImage = response->getImage();
                    float alphaVal = m_layers[i]->_getMaskAlpha();
                    pmc-> setAlpha( alphaVal );
                    qint32 maskColor = m_layers[i]->_getMaskColor();
                    pmc-> setMask( maskColor );
                    pmc->combine( image, layerImage );
                }
            }
        }
        else {
            if ( dataCount > 0 ){
                //Implement for now to just get one image.
                QString imageName = m_layers[0]->_getLayerId();
                if ( m_images.contains( imageName ) ){
                    image = m_images[imageName]->getImage();
                }
            }
        }
    }
    for ( int i = 0; i < dataCount; i++ ){
        m_layers[i]->_renderDone();
    }
    m_repaintFrameQueued = false;
    emit done( image );
}

void DrawGroupSynchronizer::setCombineMode( const QString& mode ){
    m_combineMode = mode;
}

void DrawGroupSynchronizer::setLayers( QList< std::shared_ptr<Layer> > layers){
    m_layers = layers;
}

void DrawGroupSynchronizer::viewResize( const QSize& newSize ){
    m_imageSize = newSize;
}


DrawGroupSynchronizer::~DrawGroupSynchronizer(){

}

}
}
