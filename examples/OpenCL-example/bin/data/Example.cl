__kernel void direct_convolution(__global const int *a, __global const int *b, __global int *c){
    int offset = get_global_id(0);
    int value = b[offset];
    for(int i = 0; i < 512; i++){
        int mul = a[i] * value;
        c[offset + i] += mul;
    }
}
