#include "stdafx.h"
#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
	LoadGameImage(); // 클라이언트에 사용될 모든 게임 이미지를 로드한다.
}

ResourceManager::~ResourceManager()
{
	/*LogoImage.Destroy();
	TitleImage.Destroy();

	ButtonImage[0].Destroy();
	ButtonImage[1].Destroy();
	ButtonImage[2].Destroy();
	GaugeBar.Destroy();
	QuickSlotImage[0].Destroy();
	QuickSlotImage[1].Destroy();

	MapImage[0].Destroy();

	for (auto &img : PlayerImage)
		img.Destroy();

	for (auto &img : MonsterImage)
		img.Destroy();

	for (auto &img : ItemImage)
		img.Destroy();

	EffectImage001.Destroy();*/
}

const CImage & ResourceManager::GetButtonImage(unsigned char index) const
{
	if (index < sizeof(ButtonImage) / sizeof(ButtonImage[0])) 
		return ButtonImage[index];
	return ButtonImage[0];
}

const CImage & ResourceManager::GetQuickSlotImage(unsigned char index) const
{
	if (index < sizeof(QuickSlotImage) / sizeof(QuickSlotImage[0])) 
		return QuickSlotImage[index];
	return QuickSlotImage[0];
}

const CImage & ResourceManager::GetMapImage(unsigned char index) const
{
	if (index < sizeof(MapImage) / sizeof(MapImage[0])) 
		return MapImage[index];
	return MapImage[0];
}

void ResourceManager::PlayerWalkingDraw(HDC hdc, unsigned char playerTypeIndex, unsigned short x, unsigned short y, unsigned char dir, unsigned char walkFrame, uVec2s Size) const
{
	if (playerTypeIndex < PlayerType::PlayerImageCount)
	switch (dir)
	{
	case 8:
		PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT * walkFrame / 2, RENDER_UNIT, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	case 2: case 5:
		PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT * walkFrame / 2, 0, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	case 4:
		PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT * walkFrame / 2, RENDER_UNIT * 2, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	case 6:
		PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT * walkFrame / 2, RENDER_UNIT * 3, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	case 7:
		PlayerImage[playerTypeIndex].Draw(hdc, x /* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT * (walkFrame + 3) / 2, RENDER_UNIT * 2, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	case 9:
		PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT * (walkFrame + 3) / 2, RENDER_UNIT * 3, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	case 1:
		PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT * (walkFrame + 3) / 2, 0, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	case 3:
		PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT * (walkFrame + 3) / 2, RENDER_UNIT, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	default:
		PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width, Size.height, RENDER_UNIT / 2, 0, RENDER_UNIT / 2, RENDER_UNIT);
		break;
	}
}

void ResourceManager::PlayerAttackDraw(HDC hdc, unsigned char playerTypeIndex, unsigned short x, unsigned short y, unsigned char dir, unsigned char AttackFrame, uVec2s Size) const
{
	if (playerTypeIndex < PlayerType::PlayerImageCount)
	switch (dir) {
	case 8:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 2, RENDER_UNIT * 4, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 3, RENDER_UNIT, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	case 2: case 5:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT, RENDER_UNIT * 4, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 3, 0, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	case 4:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 32/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 2, RENDER_UNIT * 6, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 32/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 3, RENDER_UNIT * 2, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	case 6:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT, RENDER_UNIT * 6, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 3, RENDER_UNIT * 3, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	case 7:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 2, RENDER_UNIT * 5, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, 0, RENDER_UNIT * 6, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	case 9:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 4/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT, RENDER_UNIT * 7, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 12/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, 0, RENDER_UNIT * 7, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	case 1:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 24/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 2, RENDER_UNIT * 7, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, 0, RENDER_UNIT * 4, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	case 3:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT, RENDER_UNIT * 5, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, 0, RENDER_UNIT * 5, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	default:
		switch (AttackFrame)
		{
		case 0:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT, RENDER_UNIT * 4, RENDER_UNIT, RENDER_UNIT);
			break;
		case 1: default:
			PlayerImage[playerTypeIndex].Draw(hdc, x - 16/* - (Size.width / 2)*/, CLIENT_HEIGHT - (y + (Size.height)), Size.width * 2, Size.height, RENDER_UNIT * 3, 0, RENDER_UNIT, RENDER_UNIT);
			break;
		}
	break;
	}
}

void ResourceManager::LoadGameImage()
{
	// 타이틀, 로고 이미지 로드
	LoadBackGroundImage();

	// UI 이미지 로드
	LoadUserInterfaceImage();

	// 액터 이미지 로드
	LoadActorImage();
	
	// 맵 이미지 로드
	LoadMapImage();

	// 이펙트 이미지 로드
	LoadEffectImage();
	
	// 아이템 이미지 로드
	LoadItemImage();
}

