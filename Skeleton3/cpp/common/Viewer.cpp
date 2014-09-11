#include "Viewer.h"
#include "Globals.h"
#include "IPlatform.h"
#include "IConnector.h"
#include "State/StateLibrary.h"
#include "State/State.h"
#include "Data/DataAnimator.h"
#include "Data/DataController.h"
#include "Data/DataLoader.h"
#include "Data/DataSource.h"
#include "misc.h"
#include "PluginManager.h"
#include "MainConfig.h"
#include "MyQApp.h"
#include "CmdLine.h"
#include "ScriptedCommandListener.h"
#include <iostream>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <cmath>
#include <QDebug>
#include <QCoreApplication>
#include "MyQApp.h"

//Globals & globals = * Globals::instance();

class TestView : public IView
{

public:

	TestView( const QString & viewName, QColor bgColor) {
		m_qimage = QImage( 100, 100, QImage::Format_RGB888);
		m_qimage.fill( bgColor);

		m_viewName = viewName;
		m_connector= nullptr;
		m_bgColor = bgColor;
	}

	virtual void registration(IConnector *connector)
	{
		m_connector = connector;
	}
	virtual const QString & name() const
	{
		return m_viewName;
	}
	virtual QSize size()
	{
		return m_qimage.size();
	}
	virtual const QImage & getBuffer()
	{
		redrawBuffer();
		return m_qimage;
	}
	virtual void handleResizeRequest(const QSize & size)
	{
		m_qimage = QImage( size, m_qimage.format());
		m_connector-> refreshView( this);
	}
	virtual void handleMouseEvent(const QMouseEvent & ev)
	{
		m_lastMouse = QPointF( ev.x(), ev.y());
		m_connector-> refreshView( this);

		m_connector-> setState( StateKey::MOUSE_X, m_viewName, QString::number(ev.x()));
		m_connector-> setState( StateKey::MOUSE_Y, m_viewName, QString::number(ev.y()));
	}
	virtual void handleKeyEvent(const QKeyEvent & /*event*/)
	{
	}

protected:

	QColor m_bgColor;

	void redrawBuffer()
	{
		QPointF center = m_qimage.rect().center();
		QPointF diff = m_lastMouse - center;
		double angle = atan2( diff.x(), diff.y());
		angle *= - 180 / M_PI;

		m_qimage.fill( m_bgColor);
		{
			QPainter p( & m_qimage);
			p.setPen( Qt::NoPen);
			p.setBrush( QColor( 255, 255, 0, 128));
			p.drawEllipse( QPoint(m_lastMouse.x(), m_lastMouse.y()), 10, 10 );
			p.setPen( QColor( 255, 255, 255));
			p.drawLine( 0, m_lastMouse.y(), m_qimage.width()-1, m_lastMouse.y());
			p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_qimage.height()-1);

			p.translate( m_qimage.rect().center());
			p.rotate( angle);
			p.translate( - m_qimage.rect().center());
			p.setFont( QFont( "Arial", 20));
			p.setPen( QColor( "white"));
			p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName);
		}

		// execute the pre-render hook
        Globals::instance()-> pluginManager()-> prepare<PreRender>( m_viewName, & m_qimage).executeAll();
	}

	IConnector * m_connector;
	QImage m_qimage;
	QString m_viewName;
	int m_timerId;
	QPointF m_lastMouse;
};

class TestView2 : public IView
{

public:

	TestView2( const QString & viewName, QColor bgColor, QImage img) {
		m_defaultImage = img;
		m_qimage = QImage( 100, 100, QImage::Format_RGB888);
		m_qimage.fill( bgColor);

		m_viewName = viewName;
		m_connector= nullptr;
		m_bgColor = bgColor;
	}

    void setImage( const QImage & img) {
        m_defaultImage = img;
        m_connector-> refreshView( this);
    }

	virtual void registration(IConnector *connector)
	{
		m_connector = connector;
	}
	virtual const QString & name() const
	{
		return m_viewName;
	}
	virtual QSize size()
	{
		return m_qimage.size();
	}
	virtual const QImage & getBuffer()
	{
		redrawBuffer();
		return m_qimage;
	}
	virtual void handleResizeRequest(const QSize & size)
	{
		m_qimage = QImage( size, m_qimage.format());
		m_connector-> refreshView( this);
	}
	virtual void handleMouseEvent(const QMouseEvent & ev)
	{
		m_lastMouse = QPointF( ev.x(), ev.y());
		m_connector-> refreshView( this);

		m_connector-> setState( StateKey::MOUSE_X, m_viewName, QString::number(ev.x()));
		m_connector-> setState( StateKey::MOUSE_Y, m_viewName, QString::number(ev.y()));
	}
	virtual void handleKeyEvent(const QKeyEvent & /*event*/)
	{
	}

protected:

