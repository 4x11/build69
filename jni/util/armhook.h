#pragma once

void UnFuck(uintptr_t ptr);
void NOP(uintptr_t addr, unsigned int count);
void WriteMemory(uintptr_t dest, uintptr_t src, size_t size);
void ReadMemory(uintptr_t dest, uintptr_t src, size_t size);

void SetUpHook(uintptr_t addr, uintptr_t func, uintptr_t *orig);
void InstallMethodHook(uintptr_t addr, uintptr_t func);
void CodeInject(uintptr_t addr, uintptr_t func, int register);