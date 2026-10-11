#include "threat_policy.h"
#include "threat_types.h"
#include "battalion_types.h"

enum ThreatLevel evaluate_battalion_threat(enum BattalionType friendly_type, enum BattalionType enemy_type) {

    switch (enemy_type) {

        case CAVALRY:
            switch (friendly_type) {
                case ARCHERS:  return VERY_HIGH;
                case SPEARMEN: return LOW;
                case INFANTRY: return MODERATE;
                case CAVALRY:  return MODERATE;
                default:       return UNKWOWN_THREAT;
            }

        case SPEARMEN:
            switch (friendly_type) {
                case CAVALRY:  return HIGH;
                case ARCHERS:  return MODERATE;
                case INFANTRY: return MODERATE;
                case SPEARMEN: return MODERATE;
                default:       return UNKWOWN_THREAT;
            }

        case ARCHERS:
            switch (friendly_type) {
                case CAVALRY:  return MODERATE;
                case SPEARMEN: return MODERATE;
                case INFANTRY: return MODERATE;
                case ARCHERS:  return MODERATE;
                default:       return UNKWOWN_THREAT;
            }

        case INFANTRY:
            switch (friendly_type) {
                case CAVALRY:  return LOW;
                case SPEARMEN: return MODERATE;
                case ARCHERS:  return MODERATE;
                case INFANTRY: return MODERATE;
                default:       return UNKWOWN_THREAT;
            }

        default:
            return UNKWOWN_THREAT;

    }

}