	QColor m_bgColor;
	QImage m_defaultImage;

	void redrawBuffer()
	{
		QPointF center = m_qimage.rect().center();
		QPointF diff = m_lastMouse - center;
		double angle = atan2( diff.x(), diff.y());
		angle *= - 180 / M_PI;

		m_qimage.fill( m_bgColor);
		{
			QPainter p( & m_qimage);
			p.drawImage( m_qimage.rect(), m_defaultImage);
			p.setPen( Qt::NoPen);
			p.setBrush( QColor( 255, 255, 0, 128));
			p.drawEllipse( QPoint(m_lastMouse.x(), m_lastMouse.y()), 10, 10 );
			p.setPen( QColor( 255, 255, 255));
			p.drawLine( 0, m_lastMouse.y(), m_qimage.width()-1, m_lastMouse.y());
			p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_qimage.height()-1);

			p.translate( m_qimage.rect().center());
			p.rotate( angle);
			p.translate( - m_qimage.rect().center());
			p.setFont( QFont( "Arial", 20));
			p.setPen( QColor( "white"));
			p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName);
		}

		// execute the pre-render hook
        Globals::instance()-> pluginManager()-> prepare<PreRender>( m_viewName, & m_qimage).executeAll();
	}

	IConnector * m_connector;
	QImage m_qimage;
	QString m_viewName;
	int m_timerId;
	QPointF m_lastMouse;
};

static TestView2 * testView2 = nullptr;

