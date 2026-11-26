#ifndef THREAT_POLICY_H
#define THREAT_POLICY_H

#include "threat_types.h"
#include "battalion_types.h"

enum ThreatLevel evaluate_battalion_threat(enum BattalionType friendly_type, enum BattalionType enemy_type);

#endif