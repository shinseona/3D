#include "CollideTesting.h"

void CollideTesting::test(AmmoManager* ammoManager, vector<Enemy> enemies) {
/*	AlgBoxBoxAABB checker;
	vector<Ammo*> toDelete;
	bool collided = false;

//	list<Ammo*> ammos = ammoManager->getAmmos();
	for (auto it = ammos.begin(); it != ammos.end(); ++it) {
		Ammo* ammo = (Ammo*)*it;
		vec3 coordsBody1 = ammo->getModel().getCoords();
		vec3 volumeBody1 = ammo->getModel().getVolume();

		for (vec3 enemy : enemyCoords) {
			vec3 coordsBody2 = enemy.getModel().getCoords();
			vec3 volumeBody2 = enemy.getModel().getVolume();
			bool collided = checker.checkCollide(coordsBody1, coordsBody1 + volumeBody1,
				coordsBody2, coordsBody2 + volumeBody2);
			if (collided) {
				cout << "Collided with enemy" << endl;
				toDelete.push_back(ammo);
			}
		}
	}

	if (collided) {
		for (Ammo* ammo : toDelete) {
			ammoManager->removeAmmo(ammo);
		}
	}
	else
		cout << "Not collided\n";*/
}
