/*
 * Copyright (C) 2020 PandaOS Team.
 *
 * Author:     rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "button.h"
#include "decoration.h"

#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/Decoration>

#include <QPainter>
#include <QPainterPath>

Button::Button(KDecoration3::DecorationButtonType type, const QPointer<KDecoration3::Decoration> &decoration, QObject *parent)
    : KDecoration3::DecorationButton(type, decoration, parent)
{
    auto c = decoration->window();

    switch (type) {
    case KDecoration3::DecorationButtonType::Menu:
        break;
    case KDecoration3::DecorationButtonType::Minimize:
        setVisible(c->isMinimizeable());
        connect(c, &KDecoration3::DecoratedWindow::minimizeableChanged, this, &Button::setVisible);
        break;
    case KDecoration3::DecorationButtonType::Maximize:
        setVisible(c->isMaximizeable());
        connect(c, &KDecoration3::DecoratedWindow::maximizeableChanged, this, &Button::setVisible);
        break;
    case KDecoration3::DecorationButtonType::Close:
        setVisible(c->isCloseable());
        connect(c, &KDecoration3::DecoratedWindow::closeableChanged, this, &Button::setVisible);
        break;
    default:
        setVisible(false);
        break;
    }
}

KDecoration3::DecorationButton *Button::create(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent)
{
    return new Button(type, decoration, parent);
}

void Button::paint(QPainter *painter, const QRectF &repaintRegion)
{
    Q_UNUSED(repaintRegion)

    Cutefish::Decoration *decoration = qobject_cast<Cutefish::Decoration *>(this->decoration());

    if (!decoration)
        return;

    auto c = decoration->window();
    const bool isDarkMode = decoration->darkMode();
    const QRect &rect = geometry().toRect();

    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->setRenderHints(QPainter::Antialiasing, true);

    QRect btnRect(0, 0, 26 * decoration->devicePixelRatio(),
                        26 * decoration->devicePixelRatio());
    btnRect.moveCenter(rect.center());

    QRect imgRect(0, 0, 24 * decoration->devicePixelRatio(),
                        24 * decoration->devicePixelRatio());
    imgRect.moveCenter(rect.center());

    if (isHovered() || isPressed()) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(isDarkMode ? isPressed() ? QColor(255, 255, 255, 255 * 0.1) : QColor(255, 255, 255, 255 * 0.15)
                                     : isPressed() ? QColor(0, 0, 0, 255 * 0.15) : QColor(0, 0, 0, 255 * 0.1));
        painter->drawRoundedRect(btnRect.adjusted(2, 2, -2, -2), btnRect.height() / 2, btnRect.height() / 2);
    }

    switch (type()) {
    case KDecoration3::DecorationButtonType::Menu: {
        c->icon().paint(painter, rect);
        break;
    }
    case KDecoration3::DecorationButtonType::ApplicationMenu: {
        break;
    }
    case KDecoration3::DecorationButtonType::Minimize: {
        painter->drawPixmap(imgRect, decoration->minimizeBtnPixmap());
        break;
    }
    case KDecoration3::DecorationButtonType::Maximize: {
        if (isChecked())
            painter->drawPixmap(imgRect, decoration->restoreBtnPixmap());
        else
            painter->drawPixmap(imgRect, decoration->maximizeBtnPixmap());
        break;
    }
    case KDecoration3::DecorationButtonType::Close: {
        painter->drawPixmap(imgRect, decoration->closeBtnPixmap());
        break;
    }
    default:
        break;
    }

    painter->restore();
}
