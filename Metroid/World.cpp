﻿#include "World.h"
#include "GroupObject.h"
#include "Brick.h"
#include "Camera.h"
#include "BulletManager.h"
#include "MorphItem.h"
#include "EnergyItem.h"
#include "MissileItem.h"
#include "ExplosionEffect.h"
#include "PositionManager.h"
#include <vector>
#include "Energy.h"
#include "Number.h"
#include "Gate.h"
#include "utils.h"

using namespace std;

World::World()
{
}

World::World(LPD3DXSPRITE spriteHandler, Metroid * metroid)
{
	isOnFloor = false;

	//Gán
	this->spriteHandler = spriteHandler;
	this->metroid = metroid;

	//Khởi tạo các đối tượng trong World
	samus = new Samus(spriteHandler, this);
	//brick = new Brick(spriteHandler, this, BLUE, 1, 32*3, 32*10);

	//qtreeGroup = new GroupObject(this);
	quadtreeGroup = new GroupObject(this);
	
	rootQNode1 = NULL;
	rootQNode2 = NULL;

	bullets = new BulletManager(spriteHandler, this, STANDARD);
	missiles = new BulletManager(spriteHandler, this, MISSILE);
	icebeam = new BulletManager(spriteHandler, this, ICEBEAM);
	bomb = new BulletManager(spriteHandler, this, BOMB);
	sentrybullets = new BulletManager(spriteHandler, this, SENTRY_BULLET);
	birdbullets = new BulletManager(spriteHandler, this, BIRD_BULLET);
	ridleyBoomerang = new BulletManager(spriteHandler, this, BOOMERANG);

	morphItem = new MorphItem(spriteHandler, this);
	energyItem = new EnergyItem(spriteHandler, this);
	missileItem = new MissileItem(spriteHandler, this);

	explsEffect = new ExplosionEffect(spriteHandler, this);

	motherBrain = new MotherBrain(spriteHandler, this);
	ridley = new Ridley(spriteHandler, this);

	sentryLeft = new Sentry(spriteHandler, this, SENTRY_LEFT);
	sentryTop = new Sentry(spriteHandler, this, SENTRY_TOP);
	sentryRight = new Sentry(spriteHandler, this, SENTRY_RIGHT);

	collisionGroup = new GroupObject(this);
	effectgroup = new GroupObject(this);
	enemyGroup = new GroupObject(this);
	otherGO = new GroupObject(this);
	staticGroup = new GroupObject(this);

	gate = new Gate(spriteHandler, this, LEFT);
	gate->Init(1700, 150);

	
	// hog yellow
	for (int i = 0; i < BEDGEHOG_YELLOW_COUNT; i++)
	{
		hogs_yellow[i] = new Bedgehog(spriteHandler, this, BEDGEHOG_YELLOW);
		hogs_yellow[i]->SetActive(false);
		enemyGroup->AddGameObject(hogs_yellow[i]);
	}

	// hog pink
	for (int i = 0; i < BEDGEHOG_PINK_COUNT; i++)
	{
		hogs_pink[i] = new Bedgehog(spriteHandler, this, BEDGEHOG_PINK);
		hogs_pink[i]->SetActive(false);
		enemyGroup->AddGameObject(hogs_pink[i]);
	}

	// bird
	for (int i = 0; i < BIRD_COUNT; i++)
	{
		birds[i] = new Bird(spriteHandler, this, BIRD);
		birds[i]->SetActive(false);
		enemyGroup->AddGameObject(birds[i]);
	}

	// block
	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		blocks[i] = new Block(spriteHandler, this, BLOCK);
		blocks[i]->SetActive(false);
		enemyGroup->AddGameObject(blocks[i]);
	}

	// bee
	for (int i = 0; i < BEE_COUNT; i++)
	{
		bees[i] = new Bee(spriteHandler, this, BEE);
		bees[i]->SetActive(false);
		enemyGroup->AddGameObject(bees[i]);
	}

	// boss
	enemyGroup->AddGameObject(motherBrain);
	enemyGroup->AddGameObject(ridley);


	posManager = new PositionManager(this);
	posManager->ImportPositionFromFile();

	colFloorBrick = new GroupObject(this);
	colGroundBrick = new GroupObject(this);

	energy = new Energy(spriteHandler, this, ENERGYINFO);
	missileinfo = new Energy(spriteHandler, this, MISSILEINFO);

	numberofenergy1 = new Number(spriteHandler, this, CHUC, NUMBEROFENERGY);
	numberofenergy2 = new Number(spriteHandler, this, DONVI, NUMBEROFENERGY);
	numberofmissile1 = new Number(spriteHandler, this, CHUC, NUMBEROFMISSILE);
	numberofmissile2 = new Number(spriteHandler, this, DONVI, NUMBEROFMISSILE);
}


