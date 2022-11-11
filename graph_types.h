#ifndef GRAPH_TYPES_H
#define GRAPH_TYPES_H

#include <cstdint>

namespace datatypes {

typedef struct {
    int16_t I;
    int16_t Q;
} int16_iq_t;

typedef struct {
    int32_t I;
    int32_t Q;
} int32_iq_t;

typedef struct {
    float I;
    float Q;
} float_iq_t;

typedef struct {
    double I;
    double Q;
} double_iq_t;

}

#endif // GRAPH_TYPES_H
