/* This file is part of Clementine.

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QMouseEvent>
#include <QStyle>

#include "trackslider.h"

TrackSlider::TrackSlider(QWidget *parent) :
        QSlider(Qt::Horizontal, parent)
{
}

void TrackSlider::mousePressEvent(QMouseEvent *e)
{
    Qt::MouseButton new_button = e->button();
    if (e->button() == Qt::LeftButton)
    {
        int abs_buttons = style()->styleHint(QStyle::SH_Slider_AbsoluteSetButtons);
        if (abs_buttons & Qt::LeftButton)
            new_button = Qt::LeftButton;
        else if (abs_buttons & Qt::MidButton)
            new_button = Qt::MidButton;
        else if (abs_buttons & Qt::RightButton)
            new_button = Qt::RightButton;
    }

    QMouseEvent new_event(e->type(), e->pos(), new_button, new_button, e->modifiers());
    QSlider::mousePressEvent(&new_event);

    if (new_event.isAccepted())
        e->accept();
}
