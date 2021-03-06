/***************************************************************************
 *   Copyright (C) 2009 by Jeff Gibbons                                    *
 *   Copyright (C) 2005-2008 by Felix Geyer                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ShortcutWidget.h"

#if defined(GLOBAL_AUTOTYPE) && (defined(Q_WS_X11) || defined(Q_WS_MAC))

#include <QKeyEvent>
#include <QPalette>
#ifdef Q_WS_X11
#include <QX11Info>
#include "HelperX11.h"
#include "AutoTypeGlobalX11.h"
#endif
#ifdef Q_WS_MAC
#include "lib/HelperMacX.h"
#include "lib/AutoTypeGlobalMacX.h"
#endif

ShortcutWidget::ShortcutWidget(QWidget* parent) : QLineEdit(parent), lock(false), failed(false){
}

Shortcut ShortcutWidget::shortcut(){
	if (lock)
		return pShortcut;
	else
		return Shortcut();
}

void ShortcutWidget::setShortcut(const Shortcut& s){
	lock = true;
	displayShortcut(s.key, false, s.ctrl, s.shift, s.alt, s.altgr, s.win);
}

void ShortcutWidget::keyPressEvent(QKeyEvent* event){
	keyEvent(event, false);
	event->accept();
}

void ShortcutWidget::keyReleaseEvent(QKeyEvent* event){
	keyEvent(event, true);
	event->accept();
}

void ShortcutWidget::keyEvent(QKeyEvent* event, bool release){
	if (release && lock)
		return;

#ifdef Q_WS_X11
	AutoTypeGlobalX11* autoTypeGlobal = static_cast<AutoTypeGlobalX11*>(autoType);
	
	unsigned int mods = HelperX11::keyboardModifiers(QX11Info::display());
	displayShortcut(event->nativeVirtualKey(), release, mods & ControlMask,
			mods & ShiftMask, mods & autoTypeGlobal->maskAlt(),
			mods & autoTypeGlobal->maskAltGr(), mods & autoTypeGlobal->maskMeta());
#endif
#ifdef Q_WS_MAC
	AutoTypeGlobalMacX* autoTypeGlobal = static_cast<AutoTypeGlobalMacX*>(autoType);
	quint32 mods = event->nativeModifiers();
	// mods >> 16 bits denote outside main keyboard eg keypad, arrow keys, home, end, etc
	if ((0 != (mods >> 16)) || (0 == mods)) return;
	quint32 key = event->nativeVirtualKey();
	// prohibited keys
	switch (key) {
		case kVK_Delete: case kVK_Escape: case kVK_Return: case kVK_Tab: case kVK_ANSI_KeypadEnter: return;
	}
	displayShortcut(HelperMacX::keycodeToKeysym(key), release,
					mods & autoTypeGlobal->maskCtrl(), mods & autoTypeGlobal->maskShift(),
					mods & autoTypeGlobal->maskAlt(),  mods & autoTypeGlobal->maskAltGr(),
					mods & autoTypeGlobal->maskMeta());
#endif
}

void ShortcutWidget::displayShortcut(quint32 key, bool release, bool ctrl, bool shift, bool alt, bool altgr, bool win){
	QString text;
	
#ifdef Q_WS_X11
	if (ctrl)
		text.append(tr("Ctrl")).append(" + ");
	if (shift)
		text.append(tr("Shift")).append(" + ");
	if (alt)
		text.append(tr("Alt")).append(" + ");
	if (altgr)
		text.append(tr("AltGr")).append(" + ");
	if (win)
		text.append(tr("Win")).append(" + ");
	
	if ( !release && (key<XK_Shift_L || key>XK_Hyper_R) && (key<XK_ISO_Lock || key>XK_ISO_Last_Group_Lock) ){
		// converts key into orignal key on the keyboard
		KeySym keysym = XKeycodeToKeysym(QX11Info::display(), XKeysymToKeycode(QX11Info::display(),key), 0);
		if (keysym>=0xfd00 && keysym<=0xffff){
			text.append(XKeysymToString(keysym));
		}
		else{
			text.append(static_cast<quint32>(keysym));
		}
#endif
#ifdef Q_WS_MAC
	if (ctrl)
		text.append(kControlUnicode);
	if (shift)
		text.append(kShiftUnicode);
	if (alt)
		text.append(kOptionUnicode);
	if (win)
		text.append(kCommandUnicode);
	KeySym keysym = key;
	if (!release && (NoSymbol != keysym)){
		text.append(QChar(keysym).toUpper());
#endif	

		lock = ctrl || shift || alt || altgr || win;
		if (lock){
			pShortcut.key = keysym;
			pShortcut.ctrl = ctrl;
			pShortcut.shift = shift;
			pShortcut.alt = alt;
			pShortcut.altgr = altgr;
			pShortcut.win = win;
			failed = !autoType->registerGlobalShortcut(pShortcut);
			if (failed)
				setBackgroundColor(QColor(255, 150, 150));
			else {
				setBackgroundColor(Qt::white);
				setText(text);
			}
		}
	}
	else {
		lock = false;
		if (failed)
			setBackgroundColor(Qt::white);
	}
}

void ShortcutWidget::setBackgroundColor(const QColor& c){
	QPalette p( palette() );
	p.setColor(backgroundRole(), c);
	setPalette(p);
}

#else

ShortcutWidget::ShortcutWidget(QWidget* parent) : QLineEdit(parent){
}

#endif
