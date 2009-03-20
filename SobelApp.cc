/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    ---
    Copyright (C) 2008, Krzysztof Kundzicz <athantor@gmail.com>
 */

#include <SobelApp.hh>

SobelApp::SobelApp(int & c, char ** v) : QApplication(c, v), mwin(new SobMainWin()) /*in_im(new QImage()), out_im(0)*/
{

    mwin -> show();

}

SobelApp::~SobelApp()
{
}
