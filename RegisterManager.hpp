#pragma once
#include <string>

std::string allocateTempRegister();
void freeTempRegister(const std::string &reg);
