///////////////////////////////////////////////////////////////////////////////
///
///  \file EulerOrder.cpp
///  Implementation of the Euler angles order as described in Graphic Gems IV,
///  EulerOrder design referenced from Insomniac Games.
///  
///  Authors: Benjamin Strukus
///  Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "MathUtilities.h"
#include "EulerOrder.h"

namespace Math
{

EulerOrder::EulerOrder(EulerOrders::Enum eulerOrder)
  : Order(eulerOrder)
{
  //
}

EulerOrder& EulerOrder::operator = (EulerOrderParam rhs)
{
  Order = rhs.Order;
  return *this;
}

bool EulerOrder::operator == (EulerOrderParam rhs)
{
  return Order == rhs.Order;
}

bool EulerOrder::operator != (EulerOrderParam rhs)
{
  return Order != rhs.Order;
}

unsigned EulerOrder::I(void) const
{
  return EulerOrders::Safe[((unsigned(Order) >> 3) & 3)];
}

unsigned EulerOrder::J(void) const
{
  return EulerOrders::Next[I() + (OddParity() ? 1 : 0)];
}

unsigned EulerOrder::K(void) const
{
  return EulerOrders::Next[I() + (OddParity() ? 0 : 1)];
}

unsigned EulerOrder::H(void) const
{
  return RepeatingAngles() == EulerOrders::No ? K() : I();
}

bool EulerOrder::RepeatingAngles(void) const
{
  return ((unsigned(Order) >> 1) & 1) == EulerOrders::Yes;
}

bool EulerOrder::RotatingFrame(void) const
{
  return (unsigned(Order) & 1) == EulerOrders::Rotated;
}

bool EulerOrder::OddParity(void) const
{
  return ((unsigned(Order) >> 2) & 1) == EulerOrders::Odd;
}

void EulerOrder::GetOrder(EulerOrder order, unsigned& i, unsigned& j, unsigned& k, unsigned& h,
                          unsigned& parity, unsigned& repeated, unsigned& frame)
{
  unsigned orderValue = unsigned(order.Order);
  
  frame = orderValue & 1;
  orderValue >>= 1;
  
  repeated = orderValue & 1;
  orderValue >>= 1;      
  
  parity = orderValue & 1;
  orderValue >>= 1;      
  
  i = EulerOrders::Safe[orderValue & 3];      
  
  j = EulerOrders::Next[i + parity];      
  
  k = EulerOrders::Next[i + 1 - parity];  
  
  h = repeated ? k : i;           
}

}// namespace Math
