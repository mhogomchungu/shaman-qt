/*
 *
 *  Copyright (c) 2014
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tray_application_type.h"
#include "shaman.h"
#include "task.h"

#include <QMetaObject>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QSettings>

const char * cmd = "shaman -l Syracuse,US -i --format \"%I,%j\\011temp: %t F\\011low: %h F\\011high: %H F\\011%c\"" ;

const char * updateInterval = "20" ;

const char * splitter = "\\011" ;

const char * icon = "shaman-qt" ;

const char * appName = "shaman-qt" ;

#if USE_KDE_STATUS_NOTIFIER
#include <kstandarddirs.h>

static QString _getShamanCmd( void )
{
	KStandardDirs k ;

	QSettings settings( appName,appName ) ;
	settings.setPath( QSettings::IniFormat,QSettings::UserScope,k.localxdgconfdir() ) ;

	QString opt = "command" ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		settings.setValue( opt,QString( cmd ) ) ;
		return cmd ;
	}
}

static int _getUpdateInterval( void )
{
	KStandardDirs k ;

	QSettings settings( appName,appName ) ;
	settings.setPath( QSettings::IniFormat,QSettings::UserScope,k.localxdgconfdir() ) ;

	QString opt = "updateInterval" ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toInt() ;
	}else{
		settings.setValue( opt,QString( updateInterval ) ) ;
		return QString( updateInterval ).toInt() ;
	}
}

#else

static QString _getShamanCmd( void )
{
	QSettings settings( appName,appName ) ;
	settings.setPath( QSettings::IniFormat,QSettings::UserScope,QDir::homePath() + "/.config" ) ;

	QString opt = "command" ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		settings.setValue( opt,QString( cmd ) ) ;
		return cmd ;
	}
}

static int _getUpdateInterval( void )
{
	QSettings settings( appName,appName ) ;
	settings.setPath( QSettings::IniFormat,QSettings::UserScope,QDir::homePath() + "/.config" ) ;

	QString opt = "updateInterval" ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toInt() ;
	}else{
		settings.setValue( opt,QString( updateInterval ) ) ;
		return QString( updateInterval ).toInt() ;
	}
}

#endif

struct weatherInfo
{
	bool success( void ) const
	{
		return exitCode == 0 ;
	}
	int  exitCode ;
	bool completed ;
	QString outPut ;
};

weatherInfo getWeatherInfo()
{
	weatherInfo w ;
	QProcess p ;
	p.start( _getShamanCmd() ) ;
	w.completed = p.waitForFinished() ;
	if( w.completed ){
		w.exitCode   = p.exitCode() ;
		w.outPut     = p.readAll() ;
	}else{
		w.exitCode = 1 ;
	}

	return w ;
}

shaman::shaman()
{
	statusicon::setCategory( statusicon::ApplicationStatus ) ;
	QCoreApplication::setApplicationName( appName ) ;
}

void shaman::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void shaman::run()
{
	QAction * ac ;
	ac = statusicon::getAction( tr( "update info" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( updateInfo() ) ) ;

	ac = statusicon::getAction( tr( "configure" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( configure() ) ) ;
	ac->setEnabled( false ) ;

	statusicon::setIcon( icon ) ;
	statusicon::setOverlayIcon( icon ) ;

	statusicon::addQuitAction() ;

	auto t = new QTimer( this ) ;
	connect( t,SIGNAL( timeout() ),this,SLOT( updateInfo() ) ) ;
	t->start( 1000 * 60 * _getUpdateInterval() ) ;

	this->updateInfo() ;
}

void shaman::updateInfo()
{
	auto _a = [](){

		return getWeatherInfo() ;
	} ;

	auto _b = [&]( const weatherInfo& w ){

		if( w.success() ){

			QStringList l = w.outPut.split( splitter,QString::SkipEmptyParts ) ;

			QString table = "<table>" ;

			for( const auto& it : l ){

				table += QString( "<tr width=50 valign=middle><td>%1</td>" ).arg( it ) ;
			}

			table += "</table>" ;

			statusicon::setToolTip( icon,"status",table ) ;
		}else{
			statusicon::setToolTip( icon,tr( "error" ),tr( "failed to fetch data" ) ) ;
		}
	} ;

	Task::run< weatherInfo >( _a ).then( _b ) ;
}

void shaman::configure()
{
	/*
	 * bring up a GUI window to configure various options.
	 *
	 * NOT implemented yet
	 *
	 */
}
