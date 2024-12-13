# 대용량 LandScapeTexture 파일 다운 링크
https://drive.google.com/file/d/1rR7eFPwnNa4ttKQ61GHodfRQygdKGofA/view?usp=sharing

# DirectX 11

# DirectTex 

# FMOD
* FMOD Engine for Windows - version 2.02.23

# ImGui


# Revision 설명

* 268    
  * 3D Animation 기능 추가
  
* 267 
  1. FBX 를 로딩하는 과정에서 MeshData 에셋이 생성되고 저장되도록 함
  2. 한번 로딩한 적이 있는 FBX 는 MeshData 에셋으로 저장된 파일로 대체됨

* 266
  1. FBX 로더 추가
  2. FBXLoader 를 이용해서 FBX 파일을 로딩 후 CMeshData 타입 에셋으로 전환해서 저장하도록 함
  3. CMesh 구조 변경
    - 하나의 버텍스 버퍼에 여러개의 인덱스 버퍼가 조합 될 수 있는 구조로 변경
    - CMesh 구조 변경에 따라서 CRenderComponent 의 CMaterial 참조 수가 CMesh 의 서브셋 개수에 따라 달라지도록 변경

* 265
  1. FBX 리소스 추가

* 264   
  * FBXLoader 헤더 및 라이브러리 추가

* 263
  1. 배열 텍스쳐 생성 기능 추가
  2. 밉맵 생성기능 추가
  3. 텍스쳐 저장기능 추가
  4. LandScape 쉐이더에서 LandScape 의 색상 및 노말을, 거리에 따른 적절한 MipMap 에서 Sampling 하는 방식으로 변경
  5. 
* 262
  1. LandScape 기능 추가
     - 가중치에 따른 지형 Splatting 기능 구현
     - 브러쉬 런더링 기능 추가
* 261
  1. LandScape 용 브러쉬 텍스쳐
  2. LandScape 높이맵을 직접 제작해서 사용할 수 있도록 함
  3. Raycasting 기능 추가
  4. Raycasting 결과를 이용해서 높이맵을 수정위치를 정하도록 함

* 258
  * 높이맵 텍스쳐
  1. 메뉴 UI 를 이용해서 각 렌더타겟을 볼 수 있도록 함
  2. LandScape 에 높이맵 적용
  3. LandScape 높이맵 적용시 Normal Tangent Binormal 재 계산
  4. LandScape 메쉬 생성 시 인덱스 순서 오류 수정

* 254
  1. LandScape 컴포넌트 구현 중
  2. LandScape Deferred 렌더링으로 화면에 출력
  3. 특정 타겟을 화면에 띄우는 기능을 MenuUI 에 추가
   
* 252
  * 테셀레이션 시작
  * Tessellation Test 해보기

* 250
  1. 광원 시점에서 물체들의 깊이를 저장하는 ShadowMap 단계 구현 중
  2. ShadowMap 렌더링 구현
  3. 광원 시점에서 ShadowMap 작성
  4. light 쉐이더에 그림자 판정 추가

* 247
  1. 디퍼드 랜더링에서 큐브맵 색상을 추가로 입히기
     - 반사벡터를 이용해서 주변 월드 환경맵을 재질에 추가
  2. Light3D 에 맴버로 ShadowMapTex 추가
     - 광원 시점에서 바라본 물체들의 깊이값을 기록하는 타겟
  3. 카메라가 바라보는 물체들이 카메라 시야안에 들어오는지 테스트하는 단계 추가 예정
     - 절두체 컬링

* 243
  1. DecalShader 구현 중
  2. 데칼 테스트용 텍스쳐 업로드
  3. Decal 컴포넌트 구현
  4. 

* 239
  1. DebugShape 추가
     - Cube, Sphere
  2. DebugLine 렌더링 문제
     - 화면에 출력되지 않는 문제 있음
  3. DebugLine 문제 해결
     - SetScalaParam 에 Vec4 입력 시 Vec3 입력하면 const expr 에 걸리는게 없어서 입력이 제대로 안되고 있는 문제

* 236
  *  stencil 을 활용해서 volumemesh 내부 체크하는 방법 설명
  *  Stencil 을 이용한 Volume mesh 체크 최적화 설명
  * Decal 방식의 volume mesh 체크 설명
  * PointLight Deferred 처리 
    - PointLight 의 영역을 Volume Mesh 로 처리, 영역안에 들어오는 부분에 광원계산 
  *  PointLight 를 여러개 중첩해서 광원 테스트
  * Decal 컴포넌트 제작 중
  * DebugRenderShape 에서 Cube, Sphere, Line 지원 예정

* 231
  * DirLightShader 구현 및 테스트
  * Albedo + Light 병합 단계 추가

* 229
  - 렌더타겟과 깊이스텐실 텍스쳐를 묶어서 MRT 로 설계
  - 렌더타겟은 최대 8개 까지 조합 가능
  - MRT 설계 완료
  - Deferred, Light MRT 생성
  -  RenderMgr 에 Render 함수 구현
  - Camera 내부에서 진행하던 내용을 RenderMgr 로 이동
  - Deferred 렌더링에서 광원 처리단계 설명

* 225
  * SkyBox 컴포넌트 구현 중
  * SkyBox 컴포넌트 추가
  
* 223
  1. 노말맵핑 설명
  2. 노말맵핑 적용
     - Std3DShader 에 적용
* 222
  1. CalcLight3D 함수 구현
     - 광원의 타입에 따라서 픽셀 표면에 들어오는 빛의 세기를 계산하는 함수
  2. Std3DShader 에서 다중광원에 대한 처리가 가능하도록 변경
     - 상수버퍼 메모리 16바이트 정렬에 따른 문제점 해결
     - 다수의 광원에 빛이 비추어지는 표면에서 빛의 세기를 누적해서 계산하도록 
    CalcLight3D 함수를 반복문 처리하도록 변경
  3. 지형 용도 텍스쳐 추가

* 220
  * std3dshader 에 반사광 적용
  * Directional Light 연산하기
  * 연산 진행 공간을 ViewSpace 로 변경
  * 광원의 데이터를 Level 에 있는 Light3D 컴포넌트 정보를 기반으로 하도록 변경

* 218
  * std3dshader 에 방향성 광원 계산 추가
  * SphereMesh 추가
  * 고로 쉐이딩, 퐁 쉐이딩 차이점
  * 반사벡터 설명

* 215
  1. 3D 수업 시작
  - Vtx 구조체에 Normal, Tangent, Binormal 정보 추가
  - CubeMesh 추가
