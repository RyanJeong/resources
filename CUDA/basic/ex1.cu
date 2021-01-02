/*
 *  A example of use the memory of a graphics card.
 */
#include <stdio.h>

int main()
{
    int i;
    int input[5]    = {1, 2, 3, 4, 5};
    int output[5]   = {0};
    int *graphicsCardMem;

    /*  cudaMalloc a device array   */
    cudaMalloc((void **) &graphicsCardMem, (5 * sizeof(int)));  
    
    /*  copy datas from the PC to the graphics card  */
    cudaMemcpy(graphicsCardMem, input, (5 * sizeof(int)), cudaMemcpyHostToDevice);

    /*  copy datas from the graphics card to the PC */
    cudaMemcpy(output, graphicsCardMem, (5 * sizeof(int)), cudaMemcpyDeviceToHost);

    for (i = 0; i < 5; ++i) {
        printf("[%d] output value: %d\n", i, output[i]);
    }
    /*  free memory */
    cudaFree(graphicsCardMem);
    
    return 0;
}
