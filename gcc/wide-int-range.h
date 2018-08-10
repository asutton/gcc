/* Support routines for range operations on wide ints.
   Copyright (C) 2018 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_WIDE_INT_RANGE_H
#define GCC_WIDE_INT_RANGE_H

extern bool wide_int_range_cross_product (wide_int &res_lb, wide_int &res_ub,
					  enum tree_code code, signop sign,
					  const wide_int &, const wide_int &,
					  const wide_int &, const wide_int &,
					  bool overflow_undefined);
extern bool wide_int_range_mult_wrapping (wide_int &res_lb,
					  wide_int &res_ub,
					  signop sign,
					  unsigned prec,
					  const wide_int &min0_,
					  const wide_int &max0_,
					  const wide_int &min1_,
					  const wide_int &max1_);
extern bool wide_int_range_multiplicative_op (wide_int &res_lb,
					      wide_int &res_ub,
					      enum tree_code code,
					      signop sign,
					      unsigned prec,
					      const wide_int &vr0_lb,
					      const wide_int &vr0_ub,
					      const wide_int &vr1_lb,
					      const wide_int &vr1_ub,
					      bool overflow_undefined,
					      bool overflow_wraps);
extern bool wide_int_range_lshift (wide_int &res_lb, wide_int &res_ub,
				   signop sign, unsigned prec,
				   const wide_int &, const wide_int &,
				   const wide_int &, const wide_int &,
				   bool overflow_undefined,
				   bool overflow_wraps);
extern void wide_int_range_set_zero_nonzero_bits (signop,
						  const wide_int &lb,
						  const wide_int &ub,
						  wide_int &may_be_nonzero,
						  wide_int &must_be_nonzero);
extern bool wide_int_range_can_optimize_bit_op (tree_code,
						const wide_int &lb,
						const wide_int &ub,
						const wide_int &mask);
extern bool wide_int_range_bit_xor (wide_int &wmin, wide_int &wmax,
				    signop sign,
				    unsigned prec,
				    const wide_int &must_be_nonzero0,
				    const wide_int &may_be_nonzero0,
				    const wide_int &must_be_nonzero1,
				    const wide_int &may_be_nonzero1);
extern bool wide_int_range_bit_ior (wide_int &wmin, wide_int &wmax,
				    signop sign,
				    const wide_int &vr0_min,
				    const wide_int &vr0_max,
				    const wide_int &vr1_min,
				    const wide_int &vr1_max,
				    const wide_int &must_be_nonzero0,
				    const wide_int &may_be_nonzero0,
				    const wide_int &must_be_nonzero1,
				    const wide_int &may_be_nonzero1);
extern bool wide_int_range_bit_and (wide_int &wmin, wide_int &wmax,
				    signop sign,
				    unsigned prec,
				    const wide_int &vr0_min,
				    const wide_int &vr0_max,
				    const wide_int &vr1_min,
				    const wide_int &vr1_max,
				    const wide_int &must_be_nonzero0,
				    const wide_int &may_be_nonzero0,
				    const wide_int &must_be_nonzero1,
				    const wide_int &may_be_nonzero1);
extern void wide_int_range_trunc_mod (wide_int &wmin, wide_int &wmax,
				      signop sign,
				      unsigned prec,
				      const wide_int &vr0_min,
				      const wide_int &vr0_max,
				      const wide_int &vr1_min,
				      const wide_int &vr1_max);

/* Return TRUE if shifting by range [MIN, MAX] is undefined behavior.  */

inline bool
wide_int_range_shift_undefined_p (signop sign, unsigned prec,
				  const wide_int &min, const wide_int &max)
{
  /* ?? Note: The original comment said this only applied to
     RSHIFT_EXPR, but it was being applied to both left and right
     shifts.  */

  /* Shifting by any values outside [0..prec-1], gets undefined
     behavior from the shift operation.  We cannot even trust
     SHIFT_COUNT_TRUNCATED at this stage, because that applies to rtl
     shifts, and the operation at the tree level may be widened.  */
  return wi::lt_p (min, 0, sign) || wi::ge_p (max, prec, sign);
}

#endif /* GCC_WIDE_INT_RANGE_H */
