#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<pdh.h>
#include<stdio.h>
#include<dxgi.h>

LRESULT CALLBACK WindowsProc(HWND handle , UINT MsgID , WPARAM eventDetails , LPARAM lEventDetails){
    static PDH_HQUERY cpu;
    static PDH_HCOUNTER totalCpu;
    static double CpuUsage = 0.0;

    static double usedRam = 0;
    static double maxRam = 0;
    static int ramUsage = 0;

    static PDH_HCOUNTER disk;
    static double diskUsage = 0.0;

    static int battery = 0;

    static wchar_t gpu[100] = L"WTF IS THIS GPU";
    static double Vram = 0.0;

    switch (MsgID){
        case WM_CREATE:
        {
            RegisterHotKey(handle , 1 , MOD_CONTROL | MOD_SHIFT , 0x59);
            PdhOpenQuery(NULL , 0 , &cpu);
            PdhAddEnglishCounterA(cpu , "\\Processor(_Total)\\% Processor Time" , 0 , &totalCpu);
            PdhAddEnglishCounterA(cpu , "\\PhysicalDisk(_Total)\\% Disk Time" , 0 , &disk);
            PdhCollectQueryData(cpu);
            SetTimer(handle , 1 , 1000 , NULL);

            IDXGIFactory* controller=NULL;
            if (CreateDXGIFactory(&IID_IDXGIFactory, (void**)&controller) == S_OK) {
                IDXGIAdapter* adaptor = NULL;
                if (controller->lpVtbl->EnumAdapters(controller, 0, &adaptor) == S_OK) {
                    DXGI_ADAPTER_DESC desc;
                    adaptor->lpVtbl->GetDesc(adaptor , &desc);
                    wcscpy(gpu, desc.Description);
                    Vram = desc.DedicatedVideoMemory/(1024.0 * 1024.0 * 1024.0);
                    adaptor->lpVtbl->Release(adaptor);
                }
                controller->lpVtbl->Release(controller);
            }

            return 0;
        }
        case WM_TIMER:
        {
            PdhCollectQueryData(cpu);
            PDH_FMT_COUNTERVALUE counter;
            PdhGetFormattedCounterValue(totalCpu , PDH_FMT_DOUBLE , NULL ,  &counter);
            CpuUsage = counter.doubleValue;
            PdhGetFormattedCounterValue(disk , PDH_FMT_DOUBLE , NULL , &counter);
            diskUsage = counter.doubleValue;

            MEMORYSTATUSEX ram;
            ram.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&ram);
            maxRam = ram.ullTotalPhys /(1024.0 * 1024.0 * 1024.0);
            double freeRam = ram.ullAvailPhys / (1024.0 * 1024.0 * 1024.0);
            usedRam = maxRam - freeRam;
            ramUsage = ram.dwMemoryLoad;

            SYSTEM_POWER_STATUS nrg ; //ben 10 reference
            GetSystemPowerStatus(&nrg);
            battery = nrg.BatteryLifePercent;

            InvalidateRect(handle , NULL , TRUE);
            return 0;

        }
        case WM_PAINT:
        {
            PAINTSTRUCT paints;
            HDC gdi = BeginPaint(handle , &paints);
            SetTextColor(gdi, RGB(0, 255, 0));  
            SetBkMode(gdi, TRANSPARENT); 
            RECT textRect = { 10, 10, 400, 400 };


            char DetailScreen[256];
            if(battery == 255){
                snprintf(DetailScreen , sizeof(DetailScreen) , "CPU Usage: %.2f%%\nGPU:%ls (%.1f GB VRAM)\nRAM Usage: %.2f GB / %.2f GB (%d%%)\nDisk Usage: %.2f%%",CpuUsage , gpu , Vram , usedRam , maxRam , ramUsage , diskUsage);
            }
            else{
                snprintf(DetailScreen , sizeof(DetailScreen) , "CPU Usage: %.2f%%\nGPU:%ls (%.1f GB VRAM)\nRAM Usage: %.2f GB / %.2f GB (%d%%)\nDisk Usage: %.2f%%\nBattery:%d%%",CpuUsage ,gpu , Vram , usedRam , maxRam , ramUsage , diskUsage , battery);

            }

            DrawTextA(gdi , DetailScreen , -1 , &textRect , DT_LEFT | DT_TOP);

            EndPaint(handle , &paints);
            return 0;
        }
        case WM_HOTKEY:
            DestroyWindow(handle);
            return 0;
        case WM_DESTROY:
            UnregisterHotKey(handle , 1);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(handle, MsgID, eventDetails, lEventDetails);
}


int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpCmdline , int nShowCmd){
    
    WNDCLASSA wc = {0};
    wc.hInstance = hInstance;
    wc.lpszClassName = "GMonitor-Win32";
    wc.lpfnWndProc = WindowsProc;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

    RegisterClassA(&wc);
    HWND display = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,"GMonitor-Win32","GMonitor-Win32",WS_POPUP , CW_USEDEFAULT , CW_USEDEFAULT , 400 , 400 , NULL , NULL , hInstance, NULL);
    SetLayeredWindowAttributes(display, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(display , nShowCmd);
    
    SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
    MSG msg = {0};
    while(GetMessageA(&msg , NULL , 0 , 0) > 0){
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}