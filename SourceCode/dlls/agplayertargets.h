#pragma once

#include <vector>

class CBasePlayer;

void AgSendPlayerTargets(CBasePlayer* pRecipient, const std::vector<CBasePlayer*>& players);

void AgClearPlayerTargets(CBasePlayer* pRecipient);

void AgClearAllPlayerTargets();
