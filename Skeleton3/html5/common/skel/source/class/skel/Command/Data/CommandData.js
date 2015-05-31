/**
 * Container for commands that open/close image data.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Data.CommandData", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Data" );
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Data.CommandDataOpen.getInstance();
        this.m_cmds[1] = skel.Command.Data.CommandDataClose.getInstance();
        this.setValue( this.m_cmds );
    },
    
    members : {
        //Note:  Overriden so that the _resetEnabled method of CommandDataClose will
        //be called, giving it the opportunity to add CommandDataCloseImage commands
        //based on the window(s) selected.
        _resetEnabled : function( ){
            arguments.callee.base.apply(this, arguments);
            var enabled = this.isEnabled();
            this.m_cmds[0].setEnabled( enabled );
            this.m_cmds[1]._resetEnabled();
            this.m_cmds[1].setEnabled( enabled );
        }
    }
});