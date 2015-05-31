/**
 * Command to show histogram preference settings.
 */

qx.Class.define("skel.Command.Histogram.Preferences", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Histogram...", null );
        this.m_toolBarVisible = false;
        this.m_global = false;
        this.setToolTipText( "Show histogram preferences.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                //Use the first one in the list that supports this cmd.
                var parentCmd = skel.Command.Histogram.Histogram.getInstance();
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( parentCmd ) ){
                        qx.event.message.Bus.dispatch(new qx.event.message.Message(
                                "showHistogramPreferences", activeWins[i]));
                        break;
                    }
                }
            }
        },
        
        setPrefs : function( val ){
        }
    }
});