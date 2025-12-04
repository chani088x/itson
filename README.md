# AIDatingSim

콘솔 기반 C++17 연애 시뮬레이션 엔진 예제입니다. 플레이어 입력 → LLM(Ollama) → 캐릭터 응답 루프, 호감도/관계 단계, 임계치 이벤트, JSON 기반 데이터/저장 시스템, 스트리밍 출력(타자 효과 포함)을 제공합니다.

## 기능 개요

- **대화 루프**: `Game::Run()`이 플레이어 입력을 받고 `PromptBuilder` → `LLMClient` → `DialogueManager`를 통해 응답을 출력합니다.
- **호감도 & 관계 단계**: 0~100 호감도와 0~3 관계 단계를 추적하며, 임계치 도달 시 자동 상승합니다.
- **이벤트 엔진**: `/data/events/*.json`에서 로드한 이벤트를 `EventManager`가 임계치에 맞춰 1회성 실행합니다.
- **선택지 시스템**: 이벤트마다 A/B/C 선택지를 정의해 호감도/플래그를 조정할 수 있습니다.
- **저장/불러오기**: `SaveSystem`이 루트별 JSON 상태(캐릭터/히스토리/플래그)를 저장합니다.
- **콘솔 UI**: ANSI/Windows 색상, 타자 효과, 스트리밍 chunk 즉시 출력.
- **LLM 연동**: 로컬 Ollama `http://localhost:11434/api/generate`에 libcurl로 연결. `stream=true`일 때 JSON line chunk를 즉시 파싱 후 출력합니다.

## 의존성

- C++17 호환 컴파일러
- [libcurl](https://curl.se/)
- [nlohmann/json](https://github.com/nlohmann/json)
- Ollama 서버 (예: `qwen2.5:7b` 모델)

### vcpkg 예시

```powershell
vcpkg install curl nlohmann-json
```

## 빌드

### Windows (PowerShell)

```powershell
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

### macOS/Linux

```bash
cmake -B build -S .
cmake --build build
```

## Ollama 설정

1. [https://ollama.com/](https://ollama.com/)에서 설치합니다.
2. 모델 풀/실행
   ```bash
   ollama pull qwen2.5:7b
   ollama run qwen2.5:7b
   ```
3. 서버 구동 (기본 11434)
   ```bash
   ollama serve
   ```

## 실행

```bash
./build/AIDatingSim
```

- 실행 후 캐릭터 루트를 선택하고 대화를 시작합니다.
- 명령어: `save`, `load`, `help`, `quit`.

## 데이터 & 설정

- `/data/characters/*.json`: 캐릭터 정의. 실제 데이터 파일을 추가하고 템플릿은 참고만 하세요.
- `/data/events/*.json`: 임계치 이벤트 템플릿. `lines`와 `choices`에 실제 내용을 채우세요(`// TODO` 유지).
- `/data/system/config.json`:
  - `model`: Ollama 모델명
  - `historyLimit`: 프롬프트에 포함할 최근 턴 수
  - `useStreaming`: true면 스트리밍 출력, false면 비스트리밍
  - `typingEffect`, `colorOutput`, `typingDelayMs`: 콘솔 효과 관련
  - `endpoint`: Ollama API 주소 (필요 시 변경)

스트리밍을 끄고 싶다면 `useStreaming`을 `false`로 수정 후 재실행하세요. 저장 데이터는 `./saves` 폴더에 캐릭터별 JSON으로 생성됩니다.

---

필요한 기능 확장(감성 점수 규칙, 이벤트 JSON 작성, 추가 캐릭터/루트 등)을 자유롭게 구현하여 자신만의 연애 시뮬레이션을 완성해 보세요!
