﻿#include "Metroid.h"
#include <time.h>
#include "trace.h"
#include "utils.h"
#include "BulletManager.h"
#include "World.h"
#include "Samus.h"
#include "MorphItem.h"
#include "ExplosionEffect.h"
#include "BulletObject.h"

void Metroid::_InitBackground()
{
}

void Metroid::_InitSprites(LPDIRECT3DDEVICE9 d3ddv)
{
	world->InitSprites(d3ddv);
	//tiles->InitSprites(d3ddv);
	
}

void Metroid::_InitPositions()
{
	world->samus->InitPostition();
	world->morphItem->Init(704, 186);
}


void Metroid::_Shoot(BULLET_DIRECTION dir)
{
	now_shoot = GetTickCount();
	if (start_shoot <= 0) //if shooting is active
	{
		start_shoot = GetTickCount();
		world->bullets->Next(dir, world->samus->GetPosX(), world->samus->GetPosY());
	}
	else if ((now_shoot - start_shoot) > SHOOTING_SPEED * tick_per_frame)
	{
		//Reset start_shoot
		start_shoot = 0;
	}
}

void Metroid::_ShootMissile(BULLET_DIRECTION dir)
{
	world->missiles->Next(dir, world->samus->GetPosX(), world->samus->GetPosY());
}

void Metroid::_ShootIceBeam(BULLET_DIRECTION dir)
{
	world->icebeam->Next(dir, world->samus->GetPosX(), world->samus->GetPosY());
}

void Metroid::_SetBomb(BULLET_DIRECTION dir)
{
	world->bomb->Next(dir, world->samus->GetPosX(), world->samus->GetPosY());
}

Metroid::Metroid(HINSTANCE hInstance, LPWSTR Name, int Mode, int IsFullScreen, int FrameRate):Game(hInstance, Name, Mode, IsFullScreen, FrameRate)
{
	//tiles = new Tiles();
	tick_per_frame = 1000 / _FrameRate;
	start_shoot = 0;

	time_jump = 3 * _DeltaTime;

	isFreezing = false;
	time_freezing = TIME_FREEZING;
	time_in_game = TIME_IN_GAME;
	isOnFloor = false;
	isInGame = false;

	//bulletManager = new BulletManager();
}


Metroid::~Metroid()
{
	//delete(tiles);
	
	//delete(bulletManager);

	delete(first_room);
	//delete(second_room);
	delete(room);
}

void Metroid::LoadResources(LPDIRECT3DDEVICE9 d3ddv)
{
	//---------Khởi tạo spriteHandler---------------
	if (d3ddv == NULL) return;
	//Create sprite handler
	HRESULT result = D3DXCreateSprite(d3ddv, &spriteHandler);
	if (result != D3D_OK) return;
	//-----------------------
	world = new World(spriteHandler, this);
	srand((unsigned)time(NULL));
	_InitSprites(d3ddv);
	_InitPositions();

	//room = new Loader(spriteHandler, 1, world);
	first_room = new Loader(spriteHandler, 1, world);
	//room->Load();
	//second_room = new Loader(spriteHandler, 2, world);
	first_room->Load();
	//second_room->Load();

	Game::gameSound->playSoundLoop(BACKGROUND_INTRO);
}

void Metroid::Update(float Delta)
{
	switch (screenMode)
	{
		// intro
	case GAMEMODE_INTRO:
		break;
		// start screen
	case GAMEMODE_START:
		break;
		// game running
	case GAMEMODE_GAMERUN:
		UpdateFrame(_DeltaTime);
		break;
		// game over
	case GAMEMODE_GAMEOVER:
	default:
		break;
	}
}

void Metroid::UpdateFrame(float Delta)
{	
	if (isInGame)
	{
		for (int i = 0; i < 4; i++)
			world->hogs_yellow[i]->SetActive(false);
		time_in_game -= Delta;
		if (time_in_game <= 0)
		{
			Game::gameSound->stopSound(BACKGROUND_APPEARANCE);
			Game::gameSound->playSoundLoop(BACKGROUND_MAP);
			world->samus->SetState(IDLE_RIGHT);
			isInGame = false;
		}
	}

	if (isFreezing)
	{
		time_freezing -= Delta;
		if (time_freezing <= 0)
		{
			Game::gameSound->playSoundLoop(BACKGROUND_MAP);
			isFreezing = false;
			time_freezing = TIME_FREEZING;
		}
		return;
	}

	world->Update(Delta);
	//Camera::MoveCameraX(0.05f, Delta);
	//bulletManager->Update(Delta, world->samus->GetPosX(), world->samus->GetPosY());
	if (world->samus->isSamusDeath() == true)
	{
		screenMode = GAMEMODE_GAMEOVER;
		return;
	}
}

