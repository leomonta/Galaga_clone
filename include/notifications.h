#pragma once

#include <raylib.h>

void notif__init();
void notif__schedule_notification(const char *s, Vector2 pos, unsigned char cdown);
void notif__render_notifications();
void notif__tick();
