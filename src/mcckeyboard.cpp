/**
 * @file mcckeyboard.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <stdlib.h>
#include "serialport.h"
#include "mcckeyboard.h"
#include "serialportlistener.h"
#include "util.h"
#include "log.h"

#define KB_UNLOAD 0
#define KB_LOADED 1
using namespace std;

/**
 * @brief constructor
 */
MCCKeyboard::MCCKeyboard(MCCKeyboardManager *mgr)
{
	mLocked = false;
	m_keyboard = NULL;
	m_matrix = NULL;
	m_priority = 0;
	m_status = KB_UNLOAD;
	mMgr = mgr;
}

/**
 * @brief destructor
 */
MCCKeyboard::~MCCKeyboard()
{
	if(m_status == KB_LOADED)
		Unload();
}

int MCCKeyboard::Load(const char *keyboard, const char *matrix, const char *setup)
{
	int baudrate, databits, stopbits;
	char parity;
	string tmp(setup); 
	vector<string> config;
	StringUtil::split(tmp, string(","), config);
	if(config.size() != 4)
		return -1;
	baudrate = StringUtil::atoi(config[0]);
	databits = StringUtil::atoi(config[1]);
	stopbits = StringUtil::atoi(config[2]);
	parity = *(config[3].c_str());
	InfoLog("Setup"<<baudrate<<','<<databits<<','<<stopbits<<','<<parity);
	return Load(keyboard, matrix, baudrate, databits, stopbits, parity);
}

int MCCKeyboard::Load(const char *keyboard, const char *matrix, int baudrate, int databits, int stopbits, char parity) 
{
	int rc;
	if(KB_LOADED == m_status)
		return 0;
	m_keyboard = new SerialPort();
	if(m_keyboard == NULL)
		return -1;
	if(m_keyboard->Init() != 0)
		return -1;
	if(m_keyboard->RegisterListener(this) != 0)
		return -1;
	rc = m_keyboard->Open(keyboard, baudrate, databits, stopbits, parity);
	if(rc != 0)
		return rc;
	m_matrix = new SerialPort();
	if(m_matrix == NULL)
	{
		m_keyboard->Close();
		delete m_keyboard;
		return -1;
	}
	if(m_matrix->Init() != 0)
	{
		m_keyboard->Close();
		delete m_keyboard;
		return -1;
	}
	if(m_matrix->RegisterListener(this) != 0)
	{
		m_keyboard->Close();
		delete m_keyboard;
		return -1;
	}
	rc = m_matrix->Open(matrix, baudrate, databits, stopbits, parity);
	if(rc != 0)
	{
		m_keyboard->Close();
		delete m_keyboard;
		return rc;
	}
	m_status = KB_LOADED;
	return 0;
}

int MCCKeyboard::Unload()
{
	int rc = 0;
	if(KB_UNLOAD == m_status)
		return 0;
	rc |= m_matrix->Close();
	delete m_matrix;
	rc |= m_keyboard->Close();
	delete m_keyboard;
	m_status = KB_UNLOAD;
	return rc;
}

int MCCKeyboard::on_recv(SerialPort *sp)
{
	if(sp == m_keyboard)
		OnRxKeyboardPacket(sp);
	else if(sp == m_matrix)
		OnRxMatrixPacket(sp);
	return 0;
}

int MCCKeyboard::SendtoKeyboard(void *data, int len){
	return m_keyboard->Write(data,len);
}

int MCCKeyboard::SendtoMatrix(void *data, int len){
	return m_matrix->Write(data,len);
}
