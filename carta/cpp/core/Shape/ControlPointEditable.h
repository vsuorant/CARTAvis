/**
 * A point which can be dragged.
 **/

#pragma once
#include "ShapeBase.h"
#include "CartaLib/CartaLib.h"
#include <QPointF>

namespace vge = Carta::Lib::VectorGraphics::Entries;


namespace Carta {

namespace Shape {

class ControlPointEditable
    : public ShapeBase
{
    CLASS_BOILERPLATE( ControlPointEditable );

public:

    /**
     * Contructor.
     * @param cb - callback for updates when a drag is finished.
     */
    ControlPointEditable( std::function < void (bool) > cb );

    /**
    	 * Return the location of the point.
    	 * @return - the location of the point.
    	 */
    const QPointF & getPosition() const;

    /**
     * Returns the graphics for drawing the control point.
     * @return - the graphics for drawing the control point.
     */
    virtual Carta::Lib::VectorGraphics::VGList getVGList() const override;

    /**
     * Notification that a drag has started.
     * @param pt - the start point of the drag.
     */
    virtual void handleDragStart( const QPointF & pt ) override;

    /**
     * Notification of a drag in progress.
     * @param pt - the current point of the drag.
     */
    virtual void handleDrag( const QPointF & pt ) override;

    /**
     * Notification that a drag has ended.
     * @param pt - the end point of the drag.
     */
    virtual void handleDragDone( const QPointF & pt ) override;

    /**
     * Returns whether or not the point is inside the control point.
     * @param pt - the point to test.
     * @return - true if the point is inside this one; false, otherwise.
     */
    virtual bool isPointInside( const QPointF & pt ) const override;

    /**
     * Set the fill color of the point.
     * @param color - the fill color for the point.
     */
    void setFillColor( QColor color );

    /**
     * Set the position of the point.
     * @param pt - the position of the point.
     */
    void setPosition( const QPointF & pt );

    virtual ~ControlPointEditable();

private:
    std::function < void (bool) > m_cb;
    QPointF m_pos = QPointF( 0, 0 );
    QColor m_fillColor = QColor( 255, 0, 0 );
    double m_size = 7;
};

}
}