void Metroid::Render(LPDIRECT3DDEVICE9 d3ddv)
{
	switch (screenMode)
	{
		// intro
	case GAMEMODE_INTRO:
		RenderIntro(d3ddv);
		break;
		// start screen
	case GAMEMODE_START:
		RenderStartScreen(d3ddv);
		break;
		// game running
	case GAMEMODE_GAMERUN:
		RenderFrame(d3ddv);	
		break;
		// game over
	case GAMEMODE_GAMEOVER:
		RenderGameOver(d3ddv);
	default:
		break;
	}
}

void Metroid::RenderStartScreen(LPDIRECT3DDEVICE9 d3ddv) 
{
	// Background
	d3ddv->StretchRect(
		startscreen,		// from 
		NULL,				// which portion?
		_BackBuffer,		// to 
		NULL,				// which portion?
		D3DTEXF_NONE);
	startscreen = CreateSurfaceFromFile(_d3ddv, STARTSCREEN_FILE);
}

void Metroid::RenderIntro(LPDIRECT3DDEVICE9 d3ddv)
{
	d3ddv->StretchRect(
		introscreen,		// from 
		NULL,				// which portion?
		_BackBuffer,		// to 
		NULL,				// which portion?
		D3DTEXF_NONE);
	introscreen = CreateSurfaceFromFile(_d3ddv, INTROSCREEN_FILE);
}

void Metroid::RenderFrame(LPDIRECT3DDEVICE9 d3ddv)
{		
	world->Render(d3ddv);

	if (isOnFloor)
		//second_room->TestRenderMapGO();
		first_room->TestRenderMapGO();
	//room->TestRenderMapGO();
	//first_room->TestRenderMapGO();
	//bulletManager->Render();
	//tiles->_Render(xc, world->samus->GetPosX());	
}

void Metroid::RenderGameOver(LPDIRECT3DDEVICE9 d3ddv)
{
	d3ddv->StretchRect(
		gameoverscreen,		// from 
		NULL,				// which portion?
		_BackBuffer,		// to 
		NULL,				// which portion?
		D3DTEXF_NONE);
	gameoverscreen = CreateSurfaceFromFile(_d3ddv, GAMEOVERSCREEN_FILE);
}

