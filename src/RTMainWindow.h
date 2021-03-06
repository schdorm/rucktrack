/*
 * RTMainWindow.h - header file for RTMainWindow class
 *
 * Copyright (c) 2009-2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * This file is part of RuckTrack - http://rucktrack.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef _RTMAINWINDOW_H
#define _RTMAINWINDOW_H

#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QMainWindow>
#include <QProgressBar>

#include "TrackPoint.h"


class RouteTableModel;

namespace Ui
{
	class RTMainWindow;
}


/**
 *  Main window of RuckTrack.
 */
class RTMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	RTMainWindow( QWidget *parent = 0 );
	~RTMainWindow();

	static RTMainWindow * instance()
	{
		return _this;
	}


private slots:
	void preferences();
	void about();
	void openFile();
	void fixElevations();
	void selectTrackPoint( double _lat, double _lon );
	void highlightSelectedTrackPoint( const QModelIndex & _current,
										const QModelIndex & _prev );
	void parseCommandLineParameters();
	void redrawRoute();

private:
	void loadRoute( const QString & fileName );
	void setPreferences();
	void setPreferenceMapProvider();
	void setPreferenceShowProgressBar();

	static RTMainWindow * _this;

	Ui::RTMainWindow *ui;
	Route m_currentRoute;
	RouteTableModel * m_routeTableModel;
	QProgressBar * m_webPageProgressBar;
} ;


#endif // _RTMAINWINDOW_H