World::~World()
{
	delete(samus);
}

void World::Update(float t)
{
	//====> Quan update

	//Lấy danh sách các vị trí xuất hiện trong camera nhưng chưa được active
	std::vector<PostInfo> listEnemy = posManager->GetListInCamera((int)OBJECT_TYPE::ENEMY);
	for (int i = 0; i < listEnemy.size(); i++)
	{
		//enemyGroup->SetEnemyActive(list[i].detail_type, list[i].x, list[i].y);
		GroupObject::SetGameObjectActive(enemyGroup, listEnemy[i].object_type, listEnemy[i].detail_type, listEnemy[i].x, listEnemy[i].y);
	}
	
	otherGO->SetOtherGOActive();

	//<=============

	samus->Update(t);
	// Cập nhật các đối tượng hiển thị trong camera

	bullets->Update(t);
	missiles->Update(t);
	icebeam->Update(t);
	bomb->Update(t);

	birdbullets->Update(t);
	ridleyBoomerang->Update(t);

	morphItem->Update(t);
	energyItem->Update(t);
	missileItem->Update(t);

	explsEffect->Update(t);

	quadtreeGroup->GetCollisionObjectQTree(1);

	quadtreeGroup->Update(t);

	// Cập nhật các đối tượng có khả năng va chạm trong frame này
	collisionGroup->GetCollisionObjects();
	effectgroup->Update(t);
	collisionGroup->Update(t);

	sentryLeft->Update(t);
	sentryTop->Update(t);
	sentryRight->Update(t);
	sentrybullets->Update(t);

	energy->Update(t);
	missileinfo->Update(t);

	numberofenergy1->Update(t);
	numberofenergy2->Update(t);

	numberofmissile1->Update(t);
	numberofmissile2->Update(t);

	enemyGroup->UpdateActive(t);
	otherGO->UpdateActive(t);

	gate->Update(t);
}

void World::Render(LPDIRECT3DDEVICE9 d3ddv)
{
	samus->Render();

	bullets->Render();
	missiles->Render();
	icebeam->Render();
	bomb->Render();

	birdbullets->Render();
	ridleyBoomerang->Render();

	morphItem->Render();
	energyItem->Render();
	missileItem->Render();

	explsEffect->Render();
	quadtreeGroup->Render();

	sentryLeft->Render();
	sentryTop->Render();
	sentryRight->Render();
	sentrybullets->Render();
	energy->Render();
	missileinfo->Render();

	numberofenergy1->Render();
	numberofenergy2->Render();

	numberofmissile1->Render();
	numberofmissile2->Render();

	collisionGroup->Render();
	effectgroup->Render();
	enemyGroup->Render();
	otherGO->Render();

	gate->Render();
}

void World::InitSprites(LPDIRECT3DDEVICE9 d3ddv)
{
	samus->InitSprites(d3ddv);
	bullets->InitSprites(d3ddv);
	missiles->InitSprites(d3ddv);
	icebeam->InitSprites(d3ddv);
	bomb->InitSprites(d3ddv);

	sentrybullets->InitSprites(d3ddv);
	birdbullets->InitSprites(d3ddv);
	ridleyBoomerang->InitSprites(d3ddv);

	morphItem->InitSprites(d3ddv);
	energyItem->InitSprites(d3ddv);
	missileItem->InitSprites(d3ddv);

	explsEffect->InitSprites(d3ddv);
}
