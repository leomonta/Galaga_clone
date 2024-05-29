
#include "notifications.h"

#include "constants.h"

typedef struct notifRes {
	const char   *texts[MAX_NOTIFICATIONS];
	unsigned char countDowns[MAX_NOTIFICATIONS];
	Vector2       positions[MAX_NOTIFICATIONS];
	int           index;
} notifRes;

notifRes nfRes;

/**
 * Initilizes essential variables for the correct functioning of the system
 */
void notif__init() {
	// just this lol
	nfRes.index = 0;
}

/**
 * Insert the notification int the queue
 */
void notif__schedule_notification(const char *s, Vector2 pos, unsigned char cdown) {
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
void notif__render_notifications() {

	for (int i = 0; i < MAX_NOTIFICATIONS; ++i) {

		Color col = WHITE;

		if (nfRes.texts[i] == nullptr) {
			continue;
		}

		col.a = nfRes.countDowns[i];

		int posX = (int)(nfRes.positions[i].x);
		int posY = (int)(nfRes.positions[i].y);
		DrawText(nfRes.texts[i], posX, posY, 7, col);
	}
}
