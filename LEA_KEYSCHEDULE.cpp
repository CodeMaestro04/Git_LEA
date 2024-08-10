#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define NUM_ROUNDS 24
#define KEY_SIZE 4
#define DELTA_SIZE 4

// ��Ÿ �迭 ����
const uint32_t delta[DELTA_SIZE] = { 0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec };

// mod4 �Լ�
uint32_t mod4(uint32_t x) {
    return x & 0b11;
}

// 32��Ʈ ���� ������Ʈ �Լ�
uint32_t rotate_L(uint32_t value, unsigned int count) {
    return (value << count) | (value >> (32 - count));
}

// 32��Ʈ ������ ������Ʈ �Լ�
uint32_t rotate_R(uint32_t value, unsigned int count) {
    return (value >> count) | (value << (32 - count));
}

// Ű ������ �Լ�
void key_schedule(const uint32_t* KEY, uint32_t* RK) {
    uint32_t T[KEY_SIZE];

    // �ʱ� T ����
    for (int j = 0; j < KEY_SIZE; j++) {
        T[j] = *(KEY + j);
    }

    // Ű ������ ����, ���� ����ŭ �ݺ���Ŵ
    for (int i = 0; i < NUM_ROUNDS; i++) {
        T[0] = rotate_L(T[0] + rotate_L(delta[mod4(i)], i), 1);
        T[1] = rotate_L(T[1] + rotate_L(delta[mod4(i)], i + 1), 3);
        T[2] = rotate_L(T[2] + rotate_L(delta[mod4(i)], i + 2), 6);
        T[3] = rotate_L(T[3] + rotate_L(delta[mod4(i)], i + 3), 11);

        *(RK + 6 * i) = T[0];
        *(RK + 6 * i + 1) = T[1];
        *(RK + 6 * i + 2) = T[2];
        *(RK + 6 * i + 3) = T[1];
        *(RK + 6 * i + 4) = T[3];
        *(RK + 6 * i + 5) = T[1];
    }
}

// ��ȣȭ �Լ� �� ���� �Լ�
void RoundEnc(uint32_t* dest, const uint32_t* src, const uint32_t* RK) {
    // �� �ʱ�ȭ
    uint32_t temp[4] = { 0 };
    memcpy(temp, src, 4 * sizeof(uint32_t));

    dest[0] = rotate_L((temp[0] ^ RK[0]) + (temp[1] ^ RK[1]), 9);
    dest[1] = rotate_R((temp[1] ^ RK[2]) + (temp[2] ^ RK[3]), 5);
    dest[2] = rotate_R((temp[2] ^ RK[4]) + (temp[3] ^ RK[5]), 3);
    dest[3] = temp[0];
}

// ��ȣȭ �Լ�
void Enc(const uint32_t* P, const uint32_t* RK, uint32_t* C) {
    uint32_t X[4];
    uint32_t temp[4];

    // �ʱ� X ����
    memcpy(X, P, 4 * sizeof(uint32_t));

    for (int i = 0; i < NUM_ROUNDS; i++) {
        RoundEnc(temp, X, RK + 6 * i);
        memcpy(X, temp, 4 * sizeof(uint32_t));
    }

    memcpy(C, X, 4 * sizeof(uint32_t));
}

// ��ȣȭ �Լ� �� ���� �Լ�
void RoundDec(uint32_t* dest, const uint32_t* src, const uint32_t* RK) {
    // �� �ʱ�ȭ
    uint32_t temp[4] = { 0 };
    memcpy(temp, src, 4 * sizeof(uint32_t));

    dest[0] = temp[3];
    dest[1] = rotate_R(temp[0], 9) - (dest[0] ^ RK[0]) ^ RK[1]);
    dest[2] = rotate_L(temp[1], 5) - (dest[1] ^ RK[2]) ^ RK[3]);
    dest[3] = rotate_L(temp[2], 3) - (dest[2] ^ RK[4]) ^ RK[5]);
}

// ��ȣȭ �Լ�
void Dec(const uint32_t* C, const uint32_t* RK, uint32_t* P) {
    uint32_t X[4];
    uint32_t temp[4];

    // �ʱ� X ����
    memcpy(X, C, 4 * sizeof(uint32_t));

    for (int i = NUM_ROUNDS - 1; i >= 0; i--) {
        RoundDec(temp, X, RK + 6 * i);
        memcpy(X, temp, 4 * sizeof(uint32_t));
    }

    memcpy(P, X, 4 * sizeof(uint32_t));
}

int main() {
    // ���Ű ����
    uint32_t K[KEY_SIZE] = { 0x0f1571c9, 0x47d9e859, 0x0c946d24, 0x8d };

    // ���� Ű ������ �迭�� ����
    uint32_t RK[NUM_ROUNDS * 6] = { 0 };
    
    key_schedule(K, RK);

    // �� ������ ����
    uint32_t P[4] = { 0x01234567, 0x89ABCDEF, 0x01234567, 0x89ABCDEF };

    // ��ȣ�� ������ �迭 ����
    uint32_t C[4] = { 0 };

    Enc(P, RK, C);

    // ��ȣȭ ������ �迭 ����
    uint32_t decrypted[4] = { 0 };

    Dec(C, RK, decrypted);

    // ��� ���
    printf("Ciphertext: %08X %08X %08X %08X\n", C[0], C[1], C[2], C[3]);

    printf("Decrypted: %08X %08X %08X %08X\n", decrypted[0], decrypted[1], decrypted[2], decrypted[3]);

    return 0;
}
