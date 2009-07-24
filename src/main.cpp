/*
 * main.cpp - implementation of main() function
 *
 * Copyright (c) 2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include "RTMainWindow.h"
#include "SrtmLayer.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RTMainWindow w;
	w.show();

	return a.exec();
	//SrtmLayer sl;
	//qDebug() << sl.getElevation( 34.2588, -107.7597222 ); = 2508
	//qDebug() << sl.getElevation( 50.8706, 12.9631 );// = 365
}
