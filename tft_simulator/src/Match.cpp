#include "Match.hpp"

Match::Match() : stage(1), round(1) {}

void Match::nextRound() {
    if (stage == 1) {
        if (round < 4) {
            round++;
        } else {
            stage++;
            round = 1;
        }
    } else {
        if (round < 8) {
            round++;
        } else {
            stage++;
            round = 1;
        }
    }
}

int Match::getStage() const {
    return stage;
}

int Match::getRound() const {
    return round;
}

bool Match::isPvE() const {
    if (stage == 1) {
        return true;
    }
    if (round == 8) {
        return true;
    }
    return false;
}

bool Match::isCarousel() const {
    if (stage == 1 && round == 1) {
        return true;
    }
    if (stage > 1 && round == 5) {
        return true;
    }
    return false;
}