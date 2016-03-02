/****************************************************************************/
/*!
\file Utility.h
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
Header file for utility
*/
/****************************************************************************/
#ifndef UTILITY_H
#define UTILITY_H

#include "Mtx44.h"
#include "Vertex.h"

Position operator*(const Mtx44& lhs, const Position& rhs);

#endif 