#pragma once

// ��� ������Ʈ�� �⺻ ������ ��� �ִ� �θ� Ŭ����.
// �� Ŭ������ ����Ͽ� ������Ʈ�� �Ļ���Ų��.

class ResourceManager;

class BaseObject
{
private:
	unsigned int m_id = 0;	// ������Ʈ ID

	uVec2i m_v2dPosition{ 0, 0 };	// ��ġ
	uVec2i m_v2dSize{ 0, 0 };		// ũ��
	Vector3D<unsigned char> m_vColor{ 0, 0, 0 };	// ����

	const ResourceManager* resourceManager = nullptr;
	CImage ObjectImage;			// ResourceManager���� ���� ���۷����� �޾ƿ� ������Ʈ�� �̹���

	uVec2i m_v2dBackgroundSize;	// �� ��ũ�Ѹ��� �ϱ� ���� �� ��׶��� ũ�� ����
	// �� ��ũ�Ѹ��� ���Ͽ� ���� ��ǥ�� ȭ�� ��ǥ�� �ٲ��ֱ� ���� ����
	unsigned int window_left = 0;	// ���� ��ġ x�� ������ �� ����ŭ ���� ���� ȭ�鿡 �׸���.
	unsigned int window_top = 0;		// y�� ������ ���� ��ǥ�� ������ ��ǥ�谡 �ݴ�ϱ� �� ���� ��ġ���� ������� �Ѵ�.

	bool DrawBoxTrigger = false;	// ��Ʈ�ڽ��� �׸��� ���� ����
	bool DrawCircleTrigger = false; // Ư�� ������ ǥ���ϴ� ���� �׸��� ���� ����

protected:
	unsigned char objectType = 0;	// � ������Ʈ�ΰ�


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
	// ������Ʈ ID ����
	void SetID(unsigned int id) { m_id = id; }
	unsigned int GetID() const { return m_id; }

	// ��ġ ������
	void SetPosition(unsigned int x = 0, unsigned int y = 0) { m_v2dPosition = { x, y }; }
	void SetPosition(uVec2i position) { m_v2dPosition = position; }
	void SetPosX(unsigned int x) { m_v2dPosition.x = x; }
	void SetPosY(unsigned int y) { m_v2dPosition.y = y; }
	uVec2i GetPosition() const { return m_v2dPosition; }

	// ũ�� ������
	void SetSize(unsigned int w = 0, unsigned int h = 0) { m_v2dSize = { w, h }; }
	void SetSize(uVec2i size) { m_v2dSize = size; }
	uVec2i GetSize() const { return m_v2dSize; }

	// ��� ũ��
	void SetBackgroundSize(unsigned int w = CLIENT_WIDTH, unsigned int h = CLIENT_HEIGHT) { m_v2dBackgroundSize = { w, h }; }	
	void SetBackgroundSize(uVec2i& BG_Size) { m_v2dBackgroundSize = BG_Size; }
	uVec2i GetBackgroundSize() const { return m_v2dBackgroundSize; }

	// �̹���
	void SetImage(CImage& Image) { ObjectImage = Image; }
	const CImage& GetImage() const { return ObjectImage; }

	void SetObjectType(unsigned char p_type) { objectType = p_type; }


	// Render �ϱ� ���� �ݵ�� ó������. �÷��̾��� CenterPlayer, �÷��̾� �̿��� ��ü�� CenterStandard �Լ��� �÷��̾� �����͸� ���ڷ� �־�����.
	void CenterPlayerScrolling() 
	{
		window_left = clamp(0, static_cast<int>(m_v2dPosition.x) - CLIENT_WIDTH / 2, static_cast<int>(m_v2dBackgroundSize.width - CLIENT_WIDTH));
		window_top = clamp(0, static_cast<int>(m_v2dPosition.y) - CLIENT_HEIGHT / 2, static_cast<int>(m_v2dBackgroundSize.height - CLIENT_HEIGHT));
	}
	void CenterStandardScrolling(BaseObject * Center) 
	{
		if (!Center) return CenterPlayerScrolling();	// �߾� ���� �ƹ��͵� �� �־���? �̷� ���� �־�� �ȵȴ�.
		window_left = clamp(0, static_cast<int>(Center->GetPosition().x) - CLIENT_WIDTH / 2, static_cast<int>(m_v2dBackgroundSize.width - CLIENT_WIDTH));
		window_top = clamp(0, static_cast<int>(Center->GetPosition().y) - CLIENT_HEIGHT / 2, static_cast<int>(m_v2dBackgroundSize.height - CLIENT_HEIGHT));
	}
	uVec2i GetWindowLT() const { return {window_left, window_top}; }

	// Game Loop
	virtual bool ProcessInput(unsigned char * keybuffer, float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void Render(HDC hdc);

	// ������Ʈ �ڽ����� �浹�� �˻��ϱ� ���� �Լ��� �����ε��Ѵ�.
	bool RectCollide(uVec2i position, uVec2i size);
	bool RectCollide(BaseObject * other);

	// ������Ʈ ������ ���� ������ �� ������ �浹�� �˻��ϱ� ���� �Լ�.
	bool CircleCollide();

	// ������Ʈ ������ ���� �ڽ��� �浹�� ��츦 �˻��ϱ� ���� �Լ�.
	bool RangeInRectCollide();

//	bool ObstacleRectCollide(PlayerObject * player, double distance); 


	
};

