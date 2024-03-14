#ifndef TREE_H
#define TREE_H

#include <cassert>
#include <iostream>
#include <optional>
#include <vector>
#include <memory>
#include <algorithm>

template <std::integral T, unsigned bit_length>
class Tree {
public:
    Tree() : bottom_layer(1 << (bit_length / 2)), minimum(std::nullopt), maximum(std::nullopt), bitmask_high(((1<<bit_length/2) - 1) << bit_length/2), bitmask_low((1<<(bit_length/2)) - 1) {}

    void insert(T value) {
        if(minimum == std::nullopt) {
            minimum = value; maximum = value;
        } else {
            if(value < minimum) std::swap(minimum.value(), value);
            if(value > maximum) maximum = value;

            T high_value = value >> (bit_length / 2);
            T low_value = value & bitmask_low;

            if(upper_layer == nullptr) {
                upper_layer = std::make_unique<Tree<T, bit_length/2>>();
            }
            if(bottom_layer[high_value] == nullptr) {
                bottom_layer[high_value] = std::make_unique<Tree<T, bit_length/2>>();
            }

            if(bottom_layer[high_value]->min() == std::nullopt) {
                upper_layer->insert(high_value);
                bottom_layer[high_value]->insert(low_value);
            } else {
                bottom_layer[high_value]->insert(low_value);
            }

        }
    }

    void remove(T value) {
        if(minimum == maximum) {
            minimum = maximum = std::nullopt;
            return;
        }

        if(value == minimum) {
            T firstcluster = upper_layer->min().value();
            T offset = bottom_layer[firstcluster]->min().value();
            value = (firstcluster << (bit_length / 2)) | offset;
            minimum = value;
        }

        T high_value = value >> (bit_length / 2);
        T low_value = value & bitmask_low;

        bottom_layer[high_value]->remove(low_value);
        if(bottom_layer[high_value]->min() == std::nullopt) {
            upper_layer->remove(high_value);
            if(value == maximum) {
                std::optional<T> upper_max = upper_layer->max();
                if(upper_max == std::nullopt) {
                    maximum = minimum;
                } else {
                    maximum = (upper_max.value() << (bit_length / 2)) | bottom_layer[upper_max.value()]->max().value();
                }
            }

        } else {
            if(value == maximum) {
                T offset = bottom_layer[high_value]->max().value();
                maximum = (high_value << (bit_length / 2)) | offset;
            }
        }
    }

    bool member(T value) {
        if (value == minimum) return true;
        if (bottom_layer[value >> (bit_length/2)] == nullptr) return false;
        return bottom_layer[value >> (bit_length/2)]->member(value & bitmask_low);
    }

    std::optional<T> min() {
        return minimum;
    }
    std::optional<T> max() {
        return maximum;
    }

    std::optional<T> succ(T value) {
        if(minimum.has_value() && value < minimum) return minimum;
        std::optional<T> max_in_cluster = bottom_layer[value>>(bit_length/2)] == nullptr ? std::nullopt : bottom_layer[value >> (bit_length/2)]->max();
        if(max_in_cluster.has_value() && max_in_cluster.value() > (value & bitmask_low)) {
            T offset = bottom_layer[value >> (bit_length/2)]->succ(value & bitmask_low).value();
            return (value & bitmask_high) | offset;
        } else {
            std::optional<T> next_cluster = upper_layer == nullptr ? std::nullopt : upper_layer->succ(value >> (bit_length/2));
            if(next_cluster.has_value()) {
                T offset = bottom_layer[next_cluster.value()]->min().value();
                return (next_cluster.value() << (bit_length / 2)) | offset;
            } else {
                return std::nullopt;
            }
        }
    }

    std::optional<T> pred(T value);

private:
    std::vector<std::unique_ptr<Tree<T, bit_length/2>>> bottom_layer;
    std::unique_ptr<Tree<T, bit_length/2>> upper_layer;
    std::optional<T> minimum;
    std::optional<T> maximum;

    uint64_t bitmask_high;
    uint64_t bitmask_low;
};


enum class State {
    EMPTY,
    ZERO,
    ONE,
    BOTH
};

template<std::integral T>
class Tree<T, 1> {
public:
    Tree() : state(State::EMPTY) {}

    void insert(T value) {
        switch (state) {
            case State::EMPTY:
                state = value == 0 ? State::ZERO : State::ONE;
                break;
            case State::ZERO:
                if(value == 1) {
                    state = State::BOTH;
                }
                break;
            case State::ONE:
                if(value == 0) {
                    state = State::BOTH;
                }
                break;
        }
    }

    void remove(T value) {
        switch (state) {
            case State::ZERO:
                if(value == 0) {
                    state = State::EMPTY;
                }
                break;
            case State::ONE:
                if(value == 1) {
                    state = State::EMPTY;
                }
                break;
            case State::BOTH:
                state = value == 0 ? State::ONE : State::ZERO;
                break;
        }
    }

    bool member(T value) {
        switch (state) {
            case State::EMPTY:
                return false;
            case State::ZERO:
                return value == 0;
            case State::ONE:
                return value == 1;
            case State::BOTH:
                return true;
        }
    }

    std::optional<T> min() {
        switch (state) {
            case State::EMPTY:
                return std::nullopt;
            case State::ZERO:
                return 0;
            case State::ONE:
                return 1;
            case State::BOTH:
                return 0;
        }
    }

    std::optional<T> max() {
        switch (state) {
            case State::EMPTY:
                return std::nullopt;
            case State::ZERO:
                return 0;
            case State::ONE:
                return 1;
            case State::BOTH:
                return 1;
        }
    }

    std::optional<T> succ(T value) {
        if(value == 0 && (state == State::ONE || state == State::BOTH)) {
            return 1;
        }
        return std::nullopt;
    }

    std::optional<T> pred(T value) {
        if(value == 1 && (state == State::ZERO || state == State::BOTH)) {
            return 0;
        }
        return std::nullopt;
    }
private:
    State state;
};

#endif //TREE_H
