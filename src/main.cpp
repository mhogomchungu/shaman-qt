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

#if USE_KDE_STATUS_NOTIFIER
#include <QApplication>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocalizedstring.h>
#include <kuniqueapplication.h>
#include <QDebug>

int main( int argc,char * argv[] )
{
	KAboutData aboutData( 	"shaman-qt",
				0,
				ki18n( "shaman-qt" ),
				"1.0.0",
				ki18n( "a front end to shaman" ),
				KAboutData::License_GPL_V2,
				ki18n( "(c)2014,ink Francis\nemail:mhogomchungu@gmail.com" ),
				ki18n( "mhogomchungu@gmail.com" ),
				"(c)2013,ink Francis\nemail:mhogomchungu@gmail.com",
				"https://github.com/mhogomchungu/shaman-qt/issues" );

	KCmdLineArgs::init( argc,argv,&aboutData ) ;

	KUniqueApplication a ;

	shaman s ;
	s.start() ;

	return a.exec() ;
}

#else
#include <QApplication>
#include "shaman.h"

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	shaman s ;
	s.start() ;

	return a.exec() ;
}

#endif