Viewer::Viewer() :
    		QObject( nullptr)
{
    int port = Globals::instance()->cmdLineInfo()-> scriptPort();
    if( port < 0) {
        qDebug() << "Not listening to scripted commands.";
    }
    else {
        m_scl = new ScriptedCommandListener( port, this);
        qDebug() << "Listening to scripted commands on port " << port;
        connect( m_scl, & ScriptedCommandListener::command,
                 this, & Viewer::scriptedCommandCB);
	auto connector = globals.connector();

	// initialize plugin manager
	globals.setPluginManager( new PluginManager);
	auto pm = globals.pluginManager();

	// tell plugin manager where to find plugins
	pm-> setPluginSearchPaths( globals.mainConfig()->pluginDirectories());

	// find and load plugins
	}
void Viewer::start()
    auto & globals = * Globals::instance();
    auto connector = globals.connector();
    // initialize plugin manager
    globals.setPluginManager( new PluginManager);
    auto pm = globals.pluginManager();

    // tell plugin manager where to find plugins
    pm-> setPluginSearchPaths( globals.mainConfig()->pluginDirectories());

    // find and load plugins
    pm-> loadPlugins();

    qDebug() << "Loading plugins...";
    auto infoList = pm-> getInfoList();
    qDebug() << "List of plugins: [" << infoList.size() << "]";
    for( const auto & entry : infoList) {
        qDebug() << "  path:" << entry.json.name;
    // tell all plugins that the core has initialized
    pm-> prepare<Initialize>().executeAll();

#ifdef DONT_COMPILE

	// associate a callback for a command
	connector->addCommandCallback( "debug", [] (const QString & cmd, const QString & params, const QString & sessionId) -> QString {
		std::cerr << "lambda command cb:\n"
				<< " " << cmd << "\n"
				<< " " << params << "\n"
				<< " " << sessionId << "\n";
		return "1";
	});

	// associate a callback for a command
	connector->addCommandCallback( "add", [] (const QString & /*cmd*/, const QString & params, const QString & /*sessionId*/) -> QString {
		std::cerr << "add command:\n"
				<< params << "\n";
		QStringList lst = params.split(" ");
		double sum = 0;
		for( auto & entry : lst){
			bool ok;
			sum += entry.toDouble( & ok);
			if( ! ok) { sum = -1; break; }
		}
		return QString("add(%1)=%2").arg(params).arg(sum);
	});

	//    auto xyzCBid =
	connector-> addStateCallback( "/xyz", [] ( const QString & path, const QString & val) {
		qDebug() << "lambda state cb:\n"
				<< "  path: " << path << "\n"
				<< "  val:  " << val;
	});
	//    connector->removeStateCallback(xyzCBid);

	static const QString varPrefix = "/myVars";
	static int pongCount = 0;
	connector-> addStateCallback(
			varPrefix + "/ping",
			[=] ( const QString & path, const QString & val) {
		std::cerr << "lcb: " << path << "=" << val << "\n";
		QString nv = QString::number( pongCount ++);
		connector-> setState( varPrefix + "/pong", nv);
	});
	connector-> setState( "/xya", "hola");
	connector-> setState( "/xyz", "8");

#endif // dont compile

	// create some views to be rendered on the client side
	connector-> registerView( new TestView( "view1", QColor( "blue")));
	connector-> registerView( new TestView( "view2", QColor( "red")));

    // ask plugins to load the image
	qDebug() << "======== trying to load image ========";
	//QString fname = Globals::fname();
    if( Globals::instance()-> platform()-> initialFileList().isEmpty()) {
        qFatal( "No input file given");
		fname = Globals::instance()-> platform()-> initialFileList() [0];
	}
    QString fname = Globals::instance()-> platform()-> initialFileList() [0];

    auto loadImageHookHelper = Globals::instance()-> pluginManager()-> prepare<LoadImage>( fname);
    Nullable<QImage> res = loadImageHookHelper.first();
    if( res.isNull()) {
        qDebug() << "Could not find any plugin to load image";
    }
    else {
        qDebug() << "Image loaded: " << res.val().size();
        testView2 = new TestView2( "view3", QColor( "pink"), res.val());
        connector-> registerView( testView2);
    }

		{
		auto pm = Globals::instance()-> pluginManager();
		auto infoList = pm-> getInfoList();
		int ind = 0;
		for( auto & entry : infoList) {
			//qDebug() << "  path:" << entry.soPath;
			QString index = QString("p%1").arg(ind);
			connector-> setState( StateKey::PLUGIN_NAME, index, entry.name);
			connector-> setState( StateKey::PLUGIN_DESCRIPTION, index, entry.description);
			connector-> setState( StateKey::PLUGIN_TYPE, index, entry.typeString);
			connector-> setState( StateKey::PLUGIN_VERSION, index, entry.version);
			connector-> setState( StateKey::PLUGIN_ERRORS, index, entry.errors.join("|"));
			ind ++;
		}
		QString pluginCountStr = QString::number( ind);
		connector-> setState( StateKey::PLUGIN_STAMP, "", pluginCountStr)   ;es
	}

	//Callback for saving state.
	connector->addCommandCallback( "/saveState", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) -> QString {
                QStringList paramList = params.split( ":");
		QString saveName="DefaultState";
		if ( paramList.length() == 2 ){
			saveName = paramList[1];
		}
                bool result = connector->saveState(saveName);
		QString returnVal = "State was successfully saved.";
		if ( !result ){
			returnVal = "There was an error saving state.";
		}
		return returnVal;
	});

    
       
          
             
                
                      // tell clients about our plugins
    {
        auto pm = Globals::instance()-> pluginManager();
        auto infoList = pm-> getInfoList();
        int ind = 0;
        for( auto & entry : infoList) {
            qDebug() << "  path:" << entry.soPath;
            QString path = QString( "/pluginList/p%1/").arg(ind);
            connector-> setState( path + "name", entry.json.name);
            connector-> setState( path + "description", entry.json.description);
            connector-> setState( path + "type", entry.json.typeString);
            connector-> setState( path + "version", entry.json.version);
//            connector-> setState( path + "dirPath", entry.dirPath);
            connector-> setState( path + "errors", entry.errors.join("|"));
            ind ++;
        }
        connector-> setState( "/pluginList/stamp", QString::number( ind));
    }
}

