#pragma once

// 모든 오브젝트의 기본 정보를 담고 있는 부모 클래스.
// 이 클래스를 상속하여 오브젝트를 파생시킨다.

class ResourceManager;

class BaseObject
{
private:
	unsigned int m_id = 0;	// 오브젝트 ID

	uVec2i m_v2dPosition{ 0, 0 };	// 위치
	uVec2i m_v2dSize{ 0, 0 };		// 크기
	Vector3D<unsigned char> m_vColor{ 0, 0, 0 };	// 색상

	const ResourceManager* resourceManager = nullptr;
	CImage ObjectImage;			// ResourceManager에서 직접 레퍼런스로 받아올 오브젝트의 이미지

	uVec2i m_v2dBackgroundSize;	// 맵 스크롤링을 하기 위한 맵 백그라운드 크기 저장
	// 맵 스크롤링을 위하여 월드 좌표와 화면 좌표를 바꿔주기 위한 변수
	unsigned int window_left = 0;	// 현재 위치 x축 값에서 이 값만큼 빼서 현재 화면에 그린다.
	unsigned int window_top = 0;		// y축 방향이 월드 좌표와 윈도우 좌표계가 반대니까 이 값은 위치에서 더해줘야 한다.

	bool DrawBoxTrigger = false;	// 히트박스를 그리기 위한 조건
	bool DrawCircleTrigger = false; // 특정 범위를 표현하는 원을 그리기 위한 조건

protected:
	unsigned char objectType = 0;	// 어떤 오브젝트인가


public:
	BaseObject() {}
	BaseObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const CImage& Image) : m_v2dPosition{ x, y }, m_v2dSize{ w, h }, ObjectImage(Image) {}
	BaseObject(uVec2i pos, uVec2i size, const CImage& Image) : m_v2dPosition(pos), m_v2dSize(size), ObjectImage(Image) {}
	virtual ~BaseObject();

	void SetResource(const ResourceManager* pRmgr) { resourceManager = pRmgr; }
	const ResourceManager* GetResource() const { return resourceManager; }

	void DrawHitBoxTriggerSet();
	void DrawHitBoxTriggerSet(bool Trigger);
	virtual void DrawHitBox(HDC hdc);
	const bool isHitBoxDraw() { return DrawBoxTrigger; }
	void DrawCircleRangeTriggerSet();
	void DrawCircleRangeTriggerSet(bool Trigger);
	virtual void DrawCircleRange(HDC hdc, short range);
	const bool isCircleRangeDraw() { return DrawCircleTrigger; }
	// 오브젝트 ID 접근
	void SetID(unsigned int id) { m_id = id; }
	unsigned int GetID() const { return m_id; }

	// 위치 접근자
	void SetPosition(unsigned int x = 0, unsigned int y = 0) { m_v2dPosition = { x, y }; }
	void SetPosition(uVec2i position) { m_v2dPosition = position; }
	void SetPosX(unsigned int x) { m_v2dPosition.x = x; }
	void SetPosY(unsigned int y) { m_v2dPosition.y = y; }
	uVec2i GetPosition() const { return m_v2dPosition; }

	// 크기 접근자
	void SetSize(unsigned int w = 0, unsigned int h = 0) { m_v2dSize = { w, h }; }
	void SetSize(uVec2i size) { m_v2dSize = size; }
	uVec2i GetSize() const { return m_v2dSize; }

	// 배경 크기
	void SetBackgroundSize(unsigned int w = CLIENT_WIDTH, unsigned int h = CLIENT_HEIGHT) { m_v2dBackgroundSize = { w, h }; }	
	void SetBackgroundSize(uVec2i& BG_Size) { m_v2dBackgroundSize = BG_Size; }
	uVec2i GetBackgroundSize() const { return m_v2dBackgroundSize; }

	// 이미지
	void SetImage(CImage& Image) { ObjectImage = Image; }
	const CImage& GetImage() const { return ObjectImage; }

	void SetObjectType(unsigned char p_type) { objectType = p_type; }


	// Render 하기 전에 반드시 처리하자. 플레이어라면 CenterPlayer, 플레이어 이외의 객체는 CenterStandard 함수에 플레이어 포인터를 인자로 넣어주자.
	void CenterPlayerScrolling() 
	{
		window_left = clamp(0, static_cast<int>(m_v2dPosition.x) - CLIENT_WIDTH / 2, static_cast<int>(m_v2dBackgroundSize.width - CLIENT_WIDTH));
		window_top = clamp(0, static_cast<int>(m_v2dPosition.y) - CLIENT_HEIGHT / 2, static_cast<int>(m_v2dBackgroundSize.height - CLIENT_HEIGHT));
	}
	void CenterStandardScrolling(BaseObject * Center) 
	{
		if (!Center) return CenterPlayerScrolling();	// 중앙 값에 아무것도 안 넣었다? 이런 경우는 있어서는 안된다.
		window_left = clamp(0, static_cast<int>(Center->GetPosition().x) - CLIENT_WIDTH / 2, static_cast<int>(m_v2dBackgroundSize.width - CLIENT_WIDTH));
		window_top = clamp(0, static_cast<int>(Center->GetPosition().y) - CLIENT_HEIGHT / 2, static_cast<int>(m_v2dBackgroundSize.height - CLIENT_HEIGHT));
	}
	uVec2i GetWindowLT() const { return {window_left, window_top}; }

	// Game Loop
	virtual bool ProcessInput(unsigned char * keybuffer, float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void Render(HDC hdc);

	// 오브젝트 박스간의 충돌을 검사하기 위한 함수를 오버로딩한다.
	bool RectCollide(uVec2i position, uVec2i size);
	bool RectCollide(BaseObject * other);

	// 오브젝트 레인지 원과 레인지 원 사이의 충돌을 검사하기 위한 함수.
	bool CircleCollide();

	// 오브젝트 레인지 원에 박스가 충돌한 경우를 검사하기 위한 함수.
	bool RangeInRectCollide();

//	bool ObstacleRectCollide(PlayerObject * player, double distance); 


	
};