void ResourceManager::LoadBackGroundImage()
{
	if (FAILED(TitleImage.Load(TEXT("Image\\Title_1024x806.png"))) || TitleImage.IsNull())
		assert(!"타이틀 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(LogoImage.Load(TEXT("Image\\GoldDragon_Logo_1024x773-by_solstice_arctic_luna-d6yrudl.png"))) || LogoImage.IsNull())
		assert(!"로고 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");

	if (FAILED(TitleTextImage.Load(TEXT("Image\\Text\\TitleText.png"))) || TitleTextImage.IsNull())
		assert(!"타이틀 텍스트 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
}

void ResourceManager::LoadUserInterfaceImage()
{
	// 버튼 이미지 로드
	if (FAILED(ButtonImage[0].Load(TEXT("Image\\User_Interface\\Button_by_JHL.png"))) || ButtonImage[0].IsNull())
		assert(!"버튼 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(ButtonImage[1].Load(TEXT("Image\\User_Interface\\ButtonOn_by_JHL.png"))) || ButtonImage[1].IsNull())
		assert(!"발광 버튼 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(ButtonImage[2].Load(TEXT("Image\\User_Interface\\ButtonPush_by_JHL.png"))) || ButtonImage[2].IsNull())
		assert(!"눌려진 버튼 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	// 게이지 바 이미지 로드
	if (FAILED(GaugeBar.Load(TEXT("Image\\User_Interface\\Gauge_Bar.png"))) || GaugeBar.IsNull())
		assert(!"버튼 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	// 퀵 슬롯 이미지 로드
	if (FAILED(QuickSlotImage[0].Load(TEXT("Image\\User_Interface\\QuickSlotDark_by_JHL.png"))) || QuickSlotImage[0].IsNull())
		assert(!"어두운 퀵슬롯 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(QuickSlotImage[1].Load(TEXT("Image\\User_Interface\\QuickSlotLight_by_JHL.png"))) || QuickSlotImage[1].IsNull())
		assert(!"밝은 퀵슬롯 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
}

void ResourceManager::LoadActorImage()
{
	// 플레이어 이미지 로드
	LoadPlayerImage();

	// 몬스터 이미지 로드
	LoadMonsterImage();

	// NPC 이미지 로드
	LoadNPCImage();
}

void ResourceManager::LoadPlayerImage()
{
	if (FAILED(PlayerImage[PlayerType::SwordMan].Load(TEXT("Image\\Actors\\Player\\Player001_SwordMan.png"))) || PlayerImage[PlayerType::SwordMan].IsNull())
		assert(!"검사 플레이어 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	// MessageBox(m_hWnd, TEXT("Fail"), TEXT("Black Pawn Image Load Fail"), MB_OK);
	if (FAILED(PlayerImage[PlayerType::Archer].Load(TEXT("Image\\Actors\\Player\\Player003_Archer.png"))) || PlayerImage[PlayerType::Archer].IsNull())
		assert(!"궁수 플레이어 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(PlayerImage[PlayerType::Witch].Load(TEXT("Image\\Actors\\Player\\Player002_Witch.png"))) || PlayerImage[PlayerType::Witch].IsNull())
		assert(!"마녀 플레이어 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
}

void ResourceManager::LoadMonsterImage()
{
	if (FAILED(MonsterImage[MonsterType::Fly].Load(TEXT("Image\\Actors\\Monster\\Monster001_fly.png"))) || MonsterImage[MonsterType::Fly].IsNull())
		assert(!"파리 몬스터 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(MonsterImage[MonsterType::Crab].Load(TEXT("Image\\Actors\\Monster\\Monster002_crab.png"))) || MonsterImage[MonsterType::Crab].IsNull())
		assert(!"게 몬스터 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(MonsterImage[MonsterType::SkullGolem].Load(TEXT("Image\\Actors\\Monster\\Monster003_SkullGolem.png"))) || MonsterImage[MonsterType::SkullGolem].IsNull())
		assert(!"뼈 골렘 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(MonsterImage[MonsterType::Spear].Load(TEXT("Image\\Actors\\Monster\\Monster004_Spear.png"))) || MonsterImage[MonsterType::Spear].IsNull())
		assert(!"창잡이 몬스터 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(MonsterImage[MonsterType::Slasher].Load(TEXT("Image\\Actors\\Monster\\Monster005_Slasher.png"))) || MonsterImage[MonsterType::Slasher].IsNull())
		assert(!"그저 베는 자 몬스터 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
}

void ResourceManager::LoadNPCImage()
{
}

void ResourceManager::LoadMapImage()
{
	if (FAILED(MapImage[0].Load(TEXT("Image\\Map\\serverMap300x300grid.png"))) || MapImage[0].IsNull())
	//if (FAILED(MapImage[0].Load(TEXT("Image\\Map\\High_Field-GoldDragon.png"))) || MapImage[0].IsNull())
		assert(!"버튼 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
}

void ResourceManager::LoadEffectImage()
{
	if (FAILED(EffectImage001.Load(TEXT("Image\\Effect\\Effects_000.png"))) || EffectImage001.IsNull())
		assert(!"이펙트 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
}

void ResourceManager::LoadItemImage()
{
	if (FAILED(ItemImage[ItemCategory::HP_Potion].Load(TEXT("Image\\Item\\Item001_HP_Potion.png"))) || ItemImage[ItemCategory::HP_Potion].IsNull())
		assert(!"HP 포션 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(ItemImage[ItemCategory::MP_Potion].Load(TEXT("Image\\Item\\Item002_MP_Potion.png"))) || ItemImage[ItemCategory::MP_Potion].IsNull())
		assert(!"MP 포션 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(ItemImage[ItemCategory::Stamina_Potion].Load(TEXT("Image\\Item\\Item003_Stamina_Potion.png"))) || ItemImage[ItemCategory::Stamina_Potion].IsNull())
		assert(!"SP 포션 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
	if (FAILED(ItemImage[ItemCategory::Stone_Shoes].Load(TEXT("Image\\Item\\stone_shoes.png"))) || ItemImage[ItemCategory::Stone_Shoes].IsNull())
		assert(!"HP 포션 이미지 파일이 제대로 로드되지 않았습니다!\n경로나 이름, 파일을 확인해주세요.");
}