void Metroid::ProcessInput(LPDIRECT3DDEVICE9 d3ddv, float Delta)
{
	if (isInGame)
		return;

	//float c = Delta;
	if (IsKeyDown(DIK_RIGHT))
	{

		/*Camera::currentCamX += 2.0f;
		float c = Camera::currentCamY + 5;*/

		//world->samus->setNormalx(1.0f);
		//world->samus->setgravity(FALLDOWN_VELOCITY_DECREASE);
		world->samus->SetVelocityXLast(world->samus->GetVelocityX());
		world->samus->SetVelocityX(SAMUS_SPEED);
		if (world->samus->GetState() != ON_MORPH_LEFT && world->samus->GetState() != ON_MORPH_RIGHT
			&& world->samus->GetState() != ON_JUMP_LEFT && world->samus->GetState() != ON_JUMP_RIGHT
			&& world->samus->GetState() != ON_JUMPING_SHOOTING_LEFT && world->samus->GetState() != ON_JUMPING_SHOOTING_RIGHT
			&& world->samus->GetState() != ON_SOMERSAULT_LEFT && world->samus->GetState() != ON_SOMERSAULT_RIGHT)
		{
			world->samus->SetState(RIGHTING);
		}

		if (world->samus->GetState() == ON_MORPH_LEFT)
			world->samus->SetState(ON_MORPH_RIGHT);
		if (world->samus->GetState() == ON_SOMERSAULT_LEFT)
			world->samus->SetState(ON_SOMERSAULT_RIGHT);
		if (world->samus->GetState() == ON_JUMP_LEFT)
			world->samus->SetState(ON_JUMP_RIGHT);
		if (world->samus->GetState() == ON_JUMPING_SHOOTING_LEFT || world->samus->GetState() == ON_JUMP_AIM_UP_LEFT)
			world->samus->SetState(ON_JUMPING_SHOOTING_RIGHT);
		if (world->samus->GetState() == RIGHTING && (IsKeyDown(DIK_C) || IsKeyDown(DIK_V)))
			world->samus->SetState(ON_RUN_SHOOTING_RIGHT);
	}
	else if (IsKeyDown(DIK_LEFT))
	{
		//Camera::currentCamX -= 2.0f;

		//world->samus->setgravity(FALLDOWN_VELOCITY_DECREASE);
		//world->samus->setNormalx(-1.0f);
		world->samus->SetVelocityXLast(world->samus->GetVelocityX());
		world->samus->SetVelocityX(-SAMUS_SPEED);
		if (world->samus->GetState() != ON_MORPH_LEFT && world->samus->GetState() != ON_MORPH_RIGHT
			&& world->samus->GetState() != ON_JUMP_LEFT && world->samus->GetState() != ON_JUMP_RIGHT
			&& world->samus->GetState() != ON_JUMPING_SHOOTING_LEFT && world->samus->GetState() != ON_JUMPING_SHOOTING_RIGHT
			&& world->samus->GetState() != ON_SOMERSAULT_LEFT && world->samus->GetState() != ON_SOMERSAULT_RIGHT)
		{
			world->samus->SetState(LEFTING);
		}

		if (world->samus->GetState() == ON_MORPH_RIGHT)
			world->samus->SetState(ON_MORPH_LEFT);
		if (world->samus->GetState() == ON_SOMERSAULT_RIGHT)
			world->samus->SetState(ON_SOMERSAULT_LEFT);
		if (world->samus->GetState() == ON_JUMP_RIGHT)
			world->samus->SetState(ON_JUMP_LEFT);
		if (world->samus->GetState() == ON_JUMPING_SHOOTING_RIGHT || world->samus->GetState() == ON_JUMP_AIM_UP_RIGHT)
			world->samus->SetState(ON_JUMPING_SHOOTING_LEFT);
		if (world->samus->GetState() == LEFTING && (IsKeyDown(DIK_C) || IsKeyDown(DIK_V)))
			world->samus->SetState(ON_RUN_SHOOTING_LEFT);
	}
	else
	{
		world->samus->SetVelocityX(0);

		if (world->samus->GetVelocityXLast() < 0)
		{
			if (world->samus->GetState() != ON_MORPH_LEFT && world->samus->GetState() != ON_JUMP_LEFT && world->samus->GetState() != ON_JUMP_RIGHT
				&& world->samus->GetState() != ON_JUMPING_SHOOTING_LEFT && world->samus->GetState() != ON_JUMPING_SHOOTING_RIGHT
				&& world->samus->GetState() != ON_SOMERSAULT_LEFT && world->samus->GetState() != ON_SOMERSAULT_RIGHT
				&& world->samus->GetState() != ON_JUMP_AIM_UP_LEFT && world->samus->GetState() != APPEARANCE)
			{
				world->samus->SetState(IDLE_LEFT);
				world->samus->ResetAllSprites();
			}
		}
		else
		{
			if (world->samus->GetState() != ON_MORPH_RIGHT && world->samus->GetState() != ON_JUMP_LEFT && world->samus->GetState() != ON_JUMP_RIGHT
				&& world->samus->GetState() != ON_JUMPING_SHOOTING_LEFT && world->samus->GetState() != ON_JUMPING_SHOOTING_RIGHT
				&& world->samus->GetState() != ON_SOMERSAULT_LEFT && world->samus->GetState() != ON_SOMERSAULT_RIGHT
				&& world->samus->GetState() != ON_JUMP_AIM_UP_RIGHT && world->samus->GetState() != APPEARANCE)
			{
				world->samus->SetState(IDLE_RIGHT);
				world->samus->ResetAllSprites();
			}
		}
	}

	if (IsKeyDown(DIK_X))
	{
		if (world->samus->GetPosY() < jumpDistance + 80)
			if (world->samus->GetVelocityY() > 0)
				world->samus->SetVelocityY(JUMP_VELOCITY_BOOST);
	}

	if (IsKeyDown(DIK_UP))
	{
		if (world->samus->GetState() == LEFTING)
			world->samus->SetState(AIMING_UP_LEFT);
		if (world->samus->GetState() == RIGHTING)
			world->samus->SetState(AIMING_UP_RIGHT);
		if (world->samus->GetState() == IDLE_LEFT)
			world->samus->SetState(IDLING_AIM_UP_LEFT);
		if (world->samus->GetState() == IDLE_RIGHT)
			world->samus->SetState(IDLING_AIM_UP_RIGHT);
		if (world->samus->GetState() == ON_JUMP_LEFT/* || world->samus->GetState() == ON_JUMPING_SHOOTING_LEFT*/)
			world->samus->SetState(ON_JUMP_AIM_UP_LEFT);
		if (world->samus->GetState() == ON_JUMP_RIGHT/* || world->samus->GetState() == ON_JUMPING_SHOOTING_RIGHT*/)
			world->samus->SetState(ON_JUMP_AIM_UP_RIGHT);
		/*if (world->samus->GetState() == ON_MORPH_LEFT)
			world->samus->SetState(IDLE_LEFT);
		if (world->samus->GetState() == ON_MORPH_RIGHT)
			world->samus->SetState(IDLE_RIGHT);*/
		if (world->samus->GetState() == ON_SOMERSAULT_LEFT)
			world->samus->SetState(ON_JUMP_AIM_UP_LEFT);
		if (world->samus->GetState() == ON_SOMERSAULT_RIGHT)
			world->samus->SetState(ON_JUMP_AIM_UP_RIGHT);

		if (world->samus->GetState() == ON_JUMPING_SHOOTING_LEFT || world->samus->GetState() == ON_JUMP_AIM_UP_LEFT)
			world->samus->SetState(ON_JUMP_AIM_UP_LEFT);
		if (world->samus->GetState() == ON_JUMPING_SHOOTING_RIGHT || world->samus->GetState() == ON_JUMP_AIM_UP_RIGHT)
			world->samus->SetState(ON_JUMP_AIM_UP_RIGHT);
	}

	if (IsKeyDown(DIK_Z))
	{
		//Để mặc định đạn thường trước
		Game::gameSound->playSound(SHOOT_BULLET);
		//State Đứng bắn lên
		if (world->samus->GetState() == IDLING_AIM_UP_LEFT)
		{
			world->samus->SetState(IDLING_SHOOTING_UP_LEFT);

			_Shoot(ON_UP);
		}
		if (world->samus->GetState() == IDLING_AIM_UP_RIGHT)
		{
			world->samus->SetState(IDLING_SHOOTING_UP_RIGHT);

			_Shoot(ON_UP);
		}
		//State Chạy bắn lên
		if (world->samus->GetState() == AIMING_UP_LEFT)
		{
			world->samus->SetState(AIMING_UP_LEFT);
			_Shoot(ON_UP);
		}
		if (world->samus->GetState() == AIMING_UP_RIGHT)
		{
			world->samus->SetState(AIMING_UP_RIGHT);
			_Shoot(ON_UP);
		}
		//State Nhảy bắn lên
		if (world->samus->GetState() == ON_JUMP_AIM_UP_LEFT || (world->samus->GetState() == ON_JUMP_AIM_UP_LEFT && IsKeyDown(DIK_LEFT)))
		{
			world->samus->SetState(ON_JUMP_AIM_UP_LEFT);

			_Shoot(ON_UP);
		}
		if (world->samus->GetState() == ON_JUMP_AIM_UP_RIGHT || (world->samus->GetState() == ON_JUMP_AIM_UP_RIGHT && IsKeyDown(DIK_RIGHT)))
		{
			world->samus->SetState(ON_JUMP_AIM_UP_RIGHT);

			_Shoot(ON_UP);
		}
		//State nhảy bắn
		if (world->samus->GetState() == ON_JUMP_LEFT || world->samus->GetState() == ON_SOMERSAULT_LEFT || world->samus->GetState() == ON_JUMPING_SHOOTING_LEFT)
		{
			world->samus->SetState(ON_JUMPING_SHOOTING_LEFT);

			_Shoot(ON_LEFT);
		}
		if (world->samus->GetState() == ON_JUMP_RIGHT || world->samus->GetState() == ON_SOMERSAULT_RIGHT || world->samus->GetState() == ON_JUMPING_SHOOTING_RIGHT)
		{
			world->samus->SetState(ON_JUMPING_SHOOTING_RIGHT);

			_Shoot(ON_RIGHT);
		}
		//State chạy bắn
		if (world->samus->GetState() == LEFTING)
		{
			world->samus->SetState(ON_RUN_SHOOTING_LEFT);

			_Shoot(ON_LEFT);
		}
		if (world->samus->GetState() == RIGHTING)
		{
			world->samus->SetState(ON_RUN_SHOOTING_RIGHT);

			_Shoot(ON_RIGHT);
		}
		//State đứng bắn
		if (world->samus->GetState() == IDLE_LEFT)
		{
			world->samus->SetState(IDLING_SHOOTING_LEFT);

			_Shoot(ON_LEFT);
		}
		if (world->samus->GetState() == IDLE_RIGHT)
		{
			world->samus->SetState(IDLING_SHOOTING_RIGHT);

			_Shoot(ON_RIGHT);
		}

		BulletObject ** list = world->samus->getlistbullet();
		int num = world->samus->getNumBullet();
		for (int i = 0; i < world->enemyGroup->size; i++)
		{
			for (int j = 0; j < num; j++)
			{
				float TimeScale = world->enemyGroup->objects[i]->SweptAABB(list[j], Delta);
				if (TimeScale < 1.0f)
				{
					if (world->enemyGroup->objects[i]->IsActive())
					{
						float damage = DAMAGE_SAMUS_BULLET;
						((Enemy*)(world->enemyGroup->objects[i]))->DeathByShoot = true;
						((Enemy*)(world->enemyGroup->objects[i]))->TakeDamage(damage);
						//list[j]->Reset();
						break;
					}
				}

			}
		}
		/*	for (int j = 0; j < num; j++)
			{
				float TimeScale1 = list[j]->SweptAABB(world->gateleft, Delta);
				if (TimeScale1 < 1.0f)
				{
				world->gateleft->SetActive(false);
					break;
				}

				float TimeScale2 = list[j]->SweptAABB(world->gateright, Delta);
				if (TimeScale2 < 1.0f)
				{
					world->gateright->SetActive(false);
					break;
				}
			}*/
	}


	/*if (IsKeyDown(DIK_UP))
		Camera::currentCamY += 2.0f;

	if (IsKeyDown(DIK_DOWN))
		Camera::currentCamY -= 2.0f;*/

	if (world->samus->GetVelocityY() < 0)
	{
		//world->samus->setNormaly(-1.0f);
	}
}

