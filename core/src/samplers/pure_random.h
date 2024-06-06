#pragma once
#include "sampler.h"


class PureRandom : public Sampler
{
public:
    PureRandom() = delete;

    explicit PureRandom(const int num);

    ~PureRandom() = default;

    PureRandom(const PureRandom &r) = default;

    PureRandom(PureRandom &&r) = default;

    PureRandom &operator=(const PureRandom &rhs) = default;

    PureRandom &operator=(PureRandom &&rhs) = default;

    virtual PureRandom *clone() const override;

private:
    virtual void generate_samples() override;
};
