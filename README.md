# 봄날의 추억 (AI Dating Sim)

콘솔 기반의 AI 연애 시뮬레이션 게임입니다. C++로 작성되었으며, LLM(Ollama 또는 OpenAI)을 활용하여 캐릭터와 자유로운 대화가 가능합니다.
호감도에 따라 관계가 발전하고, 특정 이벤트가 발생하는 인터랙티브 스토리텔링을 경험해 보세요.

## 주요 기능 (What's New)

- **AI 캐릭터와의 자유 대화**: 플레이어의 입력에 따라 실시간으로 생성되는 AI의 반응.
- **호감도 시스템**: 대화 내용에 따라 호감도가 변화하며, 관계 단계가 발전합니다. (0~100, 4단계)
- **이벤트 시스템**: 특정 호감도 도달 시 미리 정의된 이벤트가 발생하여 스토리를 진행시킵니다.
  - 선택지 없이 자연스럽게 이어지는 스토리 연출.
  - 몰입감을 위한 텍스트 타이핑 효과 적용.
- **저장 및 불러오기**:
  - `saves/` 폴더에 JSON 형식으로 진행 상황 저장.
  - 기존 세이브 파일에 덮어쓰기 및 자동 저장 기능.
- **깔끔한 TUI (Text User Interface)**: 가독성을 높인 줄바꿈 처리와 직관적인 인터페이스.
- **멀티 LLM 지원**:
  - **Ollama (Local)**: 로컬에서 `qwen2.5:7b` 등의 모델을 무료로 사용 가능.
  - **OpenAI (Cloud)**: API Key 입력을 통해 GPT-4o 등 고성능 모델 사용 가능. (시작 시 자동 감지 및 입력 요청)

## 설치 및 빌드

### 요구 사항
- C++17 호환 컴파일러 (MSVC, GCC 등)
- CMake 3.10 이상
- [Ollama](https://ollama.com/) (로컬 구동 시)
- OpenAI API Key (클라우드 모델 사용 시)

### 빌드 방법 (Windows)

```powershell
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg경로]/scripts/buildsystems/vcpkg.cmake ..
cmake --build .
```

*참고: vcpkg를 사용하여 `libcurl`, `nlohmann-json` 라이브러리를 설치해야 합니다.*

## 실행 방법

```powershell
cd build
.\AIDatingSim.exe
```

1.  시스템이 설정 파일(`data/system/config.json`)을 로드합니다.
2.  OpenAI를 사용하려면 시작 시 API Key를 입력하세요. (Ollama 사용 시 엔터)
3.  **새 게임**을 시작하거나 **불러오기**를 통해 이전 기록을 이어서 할 수 있습니다.

## 게임 플레이 가이드

- **대화하기**: 자유롭게 채팅하듯 입력하세요.
- **명령어**:
    - `/save`: 현재 상태 저장
    - `/quit` 또는 `/exit`: 게임 종료
    - `/restart`: 재시작
- **이벤트**: 호감도가 25, 50, 75, 100 특정 구간에 도달하면 이벤트 컷신이 출력됩니다.

## 파일 구조 및 커스터마이징

- `data/characters/template_character.json`: AI 캐릭터의 성격, 말투 프롬프트 설정.
- `data/events/template_events.json`: 호감도별 이벤트 대사 설정. 자유롭게 수정하여 자신만의 스토리를 만드세요.
- `data/system/config.json`:
    - `model`: 사용할 모델명 (예: `gpt-5`, `qwen2.5:7b`)
    - `useStreaming`: 텍스트 스트리밍 효과 여부
    - `savesDir`: 세이브 파일 경로 (기본: `../saves`)

---

## 라이선스 및 참고
이 프로젝트는 학습 및 예제 목적으로 제작되었습니다.
