# 🐾 Pettt Animals 🐾
> **"누가 아군이고 적군인지 모른다! 혼돈의 대난투 파티!"** > Steam의 인기 게임 'Party Animals'를 모티브로 한 Unreal Engine 기반 3D 난투 게임입니다.

---

## 🎬 Game Play (아직X)
[![GamePlay Video](https://img.shields.io/badge/YouTube-Play_Video-red?style=for-the-badge&logo=youtube)](여기에_영상_링크_넣기)

---

## 🛠 Tech Stack
- **Engine:** `Unreal Engine 5` 🎮
- **Language:** `C++` 💻
- **Tools:** - **Version Control:** `Git`
  - **Collaboration:** `Notion`, `Zep`, `Slack`
  - **Documentation:** [프로젝트 노션 페이지](https://www.notion.so/teamsparta/3-2a82dc3ef51480dab073eaa73a8eaa8d) 📝
- **VPN:** 필수설치 (https://www.radmin-vpn.com/). 📝

---

## 👥 Team & Roles
| 이름 | 역할 | 담당 업무 |
| :--- | :---: | :--- |
| **이호욱** | 🏃 애니메이터 | 캐릭터 애니메이션 구현 및 리타겟팅 |
| **박진연** | 🎨 캐릭터/패키징 | 캐릭터 제작, **레그돌(Ragdoll)** 시스템, 게임 패키징 |
| **박찬영** | ⚔️ 인게임 로직 | InGame UI 제작, 승패 판정 로직 구현 |
| **신지용** | 🏠 아웃게임/매칭 | OutGame UI 제작, **룸 기반 매칭 시스템** 구현 |
| **위우빈** | 🗺️ 레벨 디자인 | 기믹 상호작용 구현 및 맵 레벨 디자인 |

---

## 🕹️ Key Features

### 1️⃣ Title & Matching
본인의 닉네임을 설정하고, 방을 생성하거나 기존의 방을 찾아 입장할 수 있습니다.
<br><img src="https://github.com/user-attachments/assets/01f815ed-6f6a-41a0-af7e-71c1663115ad" width="500" />

### 2️⃣ Lobby & Customizing
팀 진형을 선택하고, 8종의 다양한 동물 캐릭터와 의상을 취향대로 커스터마이징할 수 있습니다.
<br><img src="https://github.com/user-attachments/assets/d7814215-6e5f-4a58-b127-2ab10d3cd6cb" width="500" />

### 3️⃣ Settings
플레이어의 환경에 맞춰 해상도, 그래픽 사양 등 다양한 옵션을 최적화할 수 있습니다.
<br><img src="https://github.com/user-attachments/assets/c134b53e-477b-4ded-8371-5ce8bfcf8e25" width="500" />

### 4️⃣ Battle & Ragdoll
주먹, 막대기, 검 등 다양한 무기를 활용해 공격하세요! 공격당한 상대는 **레그돌 상태**가 되어 무력화됩니다.
<br><img src="https://github.com/user-attachments/assets/34d3e8c5-e53e-4c23-bc76-01cbf50f8bde" width="500" />

### 5️⃣ Dance Time
승리 후 혹은 상대가 쓰러졌을 때 신나는 댄스로 굴욕을 선사하세요! 💃🕺
<br><img src="https://github.com/user-attachments/assets/95974fea-8911-4597-a207-643b7ecc3e5e" width="500" />

---

## 🛠️ Trouble Shooting

### 1. 공격 판정 중복 적용 문제 (Multi-Hit Issue)
* **문제:** `AnimNotify`가 프레임 기반으로 작동하여 렉 발생 시 중복 호출되거나, 하나의 캐릭터가 가진 여러 충돌체에 의해 `SweepMultiByChannel`이 동일 대상을 여러 번 감지함.
* **해결:** * `bHasHitThisCombo` 플래그를 도입하여 동일 콤보 내 중복 연산 방지.
    * `TSet`을 활용해 타격된 액터를 기록하고, 이미 데미지를 입은 적은 제외하여 1회만 적용되도록 로직 개선.

### 2. 승패 판단 로직 무한 루프 발생
* **문제:** 팀 전멸 시 강제로 시간을 조정하여 판정을 유도했으나, 로직 배치 문제로 인해 판정 시점마다 시간이 재설정되며 무한 순환 발생.
* **해결:** 승패 판단 로직의 우선순위와 실행 순서를 재배치하여 중복 조건 처리를 방지함으로써 해결.

### 3. 서버-클라이언트 기믹 인식 동기화 오류
* **문제:** 서버 판정 로직 중 클라이언트 전용으로 작성된 로직이 있어 값이 변경되어도 공유되지 않음 (`DOREPLIFETIME` 누락).
* **해결:** `DOREPLIFETIME` 선언 및 변수 복제 로직을 추가하여 서버의 상태 변화가 모든 클라이언트에 정상적으로 반영되도록 수정.

### 4. 테스트 세션 노출 및 동기화 문제
* **문제:** 공용 테스트 ID 사용 시 다른 세션이 노출되거나, `ServerTravel` 시 클라이언트 간 싱크가 맞지 않는 현상.
* **해결:** * **세션 격리:** 메타데이터 주입 및 `Equals` 필터링을 통해 우리 팀 세션만 필터링.
    * **시퀀스 동기화:** `Multicast RPC`를 통해 모든 인원의 시작 시퀀스를 맞춘 후 안정적으로 트래블 진행.
    * **UX 개선:** 이동 중 입력 및 패킷 송신을 차단(`bInputRestricted`)하여 끊김 없는 전환 구현.
