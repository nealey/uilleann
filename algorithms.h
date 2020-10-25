// Various FM Algorithms
// Excellent write-up:
// https://gist.github.com/bryc/e997954473940ad97a825da4e7a496fa

#pragma once

// Each operator has 4 input gains and one output gain:
//   { 1→, 2→, 3→, 4→, →out}

// 1→
#define ALG_SIMPLE \
  { \
    {0, 0, 0, 0, 1}, \
    {0}, \
    {0}, \
    {0}, \
  }

// ⮎2→1→
#define ALG_OPL2_1(feedback) \
  { \
    {0, 1, 0, 0, 1}, \
    {0, feedback, 0, 0, 0}, \
    {0}, \
    {0}, \
  }

// ⮎2→
//  1→
#define ALG_OPL2_2(feedback) \
  { \
    {0, 0, 0, 0, 1} \
    {0, feedback, 0, 0, 1} \
    {0}, \
    {0}, \
  }


// ⮎4→3→2→1→
#define ALG_DX9_1(feedback) \
  { \
    {0, 1, 0, 0, 1}, \
    {0, 0, 1, 0, 0}, \
    {0, 0, 0, 1, 0}, \
    {0, 0, 0, feedback, 0}, \
  }

// ⮎4⬎
//  3→2→1→
#define ALG_DX9_2(feedback) \
  { \
    {0, 1, 0, 0, 1}, \
    {0, 0, 1, 1, 0}, \ 
    {0, 0, 0, 0, 0}, \
    {0, 0, 0, feedback, 0}, \
  }

//   ⮎4⬎
//  3→2→1→
#define ALG_DX9_3(feedback) \
  { \
    {0, 1, 0, 1, 1}, \
    {0, 0, 1, 0, 0}, \
    {0, 0, 0, 0, 0}, \
    {0, 0, 0, feedback, 0}, \
  }

// ⮎4→3⬎
//    2→1→
#define ALG_DX9_4(feedback) \
  { \
    {0, 1, 0, 1, 1}, \
    {0, 0, 1, 0, 0}, \
    {0, 0, 0, 0, 0}, \
    {0, 0, 0, feedback, 0}, \
  }

// ⮎4→3→
//  2→1→
#define ALG_DX9_5(feedback) \
  { \
    {0, 1, 0, 0, 1}, \
    {0, 0, 0, 0, 0}, \
    {0, 0, 0, 1, 1}, \
    {0, 0, 0, feedback, 0}, \
  }

//    1→
// ⮎4→2→
//    3→
#define ALG_DX9_6(feedback) \
  { \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, 1, 1}, \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, feedback, 0}, \
  }

//    1→
//    2→
// ⮎4→3→
#define ALG_DX9_7(feedback) \
  { \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, 0, 1}, \
    {0, 0, 1, 0, 1}, \
    {0, 0, 0, feedback, 0}, \
  }

//  1→
//  2→
//  3→
// ⮎4→
#define ALG_DX9_8(feedback) \
  { \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, feedback, 1}, \
  }
