#include <stdio.h>
#include <inttypes.h>
#include <map>

#include "galois.h"

using namespace std;

void init_zero(mat32gf2* mat) {
    for (int i=0; i<32; i++) {
        mat->row[i] = 0;
    }
}

void init_id(mat32gf2* mat) {
    for (int i=0; i<32; i++) {
        mat->row[i] = 1 << (31-i);
    }
}

void init_ygo(mat32gf2* mat) {
    for (int i=0; i<31; i++) {
        mat->row[i] = 1 << (30-i);
    }
    mat->row[31] = 1 << 31;
    mat->row[14] |= (1<<31);
}

void copy_mat(mat32gf2* mat, mat32gf2* out) {
    for (int i=0; i<32; i++) {
        out->row[i] = mat->row[i];
    }
}

void transpose_mat(mat32gf2* mat, mat32gf2* out) {
    for (int j=0; j<32; j++) {
        out->row[j] = 0;
        for (int i=0; i<32; i++) {
            out->row[j] |= (((mat->row[i] & (1<<(31-j))) != 0) ? (1<<(31-i)) : 0);
        }
    }
}

void sum_mat(mat32gf2* mat1, mat32gf2* mat2, mat32gf2* out) {
    for (int i=0; i<32; i++) {
        out->row[i] = mat1->row[i] ^ mat2->row[i];
    }
}

void mul_mat(mat32gf2* mat1, mat32gf2* mat2, mat32gf2* out) {
    mat32gf2 aux;
    transpose_mat(mat2, &aux);
    for (int i=0; i<32; i++) {
        out->row[i] = 0;
        for (int j=0; j<32; j++) {
            u32 bit = __builtin_parity(mat1->row[i] & aux.row[j]);
            for (int k=0; k<32; k++) {
                out->row[i] |= (bit <<(31-j));
            }
        }
    }
}

u32 apply_mat(mat32gf2* mat, u32 seed) {
    u32 ans = 0;
    for (int i=0; i<32; i++) {
        u32 bit = __builtin_parity(mat->row[i] & seed);
        ans |= (bit << (31-i));
    }
    return ans;
}

void pow_mat(mat32gf2* mat, int n, mat32gf2* out) {
    if (n==0) {
        init_id(out);
    } else if (n==1) {
        copy_mat(mat, out);
    } else if (n==2) {
        mul_mat(mat, mat, out);
    } else {
        mat32gf2 aux, aux2;
        pow_mat(mat, n/2, &aux);
        if (n % 2 == 0) {
            mul_mat(&aux, &aux, out);
        } else {
            mul_mat(&aux, &aux, &aux2);
            mul_mat(&aux2, mat, out);
        }
    }
}

u32 period = 4292868097;
mat32gf2 pol_mat[32];
map<u32, int> indices[5];

int facs[] = {23, 49, 89, 127, 337};
int invs[] = {16, 33, 27, 48, 320};

void setup_pol_mat() {
    init_id(&pol_mat[0]);
    init_ygo(&pol_mat[1]);
    for (int i=2; i<32; i++) {
        mul_mat(&pol_mat[1], &pol_mat[i-1], &pol_mat[i]);
    }
}

void get_mat_for_num(u32 num, mat32gf2* out) {
    init_zero(out);
    for (int i=0; i<32; i++) {
        if ((num & (1<<i)) != 0) {
            sum_mat(out, &pol_mat[i], out);
        }
    }
}

void compute_indices() {
    for (int i=0; i<5; i++) {
        u32 pot = period / facs[i];
        mat32gf2 ygo_mat;
        init_ygo(&ygo_mat);
        mat32gf2 step_mat;
        pow_mat(&ygo_mat, pot, &step_mat);
        u32 el = 1;
        int idx = 0;
        while (indices[i].find(el) == indices[i].end()) {
            indices[i][el] = idx;
            el = apply_mat(&step_mat, el);
            idx++;
        }
    }
}

u32 next_seed(u32 seed) {
    u32 aux = (seed & 0x80000000) >> 15;
    seed ^= aux;
    seed = (seed << 1);
    seed |= (aux >> 16);
    return seed;
}

int get_cong_for_fac(mat32gf2* mat, int i) {
    u32 pot = period / facs[i];
    mat32gf2 step_mat;
    pow_mat(mat, pot, &step_mat);
    u32 el = apply_mat(&step_mat, 1);
    if (indices[i].find(el) == indices[i].end()) {
        return -1;
    }
    return indices[i][el];
}

u32 get_seed_for_pos(u32 pos) {
    mat32gf2 ygo_mat, step_mat;
    init_ygo(&ygo_mat);
    pow_mat(&ygo_mat, pos, &step_mat);
    return apply_mat(&step_mat, 1);
}