#pragma once

#include <raylib.h>

void notif__init();
void notif__scheduleNotification(const char *s, Vector2 pos, unsigned char cdown);
void notif__renderNotifications();
void notif__tick();