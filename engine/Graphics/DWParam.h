#pragma once
/**
 * @file DWParam.h
 * @brief FLOAT,UINT,INT に対応した変数
 */
#include <d3d12.h>
#include <wrl/client.h>

struct DWParam {
    DWParam(FLOAT f) : v{ .f = f } {}
    DWParam(UINT u) : v{ .u = u } {}
    DWParam(INT i) : v{ .i = i } {}

    void operator=(FLOAT f) { v.f = f; }
    void operator=(UINT u) { v.u = u; }
    void operator=(INT i) { v.i = i; }

    union Value {
        FLOAT f;
        UINT u;
        INT i;
    } v;
};

