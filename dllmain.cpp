// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "vector.h"
#include "csgo.hpp"
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

using namespace hazedumper::signatures;
using namespace hazedumper::netvars;

struct Entity
{
    Vec3D headPos;
    Vec3D originPos;
    Vec3D viewAngle;
    Vec3D viewAngleVec;
    uintptr_t boneMatrix;
    int health;
    int team;
};

struct
{
    uintptr_t localPlayer;
    uintptr_t gameModule;
    uintptr_t engineModule;
}val;

void main()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    val.gameModule = (uintptr_t)(GetModuleHandle("client.dll"));
    std::cout << val.gameModule << std::endl;
    val.engineModule = (uintptr_t)(GetModuleHandle("engine.dll"));
    std::cout << val.engineModule << std::endl;
    val.localPlayer = *(uintptr_t*)(val.gameModule + dwLocalPlayer);

    std::cout << "First time: " << std::hex << val.localPlayer << std::endl;

    if(val.localPlayer==NULL)
        while(val.localPlayer==NULL)
            val.localPlayer = *(uintptr_t*)(val.gameModule + dwLocalPlayer);

    std::cout << std::hex << val.localPlayer << std::endl;

    Entity currEnt;
    Entity myEnt;

    uintptr_t currEntAddr;
    uintptr_t closestEnemyAddr;

    bool shifPress = false;
    
    while (true)
    {

        shifPress = GetAsyncKeyState(VK_SHIFT) & 0x8000;

        if (shifPress)
        {
            std::cout << "Home pressed" << std::endl;

            float distance;
            float closestPlayer = FLT_MAX;
            Vec3D closestPlayerVector = { 1,2,3 };
            Vec3D newAngle;

            myEnt.health = *(int*)(val.localPlayer + m_iHealth);
            myEnt.team = *(int*)(val.localPlayer + m_iTeamNum);
            myEnt.originPos = *(Vec3D*)(val.localPlayer + m_vecOrigin);
            myEnt.viewAngleVec = *(Vec3D*)(val.localPlayer + m_vecViewOffset);
            myEnt.viewAngle = *(Vec3D*)(*(uintptr_t*)(val.engineModule + dwClientState) + dwClientState_ViewAngles);

            std::cout << "My health: " << myEnt.health << std::endl;
            std::cout << "My team: " << myEnt.team << std::endl;
            std::cout << "My position: (" << myEnt.originPos.x << ", " << myEnt.originPos.y << ", " << myEnt.originPos.z << ")" << std::endl;

            

            std::cout << '\n' << "-----------------------------------" << std::endl;

            for (int i = 1; i <= 64; i++)
            {
                currEntAddr = *(uintptr_t*)(val.gameModule + dwEntityList + i * 0x10);

                if (currEntAddr == 0)
                    continue;

                currEnt.health = *(int*)(currEntAddr + m_iHealth);

                if (currEnt.health <= 0)
                    continue;

                currEnt.team = *(int*)(currEntAddr + m_iTeamNum);

                if (currEnt.team == myEnt.team)
                    continue;

                currEnt.originPos = *(Vec3D*)(currEntAddr + m_vecOrigin);

                std::cout << "My health: " << std::hex << currEnt.health << std::endl;
                std::cout << "My team: " << currEnt.team << std::endl;
                std::cout << "My position: (" << currEnt.originPos.x << ", " << currEnt.originPos.y << ", " << currEnt.originPos.z << ")" << std::endl;

                distance = myEnt.originPos.distanceCalculate(currEnt.originPos);

                std::cout << "Distance with current player: " << distance << std::endl;

                if (distance < closestPlayer)
                {
                    closestPlayer = distance;
                    closestPlayerVector = currEnt.originPos;
                    closestEnemyAddr = currEntAddr;
                }

                std::cout << '\n' << "-----------------------------------" << std::endl;
            }

            std::cout << "Closest distance: " << closestPlayer << std::endl;

            currEnt.boneMatrix = *(uintptr_t*)(closestEnemyAddr + m_dwBoneMatrix);

            currEnt.headPos.x = *(float*)(currEnt.boneMatrix + 0x30 * 8 + 0x0C);
            currEnt.headPos.y = *(float*)(currEnt.boneMatrix + 0x30 * 8 + 0x1C);
            currEnt.headPos.z = *(float*)(currEnt.boneMatrix + 0x30 * 8 + 0x2C);

            myEnt.headPos = (myEnt.originPos + myEnt.viewAngleVec);

            std::cout << "My position: (" << myEnt.headPos.x << ", " << myEnt.headPos.y << ", " << myEnt.headPos.z << ")" << std::endl;
            
            newAngle = myEnt.headPos.relativeAngle(currEnt.headPos);

            myEnt.viewAngle = (newAngle);

            *(Vec3D*)(*(uintptr_t*)(val.engineModule + dwClientState) + dwClientState_ViewAngles) = newAngle;

            std::cout << "Finished" << std::endl;
        }
        Sleep(10);
    }
    
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

