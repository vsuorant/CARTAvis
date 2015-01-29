/**
 * Author: Pavol Feder ( federl@gmail.com )
 *
 * Entry point to experimental functionality.
 */

/*global qx, mImport, skel */

/**
 @ignore( mImport)
 */

qx.Class.define("skel.hacks.Hacks", {

    extend: qx.ui.window.Window,

    /**
     * Constructor
     *
     */
    construct: function( app)
    {
        this.base( arguments, "Experimental...");

        this.m_app = app;
        this.m_connector = mImport( "connector" );

        this.setWidth( 300 );
        this.setHeight( 200 );
        this.setShowMinimize( false );
        this.setLayout( new qx.ui.layout.VBox( 5));


        this.add(new skel.boundWidgets.Toggle( "Cursor", "/hacks/cursorVisible"));
        this.add(new skel.boundWidgets.Toggle( "Colormap", "/hacks/cm-windowVisible"));

        if( this.m_connector.getConnectionStatus() != this.m_connector.CONNECTION_STATUS.CONNECTED ) {
            console.error( "Create me only once connection is active!!!" );
            return;
        }

        var win = new qx.ui.window.Window( "Hack view" );
        win.setWidth( 300 );
        win.setHeight( 200 );
        win.setShowMinimize( false );
        win.setUseResizeFrame( false);
        win.setContentPadding( 5, 5, 5, 5 );
        win.setLayout( new qx.ui.layout.Grow() );

        //this.m_viewWithInput = new skel.boundWidgets.ViewWithInputDiv( "hackView" );
        //win.add( this.m_viewWithInput);
        win.add( new skel.hacks.HackView( "hackView"));

        //win.add( new skel.boundWidgets.View( "hackView" ) );
        //this.m_inputOverlay = new qx.ui.core.Widget();
        //win.add( this.m_inputOverlay);


        this.m_app.getRoot().add( win, {left: 200, top: 220} );
        win.open();


/*
        var mmcb = function(ev) {
            //console.log("mm", ev.getDocumentLeft());

            var box = this.m_viewWithInput.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };
            console.log( "mm", pt.x, pt.y);

        };
        mmcb = mmcb.bind(this);

        this.m_viewWithInput.overlayWidget().addListener( "mousemove", mmcb);
*/

        // hacks for temporary functionality
        this.m_cursorWindow = new skel.boundWidgets.CursorWindow();
        this.m_colormapWindow = new skel.boundWidgets.ColormapWindow();
    },

    members: {
        m_connector: null,
        m_app: null,
        m_viewWithInput: null

    }

});