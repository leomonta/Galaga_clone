#pragma once

#include <raylib.h>

void notif__scheduleNotification(const char *s, Vector2 pos, char cdown);
void notif__renderNotifications();
void notif__tick();