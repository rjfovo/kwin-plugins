#include "x11shadow.h"

#include <QGuiApplication>
#include <xcb/xcb.h>

// Qt6 兼容性处理
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // 使用完整的路径
    #include <QtGui/qpa/qplatformnativeinterface.h>
#else
    #include <QX11Info>
#endif

static xcb_atom_t internAtom(const char *name, bool only_if_exists)
{
    if (!name || *name == 0)
        return XCB_NONE;

    // Check if we're running on X11 platform
    if (QGuiApplication::platformName() != "xcb")
        return XCB_NONE;

    // Get XCB connection through platform native interface
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    if (!native)
        return XCB_NONE;

    xcb_connection_t *connection = static_cast<xcb_connection_t *>(
        native->nativeResourceForIntegration("connection"));
#else
    xcb_connection_t *connection = QX11Info::connection();
#endif
    if (!connection)
        return XCB_NONE;

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, only_if_exists, strlen(name), name);
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);

    if (!reply)
        return XCB_NONE;

    xcb_atom_t atom = reply->atom;
    free(reply);

    return atom;
}

X11Shadow::X11Shadow(QObject *parent)
    : QObject(parent)
{
    m_atom_net_wm_shadow = internAtom("_KDE_NET_WM_SHADOW", false);
    m_atom_net_wm_window_type = internAtom("_NET_WM_WINDOW_TYPE", false);
}
