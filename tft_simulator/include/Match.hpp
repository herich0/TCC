#ifndef MATCH_HPP
#define MATCH_HPP

class Match {
private:
    int stage;
    int round;

public:
    Match();
    void nextRound();
    int getStage() const;
    int getRound() const;
    bool isPvE() const;
    bool isCarousel() const;
};

#endif