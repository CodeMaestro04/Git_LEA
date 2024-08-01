#include <stdio.h>
#include <stdint.h>

#define NUM_ROUNDS 24
#define KEY_SIZE 4
#define DELTA_SIZE 4

// 델타 배열 정의
const uint32_t delta[DELTA_SIZE] = { 0xC3EFE9DB, 0x44626B02, 0x79E27C8A, 0x78DF30EC };

// 32비트 왼쪽 로테이트 함수
uint32_t rotate_left(uint32_t value, unsigned int count) {
    return (value << count) | (value >> (32 - count));
}

//mod4 함수
uint32_t mod4(uint32_t x)
{
    return x & 0b11;
}

// 키 스케줄 함수
void key_schedule(const uint32_t K[KEY_SIZE], uint32_t RK[NUM_ROUNDS][6]) {
    uint32_t T[KEY_SIZE];

    // 초기 T 설정
    for (int j = 0; j < KEY_SIZE; j++) {
        T[j] = K[j];
    }

    // 키 스케줄 루프 , 라운드 수만큼 반복시킴
    for (int i = 0; i < NUM_ROUNDS; i++) {
        T[0] = rotate_left(T[0] + rotate_left(delta[mod4(i)], i), 1);
        T[1] = rotate_left(T[1] + rotate_left(delta[mod4(i)], i + 1), 3);
        T[2] = rotate_left(T[2] + rotate_left(delta[mod4(i)], i + 2), 6);
        T[3] = rotate_left(T[3] + rotate_left(delta[mod4(i)], i + 3), 11);

        RK[i][0] = T[0];
        RK[i][1] = T[1];
        RK[i][2] = T[2];
        RK[i][3] = T[1];
        RK[i][4] = T[3];
        RK[i][5] = T[1];
    }
}

int main() {
    // 비밀키 정의
    uint32_t K[KEY_SIZE] = { 0x0f1571c9, 0x47d9e859, 0x0c946d24, 0x8d };

    // 라운드 키 저장할 배열을 정의
    uint32_t RK[NUM_ROUNDS][6];

    // 키 스케줄 함수 호출
    key_schedule(K, RK);

    return 0;
}
