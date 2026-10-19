#include "threat_policy.h"
#include "threat_types.h"
#include "battalion_types.h"
enum ThreatLevel evaluate_battalion_threat(enum BattalionType friendly_type, enum BattalionType enemy_type) {

    switch (enemy_type) {

        case ARCHERS:
            switch (friendly_type) {
                case ARCHERS:   return MODERATE;
                case CAVALRY:   return MODERATE;
                case GUARDSMEN: return LOW;
                case SWORDSMEN: return HIGH;
                case TEMPLAR:   return LOW;
                default:        return UNKNOWN_THREAT;
            }

        case CAVALRY:
            switch (friendly_type) {
                case ARCHERS:   return VERY_HIGH;
                case CAVALRY:   return MODERATE;
                case GUARDSMEN: return MODERATE;
                case SWORDSMEN: return HIGH;
                case TEMPLAR:   return MODERATE;
                default:        return UNKNOWN_THREAT;
            }

        case GUARDSMEN:
            switch (friendly_type) {
                case ARCHERS:   return HIGH;
                case CAVALRY:   return LOW;
                case GUARDSMEN: return MODERATE;
                case SWORDSMEN: return MODERATE;
                case TEMPLAR:   return LOW;
                default:        return UNKNOWN_THREAT;
            }

        case SWORDSMEN:
            switch (friendly_type) {
                case ARCHERS:   return HIGH;
                case CAVALRY:   return LOW;
                case GUARDSMEN: return MODERATE;
                case SWORDSMEN: return MODERATE;
                case TEMPLAR:   return MODERATE;
                default:        return UNKNOWN_THREAT;
            }

        case TEMPLAR:
            switch (friendly_type) {
                case ARCHERS:   return HIGH;
                case CAVALRY:   return MODERATE;
                case GUARDSMEN: return HIGH;
                case SWORDSMEN: return MODERATE;
                case TEMPLAR:   return MODERATE;
                default:        return UNKNOWN_THREAT;
            }

        default:
            return UNKNOWN_THREAT;

    }

}