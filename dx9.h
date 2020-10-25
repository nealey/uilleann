// FM Algorithms used by the DX9
// Excellent write-up:
// https://gist.github.com/bryc/e997954473940ad97a825da4e7a496fa

#pragma once

// Each operator has 4 input gains and one output gain:
//   { 1→, 2→, 3→, 4→, →out}

// ⮎4→3→2→1→
#define DX9_ALG_1(feedback) \
  { \
    {0, 1, 0, 0, 1}, \
    {0, 0, 1, 0, 0}, \
    {0, 0, 0, 1, 0}, \
    {0, 0, 0, feedback, 0}, \
  }

// ⮎4⬎
//  3→2→1→
#define DX9_ALG_2(feedback) \
  { \
    {0, 1, 0, 0, 1}, \
    {0, 0, 1, 1, 0}, \ 
    {0, 0, 0, 0, 0}, \
    {0, 0, 0, feedback, 0}, \
  }

//   ⮎4⬎
//  3→2→1→
#define DX9_ALG_3(feedback) \
  { \
    {0, 1, 0, 1, 1}, \
    {0, 0, 1, 0, 0}, \
    {0, 0, 0, 0, 0}, \
    {0, 0, 0, feedback, 0}, \
  }

// ⮎4→3⬎
//    2→1→
#define DX9_ALG_4(feedback) \
  { \
    {0, 1, 0, 1, 1}, \
    {0, 0, 1, 0, 0}, \
    {0, 0, 0, 0, 0}, \
    {0, 0, 0, feedback, 0}, \
  }

// ⮎4→3→
//  2→1→
#define DX9_ALG_5(feedback) \
  { \
    {0, 1, 0, 0, 1}, \
    {0, 0, 0, 0, 0}, \
    {0, 0, 0, 1, 1}, \
    {0, 0, 0, feedback, 0}, \
  }

//    1→
// ⮎4→2→
//    3→
#define DX9_ALG_6(feedback) \
  { \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, 1, 1}, \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, feedback, 0}, \
  }

//    1→
//    2→
// ⮎4→3→
#define DX9_ALG_7(feedback) \
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
#define DX9_ALG_8(feedback) \
  { \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, 0, 1}, \
    {0, 0, 0, feedback, 1}, \
  }
