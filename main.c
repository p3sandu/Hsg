#include <stdio.h>
#include "ApiTable.h"

int main() {
    if (!API_Initialize()) {
        return -1;
    }

    fnWaitForSingleObject pWaitForSingleObject = (fnWaitForSingleObject)API.Kb.WaitForSingleObject.lpAddr;
    
    printf("[+] resolved WaitForSingleObject: 0x%p\n", pWaitForSingleObject);
    getchar();

    return 0;
}
