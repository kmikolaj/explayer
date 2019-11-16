#include "dpms.h"
#include <QX11Info>
#include <xcb/dpms.h>

DPMS::DPMS()
{
	Store();
}

void DPMS::Enable()
{
	xcb_dpms_enable(QX11Info::connection());
}


void DPMS::Disable()
{
	xcb_dpms_disable(QX11Info::connection());
}

void DPMS::Restore()
{
	if (state)
		Enable();
	else
		Disable();
}

void DPMS::Store()
{
	state = State();
}

bool DPMS::State()
{
	u_int8_t state;
	xcb_connection_t *conn = QX11Info::connection();
	xcb_dpms_info_cookie_t dpmsc = xcb_dpms_info(conn);
	xcb_dpms_info_reply_t *dpmsr;
	if ((dpmsr = xcb_dpms_info_reply(conn, dpmsc, nullptr)))
	{
		state = dpmsr->state;
		free(dpmsr);
	}
	return bool(state);
}
