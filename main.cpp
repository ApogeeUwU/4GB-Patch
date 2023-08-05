#include <Windows.h>
#include <iostream>
int main() {
    OPENFILENAMEA ofn;
    char sf[260];
    RtlZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = 0;
    ofn.lpstrFile = sf;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(sf);
    ofn.lpstrFilter = "Executable Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = 0;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn) == 1) {
        std::cout << ofn.lpstrFile << std::endl;
        void* file = CreateFileA(ofn.lpstrFile, GENERIC_READ | GENERIC_WRITE, 0, 0, 3, FILE_ATTRIBUTE_NORMAL, 0);
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        GetFileAttributesExA(ofn.lpstrFile, GetFileExInfoStandard, &fileInfo);
        std::cout << fileInfo.dwFileAttributes << std::endl;
        if (file == INVALID_HANDLE_VALUE) {
            MessageBoxA(0, "Error", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        unsigned int fsize = GetFileSize(file, 0);
        if (fsize == INVALID_FILE_SIZE) {
            CloseHandle(file);
            MessageBoxA(0, "Error", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        void* map = CreateFileMappingA(file, 0, PAGE_READWRITE, 0, 0, 0);
        if (map == 0) {
            CloseHandle(file);
            MessageBoxA(0, "Error", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        void* baseadr = MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, 0);
        if (baseadr == 0) {
            CloseHandle(map);
            CloseHandle(file);
            MessageBoxA(0, "Error", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)baseadr;
        PIMAGE_NT_HEADERS nth = (PIMAGE_NT_HEADERS)((unsigned long)baseadr + dos->e_lfanew);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || nth->Signature != IMAGE_NT_SIGNATURE) {
            UnmapViewOfFile(baseadr);
            CloseHandle(map);
            CloseHandle(file);
            MessageBoxA(0, "Error", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        nth->FileHeader.Characteristics |= IMAGE_FILE_LARGE_ADDRESS_AWARE;
        UnmapViewOfFile(baseadr);
        CloseHandle(map);
        CloseHandle(file);
        MessageBoxA(0, "Success.", "Success", MB_OK | MB_ICONINFORMATION);
    }
    return 0;
}
