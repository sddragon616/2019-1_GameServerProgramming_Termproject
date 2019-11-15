#pragma once

class ResourceManager final
{
public:
	enum PlayerType {
		SwordMan,
		Witch,
		Archer,
		PlayerImageCount
	};
	enum MonsterType {
		Fly,
		Crab,
		SkullGolem,
		Spear,
		Slasher,
		MonsterImageCount
	};
	enum ItemCategory {
		HP_Potion,
		MP_Potion,
		Stamina_Potion,
		Stone_Shoes,
		ItemImageCount
	};
private:	
	CImage LogoImage;
	CImage TitleImage;
	CImage TitleTextImage;

	CImage ButtonImage[3];	// 0 : default, 1 : mouse on, 2 : pushed
	CImage GaugeBar;
	CImage QuickSlotImage[2];	// 0 : Dark, 1 : Light

	CImage MapImage[2];

	CImage PlayerImage[PlayerType::PlayerImageCount];
	CImage MonsterImage[MonsterType::MonsterImageCount];
	CImage EffectImage001;
	CImage ItemImage[ItemCategory::ItemImageCount];

	void LoadGameImage();
	void LoadBackGroundImage();
	void LoadUserInterfaceImage();
	void LoadActorImage();
	void LoadPlayerImage();
	void LoadMonsterImage();
	void LoadNPCImage();
	void LoadMapImage();
	void LoadEffectImage();
	void LoadItemImage();

public:
	ResourceManager();
	virtual ~ResourceManager();

	const CImage& GetLogoImage() const { return LogoImage; }
	const CImage& GetTitleImage() const { return TitleImage; }
	const CImage& GetTitleTextImage() const { return TitleTextImage; }

	const CImage& GetButtonImage(unsigned char index = 0) const;
	const CImage& GetQuickSlotImage(unsigned char index = 0) const;
	const CImage& GetGaugeBarImage() const { return GaugeBar; }

	const CImage& GetMapImage(unsigned char index = 0) const;
				
	const CImage& GetSwordManImage() const { return PlayerImage[PlayerType::SwordMan]; }
	const CImage& GetWitchImage() const { return PlayerImage[PlayerType::Witch]; }
	const CImage& GetArcherImage() const { return PlayerImage[PlayerType::Archer]; }
				
	const CImage& GetFlyImage() const { return MonsterImage[MonsterType::Fly]; }
	const CImage& GetCrabImage() const { return MonsterImage[MonsterType::Crab]; }
	const CImage& GetSkullGolemImage() const { return MonsterImage[MonsterType::SkullGolem]; }
	const CImage& GetSpearImage() const { return MonsterImage[MonsterType::Spear]; }
	const CImage& GetSlasherImage() const { return MonsterImage[MonsterType::Slasher]; }
				
	const CImage& GetEffectImage() const{ return EffectImage001; };

	void PlayerWalkingDraw(HDC hdc, unsigned char playerTypeIndex, unsigned short x, unsigned short  y, unsigned char direction, unsigned char walkFrame, uVec2s Size = { RENDER_UNIT / 2, RENDER_UNIT }) const;
	void PlayerAttackDraw(HDC hdc, unsigned char playerTypeIndex, unsigned short x, unsigned short y, unsigned char direction, unsigned char AttackFrame, uVec2s Size = { RENDER_UNIT / 2, RENDER_UNIT }) const;
};
