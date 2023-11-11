# 기본 이미지 선택, 여기서는 C++ 컴파일러와 기본 도구가 포함된 이미지를 사용
FROM gcc:latest

# 작업 디렉토리 설정
WORKDIR /app

# 필요한 도구와 라이브러리 설치
RUN apt-get update && apt-get install -y cmake git

# Catch2 설치
RUN git clone https://github.com/catchorg/Catch2.git \
    && cd Catch2 \
    && git checkout v3.4.0 \
    && cmake -Bbuild -H. -DBUILD_TESTING=OFF \
    && cmake --build build/ --target install

# GTest 라이브러리 설치
RUN apt-get install -y libgtest-dev

# 소스 코드 및 필요한 파일 복사
COPY . /app

# CMake를 사용하여 빌드
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build

# 컨테이너 실행 시 실행할 명령어 설정
CMD ["./build/1_minimal_example"]
