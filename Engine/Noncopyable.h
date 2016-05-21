/******************************************************************************/
/*!
\file       Noncopyable.h
\author     Joshua Chew Shi Yang
\par        email: joshuashiyang.chew@digipen.edu
\date       December 28, 2013
\brief      Utility class to disable the copy constructor and assignment operator
           i.e make something noncopyable.

All content © 2014 DigiPen (USA) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

#pragma warning( disable:4275 )

namespace Mocha
{
    class Noncopyable
    {
        /*Protected constructors for the derived classes*/
    protected:
        Noncopyable() {}
        ~Noncopyable() {}

        /*Ensures no copying or re-assignment of any sort due to private methods*/
    private:

        /***** Empty methods *****/
        Noncopyable(const Noncopyable &);
        const Noncopyable & operator = (const Noncopyable & rhs);
    };

}

#endif