/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/
#include <iostream>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QDockWidget>
#include <countdown-widget.hpp>

#include "plugin-macros.generated.h"


// void load_menu_option()
// {
// 	obs_frontend_add_tools_menu_item("Add Countdown Timer",
// 					 callback_function, NULL);
// }

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	const auto main_window =
		static_cast<QMainWindow *>(obs_frontend_get_main_window());
	auto *countdownWidget = new CountdownDockWidget(main_window);

	// countdownWidget->setFloating(true);
	obs_frontend_add_dock(countdownWidget);

	blog(LOG_INFO, "plugin loaded successfully (version %s)",
	     PLUGIN_VERSION);
	return true;
}

void obs_module_unload()
{
	blog(LOG_INFO, "plugin unloaded");
}