void Metroid::OnKeyDown(int KeyCode)
{
	switch (screenMode)
	{
		// intro
		case GAMEMODE_INTRO:
		{
			if (KeyCode == DIK_RETURN)
			{
				screenMode = GAMEMODE_START;
			}
			break;
		}
		// start screen
		case GAMEMODE_START://------------------------------------------------
		{
			if (KeyCode == DIK_RETURN)
			{
				screenMode = GAMEMODE_GAMERUN;
				Game::gameSound->stopSound(BACKGROUND_INTRO);
				Game::gameSound->playSound(BACKGROUND_APPEARANCE);
				isInGame = true;
			}
		}
		break;
		// game running
		case GAMEMODE_GAMERUN:// -------------------------------------------------
		{
			switch (KeyCode)
			{
			case DIK_X:
				if (world->samus->isSamusOnAir() == false && world->samus->GetState() != ON_MORPH_LEFT && world->samus->GetState() != ON_MORPH_RIGHT)
				{
					Game::gameSound->playSound(JUMP);
					world->samus->isOnAir = true;
					jumpDistance = world->samus->GetPosY();
					world->samus->setNormaly(1.0f);
					if (world->samus->GetState() != ON_SOMERSAULT_RIGHT && IsKeyDown(DIK_RIGHT))/*&& samus->GetState() != ON_JUMP_AIM_UP_RIGHT*/
					{
						world->samus->SetState(ON_SOMERSAULT_RIGHT);
						world->samus->SetVelocityY(world->samus->GetVelocityY() + JUMP_VELOCITY_BOOST_FIRST);

					}

					if (world->samus->GetState() != ON_SOMERSAULT_LEFT && IsKeyDown(DIK_LEFT))/*&& samus->GetState() != ON_JUMP_AIM_UP_LEFT*/
					{
						world->samus->SetState(ON_SOMERSAULT_LEFT);
						world->samus->SetVelocityY(world->samus->GetVelocityY() + JUMP_VELOCITY_BOOST_FIRST);

					}

					if (world->samus->GetVelocityXLast() < 0)
					{
						if (world->samus->GetState() != ON_JUMP_LEFT && world->samus->GetState() != ON_SOMERSAULT_LEFT
							&& world->samus->GetState() != ON_JUMPING_SHOOTING_LEFT && world->samus->GetState() != ON_JUMP_AIM_UP_LEFT)
						{
							if (world->samus->GetState() == IDLING_AIM_UP_LEFT)
								world->samus->SetState(ON_JUMP_AIM_UP_LEFT);
							else
								world->samus->SetState(ON_JUMP_LEFT);
							world->samus->SetVelocityY(world->samus->GetVelocityY() + JUMP_VELOCITY_BOOST_FIRST);

						}
					}
					if (world->samus->GetVelocityXLast() > 0)
					{
						if (world->samus->GetState() != ON_JUMP_RIGHT && world->samus->GetState() != ON_SOMERSAULT_RIGHT
							&& world->samus->GetState() != ON_JUMPING_SHOOTING_RIGHT && world->samus->GetState() != ON_JUMP_AIM_UP_RIGHT)
						{
							if (world->samus->GetState() == IDLING_AIM_UP_RIGHT)
								world->samus->SetState(ON_JUMP_AIM_UP_RIGHT);
							else
								world->samus->SetState(ON_JUMP_RIGHT);
							world->samus->SetVelocityY(world->samus->GetVelocityY() + JUMP_VELOCITY_BOOST_FIRST);

						}
					}
				}
			case DIK_DOWN:
				//if samus is idle then do morph
				if (world->samus->isSamusCanMorph() == true)
				{
					if (world->samus->GetState() == IDLE_LEFT)
					{
						world->samus->isCrouch = true;
						world->samus->SetVelocityX(0);
						world->samus->ResetAllSprites();
						world->samus->SetState(ON_MORPH_LEFT);
						world->samus->GetCollider()->SetCollider(0, 0, -MORPH_BALL_HEIGHT, MORPH_BALL_WIDTH);
					}
					else if (world->samus->GetState() == IDLE_RIGHT)
					{
						world->samus->isCrouch = true;
						world->samus->SetVelocityX(0);
						world->samus->ResetAllSprites();
						world->samus->SetState(ON_MORPH_RIGHT);
						world->samus->GetCollider()->SetCollider(0, 0, -MORPH_BALL_HEIGHT, MORPH_BALL_WIDTH);
					}
					else if (world->samus->GetState() == ON_MORPH_LEFT) //otherwise, reset to idle (left of right)
					{
						world->samus->isCrouch = false;
						world->samus->SetVelocityX(0);
						world->samus->ResetAllSprites();
						world->samus->SetState(IDLE_LEFT);
						//world->samus->setDirection(DirectCollision::RIGHT);
						world->samus->SetPosY(world->samus->GetPosY() + world->samus->GetHeight() - 16);
						world->samus->GetCollider()->SetCollider(0, 0, -world->samus->GetHeight(), world->samus->GetWidth());
					}
					else if (world->samus->GetState() == ON_MORPH_RIGHT)
					{
						world->samus->isCrouch = false;
						world->samus->SetVelocityX(0);
						world->samus->ResetAllSprites();
						world->samus->SetState(IDLE_RIGHT);
						world->samus->SetPosY(world->samus->GetPosY() + world->samus->GetHeight() - 16);
						world->samus->GetCollider()->SetCollider(0, 0, -world->samus->GetHeight(), world->samus->GetWidth());
						//world->samus->setDirection(DirectCollision::LEFT);
					}
					break;
				}
				//case DIK_X:
				//	//**********************************************************************************
				//	// [CAUTION!!!] Vi pos_y chua chinh theo toa do World, code duoi day chi la tam thoi,
				//	// se cap nhat lai sau
				//	if (samus->GetPosY() >= GROUND_Y)
				//	{
				//		if (samus->GetVelocityXLast() < 0)
				//		{
				//			samus->SetState(ON_JUMP_LEFT);
				//			samus->SetVelocityY(samus->GetVelocityY() - JUMP_VELOCITY_BOOST);
				//		}
				//		else if (samus->GetVelocityXLast() > 0)
				//		{
				//			samus->SetState(ON_JUMP_RIGHT);
				//			samus->SetVelocityY(samus->GetVelocityY() - JUMP_VELOCITY_BOOST);
				//		}
				//	}
				//	//***********************************************************************************
				//	break;
			case DIK_LEFT:
				/*if (samus->GetState() == ON_JUMP_RIGHT)
					samus->SetState(ON_JUMP_LEFT);*/
				if (world->samus->GetState() == ON_MORPH_RIGHT)
					world->samus->SetState(ON_MORPH_LEFT);
				/*else if (samus->GetState() != ON_MORPH_LEFT && samus->GetState() != ON_JUMP_LEFT && samus->GetState() != ON_JUMP_RIGHT
					&& samus->GetState() != ON_SOMERSAULT_RIGHT)
					samus->SetState(LEFTING);*/
				break;
			case DIK_RIGHT:
				/*if (samus->GetState() == ON_JUMP_LEFT)
					samus->SetState(ON_JUMP_RIGHT);*/
				if (world->samus->GetState() == ON_MORPH_LEFT)
					world->samus->SetState(ON_MORPH_RIGHT);
				/*else if (world->samus->GetState() != ON_MORPH_RIGHT && world->samus->GetState() != ON_JUMP_LEFT && world->samus->GetState() != ON_JUMP_RIGHT
					&& world->samus->GetState() != ON_SOMERSAULT_LEFT)
					world->samus->SetState(RIGHTING);*/
				break;

			case DIK_C:
			{
				if (world->samus->GetMissileNumbers() > 0)
				{
					Game::gameSound->playSound(SHOOT_MISSILE);
					world->samus->SetMissileNumbers(world->samus->GetMissileNumbers() - 1);
					if (world->samus->GetState() == IDLING_AIM_UP_LEFT)
					{
						world->samus->SetState(IDLING_SHOOTING_UP_LEFT);

						_ShootMissile(ON_UP);
					}
					if (world->samus->GetState() == IDLING_AIM_UP_RIGHT)
					{
						world->samus->SetState(IDLING_SHOOTING_UP_RIGHT);

						_ShootMissile(ON_UP);
					}
					//State Chạy bắn lên
					if (world->samus->GetState() == AIMING_UP_LEFT)
					{
						world->samus->SetState(AIMING_UP_LEFT);
						_ShootMissile(ON_UP);
					}
					if (world->samus->GetState() == AIMING_UP_RIGHT)
					{
						world->samus->SetState(AIMING_UP_RIGHT);
						_ShootMissile(ON_UP);
					}
					//State Nhảy bắn lên => bug
					if (world->samus->GetState() == ON_JUMP_AIM_UP_LEFT)
					{
						world->samus->SetState(ON_JUMP_AIM_UP_LEFT);

						_ShootMissile(ON_UP);
					}
					if (world->samus->GetState() == ON_JUMP_AIM_UP_RIGHT)
					{
						world->samus->SetState(ON_JUMP_AIM_UP_RIGHT);

						_ShootMissile(ON_UP);
					}
					//State nhảy bắn
					if (world->samus->GetState() == ON_JUMP_LEFT || world->samus->GetState() == ON_SOMERSAULT_LEFT || world->samus->GetState() == ON_JUMPING_SHOOTING_LEFT)
					{
						world->samus->SetState(ON_JUMPING_SHOOTING_LEFT);

						_ShootMissile(ON_LEFT);
					}
					if (world->samus->GetState() == ON_JUMP_RIGHT || world->samus->GetState() == ON_SOMERSAULT_RIGHT || world->samus->GetState() == ON_JUMPING_SHOOTING_RIGHT)
					{
						world->samus->SetState(ON_JUMPING_SHOOTING_RIGHT);

						_ShootMissile(ON_RIGHT);
					}
					//State chạy bắn
					if (world->samus->GetState() == LEFTING)
					{
						world->samus->SetState(ON_RUN_SHOOTING_LEFT);

						_ShootMissile(ON_LEFT);
					}
					if (world->samus->GetState() == RIGHTING)
					{
						world->samus->SetState(ON_RUN_SHOOTING_RIGHT);

						_ShootMissile(ON_RIGHT);
					}
					//State đứng bắn
					if (world->samus->GetState() == IDLE_LEFT)
					{
						world->samus->SetState(IDLING_SHOOTING_LEFT);

						_ShootMissile(ON_LEFT);
					}
					if (world->samus->GetState() == IDLE_RIGHT)
					{
						world->samus->SetState(IDLING_SHOOTING_RIGHT);

						_ShootMissile(ON_RIGHT);

					}
				}
				break;
			}

			case DIK_V:
				Game::gameSound->playSound(SHOOT_ICEBEAM);
				if (world->samus->GetState() == IDLING_AIM_UP_LEFT)
				{
					world->samus->SetState(IDLING_SHOOTING_UP_LEFT);

					_ShootIceBeam(ON_UP);
				}
				if (world->samus->GetState() == IDLING_AIM_UP_RIGHT)
				{
					world->samus->SetState(IDLING_SHOOTING_UP_RIGHT);

					_ShootIceBeam(ON_UP);
				}
				//State Chạy bắn lên
				if (world->samus->GetState() == AIMING_UP_LEFT)
				{
					world->samus->SetState(AIMING_UP_LEFT);
					_ShootIceBeam(ON_UP);
				}
				if (world->samus->GetState() == AIMING_UP_RIGHT)
				{
					world->samus->SetState(AIMING_UP_RIGHT);
					_ShootIceBeam(ON_UP);
				}
				//State Nhảy bắn lên => bug
				if (world->samus->GetState() == ON_JUMP_AIM_UP_LEFT)
				{
					world->samus->SetState(ON_JUMP_AIM_UP_LEFT);

					_ShootIceBeam(ON_UP);
				}
				if (world->samus->GetState() == ON_JUMP_AIM_UP_RIGHT)
				{
					world->samus->SetState(ON_JUMP_AIM_UP_RIGHT);

					_ShootIceBeam(ON_UP);
				}
				//State nhảy bắn
				if (world->samus->GetState() == ON_JUMP_LEFT || world->samus->GetState() == ON_SOMERSAULT_LEFT || world->samus->GetState() == ON_JUMPING_SHOOTING_LEFT)
				{
					world->samus->SetState(ON_JUMPING_SHOOTING_LEFT);

					_ShootIceBeam(ON_LEFT);
				}
				if (world->samus->GetState() == ON_JUMP_RIGHT || world->samus->GetState() == ON_SOMERSAULT_RIGHT || world->samus->GetState() == ON_JUMPING_SHOOTING_RIGHT)
				{
					world->samus->SetState(ON_JUMPING_SHOOTING_RIGHT);

					_ShootIceBeam(ON_RIGHT);
				}
				//State chạy bắn
				if (world->samus->GetState() == LEFTING)
				{
					world->samus->SetState(ON_RUN_SHOOTING_LEFT);

					_ShootIceBeam(ON_LEFT);
				}
				if (world->samus->GetState() == RIGHTING)
				{
					world->samus->SetState(ON_RUN_SHOOTING_RIGHT);

					_ShootIceBeam(ON_RIGHT);
				}
				//State đứng bắn
				if (world->samus->GetState() == IDLE_LEFT)
				{
					world->samus->SetState(IDLING_SHOOTING_LEFT);

					_ShootIceBeam(ON_LEFT);
				}
				if (world->samus->GetState() == IDLE_RIGHT)
				{
					world->samus->SetState(IDLING_SHOOTING_RIGHT);

					_ShootIceBeam(ON_RIGHT);

				}
				break;
			case DIK_SPACE:
				if (world->samus->isSamusCrouch() == true)
				{
					Game::gameSound->playSound(SET_BOMB);
					_SetBomb(ON_UP);
				}
				break;
			}
		}
		// game over
		case GAMEMODE_GAMEOVER://------------------------------------------------
		{
			if (KeyCode == DIK_RETURN)
			{
				screenMode = GAMEMODE_INTRO;
				Game::gameSound->playSound(BACKGROUND_INTRO);
				world->samus->Reset(1275, 150);
			}
			break;
		}
	}
}

DWORD Metroid::GetTickPerFrame()
{
	return tick_per_frame;
}
