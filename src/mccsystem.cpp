/**
 * @file mccsystem.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <pjlib.h>
#include "mccsystem.h"

/**
 * @brief constructor
 */
MCCSystem::MCCSystem()
{

}

/**
 * @brief destructor
 */
MCCSystem::~MCCSystem()
{

}

int MCCSystem::Init()
{
	pj_init();
	return 0;
}


int MCCSystem::UnInit()
{
	return 0;
}
