
#include "notifications.h"

#include "constants.h"

typedef struct notifRes {
	const char *texts[MAX_NOTIFICATIONS];
	char        countDowns[MAX_NOTIFICATIONS];
	Vector2     positions[MAX_NOTIFICATIONS];
	int         index;
} notifRes;

notifRes nfRes;

/**
 * Insert the notification int the queue
 */
void notif__scheduleNotification(const char *s, Vector2 pos, char cdown) {
	nfRes.texts[nfRes.index]      = s;
	nfRes.countDowns[nfRes.index] = cdown;
	nfRes.positions[nfRes.index]  = pos;
	nfRes.index++;

	if (nfRes.index == MAX_NOTIFICATIONS) {
		nfRes.index = 0;
	}
}

/**
 * Decrease the countDowns
 */
void notif__tick() {
	for (int i = 0; i < MAX_NOTIFICATIONS; ++i) {
		if (nfRes.countDowns[i] == 0) {
			nfRes.texts[i] = nullptr;
		} else {
			nfRes.countDowns[i]--;
			nfRes.positions[i].y += 1;
		}
	}
}

/**
 * Draw the actual notifications
 */
void notif__renderNotifications() {

	for (int i = 0; i < MAX_NOTIFICATIONS; ++i) {

		Color col = WHITE;

		if (nfRes.texts[i] == nullptr) {
			continue;
		}

		col.a = nfRes.countDowns[i];

		int posX = (nfRes.positions[i].x);
		int posY = (nfRes.positions[i].y);
		DrawText(nfRes.texts[i], posX, posY, 7, col);
	}
}