void Viewer::scriptedCommandCB( QString command)
{
    command = command.simplified();
    qDebug() << "Scripted command received:" << command;

    QStringList args = command.split( ' ', QString::SkipEmptyParts);
    qDebug() << "args=" << args;
    qDebug() << "args.size=" << args.size();
    qDebug() << "args[0].tolower=" << args[0].toLower();
    if( args.size() == 2 && args[0].toLower() == "load") {
        qDebug() << "Trying to load" << args[1];
        auto loadImageHookHelper = Globals::instance()-> pluginManager()
                                   -> prepare<LoadImage>( args[1]);
        Nullable<QImage> res = loadImageHookHelper.first();
        if( res.isNull()) {
            qDebug() << "Could not find any plugin to load image";
        }
        else {
            qDebug() << "Image loaded: " << res.val().size();
            testView2-> setImage( res.val());
        }
    }
    else if( args.size() == 1 && args[0].toLower() == "quit") {
        qDebug() << "Quitting...";
        MyQApp::exit();
        return;
    }
    else {
        qWarning() << "Sorry, unknown command";
    }

		

	//Callback for restoring state.
	connector->addCommandCallback( "/restoreState", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) -> QString {
		QStringList paramList = params.split( ":");
		QString saveName="DefaultState";
		if ( paramList.length() == 2 ){
			saveName = paramList[1];
		}

		bool result = connector->readState(saveName);
		QString returnVal = "State was successfully restored.";
		if ( !result ){
			returnVal = "There was an error restoring state.";
		}
		return returnVal;
	});

	//Callback for registering a view.
	connector->addCommandCallback( "registerView", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) -> QString {
		QList<QString> keys = {"pluginId", "winId"};
		QVector<QString> dataValues = _parseParamMap( params, keys );
		if ( dataValues.size() == keys.size()){
			std::map<QString, std::shared_ptr<DataController> >::iterator it = m_dataControllers.find( dataValues[1] );
			if ( it == m_dataControllers.end() ){
				//Need to make more generic.
				if ( dataValues[0] == "casaLoader"){
					std::shared_ptr<DataController> target(new DataController());
					m_dataControllers[dataValues[1]]= target;
					m_dataControllers[dataValues[1]]->setId( dataValues[1] );
				}
			}
		}
		QString viewId("");
		return viewId;
	});

	//Callback for linking an animator with whatever it is going to animate.
	connector->addCommandCallback( "linkAnimator", [=] (const QString & /*cmd*/,
			const QString & params, const QString & /*sessionId*/) -> QString {
		QList<QString> keys = {"animId", "winId"};
		QVector<QString> dataValues = _parseParamMap( params, keys );
		qDebug() << "LinkAnimator";
		for ( int i = 0; i < dataValues.size(); i++ ){
			qDebug() << "i="<<i<<" key="<<keys[i]<<" value="<<dataValues[i];
		}
		if ( dataValues.size() == keys.size()){

			//Go through our data animators and find the one that is supposed to
			//be hooked up to.  If there is not an existing data animator, create one.
			std::map<QString, std::shared_ptr<DataAnimator> >::iterator it = m_dataAnimators.find( dataValues[0] );
			if ( it == m_dataAnimators.end() ){
				std::shared_ptr<DataAnimator> target(new DataAnimator( dataValues[0]));
				m_dataAnimators[dataValues[0]]= target;
			}

			//Now add the DataController that should be animated to the located DataAnimator.
			std::map<QString, std::shared_ptr<DataController> >::iterator itData = m_dataControllers.find( dataValues[1] );
			if ( itData != m_dataControllers.end() ){
				m_dataAnimators[dataValues[0]]->addController( itData->second );
			}
		}
		return "";
	});

	//Callback for returning a list of data files that can be loaded.
	connector->addCommandCallback( "getData", [=] (const QString & /*cmd*/,
			const QString & params, const QString & sessionId) -> QString {
		QString xml = DataLoader::getData( params, sessionId );
		connector->setState( StateKey::DATA, "", xml );
		return xml;
	});

	//Callback for adding a data source to a DataController.
	connector->addCommandCallback( "dataLoaded", [=] (const QString & /*cmd*/,
            const QString & params, const QString & sessionId) -> QString {
		QList<QString> keys = {"id", "data"};
		QVector<QString> dataValues = _parseParamMap( params, keys );
		if ( dataValues.size() == keys.size()){
			//Find the data controller indicated DataController.
			std::map<QString, std::shared_ptr<DataController> >::iterator it = m_dataControllers.find( dataValues[0] );
			if ( it != m_dataControllers.end()){
				//Add the data to it.
                QString path = dataValues[1];
                if( ! path.startsWith( QDir::separator() + DataLoader::fakeRootDirName )) {
                    /// security issue...
                    return "";
                }
                else {
                	int keepLength = path.length() - DataLoader::fakeRootDirName.length() - 1;
                	path = path.right( keepLength );
                }
                QString realPath = QString( "%1/%2").arg( DataLoader::getRootDir( sessionId))
                       .arg( path );
                m_dataControllers[dataValues[0]]->addData( realPath );
			}
		}
		return "";
	});
}

QVector<QString> Viewer::_parseParamMap( const QString& params, const QList<QString>& keys ){
	QVector<QString> values;
	QStringList paramList = params.split( ",");
	if ( paramList.size() == keys.size() ){
		values.resize( keys.size());
		for ( QString param : paramList ){
			QStringList pair = param.split( ":");
			if ( pair.size() == 2 ){
				int keyIndex = keys.indexOf( pair[0] );
				if ( keyIndex >= 0 ){
					values[keyIndex] = pair[1];
				}
				else {
					qDebug() << "Unrecognized key="<<pair[0];
				}
			}
			else {
				qDebug() <<"Badly formatted param map="<<param;
			}
		}
	}
	else {
		qDebug() << "Discrepancy between parameter count="<<paramList.size()<<" and key count="<<keys.size();
	}
	return values;
}