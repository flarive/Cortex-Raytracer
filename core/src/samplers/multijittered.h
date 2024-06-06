#pragma once
#include "sampler.h"

class MultiJittered : public Sampler
{
public:
    MultiJittered() = default;

    explicit MultiJittered(const int num_samples);

    explicit MultiJittered(const int num_samples, const int m);

    ~MultiJittered() = default;

    MultiJittered(const MultiJittered &r) = default;

    MultiJittered(MultiJittered &&r) = default;

    MultiJittered &operator=(const MultiJittered &rhs) = default;

    MultiJittered &operator=(MultiJittered &&rhs) = default;

    virtual MultiJittered *clone() const override;

private:
    virtual void generate_samples() override;
};